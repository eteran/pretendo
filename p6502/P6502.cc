
#include "P6502.h"

#ifdef _MSC_VER
#pragma warning(disable : 4127)
#endif

#include <cassert>
#include <cstdlib>

namespace P6502 {

// public registers
register16 PC;
uint8_t    A;
uint8_t    X;
uint8_t    Y;
uint8_t    S;
uint8_t    P;

// stats
uint64_t executed_cycles;


// internal registers
uint16_t   instruction_;
int        cycle_;

// internal registers (which get trashed by instructions)
register16 effective_address16_;
register16 data16_;
register16 old_pc_;
register16 new_pc_;
uint8_t    data8_;

bool     irq_executing_;
bool     nmi_executing_;
bool     rst_executing_;
bool     irq_asserted_;
bool     nmi_asserted_;
bool     rst_asserted_;

namespace {

enum : uint8_t {
	C_MASK = 0x01,
	Z_MASK = 0x02,
	I_MASK = 0x04,
	D_MASK = 0x08,
	B_MASK = 0x10,
	R_MASK = 0x20,	// antisocial flag... always 1
	V_MASK = 0x40,
	N_MASK = 0x80
};

constexpr uint16_t NMI_VECTOR_ADDRESS = 0xfffa;
constexpr uint16_t RST_VECTOR_ADDRESS = 0xfffc;
constexpr uint16_t IRQ_VECTOR_ADDRESS = 0xfffe;
constexpr uint16_t STACK_ADDRESS      = 0x0100;


dma_handler_t spr_dma_handler_        = 0;
uint16_t      spr_dma_source_address_ = 0;
uint16_t      spr_dma_count_          = 0;
uint8_t       spr_dma_byte_           = 0;

dma_handler_t dmc_dma_handler_        = 0;
uint16_t      dmc_dma_source_address_ = 0;
uint16_t      dmc_dma_count_          = 0;
uint8_t       dmc_dma_byte_           = 0;


#define LAST_CYCLE                                    \
do {                                                  \
	rst_executing_ = false;                           \
	nmi_executing_ = false;                           \
	irq_executing_ = false;                           \
                                                      \
	if(rst_asserted_) {                               \
		rst_executing_ = true;                        \
	} else if(nmi_asserted_) {                        \
		nmi_executing_ = true;                        \
	} else if(irq_asserted_ && ((P & I_MASK) == 0)) { \
		irq_executing_ = true;                        \
	}                                                 \
	nmi_asserted_ = false;                            \
	rst_asserted_ = false;                            \
} while(0)

#define OPCODE_COMPLETE do { cycle_ = -1; return; } while(0)

const uint8_t flag_table[256] = {
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
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80
};


//------------------------------------------------------------------------------
// Name: set_flag
// Desc: sets the given flag to true
//------------------------------------------------------------------------------
template <uint8_t M>
inline void set_flag() {
	P |= M;
}

//------------------------------------------------------------------------------
// Name: clear_flag
// Desc: sets the given flag to false
//------------------------------------------------------------------------------
template <uint8_t M>
inline void clear_flag() {
	P &= ~M;
}

//------------------------------------------------------------------------------
// Name: set_flag_condition
// Desc: sets the given flag based on the given condition
//------------------------------------------------------------------------------
template <uint8_t M>
inline void set_flag_condition(bool cond) {
	if(cond) {
		set_flag<M>();
	} else {
		clear_flag<M>();
	}
}

//------------------------------------------------------------------------------
// Name: update_nz_flags
// Desc: sets the Negative and Zero flags based on the value given
//------------------------------------------------------------------------------
void update_nz_flags(uint8_t value) {

	// basically no bits set = 0x02
	// high bit set = 0x80
	// else = 0x00

	P &= ~(N_MASK | Z_MASK);
	P |= flag_table[value];
}

// opcode implementation
#include "memory.h"
#include "opcodes.h"
#include "address_modes.h"

//------------------------------------------------------------------------------
// Name: execute_opcode
// Desc:
//------------------------------------------------------------------------------
void execute_opcode() {

#if 1
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

#else
	switch(instruction_) {
	case 0x00: opcode_brk::execute(); break;
	case 0x01: indexed_indirect<opcode_ora>::execute(); break;
	case 0x02: opcode_jam::execute(); break;
	case 0x03: indexed_indirect<opcode_slo>::execute(); break;
	case 0x04: zero_page<opcode_nop>::execute(); break;
	case 0x05: zero_page<opcode_ora>::execute(); break;
	case 0x06: zero_page<opcode_asl>::execute(); break;
	case 0x07: zero_page<opcode_slo>::execute(); break;
	case 0x08: stack<opcode_php>::execute(); break;
	case 0x09: immediate<opcode_ora>::execute(); break;
	case 0x0a: accumulator<opcode_asl>::execute(); break;
	case 0x0b: immediate<opcode_aac>::execute(); break;
	case 0x0c: absolute<opcode_nop>::execute(); break;
	case 0x0d: absolute<opcode_ora>::execute(); break;
	case 0x0e: absolute<opcode_asl>::execute(); break;
	case 0x0f: absolute<opcode_slo>::execute(); break;
	case 0x10: relative<opcode_bpl>::execute(); break;
	case 0x11: indirect_indexed<opcode_ora>::execute(); break;
	case 0x12: opcode_jam::execute(); break;
	case 0x13: indirect_indexed<opcode_slo>::execute(); break;
	case 0x14: zero_page_x<opcode_nop>::execute(); break;
	case 0x15: zero_page_x<opcode_ora>::execute(); break;
	case 0x16: zero_page_x<opcode_asl>::execute(); break;
	case 0x17: zero_page_x<opcode_slo>::execute(); break;
	case 0x18: implied<opcode_clc>::execute(); break;
	case 0x19: absolute_y<opcode_ora>::execute(); break;
	case 0x1a: implied<opcode_nop>::execute(); break;
	case 0x1b: absolute_y<opcode_slo>::execute(); break;
	case 0x1c: absolute_x<opcode_nop>::execute(); break;
	case 0x1d: absolute_x<opcode_ora>::execute(); break;
	case 0x1e: absolute_x<opcode_asl>::execute(); break;
	case 0x1f: absolute_x<opcode_slo>::execute(); break;
	case 0x20: opcode_jsr::execute(); break;
	case 0x21: indexed_indirect<opcode_and>::execute(); break;
	case 0x22: opcode_jam::execute(); break;
	case 0x23: indexed_indirect<opcode_rla>::execute(); break;
	case 0x24: zero_page<opcode_bit>::execute(); break;
	case 0x25: zero_page<opcode_and>::execute(); break;
	case 0x26: zero_page<opcode_rol>::execute(); break;
	case 0x27: zero_page<opcode_rla>::execute(); break;
	case 0x28: stack<opcode_plp>::execute(); break;
	case 0x29: immediate<opcode_and>::execute(); break;
	case 0x2a: accumulator<opcode_rol>::execute(); break;
	case 0x2b: immediate<opcode_aac>::execute(); break;
	case 0x2c: absolute<opcode_bit>::execute(); break;
	case 0x2d: absolute<opcode_and>::execute(); break;
	case 0x2e: absolute<opcode_rol>::execute(); break;
	case 0x2f: absolute<opcode_rla>::execute(); break;
	case 0x30: relative<opcode_bmi>::execute(); break;
	case 0x31: indirect_indexed<opcode_and>::execute(); break;
	case 0x32: opcode_jam::execute(); break;
	case 0x33: indirect_indexed<opcode_rla>::execute(); break;
	case 0x34: zero_page_x<opcode_nop>::execute(); break;
	case 0x35: zero_page_x<opcode_and>::execute(); break;
	case 0x36: zero_page_x<opcode_rol>::execute(); break;
	case 0x37: zero_page_x<opcode_rla>::execute(); break;
	case 0x38: implied<opcode_sec>::execute(); break;
	case 0x39: absolute_y<opcode_and>::execute(); break;
	case 0x3a: implied<opcode_nop>::execute(); break;
	case 0x3b: absolute_y<opcode_rla>::execute(); break;
	case 0x3c: absolute_x<opcode_nop>::execute(); break;
	case 0x3d: absolute_x<opcode_and>::execute(); break;
	case 0x3e: absolute_x<opcode_rol>::execute(); break;
	case 0x3f: absolute_x<opcode_rla>::execute(); break;
	case 0x40: opcode_rti::execute(); break;
	case 0x41: indexed_indirect<opcode_eor>::execute(); break;
	case 0x42: opcode_jam::execute(); break;
	case 0x43: indexed_indirect<opcode_sre>::execute(); break;
	case 0x44: zero_page<opcode_nop>::execute(); break;
	case 0x45: zero_page<opcode_eor>::execute(); break;
	case 0x46: zero_page<opcode_lsr>::execute(); break;
	case 0x47: zero_page<opcode_sre>::execute(); break;
	case 0x48: stack<opcode_pha>::execute(); break;
	case 0x49: immediate<opcode_eor>::execute(); break;
	case 0x4a: accumulator<opcode_lsr>::execute(); break;
	case 0x4b: immediate<opcode_asr>::execute(); break;
	case 0x4c: absolute<opcode_jmp>::execute(); break;
	case 0x4d: absolute<opcode_eor>::execute(); break;
	case 0x4e: absolute<opcode_lsr>::execute(); break;
	case 0x4f: absolute<opcode_sre>::execute(); break;
	case 0x50: relative<opcode_bvc>::execute(); break;
	case 0x51: indirect_indexed<opcode_eor>::execute(); break;
	case 0x52: opcode_jam::execute(); break;
	case 0x53: indirect_indexed<opcode_sre>::execute(); break;
	case 0x54: zero_page_x<opcode_nop>::execute(); break;
	case 0x55: zero_page_x<opcode_eor>::execute(); break;
	case 0x56: zero_page_x<opcode_lsr>::execute(); break;
	case 0x57: zero_page_x<opcode_sre>::execute(); break;
	case 0x58: implied<opcode_cli>::execute(); break;
	case 0x59: absolute_y<opcode_eor>::execute(); break;
	case 0x5a: implied<opcode_nop>::execute(); break;
	case 0x5b: absolute_y<opcode_sre>::execute(); break;
	case 0x5c: absolute_x<opcode_nop>::execute(); break;
	case 0x5d: absolute_x<opcode_eor>::execute(); break;
	case 0x5e: absolute_x<opcode_lsr>::execute(); break;
	case 0x5f: absolute_x<opcode_sre>::execute(); break;
	case 0x60: opcode_rts::execute(); break;
	case 0x61: indexed_indirect<opcode_adc>::execute(); break;
	case 0x62: opcode_jam::execute(); break;
	case 0x63: indexed_indirect<opcode_rra>::execute(); break;
	case 0x64: zero_page<opcode_nop>::execute(); break;
	case 0x65: zero_page<opcode_adc>::execute(); break;
	case 0x66: zero_page<opcode_ror>::execute(); break;
	case 0x67: zero_page<opcode_rra>::execute(); break;
	case 0x68: stack<opcode_pla>::execute(); break;
	case 0x69: immediate<opcode_adc>::execute(); break;
	case 0x6a: accumulator<opcode_ror>::execute(); break;
	case 0x6b: immediate<opcode_arr>::execute(); break;
	case 0x6c: indirect<opcode_jmp>::execute(); break;
	case 0x6d: absolute<opcode_adc>::execute(); break;
	case 0x6e: absolute<opcode_ror>::execute(); break;
	case 0x6f: absolute<opcode_rra>::execute(); break;
	case 0x70: relative<opcode_bvs>::execute(); break;
	case 0x71: indirect_indexed<opcode_adc>::execute(); break;
	case 0x72: opcode_jam::execute(); break;
	case 0x73: indirect_indexed<opcode_rra>::execute(); break;
	case 0x74: zero_page_x<opcode_nop>::execute(); break;
	case 0x75: zero_page_x<opcode_adc>::execute(); break;
	case 0x76: zero_page_x<opcode_ror>::execute(); break;
	case 0x77: zero_page_x<opcode_rra>::execute(); break;
	case 0x78: implied<opcode_sei>::execute(); break;
	case 0x79: absolute_y<opcode_adc>::execute(); break;
	case 0x7a: implied<opcode_nop>::execute(); break;
	case 0x7b: absolute_y<opcode_rra>::execute(); break;
	case 0x7c: absolute_x<opcode_nop>::execute(); break;
	case 0x7d: absolute_x<opcode_adc>::execute(); break;
	case 0x7e: absolute_x<opcode_ror>::execute(); break;
	case 0x7f: absolute_x<opcode_rra>::execute(); break;
	case 0x80: immediate<opcode_nop>::execute(); break;
	case 0x81: indexed_indirect<opcode_sta>::execute(); break;
	case 0x82: immediate<opcode_nop>::execute(); break;
	case 0x83: indexed_indirect<opcode_aax>::execute(); break;
	case 0x84: zero_page<opcode_sty>::execute(); break;
	case 0x85: zero_page<opcode_sta>::execute(); break;
	case 0x86: zero_page<opcode_stx>::execute(); break;
	case 0x87: zero_page<opcode_aax>::execute(); break;
	case 0x88: implied<opcode_dey>::execute(); break;
	case 0x89: immediate<opcode_nop>::execute(); break;
	case 0x8a: implied<opcode_txa>::execute(); break;
	case 0x8b: immediate<opcode_xaa>::execute(); break;
	case 0x8c: absolute<opcode_sty>::execute(); break;
	case 0x8d: absolute<opcode_sta>::execute(); break;
	case 0x8e: absolute<opcode_stx>::execute(); break;
	case 0x8f: absolute<opcode_aax>::execute(); break;
	case 0x90: relative<opcode_bcc>::execute(); break;
	case 0x91: indirect_indexed<opcode_sta>::execute(); break;
	case 0x92: opcode_jam::execute(); break;
	case 0x93: indirect_indexed<opcode_axa>::execute(); break;
	case 0x94: zero_page_x<opcode_sty>::execute(); break;
	case 0x95: zero_page_x<opcode_sta>::execute(); break;
	case 0x96: zero_page_y<opcode_stx>::execute(); break;
	case 0x97: zero_page_y<opcode_aax>::execute(); break;
	case 0x98: implied<opcode_tya>::execute(); break;
	case 0x99: absolute_y<opcode_sta>::execute(); break;
	case 0x9a: implied<opcode_txs>::execute(); break;
	case 0x9b: absolute_y<opcode_xas>::execute(); break;
	case 0x9c: absolute_x<opcode_sya>::execute(); break;
	case 0x9d: absolute_x<opcode_sta>::execute(); break;
	case 0x9e: absolute_y<opcode_sxa>::execute(); break;
	case 0x9f: absolute_y<opcode_axa>::execute(); break;
	case 0xa0: immediate<opcode_ldy>::execute(); break;
	case 0xa1: indexed_indirect<opcode_lda>::execute(); break;
	case 0xa2: immediate<opcode_ldx>::execute(); break;
	case 0xa3: indexed_indirect<opcode_lax>::execute(); break;
	case 0xa4: zero_page<opcode_ldy>::execute(); break;
	case 0xa5: zero_page<opcode_lda>::execute(); break;
	case 0xa6: zero_page<opcode_ldx>::execute(); break;
	case 0xa7: zero_page<opcode_lax>::execute(); break;
	case 0xa8: implied<opcode_tay>::execute(); break;
	case 0xa9: immediate<opcode_lda>::execute(); break;
	case 0xaa: implied<opcode_tax>::execute(); break;
	case 0xab: immediate<opcode_lax>::execute(); break;
	case 0xac: absolute<opcode_ldy>::execute(); break;
	case 0xad: absolute<opcode_lda>::execute(); break;
	case 0xae: absolute<opcode_ldx>::execute(); break;
	case 0xaf: absolute<opcode_lax>::execute(); break;
	case 0xb0: relative<opcode_bcs>::execute(); break;
	case 0xb1: indirect_indexed<opcode_lda>::execute(); break;
	case 0xb2: opcode_jam::execute(); break;
	case 0xb3: indirect_indexed<opcode_lax>::execute(); break;
	case 0xb4: zero_page_x<opcode_ldy>::execute(); break;
	case 0xb5: zero_page_x<opcode_lda>::execute(); break;
	case 0xb6: zero_page_y<opcode_ldx>::execute(); break;
	case 0xb7: zero_page_y<opcode_lax>::execute(); break;
	case 0xb8: implied<opcode_clv>::execute(); break;
	case 0xb9: absolute_y<opcode_lda>::execute(); break;
	case 0xba: implied<opcode_tsx>::execute(); break;
	case 0xbb: absolute_y<opcode_lar>::execute(); break;
	case 0xbc: absolute_x<opcode_ldy>::execute(); break;
	case 0xbd: absolute_x<opcode_lda>::execute(); break;
	case 0xbe: absolute_y<opcode_ldx>::execute(); break;
	case 0xbf: absolute_y<opcode_lax>::execute(); break;
	case 0xc0: immediate<opcode_cpy>::execute(); break;
	case 0xc1: indexed_indirect<opcode_cmp>::execute(); break;
	case 0xc2: immediate<opcode_nop>::execute(); break;
	case 0xc3: indexed_indirect<opcode_dcp>::execute(); break;
	case 0xc4: zero_page<opcode_cpy>::execute(); break;
	case 0xc5: zero_page<opcode_cmp>::execute(); break;
	case 0xc6: zero_page<opcode_dec>::execute(); break;
	case 0xc7: zero_page<opcode_dcp>::execute(); break;
	case 0xc8: implied<opcode_iny>::execute(); break;
	case 0xc9: immediate<opcode_cmp>::execute(); break;
	case 0xca: implied<opcode_dex>::execute(); break;
	case 0xcb: immediate<opcode_axs>::execute(); break;
	case 0xcc: absolute<opcode_cpy>::execute(); break;
	case 0xcd: absolute<opcode_cmp>::execute(); break;
	case 0xce: absolute<opcode_dec>::execute(); break;
	case 0xcf: absolute<opcode_dcp>::execute(); break;
	case 0xd0: relative<opcode_bne>::execute(); break;
	case 0xd1: indirect_indexed<opcode_cmp>::execute(); break;
	case 0xd2: opcode_jam::execute(); break;
	case 0xd3: indirect_indexed<opcode_dcp>::execute(); break;
	case 0xd4: zero_page_x<opcode_nop>::execute(); break;
	case 0xd5: zero_page_x<opcode_cmp>::execute(); break;
	case 0xd6: zero_page_x<opcode_dec>::execute(); break;
	case 0xd7: zero_page_x<opcode_dcp>::execute(); break;
	case 0xd8: implied<opcode_cld>::execute(); break;
	case 0xd9: absolute_y<opcode_cmp>::execute(); break;
	case 0xda: implied<opcode_nop>::execute(); break;
	case 0xdb: absolute_y<opcode_dcp>::execute(); break;
	case 0xdc: absolute_x<opcode_nop>::execute(); break;
	case 0xdd: absolute_x<opcode_cmp>::execute(); break;
	case 0xde: absolute_x<opcode_dec>::execute(); break;
	case 0xdf: absolute_x<opcode_dcp>::execute(); break;
	case 0xe0: immediate<opcode_cpx>::execute(); break;
	case 0xe1: indexed_indirect<opcode_sbc>::execute(); break;
	case 0xe2: immediate<opcode_nop>::execute(); break;
	case 0xe3: indexed_indirect<opcode_isc>::execute(); break;
	case 0xe4: zero_page<opcode_cpx>::execute(); break;
	case 0xe5: zero_page<opcode_sbc>::execute(); break;
	case 0xe6: zero_page<opcode_inc>::execute(); break;
	case 0xe7: zero_page<opcode_isc>::execute(); break;
	case 0xe8: implied<opcode_inx>::execute(); break;
	case 0xe9: immediate<opcode_sbc>::execute(); break;
	case 0xea: implied<opcode_nop>::execute(); break;
	case 0xeb: immediate<opcode_sbc>::execute(); break;
	case 0xec: absolute<opcode_cpx>::execute(); break;
	case 0xed: absolute<opcode_sbc>::execute(); break;
	case 0xee: absolute<opcode_inc>::execute(); break;
	case 0xef: absolute<opcode_isc>::execute(); break;
	case 0xf0: relative<opcode_beq>::execute(); break;
	case 0xf1: indirect_indexed<opcode_sbc>::execute(); break;
	case 0xf2: opcode_jam::execute(); break;
	case 0xf3: indirect_indexed<opcode_isc>::execute(); break;
	case 0xf4: zero_page_x<opcode_nop>::execute(); break;
	case 0xf5: zero_page_x<opcode_sbc>::execute(); break;
	case 0xf6: zero_page_x<opcode_inc>::execute(); break;
	case 0xf7: zero_page_x<opcode_isc>::execute(); break;
	case 0xf8: implied<opcode_sed>::execute(); break;
	case 0xf9: absolute_y<opcode_sbc>::execute(); break;
	case 0xfa: implied<opcode_nop>::execute(); break;
	case 0xfb: absolute_y<opcode_isc>::execute(); break;
	case 0xfc: absolute_x<opcode_nop>::execute(); break;
	case 0xfd: absolute_x<opcode_sbc>::execute(); break;
	case 0xfe: absolute_x<opcode_inc>::execute(); break;
	case 0xff: absolute_x<opcode_isc>::execute(); break;

	// IRQ/NMI/RESET
	case 0x100: opcode_rst::execute(); break;
	case 0x101: opcode_nmi::execute(); break;
	case 0x102: opcode_irq::execute(); break;
	default:
		abort();
	}
#endif
}

//------------------------------------------------------------------------------
// Name: clock
// Desc: steps the emulation 1 cycle
//------------------------------------------------------------------------------
void clock() {

	if(dmc_dma_count_) {

		// the count will always be initially even (we multiply by 2)
		// so, this forces us to idle for 1 cycle if
		// the CPU starts DMA on an odd cycle
		// after that, they should stay in sync
		if((dmc_dma_count_ & 1) == (executed_cycles & 1)) {
			if((dmc_dma_count_ & 1) == 0) {
				// read cycle
				dmc_dma_byte_ = read_byte(dmc_dma_source_address_++);
			} else {
				// write cycle
				(*dmc_dma_handler_)(dmc_dma_byte_);
			}

			--dmc_dma_count_;
		}
		
	} else if(spr_dma_count_) {

		// the count will always be initially even (we multiply by 2)
		// so, this forces us to idle for 1 cycle if
		// the CPU starts DMA on an odd cycle
		// after that, they should stay in sync
		if((spr_dma_count_ & 1) == (executed_cycles & 1)) {
			if((executed_cycles & 1) == 0) {
				// read cycle
				spr_dma_byte_ = read_byte(spr_dma_source_address_++);
			} else {
				// write cycle
				(*spr_dma_handler_)(spr_dma_byte_);
			}

			--spr_dma_count_;
		}
		
	} else {
		assert(cycle_ < 10);
		
		if(cycle_ == 0) {

			// first cycle is always instruction fetch
			// or do we force an interrupt?
			if(rst_executing_) {
				read_byte(PC.raw);
				instruction_ = 0x100;
			} else if(nmi_executing_) {
				read_byte(PC.raw);
				instruction_ = 0x101;
			} else if(irq_executing_) {
				read_byte(PC.raw);
				instruction_ = 0x102;
			} else {
				instruction_ = read_byte(PC.raw++);
			}

		} else {
			// execute the current part of the instruction
			execute_opcode();
		}

		++cycle_;
	}
}

}

//------------------------------------------------------------------------------
// Name: tick
// Desc:
//------------------------------------------------------------------------------
void tick() {
	clock();
	sync_handler();
	++executed_cycles;
}

//------------------------------------------------------------------------------
// Name: run
// Desc:
//------------------------------------------------------------------------------
void run(int cycles) {

	while(cycles-- > 0) {
		tick();
	}
}

//------------------------------------------------------------------------------
// Name: irq
// Desc:
//------------------------------------------------------------------------------
void irq() {
	irq_asserted_ = true;
}

//------------------------------------------------------------------------------
// Name: nmi
// Desc:
//------------------------------------------------------------------------------
void nmi() {
	nmi_asserted_ = true;
}

//------------------------------------------------------------------------------
// Name: reset
// Desc:
//------------------------------------------------------------------------------
void reset() {
	if(!rst_executing_) {
		rst_asserted_ = true;
		irq_asserted_ = false;
		nmi_asserted_ = false;
	}
}

//------------------------------------------------------------------------------
// Name: reset_irq
// Desc:
//------------------------------------------------------------------------------
void reset_irq() {
	irq_asserted_ = false;
}

//------------------------------------------------------------------------------
// Name: reset_nmi
// Desc:
//------------------------------------------------------------------------------
void reset_nmi() {
	nmi_asserted_ = false;
}

//------------------------------------------------------------------------------
// Name: init
// Desc:
//------------------------------------------------------------------------------
void init() {
	stop();
}

//------------------------------------------------------------------------------
// Name: stop
// Desc:
//------------------------------------------------------------------------------
void stop() {

	A = 0;
	X = 0;
	Y = 0;
	S = 0;
	P = I_MASK | R_MASK;

	executed_cycles  = 0;
	irq_asserted_          = false;
	nmi_asserted_          = false;
	rst_asserted_          = false;
	irq_executing_         = false;
	nmi_executing_         = false;
	rst_executing_         = true;
	cycle_                 = 0;
		
	spr_dma_handler_ 	    = nullptr;
	spr_dma_source_address_ = 0;
	spr_dma_count_		    = 0;

	dmc_dma_handler_ 	    = nullptr;
	dmc_dma_source_address_ = 0;
	dmc_dma_count_		    = 0;
}

//------------------------------------------------------------------------------
// Name: schedule_dma
// Desc:
//------------------------------------------------------------------------------
void schedule_dma(dma_handler_t dma_handler, uint16_t source_address, uint16_t count, DMA_SOURCE source) {
	
	assert(dma_handler);
	
	switch(source) {
	case SPR_DMA:
		spr_dma_handler_        = dma_handler;
		spr_dma_source_address_ = source_address;
		spr_dma_count_          = count * 2;
		break;
	case DMC_DMA:
		dmc_dma_handler_        = dma_handler;
		dmc_dma_source_address_ = source_address;
		dmc_dma_count_          = count * 2;
		break;
	}
}

}
