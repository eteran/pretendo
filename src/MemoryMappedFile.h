
#ifndef MEMORY_MAPPED_FILE_H_
#define MEMORY_MAPPED_FILE_H_

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

class MemoryMappedFile {
public:
	MemoryMappedFile(const std::string &filename, size_t size);
	MemoryMappedFile()                    = default;
	MemoryMappedFile(MemoryMappedFile &&) = default;
	MemoryMappedFile &operator=(MemoryMappedFile &&) = default;
	MemoryMappedFile(const MemoryMappedFile &)       = delete;
	MemoryMappedFile &operator=(const MemoryMappedFile &) = delete;
	~MemoryMappedFile();

public:
	uint8_t operator[](size_t index) const;
	uint8_t &operator[](size_t index);

private:
	uint8_t *ptr_ = nullptr;
	std::function<void(uint8_t *)> deleter_;
};

#endif
