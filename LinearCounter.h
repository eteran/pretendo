
#ifndef LINEAR_COUNTER_20130206_H_
#define LINEAR_COUNTER_20130206_H_

#include <cstdint>

namespace nes {
namespace apu {

using std::uint8_t;

class LinearCounter {
public:
	LinearCounter();
	~LinearCounter() = default;

private:
	LinearCounter(const LinearCounter &) = delete;
	LinearCounter &operator=(const LinearCounter &) = delete;

public:
	uint8_t value() const { return value_; }
	void clock();
	void reload();
	void set_control(uint8_t value);

private:
	uint8_t value_;
	uint8_t control_;
	bool    reload_;
};

}
}

#endif
