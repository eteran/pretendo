
#include "MemoryMappedFile.h"

#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

namespace {

struct Mapping {
	uint8_t *ptr          = nullptr;
	void *file_handle     = nullptr; // Windows HANDLE
	void *mapping_handle  = nullptr; // Windows HANDLE
};

#if defined(_WIN32)

//------------------------------------------------------------------------------
// Name: try_map
// Desc: maps filename into memory, returning an empty Mapping on any failure.
//       The file is opened with OPEN_ALWAYS so that an existing save is kept;
//       SetEndOfFile then grows it to size, zero filling as needed.
//------------------------------------------------------------------------------
Mapping try_map(const std::string &filename, size_t size) {

	Mapping result;

	HANDLE file = CreateFileA(
		filename.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0, // no sharing
		nullptr,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (file == INVALID_HANDLE_VALUE) {
		return result;
	}

	LARGE_INTEGER file_size;
	file_size.QuadPart = static_cast<LONGLONG>(size);

	if (SetFilePointerEx(file, file_size, nullptr, FILE_BEGIN) && SetEndOfFile(file)) {

		const auto size64 = static_cast<uint64_t>(size);

		HANDLE mapping = CreateFileMapping(
			file,
			nullptr,
			PAGE_READWRITE,
			static_cast<DWORD>((size64 >> 32) & 0xffffffff),
			static_cast<DWORD>(size64 & 0xffffffff),
			nullptr);

		if (mapping != nullptr && mapping != INVALID_HANDLE_VALUE) {
			auto p = static_cast<uint8_t *>(MapViewOfFile(mapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, size));
			if (p != nullptr) {
				result.ptr            = p;
				result.file_handle    = file;
				result.mapping_handle = mapping;
				return result;
			}

			CloseHandle(mapping);
		}
	}

	CloseHandle(file);
	return result;
}

#else

//------------------------------------------------------------------------------
// Name: try_map
// Desc: maps filename into memory, returning an empty Mapping on any failure.
//       Plain POSIX, so this covers Linux, macOS and the BSDs alike.
//------------------------------------------------------------------------------
Mapping try_map(const std::string &filename, size_t size) {

	Mapping result;

	const int fd = ::open(filename.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		return result;
	}

	if (::ftruncate(fd, static_cast<off_t>(size)) == 0) {
		auto p = static_cast<uint8_t *>(::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
		if (p != MAP_FAILED) {
			result.ptr = p;
		}
	}

	::close(fd);
	return result;
}

#endif

}

//------------------------------------------------------------------------------
// Name: MemoryMappedFile
//------------------------------------------------------------------------------
MemoryMappedFile::MemoryMappedFile(const std::string &filename, size_t size)
	: size_(size) {

	const Mapping mapping = try_map(filename, size);
	if (mapping.ptr) {
		ptr_            = mapping.ptr;
		kind_           = Kind::Mapped;
		file_handle_    = mapping.file_handle;
		mapping_handle_ = mapping.mapping_handle;
		return;
	}

	// the contents are the cartridge's battery backed RAM, so they must start
	// zeroed the way a freshly sized save file would
	std::cerr << "Failed to map SRAM to file, using fallback implementation" << std::endl;

	ptr_  = new uint8_t[size]();
	kind_ = Kind::Heap;
}

//------------------------------------------------------------------------------
// Name: MemoryMappedFile
//------------------------------------------------------------------------------
MemoryMappedFile::MemoryMappedFile(MemoryMappedFile &&other) noexcept
	: ptr_(other.ptr_), size_(other.size_), kind_(other.kind_), file_handle_(other.file_handle_), mapping_handle_(other.mapping_handle_) {

	other.ptr_            = nullptr;
	other.size_           = 0;
	other.file_handle_    = nullptr;
	other.mapping_handle_ = nullptr;
}

//------------------------------------------------------------------------------
// Name: operator=
//------------------------------------------------------------------------------
MemoryMappedFile &MemoryMappedFile::operator=(MemoryMappedFile &&rhs) noexcept {

	if (this != &rhs) {
		release();

		ptr_            = rhs.ptr_;
		size_           = rhs.size_;
		kind_           = rhs.kind_;
		file_handle_    = rhs.file_handle_;
		mapping_handle_ = rhs.mapping_handle_;

		rhs.ptr_            = nullptr;
		rhs.size_           = 0;
		rhs.file_handle_    = nullptr;
		rhs.mapping_handle_ = nullptr;
	}

	return *this;
}

//------------------------------------------------------------------------------
// Name: ~MemoryMappedFile
//------------------------------------------------------------------------------
MemoryMappedFile::~MemoryMappedFile() {
	release();
}

//------------------------------------------------------------------------------
// Name: release
// Desc: frees whatever ptr_ owns and leaves the object empty
//------------------------------------------------------------------------------
void MemoryMappedFile::release() {

	if (!ptr_) {
		return;
	}

	switch (kind_) {
	case Kind::Mapped:
#if defined(_WIN32)
		UnmapViewOfFile(ptr_);
		CloseHandle(static_cast<HANDLE>(mapping_handle_));
		CloseHandle(static_cast<HANDLE>(file_handle_));
#else
		::munmap(ptr_, size_);
#endif
		break;
	case Kind::Heap:
		delete[] ptr_;
		break;
	}

	ptr_            = nullptr;
	size_           = 0;
	file_handle_    = nullptr;
	mapping_handle_ = nullptr;
}

uint8_t MemoryMappedFile::operator[](size_t index) const {
	return ptr_[index];
}

uint8_t &MemoryMappedFile::operator[](size_t index) {
	return ptr_[index];
}
