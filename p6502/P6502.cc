
#include "P6502.h"

#ifdef _MSC_VER
#pragma warning(disable : 4127)
#endif

#include <cassert>
#include <cstdio>
#include <cstdlib>

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

namespace P6502 {

// visible registers
uint16_t PC;
uint8_t  A;
uint8_t  X;
uint8_t  Y;
uint8_t  S;
uint8_t  P;

namespace {

void set_pc_lo(uint8_t value) {
	PC = (PC & 0xff00) | value;
}

void set_pc_hi(uint8_t value) {
	PC = (PC & 0x00ff) | (value << 8);
}

uint8_t pc_lo() {
	return PC & 0x00ff;
}

uint8_t pc_hi() {
	return ((PC >> 8) & 0x00ff);
}

enum {
	NMI_VECTOR_ADDRESS	= 0xfffa,
	RST_VECTOR_ADDRESS	= 0xfffc,
	IRQ_VECTOR_ADDRESS	= 0xfffe,
	STACK_ADDRESS		= 0x0100
};

jam_handler_t		jam_handler		= 0;
write_handler_t		write_handler	= 0;
read_handler_t		read_handler	= 0;
sync_handler_t      sync_handler    = 0;

#define LAST_CYCLE                                   \
do {                                                 \
	rst_executing = false;                           \
	nmi_executing = false;                           \
	irq_executing = false;                           \
													 \
	if(rst_asserted) {                               \
		rst_executing = true;                        \
	} else if(nmi_asserted) {                        \
		nmi_executing = true;                        \
	} else if(irq_asserted && ((P & I_MASK) == 0)) { \
		irq_executing = true;                        \
	}                                                \
	nmi_asserted = false;                            \
	rst_asserted = false;                            \
} while(0)

#define OPCODE_COMPLETE do { current_cycle = -1; return; } while(0)
#if 0
#define TRACE           do {             \
	switch(instruction) {                \
	case 0x100: printf("RST!\n"); break; \
	case 0x101: printf("NMI!\n"); break; \
	case 0x102: printf("IRQ!\n"); break; \
	default:                             \
		printf("%04X  [%02X] A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%3lu [%u]\n", (PC - 1) & 0xffff, instruction & 0xff, A, X, Y, P, S, executed_cycle_count, current_cycle); \
	}	                                 \
} while(0)
#else
#define TRACE           do { } while(0)
#endif
int current_cycle = 0;

// internal registers
bool     irq_executing        = false;
bool     nmi_executing        = false;
bool     rst_executing        = false;
bool     irq_asserted         = false;
bool     nmi_asserted         = false;
bool     rst_asserted         = false;
int      burn_count           = 0;
uint64_t executed_cycle_count = 0;
uint16_t instruction          = 0;

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
inline void update_nz_flags(uint8_t value) {

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

	// static so they preserve state between cycles
	static mode::absolute		  absolute_insn;
	static mode::absolute_x 	  absolute_x_insn;
	static mode::absolute_y 	  absolute_y_insn;
	static mode::immediate  	  immediate_insn;
	static mode::implied  	      implied_insn;
	static mode::indexed_indirect indexed_indirect_insn;
	static mode::indirect         indirect_insn;
	static mode::indirect_indexed indirect_indexed_insn;
	static mode::accumulator 	  accumulator_insn;
	static mode::relative         relative_insn;
	static mode::stack	          stack_insn;
	static mode::zero_page  	  zero_page_insn;
	static mode::zero_page_x	  zero_page_x_insn;
	static mode::zero_page_y	  zero_page_y_insn;
	static opcode_irq             op_irq;
	static opcode_nmi             op_nmi;
	static opcode_brk             op_brk;
	static opcode_rts             op_rts;
	static opcode_rti             op_rti;
	static opcode_jsr             op_jsr;
	static opcode_rst             op_rst;

	switch(instruction) {
	case 0x00: op_brk(current_cycle); break;
	case 0x01: indexed_indirect_insn(current_cycle, opcode_ora()); break;
	case 0x02: opcode_jam(); break;
	case 0x03: indexed_indirect_insn(current_cycle, opcode_slo()); break;
	case 0x04: zero_page_insn(current_cycle, opcode_nop()); break;
	case 0x05: zero_page_insn(current_cycle, opcode_ora()); break;
	case 0x06: zero_page_insn(current_cycle, opcode_asl()); break;
	case 0x07: zero_page_insn(current_cycle, opcode_slo()); break;
	case 0x08: stack_insn(current_cycle, opcode_php()); break;
	case 0x09: immediate_insn(current_cycle, opcode_ora()); break;
	case 0x0a: accumulator_insn(current_cycle, opcode_asl()); break;
	case 0x0b: immediate_insn(current_cycle, opcode_aac()); break;
	case 0x0c: absolute_insn(current_cycle, opcode_nop()); break;
	case 0x0d: absolute_insn(current_cycle, opcode_ora()); break;
	case 0x0e: absolute_insn(current_cycle, opcode_asl()); break;
	case 0x0f: absolute_insn(current_cycle, opcode_slo()); break;
	case 0x10: relative_insn(current_cycle, opcode_bpl()); break;
	case 0x11: indirect_indexed_insn(current_cycle, opcode_ora()); break;
	case 0x12: opcode_jam(); break;
	case 0x13: indirect_indexed_insn(current_cycle, opcode_slo()); break;
	case 0x14: zero_page_x_insn(current_cycle, opcode_nop()); break;
	case 0x15: zero_page_x_insn(current_cycle, opcode_ora()); break;
	case 0x16: zero_page_x_insn(current_cycle, opcode_asl()); break;
	case 0x17: zero_page_x_insn(current_cycle, opcode_slo()); break;
	case 0x18: implied_insn(current_cycle, opcode_clc()); break;
	case 0x19: absolute_y_insn(current_cycle, opcode_ora()); break;
	case 0x1a: implied_insn(current_cycle, opcode_nop()); break;
	case 0x1b: absolute_y_insn(current_cycle, opcode_slo()); break;
	case 0x1c: absolute_x_insn(current_cycle, opcode_nop()); break;
	case 0x1d: absolute_x_insn(current_cycle, opcode_ora()); break;
	case 0x1e: absolute_x_insn(current_cycle, opcode_asl()); break;
	case 0x1f: absolute_x_insn(current_cycle, opcode_slo()); break;
	case 0x20: op_jsr(current_cycle); break;
	case 0x21: indexed_indirect_insn(current_cycle, opcode_and()); break;
	case 0x22: opcode_jam(); break;
	case 0x23: indexed_indirect_insn(current_cycle, opcode_rla()); break;
	case 0x24: zero_page_insn(current_cycle, opcode_bit()); break;
	case 0x25: zero_page_insn(current_cycle, opcode_and()); break;
	case 0x26: zero_page_insn(current_cycle, opcode_rol()); break;
	case 0x27: zero_page_insn(current_cycle, opcode_rla()); break;
	case 0x28: stack_insn(current_cycle, opcode_plp()); break;
	case 0x29: immediate_insn(current_cycle, opcode_and()); break;
	case 0x2a: accumulator_insn(current_cycle, opcode_rol()); break;
	case 0x2b: immediate_insn(current_cycle, opcode_aac()); break;
	case 0x2c: absolute_insn(current_cycle, opcode_bit()); break;
	case 0x2d: absolute_insn(current_cycle, opcode_and()); break;
	case 0x2e: absolute_insn(current_cycle, opcode_rol()); break;
	case 0x2f: absolute_insn(current_cycle, opcode_rla()); break;
	case 0x30: relative_insn(current_cycle, opcode_bmi()); break;
	case 0x31: indirect_indexed_insn(current_cycle, opcode_and()); break;
	case 0x32: opcode_jam(); break;
	case 0x33: indirect_indexed_insn(current_cycle, opcode_rla()); break;
	case 0x34: zero_page_x_insn(current_cycle, opcode_nop()); break;
	case 0x35: zero_page_x_insn(current_cycle, opcode_and()); break;
	case 0x36: zero_page_x_insn(current_cycle, opcode_rol()); break;
	case 0x37: zero_page_x_insn(current_cycle, opcode_rla()); break;
	case 0x38: implied_insn(current_cycle, opcode_sec()); break;
	case 0x39: absolute_y_insn(current_cycle, opcode_and()); break;
	case 0x3a: implied_insn(current_cycle, opcode_nop()); break;
	case 0x3b: absolute_y_insn(current_cycle, opcode_rla()); break;
	case 0x3c: absolute_x_insn(current_cycle, opcode_nop()); break;
	case 0x3d: absolute_x_insn(current_cycle, opcode_and()); break;
	case 0x3e: absolute_x_insn(current_cycle, opcode_rol()); break;
	case 0x3f: absolute_x_insn(current_cycle, opcode_rla()); break;
	case 0x40: op_rti(current_cycle); break;
	case 0x41: indexed_indirect_insn(current_cycle, opcode_eor()); break;
	case 0x42: opcode_jam(); break;
	case 0x43: indexed_indirect_insn(current_cycle, opcode_sre()); break;
	case 0x44: zero_page_insn(current_cycle, opcode_nop()); break;
	case 0x45: zero_page_insn(current_cycle, opcode_eor()); break;
	case 0x46: zero_page_insn(current_cycle, opcode_lsr()); break;
	case 0x47: zero_page_insn(current_cycle, opcode_sre()); break;
	case 0x48: stack_insn(current_cycle, opcode_pha()); break;
	case 0x49: immediate_insn(current_cycle, opcode_eor()); break;
	case 0x4a: accumulator_insn(current_cycle, opcode_lsr()); break;
	case 0x4b: immediate_insn(current_cycle, opcode_asr()); break;
	case 0x4c: absolute_insn(current_cycle, opcode_jmp()); break;
	case 0x4d: absolute_insn(current_cycle, opcode_eor()); break;
	case 0x4e: absolute_insn(current_cycle, opcode_lsr()); break;
	case 0x4f: absolute_insn(current_cycle, opcode_sre()); break;
	case 0x50: relative_insn(current_cycle, opcode_bvc()); break;
	case 0x51: indirect_indexed_insn(current_cycle, opcode_eor()); break;
	case 0x52: opcode_jam(); break;
	case 0x53: indirect_indexed_insn(current_cycle, opcode_sre()); break;
	case 0x54: zero_page_x_insn(current_cycle, opcode_nop()); break;
	case 0x55: zero_page_x_insn(current_cycle, opcode_eor()); break;
	case 0x56: zero_page_x_insn(current_cycle, opcode_lsr()); break;
	case 0x57: zero_page_x_insn(current_cycle, opcode_sre()); break;
	case 0x58: implied_insn(current_cycle, opcode_cli()); break;
	case 0x59: absolute_y_insn(current_cycle, opcode_eor()); break;
	case 0x5a: implied_insn(current_cycle, opcode_nop()); break;
	case 0x5b: absolute_y_insn(current_cycle, opcode_sre()); break;
	case 0x5c: absolute_x_insn(current_cycle, opcode_nop()); break;
	case 0x5d: absolute_x_insn(current_cycle, opcode_eor()); break;
	case 0x5e: absolute_x_insn(current_cycle, opcode_lsr()); break;
	case 0x5f: absolute_x_insn(current_cycle, opcode_sre()); break;
	case 0x60: op_rts(current_cycle); break;
	case 0x61: indexed_indirect_insn(current_cycle, opcode_adc()); break;
	case 0x62: opcode_jam(); break;
	case 0x63: indexed_indirect_insn(current_cycle, opcode_rra()); break;
	case 0x64: zero_page_insn(current_cycle, opcode_nop()); break;
	case 0x65: zero_page_insn(current_cycle, opcode_adc()); break;
	case 0x66: zero_page_insn(current_cycle, opcode_ror()); break;
	case 0x67: zero_page_insn(current_cycle, opcode_rra()); break;
	case 0x68: stack_insn(current_cycle, opcode_pla()); break;
	case 0x69: immediate_insn(current_cycle, opcode_adc()); break;
	case 0x6a: accumulator_insn(current_cycle, opcode_ror()); break;
	case 0x6b: immediate_insn(current_cycle, opcode_arr()); break;
	case 0x6c: indirect_insn(current_cycle, opcode_jmp()); break;
	case 0x6d: absolute_insn(current_cycle, opcode_adc()); break;
	case 0x6e: absolute_insn(current_cycle, opcode_ror()); break;
	case 0x6f: absolute_insn(current_cycle, opcode_rra()); break;
	case 0x70: relative_insn(current_cycle, opcode_bvs()); break;
	case 0x71: indirect_indexed_insn(current_cycle, opcode_adc()); break;
	case 0x72: opcode_jam(); break;
	case 0x73: indirect_indexed_insn(current_cycle, opcode_rra()); break;
	case 0x74: zero_page_x_insn(current_cycle, opcode_nop()); break;
	case 0x75: zero_page_x_insn(current_cycle, opcode_adc()); break;
	case 0x76: zero_page_x_insn(current_cycle, opcode_ror()); break;
	case 0x77: zero_page_x_insn(current_cycle, opcode_rra()); break;
	case 0x78: implied_insn(current_cycle, opcode_sei()); break;
	case 0x79: absolute_y_insn(current_cycle, opcode_adc()); break;
	case 0x7a: implied_insn(current_cycle, opcode_nop()); break;
	case 0x7b: absolute_y_insn(current_cycle, opcode_rra()); break;
	case 0x7c: absolute_x_insn(current_cycle, opcode_nop()); break;
	case 0x7d: absolute_x_insn(current_cycle, opcode_adc()); break;
	case 0x7e: absolute_x_insn(current_cycle, opcode_ror()); break;
	case 0x7f: absolute_x_insn(current_cycle, opcode_rra()); break;
	case 0x80: immediate_insn(current_cycle, opcode_nop()); break;
	case 0x81: indexed_indirect_insn(current_cycle, opcode_sta()); break;
	case 0x82: immediate_insn(current_cycle, opcode_nop()); break;
	case 0x83: indexed_indirect_insn(current_cycle, opcode_aax()); break;
	case 0x84: zero_page_insn(current_cycle, opcode_sty()); break;
	case 0x85: zero_page_insn(current_cycle, opcode_sta()); break;
	case 0x86: zero_page_insn(current_cycle, opcode_stx()); break;
	case 0x87: zero_page_insn(current_cycle, opcode_aax()); break;
	case 0x88: implied_insn(current_cycle, opcode_dey()); break;
	case 0x89: immediate_insn(current_cycle, opcode_nop()); break;
	case 0x8a: implied_insn(current_cycle, opcode_txa()); break;
	case 0x8b: immediate_insn(current_cycle, opcode_xaa()); break;
	case 0x8c: absolute_insn(current_cycle, opcode_sty()); break;
	case 0x8d: absolute_insn(current_cycle, opcode_sta()); break;
	case 0x8e: absolute_insn(current_cycle, opcode_stx()); break;
	case 0x8f: absolute_insn(current_cycle, opcode_aax()); break;
	case 0x90: relative_insn(current_cycle, opcode_bcc()); break;
	case 0x91: indirect_indexed_insn(current_cycle, opcode_sta()); break;
	case 0x92: opcode_jam(); break;
	case 0x93: indirect_indexed_insn(current_cycle, opcode_axa()); break;
	case 0x94: zero_page_x_insn(current_cycle, opcode_sty()); break;
	case 0x95: zero_page_x_insn(current_cycle, opcode_sta()); break;
	case 0x96: zero_page_y_insn(current_cycle, opcode_stx()); break;
	case 0x97: zero_page_y_insn(current_cycle, opcode_aax()); break;
	case 0x98: implied_insn(current_cycle, opcode_tya()); break;
	case 0x99: absolute_y_insn(current_cycle, opcode_sta()); break;
	case 0x9a: implied_insn(current_cycle, opcode_txs()); break;
	case 0x9b: absolute_y_insn(current_cycle, opcode_xas()); break;
	case 0x9c: absolute_x_insn(current_cycle, opcode_sya()); break;
	case 0x9d: absolute_x_insn(current_cycle, opcode_sta()); break;
	case 0x9e: absolute_y_insn(current_cycle, opcode_sxa()); break;
	case 0x9f: absolute_y_insn(current_cycle, opcode_axa()); break;
	case 0xa0: immediate_insn(current_cycle, opcode_ldy()); break;
	case 0xa1: indexed_indirect_insn(current_cycle, opcode_lda()); break;
	case 0xa2: immediate_insn(current_cycle, opcode_ldx()); break;
	case 0xa3: indexed_indirect_insn(current_cycle, opcode_lax()); break;
	case 0xa4: zero_page_insn(current_cycle, opcode_ldy()); break;
	case 0xa5: zero_page_insn(current_cycle, opcode_lda()); break;
	case 0xa6: zero_page_insn(current_cycle, opcode_ldx()); break;
	case 0xa7: zero_page_insn(current_cycle, opcode_lax()); break;
	case 0xa8: implied_insn(current_cycle, opcode_tay()); break;
	case 0xa9: immediate_insn(current_cycle, opcode_lda()); break;
	case 0xaa: implied_insn(current_cycle, opcode_tax()); break;
	case 0xab: immediate_insn(current_cycle, opcode_lax()); break;
	case 0xac: absolute_insn(current_cycle, opcode_ldy()); break;
	case 0xad: absolute_insn(current_cycle, opcode_lda()); break;
	case 0xae: absolute_insn(current_cycle, opcode_ldx()); break;
	case 0xaf: absolute_insn(current_cycle, opcode_lax()); break;
	case 0xb0: relative_insn(current_cycle, opcode_bcs()); break;
	case 0xb1: indirect_indexed_insn(current_cycle, opcode_lda()); break;
	case 0xb2: opcode_jam(); break;
	case 0xb3: indirect_indexed_insn(current_cycle, opcode_lax()); break;
	case 0xb4: zero_page_x_insn(current_cycle, opcode_ldy()); break;
	case 0xb5: zero_page_x_insn(current_cycle, opcode_lda()); break;
	case 0xb6: zero_page_y_insn(current_cycle, opcode_ldx()); break;
	case 0xb7: zero_page_y_insn(current_cycle, opcode_lax()); break;
	case 0xb8: implied_insn(current_cycle, opcode_clv()); break;
	case 0xb9: absolute_y_insn(current_cycle, opcode_lda()); break;
	case 0xba: implied_insn(current_cycle, opcode_tsx()); break;
	case 0xbb: absolute_y_insn(current_cycle, opcode_lar()); break;
	case 0xbc: absolute_x_insn(current_cycle, opcode_ldy()); break;
	case 0xbd: absolute_x_insn(current_cycle, opcode_lda()); break;
	case 0xbe: absolute_y_insn(current_cycle, opcode_ldx()); break;
	case 0xbf: absolute_y_insn(current_cycle, opcode_lax()); break;
	case 0xc0: immediate_insn(current_cycle, opcode_cpy()); break;
	case 0xc1: indexed_indirect_insn(current_cycle, opcode_cmp()); break;
	case 0xc2: immediate_insn(current_cycle, opcode_nop()); break;
	case 0xc3: indexed_indirect_insn(current_cycle, opcode_dcp()); break;
	case 0xc4: zero_page_insn(current_cycle, opcode_cpy()); break;
	case 0xc5: zero_page_insn(current_cycle, opcode_cmp()); break;
	case 0xc6: zero_page_insn(current_cycle, opcode_dec()); break;
	case 0xc7: zero_page_insn(current_cycle, opcode_dcp()); break;
	case 0xc8: implied_insn(current_cycle, opcode_iny()); break;
	case 0xc9: immediate_insn(current_cycle, opcode_cmp()); break;
	case 0xca: implied_insn(current_cycle, opcode_dex()); break;
	case 0xcb: immediate_insn(current_cycle, opcode_axs()); break;
	case 0xcc: absolute_insn(current_cycle, opcode_cpy()); break;
	case 0xcd: absolute_insn(current_cycle, opcode_cmp()); break;
	case 0xce: absolute_insn(current_cycle, opcode_dec()); break;
	case 0xcf: absolute_insn(current_cycle, opcode_dcp()); break;
	case 0xd0: relative_insn(current_cycle, opcode_bne()); break;
	case 0xd1: indirect_indexed_insn(current_cycle, opcode_cmp()); break;
	case 0xd2: opcode_jam(); break;
	case 0xd3: indirect_indexed_insn(current_cycle, opcode_dcp()); break;
	case 0xd4: zero_page_x_insn(current_cycle, opcode_nop()); break;
	case 0xd5: zero_page_x_insn(current_cycle, opcode_cmp()); break;
	case 0xd6: zero_page_x_insn(current_cycle, opcode_dec()); break;
	case 0xd7: zero_page_x_insn(current_cycle, opcode_dcp()); break;
	case 0xd8: implied_insn(current_cycle, opcode_cld()); break;
	case 0xd9: absolute_y_insn(current_cycle, opcode_cmp()); break;
	case 0xda: implied_insn(current_cycle, opcode_nop()); break;
	case 0xdb: absolute_y_insn(current_cycle, opcode_dcp()); break;
	case 0xdc: absolute_x_insn(current_cycle, opcode_nop()); break;
	case 0xdd: absolute_x_insn(current_cycle, opcode_cmp()); break;
	case 0xde: absolute_x_insn(current_cycle, opcode_dec()); break;
	case 0xdf: absolute_x_insn(current_cycle, opcode_dcp()); break;
	case 0xe0: immediate_insn(current_cycle, opcode_cpx()); break;
	case 0xe1: indexed_indirect_insn(current_cycle, opcode_sbc()); break;
	case 0xe2: immediate_insn(current_cycle, opcode_nop()); break;
	case 0xe3: indexed_indirect_insn(current_cycle, opcode_isc()); break;
	case 0xe4: zero_page_insn(current_cycle, opcode_cpx()); break;
	case 0xe5: zero_page_insn(current_cycle, opcode_sbc()); break;
	case 0xe6: zero_page_insn(current_cycle, opcode_inc()); break;
	case 0xe7: zero_page_insn(current_cycle, opcode_isc()); break;
	case 0xe8: implied_insn(current_cycle, opcode_inx()); break;
	case 0xe9: immediate_insn(current_cycle, opcode_sbc()); break;
	case 0xea: implied_insn(current_cycle, opcode_nop()); break;
	case 0xeb: immediate_insn(current_cycle, opcode_sbc()); break;
	case 0xec: absolute_insn(current_cycle, opcode_cpx()); break;
	case 0xed: absolute_insn(current_cycle, opcode_sbc()); break;
	case 0xee: absolute_insn(current_cycle, opcode_inc()); break;
	case 0xef: absolute_insn(current_cycle, opcode_isc()); break;
	case 0xf0: relative_insn(current_cycle, opcode_beq()); break;
	case 0xf1: indirect_indexed_insn(current_cycle, opcode_sbc()); break;
	case 0xf2: opcode_jam(); break;
	case 0xf3: indirect_indexed_insn(current_cycle, opcode_isc()); break;
	case 0xf4: zero_page_x_insn(current_cycle, opcode_nop()); break;
	case 0xf5: zero_page_x_insn(current_cycle, opcode_sbc()); break;
	case 0xf6: zero_page_x_insn(current_cycle, opcode_inc()); break;
	case 0xf7: zero_page_x_insn(current_cycle, opcode_isc()); break;
	case 0xf8: implied_insn(current_cycle, opcode_sed()); break;
	case 0xf9: absolute_y_insn(current_cycle, opcode_sbc()); break;
	case 0xfa: implied_insn(current_cycle, opcode_nop()); break;
	case 0xfb: absolute_y_insn(current_cycle, opcode_isc()); break;
	case 0xfc: absolute_x_insn(current_cycle, opcode_nop()); break;
	case 0xfd: absolute_x_insn(current_cycle, opcode_sbc()); break;
	case 0xfe: absolute_x_insn(current_cycle, opcode_inc()); break;
	case 0xff: absolute_x_insn(current_cycle, opcode_isc()); break;

	// IRQ/NMI/RESET
	case 0x100: op_rst(current_cycle); break;
	case 0x101: op_nmi(current_cycle); break;
	case 0x102: op_irq(current_cycle); break;
	default:
		abort();
	}
}

//------------------------------------------------------------------------------
// Name: clock
// Desc: steps the emulation 1 cycle
//------------------------------------------------------------------------------
void clock() {

	assert(current_cycle < 10);

	if(current_cycle == 0) {

		// first cycle is always instruction fetch
		// or do we force an interrupt?
		if(rst_executing) {
			read_byte(PC);
			instruction = 0x100;
		} else if(nmi_executing) {
			read_byte(PC);
			instruction = 0x101;
		} else if(irq_executing) {
			read_byte(PC);
			instruction = 0x102;
		} else {
			instruction = read_byte(PC++);
		}
		
		TRACE;
		
	} else {
		// execute the current part of the instruction
		execute_opcode();
	}

	++current_cycle;
}

}

