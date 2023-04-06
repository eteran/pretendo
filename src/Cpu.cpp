
#ifdef _MSC_VER
#pragma warning(disable : 4127)
#endif

#include "Cpu.h"
#include "Bus.h"
#include "Cart.h"
#include "Compiler.h"
#include "Mapper.h"
#include "Nes.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>

#define LAST_CYCLE                                         \
	do {                                                   \
		rst_executing_ = false;                            \
		nmi_executing_ = false;                            \
		irq_executing_ = false;                            \
                                                           \
		if (rst_asserted_) {                               \
			rst_executing_ = true;                         \
		} else if (nmi_asserted_) {                        \
			nmi_executing_ = true;                         \
		} else if (irq_asserted_ && ((P & I_MASK) == 0)) { \
			irq_executing_ = true;                         \
		}                                                  \
		nmi_asserted_ = false;                             \
		rst_asserted_ = false;                             \
	} while (0)

#define OPCODE_COMPLETE \
	do {                \
		cycle_ = -1;    \
		return;         \
	} while (0)

namespace nes::cpu {

// public registers
register16 PC = {};
uint8_t A     = 0;
uint8_t X     = 0;
uint8_t Y     = 0;
uint8_t S     = 0;
uint8_t P     = I_MASK | R_MASK;

namespace {

constexpr uint16_t NmiVectorAddress = 0xfffa;
constexpr uint16_t RstVectorAddress = 0xfffc;
constexpr uint16_t IrqVectorAddress = 0xfffe;
constexpr uint16_t StackAddress     = 0x0100;

constexpr uint8_t flag_table_[256] = {
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};

uint8_t irq_sources_ = 0x00;

// internal registers
uint16_t instruction_ = 0;
int cycle_            = 0;

// internal registers (which get trashed by instructions)
register16 effective_address_ = {};
register16 data16_            = {};
register16 old_pc_            = {};
register16 new_pc_            = {};
uint8_t data8_                = {};

bool irq_asserted_  = false;
bool nmi_asserted_  = false;
bool rst_asserted_  = false;
bool irq_executing_ = false;
bool nmi_executing_ = false;
bool rst_executing_ = true;

dma_handler_t spr_dma_handler_   = nullptr;
uint_least16_t spr_dma_source_address_ = 0;
uint_least16_t spr_dma_count_          = 0;
uint8_t spr_dma_byte_            = 0;
uint8_t spr_dma_delay_           = 0;

dma_handler_t dmc_dma_handler_   = nullptr;
uint_least16_t dmc_dma_source_address_ = 0;
uint_least16_t dmc_dma_count_          = 0;
uint8_t dmc_dma_byte_            = 0;
uint8_t dmc_dma_delay_           = 0;

// stats
uint64_t executed_cycles_ = 1; // NOTE(eteran): 1 instead of 0 makes 4.irq_and_dma.nes pass...

/**
 * @brief jam_handler
 */
[[noreturn]] void jam_handler() {
	// TODO(eteran): do something useful here...
	abort();
}

/**
 * @brief sync_handler
 */
void sync_handler() {
	return nes::cart.mapper()->cpu_sync();
}

/**
 * @brief set_flag - sets the given flag to true
 */
template <uint8_t M>
void set_flag() {
	P |= M;
}

/**
 * @brief clear_flag - sets the given flag to false
 */
template <uint8_t M>
void clear_flag() {
	P &= ~M;
}

/**
 * @brief set_flag_condition - sets the given flag based on the given condition
 * @param cond
 */
template <uint8_t M>
void set_flag_condition(bool cond) {
	if (cond) {
		set_flag<M>();
	} else {
		clear_flag<M>();
	}
}

/**
 * @brief update_nz_flags - sets the Negative and Zero flags based on the value given
 * @param value
 */
void update_nz_flags(uint8_t value) {

	// basically no bits set = 0x02
	// high bit set = 0x80
	// else = 0x00

	P &= ~(N_MASK | Z_MASK);
	P |= flag_table_[value];
}

// opcode implementation
#include "memory.h"
#include "opcodes.h"

void cycle_0(uint8_t next_op) {
	// first cycle is always instruction fetch
	// or do we force an interrupt?

	if (rst_executing_) {
		instruction_ = 0x100;
	} else if (nmi_executing_) {
		instruction_ = 0x101;
	} else if (irq_executing_) {
		instruction_ = 0x102;
	} else {
		instruction_ = next_op;
		++PC.raw;
	}
}

#include "address_modes.h"

/**
 * @brief execute_opcode
 */
void execute_opcode() {

	using fptr_t = void (*)();

	static const fptr_t table[] = {
		opcode_brk::execute,
		indexed_indirect<opcode_ora>::execute,
		opcode_jam::execute,
		indexed_indirect<opcode_slo>::execute,
		zero_page<opcode_nop>::execute,
		zero_page<opcode_ora>::execute,
		zero_page<opcode_asl>::execute,
		zero_page<opcode_slo>::execute,
		stack<opcode_php>::execute,
		immediate<opcode_ora>::execute,
		accumulator<opcode_asl>::execute,
		immediate<opcode_aac>::execute,
		absolute<opcode_nop>::execute,
		absolute<opcode_ora>::execute,
		absolute<opcode_asl>::execute,
		absolute<opcode_slo>::execute,
		relative<opcode_bpl>::execute,
		indirect_indexed<opcode_ora>::execute,
		opcode_jam::execute,
		indirect_indexed<opcode_slo>::execute,
		zero_page_x<opcode_nop>::execute,
		zero_page_x<opcode_ora>::execute,
		zero_page_x<opcode_asl>::execute,
		zero_page_x<opcode_slo>::execute,
		implied<opcode_clc>::execute,
		absolute_y<opcode_ora>::execute,
		implied<opcode_nop>::execute,
		absolute_y<opcode_slo>::execute,
		absolute_x<opcode_nop>::execute,
		absolute_x<opcode_ora>::execute,
		absolute_x<opcode_asl>::execute,
		absolute_x<opcode_slo>::execute,
		opcode_jsr::execute,
		indexed_indirect<opcode_and>::execute,
		opcode_jam::execute,
		indexed_indirect<opcode_rla>::execute,
		zero_page<opcode_bit>::execute,
		zero_page<opcode_and>::execute,
		zero_page<opcode_rol>::execute,
		zero_page<opcode_rla>::execute,
		stack<opcode_plp>::execute,
		immediate<opcode_and>::execute,
		accumulator<opcode_rol>::execute,
		immediate<opcode_aac>::execute,
		absolute<opcode_bit>::execute,
		absolute<opcode_and>::execute,
		absolute<opcode_rol>::execute,
		absolute<opcode_rla>::execute,
		relative<opcode_bmi>::execute,
		indirect_indexed<opcode_and>::execute,
		opcode_jam::execute,
		indirect_indexed<opcode_rla>::execute,
		zero_page_x<opcode_nop>::execute,
		zero_page_x<opcode_and>::execute,
		zero_page_x<opcode_rol>::execute,
		zero_page_x<opcode_rla>::execute,
		implied<opcode_sec>::execute,
		absolute_y<opcode_and>::execute,
		implied<opcode_nop>::execute,
		absolute_y<opcode_rla>::execute,
		absolute_x<opcode_nop>::execute,
		absolute_x<opcode_and>::execute,
		absolute_x<opcode_rol>::execute,
		absolute_x<opcode_rla>::execute,
		opcode_rti::execute,
		indexed_indirect<opcode_eor>::execute,
		opcode_jam::execute,
		indexed_indirect<opcode_sre>::execute,
		zero_page<opcode_nop>::execute,
		zero_page<opcode_eor>::execute,
		zero_page<opcode_lsr>::execute,
		zero_page<opcode_sre>::execute,
		stack<opcode_pha>::execute,
		immediate<opcode_eor>::execute,
		accumulator<opcode_lsr>::execute,
		immediate<opcode_asr>::execute,
		absolute<opcode_jmp>::execute,
		absolute<opcode_eor>::execute,
		absolute<opcode_lsr>::execute,
		absolute<opcode_sre>::execute,
		relative<opcode_bvc>::execute,
		indirect_indexed<opcode_eor>::execute,
		opcode_jam::execute,
		indirect_indexed<opcode_sre>::execute,
		zero_page_x<opcode_nop>::execute,
		zero_page_x<opcode_eor>::execute,
		zero_page_x<opcode_lsr>::execute,
		zero_page_x<opcode_sre>::execute,
		implied<opcode_cli>::execute,
		absolute_y<opcode_eor>::execute,
		implied<opcode_nop>::execute,
		absolute_y<opcode_sre>::execute,
		absolute_x<opcode_nop>::execute,
		absolute_x<opcode_eor>::execute,
		absolute_x<opcode_lsr>::execute,
		absolute_x<opcode_sre>::execute,
		opcode_rts::execute,
		indexed_indirect<opcode_adc>::execute,
		opcode_jam::execute,
		indexed_indirect<opcode_rra>::execute,
		zero_page<opcode_nop>::execute,
		zero_page<opcode_adc>::execute,
		zero_page<opcode_ror>::execute,
		zero_page<opcode_rra>::execute,
		stack<opcode_pla>::execute,
		immediate<opcode_adc>::execute,
		accumulator<opcode_ror>::execute,
		immediate<opcode_arr>::execute,
		indirect<opcode_jmp>::execute,
		absolute<opcode_adc>::execute,
		absolute<opcode_ror>::execute,
		absolute<opcode_rra>::execute,
		relative<opcode_bvs>::execute,
		indirect_indexed<opcode_adc>::execute,
		opcode_jam::execute,
		indirect_indexed<opcode_rra>::execute,
		zero_page_x<opcode_nop>::execute,
		zero_page_x<opcode_adc>::execute,
		zero_page_x<opcode_ror>::execute,
		zero_page_x<opcode_rra>::execute,
		implied<opcode_sei>::execute,
		absolute_y<opcode_adc>::execute,
		implied<opcode_nop>::execute,
		absolute_y<opcode_rra>::execute,
		absolute_x<opcode_nop>::execute,
		absolute_x<opcode_adc>::execute,
		absolute_x<opcode_ror>::execute,
		absolute_x<opcode_rra>::execute,
		immediate<opcode_nop>::execute,
		indexed_indirect<opcode_sta>::execute,
		immediate<opcode_nop>::execute,
		indexed_indirect<opcode_aax>::execute,
		zero_page<opcode_sty>::execute,
		zero_page<opcode_sta>::execute,
		zero_page<opcode_stx>::execute,
		zero_page<opcode_aax>::execute,
		implied<opcode_dey>::execute,
		immediate<opcode_nop>::execute,
		implied<opcode_txa>::execute,
		immediate<opcode_xaa>::execute,
		absolute<opcode_sty>::execute,
		absolute<opcode_sta>::execute,
		absolute<opcode_stx>::execute,
		absolute<opcode_aax>::execute,
		relative<opcode_bcc>::execute,
		indirect_indexed<opcode_sta>::execute,
		opcode_jam::execute,
		indirect_indexed<opcode_axa>::execute,
		zero_page_x<opcode_sty>::execute,
		zero_page_x<opcode_sta>::execute,
		zero_page_y<opcode_stx>::execute,
		zero_page_y<opcode_aax>::execute,
		implied<opcode_tya>::execute,
		absolute_y<opcode_sta>::execute,
		implied<opcode_txs>::execute,
		absolute_y<opcode_xas>::execute,
		absolute_x<opcode_sya>::execute,
		absolute_x<opcode_sta>::execute,
		absolute_y<opcode_sxa>::execute,
		absolute_y<opcode_axa>::execute,
		immediate<opcode_ldy>::execute,
		indexed_indirect<opcode_lda>::execute,
		immediate<opcode_ldx>::execute,
		indexed_indirect<opcode_lax>::execute,
		zero_page<opcode_ldy>::execute,
		zero_page<opcode_lda>::execute,
		zero_page<opcode_ldx>::execute,
		zero_page<opcode_lax>::execute,
		implied<opcode_tay>::execute,
		immediate<opcode_lda>::execute,
		implied<opcode_tax>::execute,
		immediate<opcode_lax>::execute,
		absolute<opcode_ldy>::execute,
		absolute<opcode_lda>::execute,
		absolute<opcode_ldx>::execute,
		absolute<opcode_lax>::execute,
		relative<opcode_bcs>::execute,
		indirect_indexed<opcode_lda>::execute,
		opcode_jam::execute,
		indirect_indexed<opcode_lax>::execute,
		zero_page_x<opcode_ldy>::execute,
		zero_page_x<opcode_lda>::execute,
		zero_page_y<opcode_ldx>::execute,
		zero_page_y<opcode_lax>::execute,
		implied<opcode_clv>::execute,
		absolute_y<opcode_lda>::execute,
		implied<opcode_tsx>::execute,
		absolute_y<opcode_lar>::execute,
		absolute_x<opcode_ldy>::execute,
		absolute_x<opcode_lda>::execute,
		absolute_y<opcode_ldx>::execute,
		absolute_y<opcode_lax>::execute,
		immediate<opcode_cpy>::execute,
		indexed_indirect<opcode_cmp>::execute,
		immediate<opcode_nop>::execute,
		indexed_indirect<opcode_dcp>::execute,
		zero_page<opcode_cpy>::execute,
		zero_page<opcode_cmp>::execute,
		zero_page<opcode_dec>::execute,
		zero_page<opcode_dcp>::execute,
		implied<opcode_iny>::execute,
		immediate<opcode_cmp>::execute,
		implied<opcode_dex>::execute,
		immediate<opcode_axs>::execute,
		absolute<opcode_cpy>::execute,
		absolute<opcode_cmp>::execute,
		absolute<opcode_dec>::execute,
		absolute<opcode_dcp>::execute,
		relative<opcode_bne>::execute,
		indirect_indexed<opcode_cmp>::execute,
		opcode_jam::execute,
		indirect_indexed<opcode_dcp>::execute,
		zero_page_x<opcode_nop>::execute,
		zero_page_x<opcode_cmp>::execute,
		zero_page_x<opcode_dec>::execute,
		zero_page_x<opcode_dcp>::execute,
		implied<opcode_cld>::execute,
		absolute_y<opcode_cmp>::execute,
		implied<opcode_nop>::execute,
		absolute_y<opcode_dcp>::execute,
		absolute_x<opcode_nop>::execute,
		absolute_x<opcode_cmp>::execute,
		absolute_x<opcode_dec>::execute,
		absolute_x<opcode_dcp>::execute,
		immediate<opcode_cpx>::execute,
		indexed_indirect<opcode_sbc>::execute,
		immediate<opcode_nop>::execute,
		indexed_indirect<opcode_isc>::execute,
		zero_page<opcode_cpx>::execute,
		zero_page<opcode_sbc>::execute,
		zero_page<opcode_inc>::execute,
		zero_page<opcode_isc>::execute,
		implied<opcode_inx>::execute,
		immediate<opcode_sbc>::execute,
		implied<opcode_nop>::execute,
		immediate<opcode_sbc>::execute,
		absolute<opcode_cpx>::execute,
		absolute<opcode_sbc>::execute,
		absolute<opcode_inc>::execute,
		absolute<opcode_isc>::execute,
		relative<opcode_beq>::execute,
		indirect_indexed<opcode_sbc>::execute,
		opcode_jam::execute,
		indirect_indexed<opcode_isc>::execute,
		zero_page_x<opcode_nop>::execute,
		zero_page_x<opcode_sbc>::execute,
		zero_page_x<opcode_inc>::execute,
		zero_page_x<opcode_isc>::execute,
		implied<opcode_sed>::execute,
		absolute_y<opcode_sbc>::execute,
		implied<opcode_nop>::execute,
		absolute_y<opcode_isc>::execute,
		absolute_x<opcode_nop>::execute,
		absolute_x<opcode_sbc>::execute,
		absolute_x<opcode_inc>::execute,
		absolute_x<opcode_isc>::execute,

		opcode_rst::execute,
		opcode_nmi::execute,
		opcode_irq::execute,
	};

	assert(instruction_ <= 0x102);
	table[instruction_]();
}

/**
 * @brief clock - steps the emulation 1 cycle
 */
void clock() {

	if (UNLIKELY(dmc_dma_count_)) {

		if (dmc_dma_delay_ != 0) {
			read_byte(dmc_dma_source_address_);
			--dmc_dma_delay_;
			return;
		}

		// the count will always be initially even (we multiply by 2)
		// so, this forces us to idle for 1 cycle if
		// the CPU starts DMA on an odd cycle
		// after that, they should stay in sync
		if ((dmc_dma_count_ & 1) != (executed_cycles_ & 1)) {
			read_byte(dmc_dma_source_address_);
			return;
		}

		if ((dmc_dma_count_ & 1) == 0) {
			// read cycle
			dmc_dma_byte_ = read_byte(dmc_dma_source_address_++);
		} else {
			// write cycle
			(*dmc_dma_handler_)(dmc_dma_byte_);
		}

		--dmc_dma_count_;

	} else if (UNLIKELY(spr_dma_count_)) {

		if (spr_dma_delay_ != 0) {
			read_byte(spr_dma_source_address_);
			--spr_dma_delay_;
			return;
		}

		// the count will always be initially even (we multiply by 2)
		// so, this forces us to idle for 1 cycle if
		// the CPU starts DMA on an odd cycle
		// after that, they should stay in sync
		if ((spr_dma_count_ & 1) != (executed_cycles_ & 1)) {
			read_byte(spr_dma_source_address_);
			return;
		}

		if ((executed_cycles_ & 1) == 0) {
			// read cycle
			spr_dma_byte_ = read_byte(spr_dma_source_address_++);
		} else {
			// write cycle
			(*spr_dma_handler_)(spr_dma_byte_);
		}

		--spr_dma_count_;
	} else {
		assert(cycle_ < 10);

		if (cycle_ == 0) {
			const uint8_t next_op = read_byte(PC.raw);
			cycle_0(next_op);
		} else {
			// execute the current part of the instruction
			execute_opcode();
		}

		++cycle_;
	}
}

}

//------------------------------------------------------------------------------
// Start Public Functions
//------------------------------------------------------------------------------

/**
 * @brief tick
 */
void tick() {
	clock();
	sync_handler();
	++executed_cycles_;
}

/**
 * @brief nmi
 */
void nmi() {
	nmi_asserted_ = true;
}

/**
 * @brief reset
 */
void reset() {
	if (!rst_executing_) {
		rst_asserted_    = true;
		irq_asserted_    = false;
		nmi_asserted_    = false;
		executed_cycles_ = 1;
	}
}

/**
 * @brief clear_nmi
 */
void clear_nmi() {
	nmi_asserted_ = false;
}

/**
 * @brief stop
 */
void stop() {

	A = 0;
	X = 0;
	Y = 0;
	S = 0;
	P = I_MASK | R_MASK;

	executed_cycles_ = 1;
	irq_asserted_    = false;
	nmi_asserted_    = false;
	rst_asserted_    = false;
	irq_executing_   = false;
	nmi_executing_   = false;
	rst_executing_   = true;
	cycle_           = 0;

	spr_dma_handler_        = nullptr;
	spr_dma_source_address_ = 0;
	spr_dma_count_          = 0;
	spr_dma_delay_          = 0;

	dmc_dma_handler_        = nullptr;
	dmc_dma_source_address_ = 0;
	dmc_dma_count_          = 0;
	dmc_dma_delay_          = 0;
}

/**
 * @brief reset
 * @param reset_type
 */
void reset(Reset reset_type) {

	if (reset_type == Reset::Hard) {
		stop();
		nes::bus::trash_ram();
	}

	reset();
	std::cout << "CPU reset complete" << std::endl;
}

/**
 * @brief irq
 * @param source
 */
void irq(IrqSource source) {

	irq_sources_ |= source;

	if (irq_sources_) {
		irq_asserted_ = true;
	}
}

/**
 * @brief clear_irq
 * @param source
 */
void clear_irq(IrqSource source) {

	irq_sources_ &= ~source;

	if (!irq_sources_) {
		irq_asserted_ = false;
	}
}

/**
 * @brief schedule_spr_dma
 * @param dma_handler
 * @param source_address
 * @param count
 */
void schedule_spr_dma(dma_handler_t dma_handler, uint_least16_t source_address, uint_least16_t count) {
	spr_dma_handler_        = dma_handler;
	spr_dma_source_address_ = source_address;
	spr_dma_count_          = count * 2;
	spr_dma_delay_          = 1;
}

/**
 * @brief schedule_dmc_dma
 * @param dma_handler
 * @param source_address
 * @param count
 */
void schedule_dmc_dma(dma_handler_t dma_handler, uint_least16_t source_address, uint_least16_t count) {
	dmc_dma_handler_        = dma_handler;
	dmc_dma_source_address_ = source_address;
	dmc_dma_count_          = count * 2;
	dmc_dma_delay_          = 0;
}

/**
 * @brief cycle_count
 * @return
 */
uint64_t cycle_count() {
	return executed_cycles_;
}

}
