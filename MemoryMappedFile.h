
#ifndef MEMORY_MAPPED_FILE_H_
#define MEMORY_MAPPED_FILE_H_

#include <string>
#include <cstdint>

class MemoryMappedFile {
public:
    MemoryMappedFile();
    MemoryMappedFile(const std::string &filename, size_t size);
    MemoryMappedFile(MemoryMappedFile &&other);
    MemoryMappedFile& operator=(MemoryMappedFile &&rhs);
    MemoryMappedFile(const MemoryMappedFile &) = delete;
    MemoryMappedFile& operator=(const MemoryMappedFile &) = delete;
    ~MemoryMappedFile();

public:
    uint8_t operator[](size_t index) const;
    uint8_t& operator[](size_t index);

public:
    void swap(MemoryMappedFile &other);

private:
    uint8_t *ptr_;
    size_t size_;
    bool memory_;
};

#endif
