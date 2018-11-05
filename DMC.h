
#ifndef DMC_20130206_H_
#define DMC_20130206_H_

#include "ShiftRegister.h"
#include "Timer.h"
#include "BitField.h"
#include <cstdint>

namespace nes {
namespace apu {

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

union DMCControl {
	uint8_t       value;
	BitField<0,4> frequency;
	BitField<6>   loop;
	BitField<7>   irq;
};

class DMC {
public:
    DMC()                       = default;
    ~DMC()                      = default;
    DMC(const DMC &)            = delete;
	DMC &operator=(const DMC &) = delete;

public:
	void enable();
	void disable();
	void set_enabled(bool value);

public:
	void write_reg0(uint8_t value);
	void write_reg1(uint8_t value);
	void write_reg2(uint8_t value);
	void write_reg3(uint8_t value);

public:
	uint16_t bytes_remaining() const;
	void load_sample_buffer(uint8_t value);

public:
	void tick();
	uint8_t output() const;

private:
	bool irq_enabled() const;
	bool loop() const;
	bool output_clock();
	void start_cycle();


private:
	bool                   muted_              = false;
	bool                   sample_buffer_empty = true;
	uint16_t               sample_pointer_     = 0xc000;
	uint16_t               sample_address_     = 0xc000;
	uint16_t               bytes_remaining_    = 0;
	uint16_t               sample_length_      = 0;
	uint8_t                bits_remaining_     = 0;
	uint8_t                output_             = 0;
	ShiftRegister<uint8_t> shift_register_{0};
    DMCControl             control_{0};
    Timer                  timer_;
};


}
}

#endif
