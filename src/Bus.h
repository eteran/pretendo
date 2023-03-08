
#ifndef BUS_H_
#define BUS_H_

#include <cstdint>

namespace nes {
namespace bus {

void write_memory(uint32_t address, uint8_t value);
uint8_t read_memory(uint32_t address);
void trash_ram();

}
}

#endif
