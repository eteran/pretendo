
#ifndef ENVELOPE_20130501_H_
#define ENVELOPE_20130501_H_

#include <cstdint>

namespace nes {
namespace apu {

class Envelope {
public:
	void clock();
	void start();
	void set_control(uint8_t value);
	uint8_t volume() const;

private:
	void clock_divider();

private:
    bool    start_   = false;
    uint8_t counter_ = 0;
    uint8_t divider_ = 0xff;
    uint8_t control_ = 0;
};

}
}

#endif
