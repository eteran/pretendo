
#ifndef OPCODES_20121204_H_
#define OPCODES_20121204_H_

struct operation_type {};
struct operation_none        : operation_type {};
struct operation_read        : operation_type {};
struct operation_modify      : operation_type {};
struct operation_write       : operation_type {};
struct operation_stack_read  : operation_type {};
struct operation_stack_write : operation_type {};
struct operation_jump        : operation_type {};
struct operation_branch      : operation_type {};

//------------------------------------------------------------------------------
// Name: opcode_jam
// Desc: stall the CPU
//------------------------------------------------------------------------------
void opcode_jam() {
	// make sure we spin forever
	--PC;
	if(jam_handler) {
		(*jam_handler)();
	}
	OPCODE_COMPLETE;
}

#include "opcodes/adc.h"
#include "opcodes/and.h"
#include "opcodes/asl.h"
#include "opcodes/bcc.h"
#include "opcodes/bcs.h"
#include "opcodes/beq.h"
#include "opcodes/bit.h"
#include "opcodes/bmi.h"
#include "opcodes/bne.h"
#include "opcodes/bpl.h"
#include "opcodes/brk.h"
#include "opcodes/bvc.h"
#include "opcodes/bvs.h"
#include "opcodes/clc.h"
#include "opcodes/cld.h"
#include "opcodes/cli.h"
#include "opcodes/clv.h"
#include "opcodes/cmp.h"
#include "opcodes/cpx.h"
#include "opcodes/cpy.h"
#include "opcodes/dec.h"
#include "opcodes/dex.h"
#include "opcodes/dey.h"
#include "opcodes/eor.h"
#include "opcodes/inc.h"
#include "opcodes/inx.h"
#include "opcodes/iny.h"
#include "opcodes/jmp.h"
#include "opcodes/jsr.h"
#include "opcodes/lda.h"
#include "opcodes/ldx.h"
#include "opcodes/ldy.h"
#include "opcodes/lsr.h"
#include "opcodes/nop.h"
#include "opcodes/ora.h"
#include "opcodes/pha.h"
#include "opcodes/php.h"
#include "opcodes/pla.h"
#include "opcodes/plp.h"
#include "opcodes/rol.h"
#include "opcodes/ror.h"
#include "opcodes/rti.h"
#include "opcodes/rts.h"
#include "opcodes/sbc.h"
#include "opcodes/sec.h"
#include "opcodes/sed.h"
#include "opcodes/sei.h"
#include "opcodes/sta.h"
#include "opcodes/stx.h"
#include "opcodes/sty.h"
#include "opcodes/tax.h"
#include "opcodes/tay.h"
#include "opcodes/tsx.h"
#include "opcodes/txa.h"
#include "opcodes/txs.h"
#include "opcodes/tya.h"

// special
#include "opcodes/special/irq.h"
#include "opcodes/special/nmi.h"
#include "opcodes/special/brk.h"
#include "opcodes/special/jsr.h"
#include "opcodes/special/rti.h"
#include "opcodes/special/rts.h"

// unofficial
#include "opcodes/unofficial/asr.h"
#include "opcodes/unofficial/aac.h"
#include "opcodes/unofficial/arr.h"
#include "opcodes/unofficial/axa.h"
#include "opcodes/unofficial/axs.h"
#include "opcodes/unofficial/dcp.h"
#include "opcodes/unofficial/isc.h"
#include "opcodes/unofficial/lar.h"
#include "opcodes/unofficial/lax.h"
#include "opcodes/unofficial/rla.h"
#include "opcodes/unofficial/rra.h"
#include "opcodes/unofficial/aax.h"
#include "opcodes/unofficial/slo.h"
#include "opcodes/unofficial/sre.h"
#include "opcodes/unofficial/sxa.h"
#include "opcodes/unofficial/sya.h"
#include "opcodes/unofficial/xaa.h"
#include "opcodes/unofficial/xas.h"

#endif
