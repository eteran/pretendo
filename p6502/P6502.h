
#ifndef P6502_20080314_H_
#define P6502_20080314_H_

#include <boost/cstdint.hpp>

using boost::int8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::uint64_t;
using boost::uint8_t;

namespace P6502 {

	namespace {
		typedef uint8_t (*read_handler_t)(uint16_t);
		typedef void (*jam_handler_t)();
		typedef void (*sync_handler_t)();
		typedef void (*write_handler_t)(uint16_t, uint8_t);
	}

	extern uint16_t	PC;
	extern uint8_t	A;
	extern uint8_t	X;
	extern uint8_t	Y;
	extern uint8_t	S;
	extern uint8_t	P;

	void init(jam_handler_t jam, read_handler_t read, write_handler_t write, sync_handler_t sync);
	void reset();
	void stop();
	void burn(int cycles);
	void irq();
	void nmi();
	void reset_irq();
	void run(int cycles);
	uint64_t executed_cycles();
}

#endif
