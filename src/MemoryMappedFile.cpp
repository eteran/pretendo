
#include "MemoryMappedFile.h"

#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef __linux__
#include <sys/mman.h>
#include <unistd.h>
#endif


MemoryMappedFile::MemoryMappedFile(const std::string &filename, size_t size) {

#ifdef __linux__
    int fd = ::open(filename.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if(fd != -1) {
		(void)::ftruncate(fd, size);

        auto p = static_cast<uint8_t *>(mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
        if(p != MAP_FAILED) {

            ptr_     = p;
            deleter_ = [size](uint8_t *ptr) {
                ::munmap(ptr, size);
            };
        } else {
            std::cerr << "Failed to map SRAM to file, using fallback implemention" << std::endl;

            ptr_     = new uint8_t[size];
            deleter_ = [](uint8_t *ptr) {
                delete [] ptr;
            };
        }

        ::close(fd);
    }
#else
	std::cerr << "Failed to map SRAM to file, using fallback implemention" << std::endl;

	ptr_ = new uint8_t[size];
	deleter_ = [](uint8_t *ptr) {
		delete[] ptr;
	};
#endif
}

MemoryMappedFile::~MemoryMappedFile() {
    if(deleter_) {
        deleter_(ptr_);
    }
}

uint8_t MemoryMappedFile::operator[](size_t index) const {
    return ptr_[index];
}

uint8_t& MemoryMappedFile::operator[](size_t index) {
    return ptr_[index];
}

