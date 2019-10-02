; Times all documented and undocumented 6502 instructions except the
; 8 branch instructions (Bxx) and the 12 halt instructions (HLT).
; See readme.txt for operation and overview of algorithm used.

counter     = $10       ; number of iterations of test loop
nmi_flag    = $12       ; tells NMI what to do
test_index  = $13       ; opcode being tested
test_mode   = $14       ; 0 = normal, $FE = page crossing
type_mask   = $15       ; which subset of instructions to test
test_code   = $200      ; test loop is copied here
test_code_page = $02

.org $202
      irq:
      .code

reset:
      ; Standard NES init
      sei
      lda   #0
      sta   $2000
      sta   $2001
      jsr   wait_vbl
      jsr   wait_vbl
      cld
      
      jsr   init_console
      lda   #$A5        ; tell runtime that console is inited
      sta   $7F1
      
      ; Print title
      ldx   #title_str.msb
      ldy   #title_str.lsb
      jsr   print_str
      jsr   console_newline
      jsr   console_newline
      
      ; Poll joypad to determine which tests to run
      lda   #1
      sta   $4016
      lda   #0
      sta   $4016
      lda   $4016       ; A button status
      and   #$01
      beq   +
      ldx   #official_nop_str.msb
      ldy   #official_nop_str.lsb
      lda   #$70
      jmp   set_type
:     lda   $4016       ; B button status
      and   #$01
      beq   +
      ldx   #all_instrs_str.msb
      ldy   #all_instrs_str.lsb
      lda   #$30
      jmp   set_type
:     lda   #$F0
      ldx   #official_str.msb
      ldy   #official_str.lsb
set_type:
      sta   type_mask
      jsr   print_str
      jsr   console_newline
      jsr   console_newline
      
      ; Disable APU IRQ
      lda   #$C0
      sta   $4017
      
      ; Start NMI
      lda   #0
      sta   nmi_flag
      lda   #$80
      sta   $2000

      ; Be sure basic NMI timing is correct
      lda   #$F7
      sta   counter + 1
      lda   #$18
      sta   counter
      ldx   #255
      stx   nmi_flag
:     cpx   nmi_flag
      beq   -
      dex
:     cpx   nmi_flag
      bne   check_nmi_time
      inc   counter
      bne   -
      inc   counter + 1
      jmp   -
check_nmi_time:
      lda   counter + 1
      bne   nmi_time_wrong
      lda   counter
      cmp   #12
      bcs   nmi_time_wrong
      
      ; Begin tests
      lda   #0
      sta   test_mode
      ldx   #0
      jmp   test_loop

nmi_time_wrong:
      lda   #0
      sta   $2000
      ldx   #nmi_time_str.msb
      ldy   #nmi_time_str.lsb
      jsr   print_str
error_beep_exit:
      lda   #1
      jsr   debug_beeps
      jmp   forever
      .code
      
      ; Print current opcode
print_opcode:
      ldx   #error_str.msb
      ldy   #error_str.lsb
      jsr   print_str
      lda   test_index
      jsr   print_a
      lda   test_mode
      beq   +
      ldx   #cross_str.msb
      ldy   #cross_str.lsb
      jsr   print_str
:     jsr   console_newline
      jsr   console_newline
      rts
      .code
      
      ; NMI handler
:     rti
nmi:  dec   nmi_flag
      bmi   -
      beq   check_time
      
      ; Restore overwritten stack values to $02
      pla
      pla
      pla
      lda   #$02
      pha
      pha
      pha
      
      ; Run test loop until next NMI interrupts it
      lda   test_mode
      tay
      tax
      jmp   test_code
      
error:
      lda   #0
      sta   $2000
      jsr   print_opcode
      ldx   #unknown_str.msb
      ldy   #unknown_str.lsb
      jsr   print_str
      jsr   console_newline
      jsr   console_newline
      lda   counter + 1
      ldy   counter
      jsr   print_ay
      jmp   error_beep_exit

clock_table: ; doubled entries are to subtract value greater than $FF
      .db   $A5, $A5, $87, $86, $E1, $BE, $A2, $8D, $7C, $6D, $08
clock_times:
      .db   $01, $00, $02, $00, $03, $04, $05, $06, $07, $08, $09

      ; Calculate number of clocks instruction took
check_time:
      cld               ; avoid buggy NES emulator breaking test
      lda   counter
      ldx   counter + 1
      ldy   #11
clock_loop:
      dey
      bmi   error
      sec
      sbc   clock_table,y
      bcs   no_carry
      dex
no_carry:
      cpx   #0
      bmi   error
      bne   clock_loop
      cmp   #12
      bcs   clock_loop
      
      lda   clock_times,y
      beq   error
      
      ; Compare result with correct timing
      ldx   test_index
      ldy   test_mode
      bne   +
      cmp   instr_times,x
      beq   next_instr
      ldy   instr_times,x
      jmp   print_error
:     cmp   instr_times_cross,x
      beq   next_instr
      ldy   instr_times_cross,x
      
      ; Print measured and correct times
