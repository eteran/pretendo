
#include "MemoryMappedFile.h"

#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef __linux__
#include <sys/mman.h>
#include <unistd.h>
#endif
#ifdef _WIN32
#include <windows.h>
#endif

MemoryMappedFile::MemoryMappedFile(const std::string &filename, size_t size) {

#ifdef __linux__
	int fd = ::open(filename.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd != -1) {
		::ftruncate(fd, size);

		auto p = static_cast<uint8_t *>(mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
		if (p != MAP_FAILED) {

			ptr_     = p;
			deleter_ = [size](uint8_t *ptr) {
				::munmap(ptr, size);
			};
		} else {
			std::cerr << "Failed to map SRAM to file, using fallback implementation" << std::endl;

			ptr_     = new uint8_t[size];
			deleter_ = [](uint8_t *ptr) {
				delete[] ptr;
			};
		}

		::close(fd);
	}
#elif _WIN32
	// Windows memory mapping
	HANDLE hFile = CreateFileA(
		filename.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,                          // no sharing
		nullptr,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);

	if (hFile != INVALID_HANDLE_VALUE) {
		// Set file size
		LARGE_INTEGER file_size;
		file_size.QuadPart = size;
		if (SetFilePointerEx(hFile, file_size, nullptr, FILE_BEGIN) &&
			SetEndOfFile(hFile)) {

			HANDLE hMapping = CreateFileMapping(
				hFile,
				nullptr,
				PAGE_READWRITE,
				(size >> 32) & 0xFFFFFFFF,  // high 32 bits of size
				size & 0xFFFFFFFF,           // low 32 bits of size
				nullptr
			);

			if (hMapping != nullptr && hMapping != INVALID_HANDLE_VALUE) {
				auto p = static_cast<uint8_t *>(
					MapViewOfFile(hMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, size)
				);

				if (p != nullptr) {
					ptr_           = p;
					file_handle_   = hFile;
					mapping_handle_ = hMapping;

					deleter_ = [this](uint8_t *ptr) {
						if (ptr && mapping_handle_) {
							UnmapViewOfFile(ptr);
							CloseHandle(static_cast<HANDLE>(mapping_handle_));
							CloseHandle(static_cast<HANDLE>(file_handle_));
						}
					};
					return;
				}
			}
			if (hMapping != nullptr && hMapping != INVALID_HANDLE_VALUE) {
				CloseHandle(hMapping);
			}
		}
		CloseHandle(hFile);
	}

	std::cerr << "Failed to map SRAM to file, using fallback implementation" << std::endl;

	ptr_     = new uint8_t[size];
	deleter_ = [](uint8_t *ptr) {
		delete[] ptr;
	};
#else
	std::cerr << "Failed to map SRAM to file, using fallback implementation" << std::endl;

	ptr_     = new uint8_t[size];
	deleter_ = [](uint8_t *ptr) {
		delete[] ptr;
	};
#endif
}

MemoryMappedFile::~MemoryMappedFile() {
	if (deleter_) {
		deleter_(ptr_);
	}
}

uint8_t MemoryMappedFile::operator[](size_t index) const {
	return ptr_[index];
}

uint8_t &MemoryMappedFile::operator[](size_t index) {
	return ptr_[index];
}