//------------------------------------------------------------------------------
// Name: run
// Desc:
//------------------------------------------------------------------------------
void run(int cycles) {

	while(cycles-- > 0) {

		if(burn_count != 0) {
			assert(burn_count >= 0);
			--burn_count;
		} else {
			clock();
		}

		if(sync_handler) {
			(*sync_handler)();
		}

		++executed_cycle_count;
	}
}

//------------------------------------------------------------------------------
// Name: irq
// Desc:
//------------------------------------------------------------------------------
void irq() {
	irq_asserted = true;
}

//------------------------------------------------------------------------------
// Name: nmi
// Desc:
//------------------------------------------------------------------------------
void nmi() {
	nmi_asserted = true;
}

//------------------------------------------------------------------------------
// Name: reset
// Desc:
//------------------------------------------------------------------------------
void reset() {
	if(!rst_executing) {
		rst_asserted = true;
		irq_asserted = false;
		nmi_asserted = false;
	}
}

//------------------------------------------------------------------------------
// Name: reset_irq
// Desc:
//------------------------------------------------------------------------------
void reset_irq() {
	irq_asserted = false;
}

//------------------------------------------------------------------------------
// Name: init
// Desc:
//------------------------------------------------------------------------------
void init(jam_handler_t jam, read_handler_t read, write_handler_t write, sync_handler_t sync) {

	// these two are required
	assert(read);
	assert(write);

	jam_handler   = jam;
	read_handler  = read;
	write_handler = write;
	sync_handler  = sync;

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

	executed_cycle_count = 0;
	irq_asserted         = false;
	nmi_asserted         = false;
	rst_asserted         = false;
	irq_executing        = false;
	nmi_executing        = false;
	rst_executing        = true;
	current_cycle        = 0;
}

//------------------------------------------------------------------------------
// Name: burn
// Desc:
//------------------------------------------------------------------------------
void burn(int cycles) {
	burn_count += cycles;
}

//------------------------------------------------------------------------------
// Name: executed_cycles
// Desc:
//------------------------------------------------------------------------------
uint64_t executed_cycles() {
	return executed_cycle_count;
}

}