print_error:
      sta   <2
      sty   <3
      lda   #0
      sta   $2000
      
      jsr   print_opcode
      
      ldx   #time_str.msb
      ldy   #time_str.lsb
      jsr   print_str
      lda   <2
      ora   #$30
      jsr   print_char
      jsr   console_newline
      jsr   console_newline
      
      ldx   #correct_str.msb
      ldy   #correct_str.lsb
      jsr   print_str
      lda   <3
      ora   #$30
      jsr   print_char
      jmp   error_beep_exit
      
      ; Report success
done: lda   #0
      sta   $2000
      ldx   #passed_str.msb
      ldy   #passed_str.lsb
      jsr   print_str
      lda   #2
      jsr   debug_beeps
      jmp   forever
      
next_instr:
      ; Next instruction
:     inx
      bne   test_loop
      
      ; Toggle between normal and page crossing
      lda   test_mode
      eor   #-2
      sta   test_mode
      beq   done
test_loop:
      lda   instr_types,x
      and   type_mask         ; skip instructions not being tested
      bne   -
      stx   test_index
      
      ; Get instruction length
      lda   instr_types,x
      and   #$07
      tay
      
      ; Copy test_code to RAM
      lda   test_addrs_lsb - 1,y
      sta   <0
      lda   test_addrs_msb - 1,y
      sta   <1
      ldy   #0
      txa
:     sta   test_code,y
      iny
      lda   (0),y
      cpy   #16
      bne   -
      
      ; Set zero-page values
      lda   #$84
      sta   <$80
      sta   <$7E
      lda   #$00
      sta   <$81
      sta   <$7F
      lda   test_mode
      sta   <$84        ; $A3 LAX (ab,X) will load X from here
      
      ; Set zero-page JMP (ind) pointer
      lda   #$03
      sta   <$82
      lda   #test_code_page
      sta   <$83
      sta   <$83

      ; Fill stack with the value $02 (for RTS/RTI test)
      ldx   #$FF
      txs
      inx
      lda   #$02
:     pha
      inx
      bne   -
      
      ; Load timer with -$6C4
      lda   #$F9
      sta   counter + 1
      lda   #$3C
      sta   counter
      
      ; Tell NMI to run test code
      lda   #2
      sta   nmi_flag
:     cmp   nmi_flag
      beq   -
      jmp   error
      .code
      
test_1:
:     .db   0
      inc   counter
      bne   -
      inc   counter + 1
      jmp   test_code
      
test_2:
:     .db   0,$80
      inc   counter
      bne   -
      inc   counter + 1
      jmp   test_code
      
test_3:
:     .db   0,$80,0
      inc   counter
      bne   -
      inc   counter + 1
      jmp   test_code
      
test_jmp:
:     jmp   test_code + 3
      inc   counter
      bne   -
      inc   counter + 1
      jmp   test_code
      
test_jmp_ind:
:     jmp   ($0082)
      inc   counter
      bne   -
      inc   counter + 1
      jmp   test_code
      
test_addrs_lsb:
      .db   test_1.lsb, test_2.lsb, test_3.lsb, test_jmp.lsb, test_jmp_ind.lsb
test_addrs_msb:
      .db   test_1.msb, test_2.msb, test_3.msb, test_jmp.msb, test_jmp_ind.msb
      .code
      
; $4n = undocumented NOPs and $EB equivalent of SBC #imm
; $8n = all other undocumented opcodes
; $0n = normal opcodes
; -1  = not tested
; n = instruction length, 4 = JMP/JSR, 5 = JMP (ind)
instr_types:
        ;   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
      .db   2,  2, -1,$42,$82,  2,  2,$42,  1,  2,  1,$42,$83,  3,  3,$43 ; 0
      .db  -1,  2, -1,$42,$82,  2,  2,$42,  1,  3,$81,$43,$83,  3,  3,$43 ; 1
      .db   4,  2, -1,$42,  2,  2,  2,$42,  1,  2,  1,$42,  3,  3,  3,$43 ; 2
      .db  -1,  2, -1,$42,$82,  2,  2,$42,  1,  3,$81,$43,$83,  3,  3,$43 ; 3
      .db   2,  2, -1,$42,$82,  2,  2,$42,  1,  2,  1,$42,  4,  3,  3,$43 ; 4
      .db  -1,  2, -1,$42,$82,  2,  2,$42,  1,  3,$81,$43,$83,  3,  3,$43 ; 5
      .db   3,  2, -1,$42,$82,  2,  2,$42,  1,  2,  1,$42,  5,  3,  3,$43 ; 6
      .db  -1,  2, -1,$42,$82,  2,  2,$42,  1,  3,$81,$43,$83,  3,  3,$43 ; 7
      .db $82,  2,$82,$42,  2,  2,  2,$42,  1,  2,  1,$42,  3,  3,  3,$43 ; 8
      .db  -1,  2, -1,$42,  2,  2,  2,$42,  1,  3,  1,$43,$43,  3,$43,$43 ; 9
      .db   2,  2,  2,$42,  2,  2,  2,$42,  1,  2,  1,$42,  3,  3,  3,$43 ; A
      .db  -1,  2, -1,$42,  2,  2,  2,$42,  1,  3,  1,$43,  3,  3,  3,$43 ; B
      .db   2,  2,$82,$42,  2,  2,  2,$42,  1,  2,  1,$42,  3,  3,  3,$43 ; C
      .db  -1,  2, -1,$42,$82,  2,  2,$42,  1,  3,$81,$43,$83,  3,  3,$43 ; D
      .db   2,  2,$82,$42,  2,  2,  2,$42,  1,  2,  1,$82,  3,  3,  3,$43 ; E
      .db  -1,  2, -1,$42,$82,  2,  2,$42,  1,  3,$81,$43,$83,  3,  3,$43 ; F
       ;   Bxx    HLT

