
#include "MemoryMappedFile.h"

#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>

namespace {

const auto InvalidAddress = reinterpret_cast<void *>(-1);

}

MemoryMappedFile::MemoryMappedFile() : ptr_(static_cast<uint8_t *>(InvalidAddress)), size_(0), memory_(false) {
}

MemoryMappedFile::MemoryMappedFile(const std::string &filename, size_t size) : ptr_(static_cast<uint8_t *>(InvalidAddress)), size_(size), memory_(false) {

    int fd = ::open(filename.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if(fd != -1) {
        ::ftruncate(fd, size);
        ptr_ = static_cast<uint8_t *>(mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
        ::close(fd);
    }

    if(ptr_ == InvalidAddress) {
        std::cerr << "Failed to map SRAM to file, using fallback implemention" << std::endl;
        ptr_ = new uint8_t[size]();
        memory_ = true;
    }
}

MemoryMappedFile::MemoryMappedFile(MemoryMappedFile &&other) : ptr_(other.ptr_), size_(other.size_), memory_(other.memory_) {
    other.ptr_ = static_cast<uint8_t *>(InvalidAddress);
    other.size_ = 0;
    other.memory_ = false;
}

MemoryMappedFile& MemoryMappedFile::operator=(MemoryMappedFile &&rhs) {
    MemoryMappedFile(std::move(rhs)).swap(*this);
    return *this;
}

MemoryMappedFile::~MemoryMappedFile() {
    if(memory_) {
		
		std::cout << "Deleting Pointer..." << std::endl;
	
        delete [] ptr_;
    } else {
        if(ptr_ != InvalidAddress) {
		
			std::cout << "Unmapping Pointer..." << std::endl;
		
            ::munmap(ptr_, size_);
        }
    }
}

uint8_t MemoryMappedFile::operator[](size_t index) const {
    return ptr_[index];
}

uint8_t& MemoryMappedFile::operator[](size_t index) {
    return ptr_[index];
}

void MemoryMappedFile::swap(MemoryMappedFile &other) {
    using std::swap;
    swap(ptr_, other.ptr_);
    swap(size_, other.size_);
}


#if 0
//------------------------------------------------------------------------------
// Name:
//------------------------------------------------------------------------------
uint8_t *Mapper::open_sram(size_t size) {

    const char *homedir;

    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }

    if(!homedir) {
        return nullptr;
    }


    const boost::filesystem::path home_directory(homedir);
    const boost::filesystem::path cache_path = home_directory / ".cache" / "pretendo";

    boost::filesystem::create_directories(cache_path);

    char hex_buf[32];
    snprintf(hex_buf, sizeof(hex_buf), "%08x", nes::cart.rom_hash());
    boost::filesystem::path save_file = cache_path / (std::string(hex_buf) + ".sav");


    uint8_t *ptr = nullptr;

    int fd = ::open(save_file.string().c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if(fd != -1) {
        ::ftruncate(fd, size);
        ptr = static_cast<uint8_t *>(mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
        ::close(fd);
    }

    if(ptr == reinterpret_cast<void *>(-1)) {
        return nullptr;
    }

    return ptr;
}


#endif
