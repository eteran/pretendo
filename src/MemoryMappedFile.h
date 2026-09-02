
#ifndef MEMORY_MAPPED_FILE_H_
#define MEMORY_MAPPED_FILE_H_

#include <cstddef>
#include <cstdint>
#include <string>

class MemoryMappedFile {
public:
	MemoryMappedFile(const std::string &filename, size_t size);
	MemoryMappedFile() = default;
	MemoryMappedFile(MemoryMappedFile &&other) noexcept;
	MemoryMappedFile &operator=(MemoryMappedFile &&rhs) noexcept;
	MemoryMappedFile(const MemoryMappedFile &)            = delete;
	MemoryMappedFile &operator=(const MemoryMappedFile &) = delete;
	~MemoryMappedFile();

public:
	uint8_t operator[](size_t index) const;
	uint8_t &operator[](size_t index);

private:
	// how ptr_ was obtained, and therefore how it must be released
	enum class Kind {
		Heap,
		Mapped
	};

private:
	void release();

private:
	uint8_t *ptr_         = nullptr;
	size_t size_          = 0;
	Kind kind_            = Kind::Heap;
	void *file_handle_    = nullptr; // Windows HANDLE
	void *mapping_handle_ = nullptr; // Windows HANDLE
};

#endif