; Clocks when no page crossing occurs
instr_times:
        ; 0 1 2 3 4 5 6 7 8 9 A B C D E F
      .db 7,6,0,8,3,3,5,5,3,2,2,2,4,4,6,6 ; 0
      .db 0,5,0,8,4,4,6,6,2,4,2,7,4,4,7,7 ; 1
      .db 6,6,0,8,3,3,5,5,4,2,2,2,4,4,6,6 ; 2
      .db 0,5,0,8,4,4,6,6,2,4,2,7,4,4,7,7 ; 3
      .db 6,6,0,8,3,3,5,5,3,2,2,2,3,4,6,6 ; 4
      .db 0,5,0,8,4,4,6,6,2,4,2,7,4,4,7,7 ; 5
      .db 6,6,0,8,3,3,5,5,4,2,2,2,5,4,6,6 ; 6
      .db 0,5,0,8,4,4,6,6,2,4,2,7,4,4,7,7 ; 7
      .db 2,6,2,6,3,3,3,3,2,2,2,2,4,4,4,4 ; 8
      .db 0,6,0,6,4,4,4,4,2,5,2,5,5,5,5,5 ; 9
      .db 2,6,2,6,3,3,3,3,2,2,2,2,4,4,4,4 ; A
      .db 0,5,0,5,4,4,4,4,2,4,2,4,4,4,4,4 ; B
      .db 2,6,2,8,3,3,5,5,2,2,2,2,4,4,6,6 ; C
      .db 0,5,0,8,4,4,6,6,2,4,2,7,4,4,7,7 ; D
      .db 2,6,2,8,3,3,5,5,2,2,2,2,4,4,6,6 ; E
      .db 0,5,0,8,4,4,6,6,2,4,2,7,4,4,7,7 ; F
      
; Clocks when page crossing occurs
instr_times_cross:
        ; 0 1 2 3 4 5 6 7 8 9 A B C D E F
      .db 7,6,0,8,3,3,5,5,3,2,2,2,4,4,6,6 ; 0
      .db 0,6,0,8,4,4,6,6,2,5,2,7,5,5,7,7 ; 1
      .db 6,6,0,8,3,3,5,5,4,2,2,2,4,4,6,6 ; 2
      .db 0,6,0,8,4,4,6,6,2,5,2,7,5,5,7,7 ; 3
      .db 6,6,0,8,3,3,5,5,3,2,2,2,3,4,6,6 ; 4
      .db 0,6,0,8,4,4,6,6,2,5,2,7,5,5,7,7 ; 5
      .db 6,6,0,8,3,3,5,5,4,2,2,2,5,4,6,6 ; 6
      .db 0,6,0,8,4,4,6,6,2,5,2,7,5,5,7,7 ; 7
      .db 2,6,2,6,3,3,3,3,2,2,2,2,4,4,4,4 ; 8
      .db 0,6,0,6,4,4,4,4,2,5,2,5,5,5,5,5 ; 9
      .db 2,6,2,6,3,3,3,3,2,2,2,2,4,4,4,4 ; A
      .db 0,6,0,6,4,4,4,4,2,5,2,5,5,5,5,5 ; B
      .db 2,6,2,8,3,3,5,5,2,2,2,2,4,4,6,6 ; C
      .db 0,6,0,8,4,4,6,6,2,5,2,7,5,5,7,7 ; D
      .db 2,6,2,8,3,3,5,5,2,2,2,2,4,4,6,6 ; E
      .db 0,6,0,8,4,4,6,6,2,5,2,7,5,5,7,7 ; F

title_str:
      .db   "6502 TIMING TEST (16 SECONDS)",0
official_str:
      .db   "OFFICIAL INSTRUCTIONS ONLY",0
official_nop_str:
      .db   "OFFICIAL + NOP",0
all_instrs_str:
      .db   "OFFICIAL + UNDOCUMENTED",0
passed_str:
      .db   "PASSED",0
error_str:
      .db   "FAIL OP :",0
cross_str:
      .db   "WITH PAGE CROSS",0
time_str:
      .db   "EMULATOR: ",0
correct_str:
      .db   "CORRECT : ",0
unknown_str:
      .db   "UNKNOWN ERROR",0
nmi_time_str:
      .db   "BASIC TIMING WRONG",0
      
; Prints string at address XY
print_str:
      stx   <1
      sty   <0
      ldy   #0
      lda   (0),y
      jsr   print_char
      iny
      lda   (0),y
:     jsr   print_char_no_wait
      iny
      lda   (0),y
      bne   -
      rts
      .code
      
