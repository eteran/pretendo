
#ifndef LINEAR_COUNTER_20130206_H_
#define LINEAR_COUNTER_20130206_H_

#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>

using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::uint64_t;

class LinearCounter : public boost::noncopyable {
public:
	LinearCounter();
	~LinearCounter();

public:
	uint8_t value() const;
	void clock();
	void halt();
	void set_reload(uint8_t value);
	void set_mode(uint8_t value);
	
private:
	uint8_t value_;
	uint8_t reload_;
	uint8_t control_;
	bool    halt_;
};

#endif
