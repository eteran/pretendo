
#ifndef P6502_20080314_H_
#define P6502_20080314_H_

#include <cstdint>

using std::int8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::uint8_t;

namespace P6502 {

	namespace {
		typedef uint8_t (*read_handler_t)(uint16_t);
		typedef void (*jam_handler_t)();
		typedef void (*sync_handler_t)();
		typedef void (*write_handler_t)(uint16_t, uint8_t);
		typedef void (*dma_handler_t)(uint8_t);
	}
	
	enum DMA_SOURCE {
		SPR_DMA = 0,
		DMC_DMA = 1
	};

	struct Context {
		// public registers
		uint16_t PC;
		uint8_t  A;
		uint8_t  X;
		uint8_t  Y;
		uint8_t  S;
		uint8_t  P;
		
		// internal registers
		uint16_t instruction;
		int      cycle;
		
		bool     irq_executing;
		bool     nmi_executing;
		bool     rst_executing;
		bool     irq_asserted;
		bool     nmi_asserted;
		bool     rst_asserted;
		
		// stats
		uint64_t executed_cycles;
		
		// handlers
		jam_handler_t   jam_handler;
		write_handler_t write_handler;
		read_handler_t  read_handler;
		sync_handler_t  sync_handler;
	};

	void init(Context &ctx, jam_handler_t jam, read_handler_t read, write_handler_t write, sync_handler_t sync);
	void reset(Context &ctx);
	void stop(Context &ctx);
	void irq(Context &ctx);
	void nmi(Context &ctx);
	void reset_irq(Context &ctx);
	void run(Context &ctx, int cycles);
	void schedule_dma(Context &ctx, dma_handler_t dma_handler, uint16_t source_address, uint16_t count, DMA_SOURCE source);
}

#endif
