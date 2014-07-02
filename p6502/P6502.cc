
#include "P6502.h"

#ifdef _MSC_VER
#pragma warning(disable : 4127)
#endif

#include <cassert>
#include <cstdlib>

namespace P6502 {

// public registers
uint16_t PC;
uint8_t  A;
uint8_t  X;
uint8_t  Y;
uint8_t  S;
uint8_t  P;

// stats
uint64_t executed_cycles;

// handlers
jam_handler_t   jam_handler_;
write_handler_t write_handler_;
read_handler_t  read_handler_;
sync_handler_t  sync_handler_;

// internal registers
uint16_t instruction_;
uint16_t effective_address16_;
uint16_t data16_;
uint8_t  data8_;
int      cycle_;

bool     irq_executing_;
bool     nmi_executing_;
bool     rst_executing_;
bool     irq_asserted_;
bool     nmi_asserted_;
bool     rst_asserted_;

namespace {

enum {
	C_MASK = 0x01,
	Z_MASK = 0x02,
	I_MASK = 0x04,
	D_MASK = 0x08,
	B_MASK = 0x10,
	R_MASK = 0x20,	// antisocial flag... always 1
	V_MASK = 0x40,
	N_MASK = 0x80
};

const uint16_t NMI_VECTOR_ADDRESS = 0xfffa;
const uint16_t RST_VECTOR_ADDRESS = 0xfffc;
const uint16_t IRQ_VECTOR_ADDRESS = 0xfffe;
const uint16_t STACK_ADDRESS      = 0x0100;


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

//------------------------------------------------------------------------------
// Name: set_pc_lo
// Desc: 
//------------------------------------------------------------------------------
void set_pc_lo(uint8_t value) {
	PC = (PC & 0xff00) | value;
}

//------------------------------------------------------------------------------
// Name: set_pc_hi
// Desc: 
//------------------------------------------------------------------------------
void set_pc_hi(uint8_t value) {
	PC = (PC & 0x00ff) | (value << 8);
}

//------------------------------------------------------------------------------
// Name: pc_lo
// Desc: 
//------------------------------------------------------------------------------
uint8_t pc_lo() {
	return PC & 0x00ff;
}

//------------------------------------------------------------------------------
// Name: pc_hi
// Desc: 
//------------------------------------------------------------------------------
uint8_t pc_hi() {
	return ((PC >> 8) & 0x00ff);
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

	// static so they preserve state between cycles
	static mode::indirect         indirect_insn;
	static mode::relative         relative_insn;
	
	switch(instruction_) {
	case 0x00: opcode_brk()(); break;
	case 0x01: mode::indexed_indirect()(opcode_ora()); break;
	case 0x02: opcode_jam()(); break;
	case 0x03: mode::indexed_indirect()(opcode_slo()); break;
	case 0x04: mode::zero_page()(opcode_nop()); break;
	case 0x05: mode::zero_page()(opcode_ora()); break;
	case 0x06: mode::zero_page()(opcode_asl()); break;
	case 0x07: mode::zero_page()(opcode_slo()); break;
	case 0x08: mode::stack()(opcode_php()); break;
	case 0x09: mode::immediate()(opcode_ora()); break;
	case 0x0a: mode::accumulator()(opcode_asl()); break;
	case 0x0b: mode::immediate()(opcode_aac()); break;
	case 0x0c: mode::absolute()(opcode_nop()); break;
	case 0x0d: mode::absolute()(opcode_ora()); break;
	case 0x0e: mode::absolute()(opcode_asl()); break;
	case 0x0f: mode::absolute()(opcode_slo()); break;
	case 0x10: relative_insn(opcode_bpl()); break;
	case 0x11: mode::indirect_indexed()(opcode_ora()); break;
	case 0x12: opcode_jam()(); break;
	case 0x13: mode::indirect_indexed()(opcode_slo()); break;
	case 0x14: mode::zero_page_x()(opcode_nop()); break;
	case 0x15: mode::zero_page_x()(opcode_ora()); break;
	case 0x16: mode::zero_page_x()(opcode_asl()); break;
	case 0x17: mode::zero_page_x()(opcode_slo()); break;
	case 0x18: mode::implied()(opcode_clc()); break;
	case 0x19: mode::absolute_y()(opcode_ora()); break;
	case 0x1a: mode::implied()(opcode_nop()); break;
	case 0x1b: mode::absolute_y()(opcode_slo()); break;
	case 0x1c: mode::absolute_x()(opcode_nop()); break;
	case 0x1d: mode::absolute_x()(opcode_ora()); break;
	case 0x1e: mode::absolute_x()(opcode_asl()); break;
	case 0x1f: mode::absolute_x()(opcode_slo()); break;
	case 0x20: opcode_jsr()(); break;
	case 0x21: mode::indexed_indirect()(opcode_and()); break;
	case 0x22: opcode_jam()(); break;
	case 0x23: mode::indexed_indirect()(opcode_rla()); break;
	case 0x24: mode::zero_page()(opcode_bit()); break;
	case 0x25: mode::zero_page()(opcode_and()); break;
	case 0x26: mode::zero_page()(opcode_rol()); break;
	case 0x27: mode::zero_page()(opcode_rla()); break;
	case 0x28: mode::stack()(opcode_plp()); break;
	case 0x29: mode::immediate()(opcode_and()); break;
	case 0x2a: mode::accumulator()(opcode_rol()); break;
	case 0x2b: mode::immediate()(opcode_aac()); break;
	case 0x2c: mode::absolute()(opcode_bit()); break;
	case 0x2d: mode::absolute()(opcode_and()); break;
	case 0x2e: mode::absolute()(opcode_rol()); break;
	case 0x2f: mode::absolute()(opcode_rla()); break;
	case 0x30: relative_insn(opcode_bmi()); break;
	case 0x31: mode::indirect_indexed()(opcode_and()); break;
	case 0x32: opcode_jam()(); break;
	case 0x33: mode::indirect_indexed()(opcode_rla()); break;
	case 0x34: mode::zero_page_x()(opcode_nop()); break;
	case 0x35: mode::zero_page_x()(opcode_and()); break;
	case 0x36: mode::zero_page_x()(opcode_rol()); break;
	case 0x37: mode::zero_page_x()(opcode_rla()); break;
	case 0x38: mode::implied()(opcode_sec()); break;
	case 0x39: mode::absolute_y()(opcode_and()); break;
	case 0x3a: mode::implied()(opcode_nop()); break;
	case 0x3b: mode::absolute_y()(opcode_rla()); break;
	case 0x3c: mode::absolute_x()(opcode_nop()); break;
	case 0x3d: mode::absolute_x()(opcode_and()); break;
	case 0x3e: mode::absolute_x()(opcode_rol()); break;
	case 0x3f: mode::absolute_x()(opcode_rla()); break;
	case 0x40: opcode_rti()(); break;
	case 0x41: mode::indexed_indirect()(opcode_eor()); break;
	case 0x42: opcode_jam()(); break;
	case 0x43: mode::indexed_indirect()(opcode_sre()); break;
	case 0x44: mode::zero_page()(opcode_nop()); break;
	case 0x45: mode::zero_page()(opcode_eor()); break;
	case 0x46: mode::zero_page()(opcode_lsr()); break;
	case 0x47: mode::zero_page()(opcode_sre()); break;
	case 0x48: mode::stack()(opcode_pha()); break;
	case 0x49: mode::immediate()(opcode_eor()); break;
	case 0x4a: mode::accumulator()(opcode_lsr()); break;
	case 0x4b: mode::immediate()(opcode_asr()); break;
	case 0x4c: mode::absolute()(opcode_jmp()); break;
	case 0x4d: mode::absolute()(opcode_eor()); break;
	case 0x4e: mode::absolute()(opcode_lsr()); break;
	case 0x4f: mode::absolute()(opcode_sre()); break;
	case 0x50: relative_insn(opcode_bvc()); break;
	case 0x51: mode::indirect_indexed()(opcode_eor()); break;
	case 0x52: opcode_jam()(); break;
	case 0x53: mode::indirect_indexed()(opcode_sre()); break;
	case 0x54: mode::zero_page_x()(opcode_nop()); break;
	case 0x55: mode::zero_page_x()(opcode_eor()); break;
	case 0x56: mode::zero_page_x()(opcode_lsr()); break;
	case 0x57: mode::zero_page_x()(opcode_sre()); break;
	case 0x58: mode::implied()(opcode_cli()); break;
	case 0x59: mode::absolute_y()(opcode_eor()); break;
	case 0x5a: mode::implied()(opcode_nop()); break;
	case 0x5b: mode::absolute_y()(opcode_sre()); break;
	case 0x5c: mode::absolute_x()(opcode_nop()); break;
	case 0x5d: mode::absolute_x()(opcode_eor()); break;
	case 0x5e: mode::absolute_x()(opcode_lsr()); break;
	case 0x5f: mode::absolute_x()(opcode_sre()); break;
	case 0x60: opcode_rts()(); break;
	case 0x61: mode::indexed_indirect()(opcode_adc()); break;
	case 0x62: opcode_jam()(); break;
	case 0x63: mode::indexed_indirect()(opcode_rra()); break;
	case 0x64: mode::zero_page()(opcode_nop()); break;
	case 0x65: mode::zero_page()(opcode_adc()); break;
	case 0x66: mode::zero_page()(opcode_ror()); break;
	case 0x67: mode::zero_page()(opcode_rra()); break;
	case 0x68: mode::stack()(opcode_pla()); break;
	case 0x69: mode::immediate()(opcode_adc()); break;
	case 0x6a: mode::accumulator()(opcode_ror()); break;
	case 0x6b: mode::immediate()(opcode_arr()); break;
	case 0x6c: indirect_insn(opcode_jmp()); break;
	case 0x6d: mode::absolute()(opcode_adc()); break;
	case 0x6e: mode::absolute()(opcode_ror()); break;
	case 0x6f: mode::absolute()(opcode_rra()); break;
	case 0x70: relative_insn(opcode_bvs()); break;
	case 0x71: mode::indirect_indexed()(opcode_adc()); break;
	case 0x72: opcode_jam()(); break;
	case 0x73: mode::indirect_indexed()(opcode_rra()); break;
	case 0x74: mode::zero_page_x()(opcode_nop()); break;
	case 0x75: mode::zero_page_x()(opcode_adc()); break;
	case 0x76: mode::zero_page_x()(opcode_ror()); break;
	case 0x77: mode::zero_page_x()(opcode_rra()); break;
	case 0x78: mode::implied()(opcode_sei()); break;
	case 0x79: mode::absolute_y()(opcode_adc()); break;
	case 0x7a: mode::implied()(opcode_nop()); break;
	case 0x7b: mode::absolute_y()(opcode_rra()); break;
	case 0x7c: mode::absolute_x()(opcode_nop()); break;
	case 0x7d: mode::absolute_x()(opcode_adc()); break;
	case 0x7e: mode::absolute_x()(opcode_ror()); break;
	case 0x7f: mode::absolute_x()(opcode_rra()); break;
	case 0x80: mode::immediate()(opcode_nop()); break;
	case 0x81: mode::indexed_indirect()(opcode_sta()); break;
	case 0x82: mode::immediate()(opcode_nop()); break;
	case 0x83: mode::indexed_indirect()(opcode_aax()); break;
	case 0x84: mode::zero_page()(opcode_sty()); break;
	case 0x85: mode::zero_page()(opcode_sta()); break;
	case 0x86: mode::zero_page()(opcode_stx()); break;
	case 0x87: mode::zero_page()(opcode_aax()); break;
	case 0x88: mode::implied()(opcode_dey()); break;
	case 0x89: mode::immediate()(opcode_nop()); break;
	case 0x8a: mode::implied()(opcode_txa()); break;
	case 0x8b: mode::immediate()(opcode_xaa()); break;
	case 0x8c: mode::absolute()(opcode_sty()); break;
	case 0x8d: mode::absolute()(opcode_sta()); break;
	case 0x8e: mode::absolute()(opcode_stx()); break;
	case 0x8f: mode::absolute()(opcode_aax()); break;
	case 0x90: relative_insn(opcode_bcc()); break;
	case 0x91: mode::indirect_indexed()(opcode_sta()); break;
	case 0x92: opcode_jam()(); break;
	case 0x93: mode::indirect_indexed()(opcode_axa()); break;
	case 0x94: mode::zero_page_x()(opcode_sty()); break;
	case 0x95: mode::zero_page_x()(opcode_sta()); break;
	case 0x96: mode::zero_page_y()(opcode_stx()); break;
	case 0x97: mode::zero_page_y()(opcode_aax()); break;
	case 0x98: mode::implied()(opcode_tya()); break;
	case 0x99: mode::absolute_y()(opcode_sta()); break;
	case 0x9a: mode::implied()(opcode_txs()); break;
	case 0x9b: mode::absolute_y()(opcode_xas()); break;
	case 0x9c: mode::absolute_x()(opcode_sya()); break;
	case 0x9d: mode::absolute_x()(opcode_sta()); break;
	case 0x9e: mode::absolute_y()(opcode_sxa()); break;
	case 0x9f: mode::absolute_y()(opcode_axa()); break;
	case 0xa0: mode::immediate()(opcode_ldy()); break;
	case 0xa1: mode::indexed_indirect()(opcode_lda()); break;
	case 0xa2: mode::immediate()(opcode_ldx()); break;
	case 0xa3: mode::indexed_indirect()(opcode_lax()); break;
	case 0xa4: mode::zero_page()(opcode_ldy()); break;
	case 0xa5: mode::zero_page()(opcode_lda()); break;
	case 0xa6: mode::zero_page()(opcode_ldx()); break;
	case 0xa7: mode::zero_page()(opcode_lax()); break;
	case 0xa8: mode::implied()(opcode_tay()); break;
	case 0xa9: mode::immediate()(opcode_lda()); break;
	case 0xaa: mode::implied()(opcode_tax()); break;
	case 0xab: mode::immediate()(opcode_lax()); break;
	case 0xac: mode::absolute()(opcode_ldy()); break;
	case 0xad: mode::absolute()(opcode_lda()); break;
	case 0xae: mode::absolute()(opcode_ldx()); break;
	case 0xaf: mode::absolute()(opcode_lax()); break;
	case 0xb0: relative_insn(opcode_bcs()); break;
	case 0xb1: mode::indirect_indexed()(opcode_lda()); break;
	case 0xb2: opcode_jam()(); break;
	case 0xb3: mode::indirect_indexed()(opcode_lax()); break;
	case 0xb4: mode::zero_page_x()(opcode_ldy()); break;
	case 0xb5: mode::zero_page_x()(opcode_lda()); break;
	case 0xb6: mode::zero_page_y()(opcode_ldx()); break;
	case 0xb7: mode::zero_page_y()(opcode_lax()); break;
	case 0xb8: mode::implied()(opcode_clv()); break;
	case 0xb9: mode::absolute_y()(opcode_lda()); break;
	case 0xba: mode::implied()(opcode_tsx()); break;
	case 0xbb: mode::absolute_y()(opcode_lar()); break;
	case 0xbc: mode::absolute_x()(opcode_ldy()); break;
	case 0xbd: mode::absolute_x()(opcode_lda()); break;
	case 0xbe: mode::absolute_y()(opcode_ldx()); break;
	case 0xbf: mode::absolute_y()(opcode_lax()); break;
	case 0xc0: mode::immediate()(opcode_cpy()); break;
	case 0xc1: mode::indexed_indirect()(opcode_cmp()); break;
	case 0xc2: mode::immediate()(opcode_nop()); break;
	case 0xc3: mode::indexed_indirect()(opcode_dcp()); break;
	case 0xc4: mode::zero_page()(opcode_cpy()); break;
	case 0xc5: mode::zero_page()(opcode_cmp()); break;
	case 0xc6: mode::zero_page()(opcode_dec()); break;
	case 0xc7: mode::zero_page()(opcode_dcp()); break;
	case 0xc8: mode::implied()(opcode_iny()); break;
	case 0xc9: mode::immediate()(opcode_cmp()); break;
	case 0xca: mode::implied()(opcode_dex()); break;
	case 0xcb: mode::immediate()(opcode_axs()); break;
	case 0xcc: mode::absolute()(opcode_cpy()); break;
	case 0xcd: mode::absolute()(opcode_cmp()); break;
	case 0xce: mode::absolute()(opcode_dec()); break;
	case 0xcf: mode::absolute()(opcode_dcp()); break;
	case 0xd0: relative_insn(opcode_bne()); break;
	case 0xd1: mode::indirect_indexed()(opcode_cmp()); break;
	case 0xd2: opcode_jam()(); break;
	case 0xd3: mode::indirect_indexed()(opcode_dcp()); break;
	case 0xd4: mode::zero_page_x()(opcode_nop()); break;
	case 0xd5: mode::zero_page_x()(opcode_cmp()); break;
	case 0xd6: mode::zero_page_x()(opcode_dec()); break;
	case 0xd7: mode::zero_page_x()(opcode_dcp()); break;
	case 0xd8: mode::implied()(opcode_cld()); break;
	case 0xd9: mode::absolute_y()(opcode_cmp()); break;
	case 0xda: mode::implied()(opcode_nop()); break;
	case 0xdb: mode::absolute_y()(opcode_dcp()); break;
	case 0xdc: mode::absolute_x()(opcode_nop()); break;
	case 0xdd: mode::absolute_x()(opcode_cmp()); break;
	case 0xde: mode::absolute_x()(opcode_dec()); break;
	case 0xdf: mode::absolute_x()(opcode_dcp()); break;
	case 0xe0: mode::immediate()(opcode_cpx()); break;
	case 0xe1: mode::indexed_indirect()(opcode_sbc()); break;
	case 0xe2: mode::immediate()(opcode_nop()); break;
	case 0xe3: mode::indexed_indirect()(opcode_isc()); break;
	case 0xe4: mode::zero_page()(opcode_cpx()); break;
	case 0xe5: mode::zero_page()(opcode_sbc()); break;
	case 0xe6: mode::zero_page()(opcode_inc()); break;
	case 0xe7: mode::zero_page()(opcode_isc()); break;
	case 0xe8: mode::implied()(opcode_inx()); break;
	case 0xe9: mode::immediate()(opcode_sbc()); break;
	case 0xea: mode::implied()(opcode_nop()); break;
	case 0xeb: mode::immediate()(opcode_sbc()); break;
	case 0xec: mode::absolute()(opcode_cpx()); break;
	case 0xed: mode::absolute()(opcode_sbc()); break;
	case 0xee: mode::absolute()(opcode_inc()); break;
	case 0xef: mode::absolute()(opcode_isc()); break;
	case 0xf0: relative_insn(opcode_beq()); break;
	case 0xf1: mode::indirect_indexed()(opcode_sbc()); break;
	case 0xf2: opcode_jam()(); break;
	case 0xf3: mode::indirect_indexed()(opcode_isc()); break;
	case 0xf4: mode::zero_page_x()(opcode_nop()); break;
	case 0xf5: mode::zero_page_x()(opcode_sbc()); break;
	case 0xf6: mode::zero_page_x()(opcode_inc()); break;
	case 0xf7: mode::zero_page_x()(opcode_isc()); break;
	case 0xf8: mode::implied()(opcode_sed()); break;
	case 0xf9: mode::absolute_y()(opcode_sbc()); break;
	case 0xfa: mode::implied()(opcode_nop()); break;
	case 0xfb: mode::absolute_y()(opcode_isc()); break;
	case 0xfc: mode::absolute_x()(opcode_nop()); break;
	case 0xfd: mode::absolute_x()(opcode_sbc()); break;
	case 0xfe: mode::absolute_x()(opcode_inc()); break;
	case 0xff: mode::absolute_x()(opcode_isc()); break;

	// IRQ/NMI/RESET
	case 0x100: opcode_rst()(); break;
	case 0x101: opcode_nmi()(); break;
	case 0x102: opcode_irq()(); break;
	default:
		abort();
	}
}

//------------------------------------------------------------------------------
// Name: clock
// Desc: steps the emulation 1 cycle
//------------------------------------------------------------------------------
void clock() {

	assert(cycle_ < 10);
	
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
		if(cycle_ == 0) {

			// first cycle is always instruction fetch
			// or do we force an interrupt?
			if(rst_executing_) {
				read_byte(PC);
				instruction_ = 0x100;
			} else if(nmi_executing_) {
				read_byte(PC);
				instruction_ = 0x101;
			} else if(irq_executing_) {
				read_byte(PC);
				instruction_ = 0x102;
			} else {
				instruction_ = read_byte(PC++);
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
// Name: run
// Desc:
//------------------------------------------------------------------------------
void run(int cycles) {

	while(cycles-- > 0) {
		clock();

		if(sync_handler_) {
			(*sync_handler_)();
		}

		++executed_cycles;
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
// Name: init
// Desc:
//------------------------------------------------------------------------------
void init(jam_handler_t jam, read_handler_t read, write_handler_t write, sync_handler_t sync) {

	// these two are required
	assert(read);
	assert(write);

	jam_handler_   = jam;
	read_handler_  = read;
	write_handler_ = write;
	sync_handler_  = sync;

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
	
	assert(dma_handler_);
	
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
