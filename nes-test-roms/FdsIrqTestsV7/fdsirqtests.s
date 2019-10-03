;
; FDS headers and stuff
;

BYPASS = 1

; setting the file count 1 higher than files on disk for the license "bypass" technique
FILE_COUNT = 5 + BYPASS

.segment "HEADER"
.byte 'F','D','S',$1A
.byte 1 ; side count

.segment "SIDE1A"
; block 1
.byte $01
.byte "*NINTENDO-HVC*"
.byte $00 ; manufacturer
.byte "EXA"
.byte $20 ; normal disk
.byte $00 ; game version
.byte $00 ; side
.byte $00 ; disk
.byte $00 ; disk type
.byte $00 ; unknown
.byte FILE_COUNT ; boot file count
.byte $FF,$FF,$FF,$FF,$FF
.byte $92 ; 2017
.byte $04 ; april
.byte $17 ; 17
.byte $49 ; country
.byte $61, $00, $00, $02, $00, $00, $00, $00, $00 ; unknown
.byte $92 ; 2017
.byte $04 ; april
.byte $17 ; 17
.byte $00, $80 ; unknown
.byte $00, $00 ; disk writer serial number
.byte $07 ; unknown
.byte $00 ; disk write count
.byte $00 ; actual disk side
.byte $00 ; unknown
.byte $00 ; price
; block 2
.byte $02
.byte FILE_COUNT

.segment "FILE0_HDR"
; block 3
.import __FILE0_DAT_RUN__
.import __FILE0_DAT_SIZE__
.byte $03
.byte 0,0
.byte "FILE0..."
.word __FILE0_DAT_RUN__
.word __FILE0_DAT_SIZE__
.byte 0 ; PRG
; block 4
.byte $04
;.segment "FILE0_DAT"
;.incbin "" ; this is code below

.segment "FILE1_HDR"
; block 3
.import __FILE1_DAT_RUN__
.import __FILE1_DAT_SIZE__
.byte $03
.byte 1,1
.byte "FILE2..."
.word __FILE1_DAT_RUN__
.word __FILE1_DAT_SIZE__
.byte 0 ; PRG
; block 4
.byte $04
;.segment "FILE1_DAT"
;.incbin "" ; this is code below

.segment "FILE2_HDR"
; block 3
.import __FILE2_DAT_SIZE__
.import __FILE2_DAT_RUN__
.byte $03
.byte 2,2
.byte "FILE2..."
.word __FILE2_DAT_RUN__
.word __FILE2_DAT_SIZE__
.byte 1 ; CHR
; block 4
.byte $04
.segment "FILE2_DAT"
.incbin "test.chr"

.segment "FILE3_HDR"
; block 3
.import __FILE3_DAT_SIZE__
.import __FILE3_DAT_RUN__
.byte $03
.byte 3,3
.byte "FILE3..."
.word __FILE3_DAT_RUN__
.word __FILE3_DAT_SIZE__
.byte 1 ; CHR
; block 4
.byte $04
.segment "FILE3_DAT"
.incbin "test.chr"

.if (BYPASS <> 0)

.segment "FILE4_HDR"
; block 3
.import __FILE4_DAT_SIZE__
.import __FILE4_DAT_RUN__
.byte $03
.byte 4,4
.byte "FILE4..."
.word __FILE4_DAT_RUN__
.word __FILE4_DAT_SIZE__
.byte 0 ; PRG
; block 4
.byte $04
.segment "FILE4_DAT"
; "loading" this into $2000 as the last booting file allows us to skips the license screen
.byte $90

.else

.segment "FILE4_HDR"
; block 3
.import __FILE4_DAT_SIZE__
.import __FILE4_DAT_RUN__
.byte $03
.byte 4,4
.byte "KYODAKU-"
.word $2800
.word __FILE4_DAT_SIZE__
.byte 2 ; nametable
; block 4
.byte $04
.segment "FILE4_DAT"
.incbin "check.bin"

.endif

;
; FDS vectors
;

.segment "FILE1_DAT"
.word nmi
.word nmi
.if (BYPASS <> 0)
	.word bypass
.else
	.word nmi
.endif
.word reset
.word irq

;
; reset routine
;

.segment "FILE0_DAT"

; this routine is entered by interrupting the last boot file load
; by forcing an NMI not expected by the BIOS, allowing the license
; screen to be skipped entirely.
;
; The last file writes $90 to $2000, enabling NMI during the file load.
; The "extra" file in the FILE_COUNT causes the disk to keep seeking
; past the last file, giving enough delay for an NMI to fire and interrupt
; the process.
bypass:
	; disable NMI
	lda #0
	sta $2000
	; replace NMI 3 "bypass" vector at $DFFA
	lda #<nmi
	sta $DFFA
	lda #>nmi
	sta $DFFB
	; tell the FDS reset routine that the BIOS initialized correctly
	lda #$35
	sta $0102
	lda #$AC
	sta $0103
	; reset the FDS to begin our program properly
	jmp ($FFFC)


; Delays n clocks, from 2 to 16777215
; Preserved: A, X, Y, flags
.macro delay n
    .if (n) < 0 .or (n) = 1 .or (n) > 16777215
	.error "Delay out of range"
    .endif
    delay_ (n)
.endmacro

; Delays A clocks + overhead
; Preserved: X, Y
; Time: A+25 clocks (including JSR)
:       sbc #7          ; carry set by CMP
delay_a_25_clocks:
	cmp #7
	bcs :-          ; do multiples of 7
	lsr a           ; bit 0
	bcs :+
:                       ; A=clocks/2, either 0,1,2,3
	beq @zero       ; 0: 5
	lsr a
	beq :+          ; 1: 7
	bcc :+          ; 2: 9
@zero:  bne :+          ; 3: 11
:       rts             ; (thanks to dclxvi for the algorithm)


; Delays A*256 clocks + overhead
; Preserved: X, Y
; Time: A*256+16 clocks (including JSR)
delay_256a_16_clocks:
	cmp #0
	bne :+
	rts
delay_256a_11_clocks_:
:       pha
	lda #256-19-22
	jsr delay_a_25_clocks
	pla
	clc
	adc #$FF
	bne :-
	rts


; Delays A*65536 clocks + overhead
; Preserved: X, Y
; Time: A*65536+16 clocks (including JSR)
delay_65536a_16_clocks:
	cmp #0
	bne :+
	rts
delay_65536a_11_clocks_:
:       pha
	lda #256-19-22-13
	jsr delay_a_25_clocks
	lda #255
	jsr delay_256a_11_clocks_
	pla
	clc
	adc #$FF
	bne :-
	rts

max_short_delay = 41

	; delay_short_ macro jumps into these
	.res (max_short_delay-12)/2,$EA ; NOP
delay_unrolled_:
	rts

.macro delay_short_ n
    .if n < 0 .or n = 1 .or n > max_short_delay
	.error "Internal delay error"
    .endif
    .if n = 0
    	; nothing
    .elseif n = 2
	nop
    .elseif n = 3
	sta <delay_temp_
    .elseif n = 4
	nop
	nop
    .elseif n = 5
	sta <delay_temp_
	nop
    .elseif n = 6
	nop
	nop
	nop
    .elseif n = 7
	php
	plp
    .elseif n = 8
	nop
	nop
	nop
	nop
    .elseif n = 9
	php
	plp
	nop
    .elseif n = 10
	sta <delay_temp_
	php
	plp
    .elseif n = 11
	php
	plp
	nop
	nop
    .elseif n = 13
	php
	plp
	nop
	nop
	nop
    .elseif n & 1
	sta <delay_temp_
	jsr delay_unrolled_-((n-15)/2)
    .else
	jsr delay_unrolled_-((n-12)/2)
    .endif
.endmacro

.macro delay_nosave_ n
    ; 65536+17 = maximum delay using delay_256a_11_clocks_
    ; 255+27   = maximum delay using delay_a_25_clocks
    ; 27       = minimum delay using delay_a_25_clocks
    .if n > 65536+17
	lda #^(n - 15)
	jsr delay_65536a_11_clocks_
	; +2 ensures remaining clocks is never 1
	delay_nosave_ (((n - 15) & $FFFF) + 2)
    .elseif n > 255+27
	lda #>(n - 15)
	jsr delay_256a_11_clocks_
	; +2 ensures remaining clocks is never 1
	delay_nosave_ (<(n - 15) + 2)
    .elseif n >= 27
	lda #<(n - 27)
	jsr delay_a_25_clocks
    .else
	delay_short_ n
    .endif
.endmacro

.macro delay_ n
    .if n > max_short_delay
	php
	pha
	delay_nosave_ (n - 14)
	pla
	plp
    .else
	delay_short_ n
    .endif
.endmacro	
	
;
; other stuff
;

.segment "ZEROPAGE"
delay_temp_: .res 1   ; only written to
compareValue: .res 1
pointer: .res 2

.segment "OAM"
oam:        .res 256

.segment "BSS"
log_expectedresult: .byte $00, $00, $00
testNumber: .res 1
log_irqcount: .res 256
log_testresult: .res 256

.segment "FILE0_DAT"

reset:
	sei
	
	lda #$40
	sta $4017
	lda #$C0
	sta $0101
	
	; clear all RAM to 0
	lda #0
	ldx #0
	:
		sta $0000, X
		;sta $0100, X ; don't clear the stack! the FDS uses it
		sta $0200, X
		sta $0300, X
		sta $0400, X
		sta $0500, X
		sta $0600, X
		sta $0700, X
		inx
		bne :-	
		
	jsr runTests
	jsr displayResults
:
	jmp :-

nmi:
	rti

.macro irqmacro n, write, writeValue
	pha
	txa
	pha
	
	;increase counter
	ldx testNumber
	inc log_irqcount,x
	
	;ack irq
	.if write <> 1
	lda n
	.else
	lda #writeValue
	sta n
	.endif
	
	.if n <> $4030
		lda log_irqcount,x
		cmp #$20
		bne :+
		lda $4030 ;safety net to prevent infinite loop when ack doesn't work
		:
	.endif
	
	lda log_irqcount,x
	cmp #$40
	bne :+
	;too many irqs, disable irqs
	lda #00
	sta $4022
:
	
	pla
	tax
	pla
	rti
.endmacro	
	
irq:
	irqmacro $4030, 0, 0

irq_4020:
	irqmacro $4020, 0, 0

irq_4021:
	irqmacro $4021, 0, 0

irq_4022_0:
	irqmacro $4022, 1, 0
	
irq_4022_2:
	irqmacro $4022, 1, 2

irq_4023_0:
	irqmacro $4023, 1, 0
	
irq_4023_1:
	irqmacro $4023, 1, 1
	

inittest:
	;Set timer to 200 cycles, stop any running IRQ
	sei
	
	;Enable all registers
	lda #$83
	sta $4023
	
	lda #00
	sta $4020
	sta $4021
	sta $4022
	
	inc testNumber
	ldx testNumber
	sta log_testresult,x
	sta log_irqcount,x
	
	lda #200
	sta $4020	
	
	cli
	rts
	
failTest:
	pha
	txa
	pha
	
	ldx testNumber	
	lda #$58 ;$58 = test failed 'X'
	sta log_testresult,x	
	
	pla
	tax
	pla
	rts
	
;Input: a = expected IRQ counter value (at the end of test)
endTest:
	sei
	jsr validateValue
	pha
	txa
	pha

	;disable irqs, and make sure we ack any pending ones
	sei
	lda #00
	sta $4022
	lda $4030
	
	;restore default irq handler
	lda #<irq
	sta $DFFE
	lda #>irq
	sta $DFFF	
	
	;check if test has failed
	ldx testNumber
	lda log_testresult,x
	cmp #00
	bne :+ ;test was failed, skip pass code
	
	;if not failed, mark as passed
	lda #$4F  ;$4F = test passed 'O'
	sta log_testresult,x	
:		
	pla
	tax
	pla
	cli
	rts
	
	
;returns irq count in Y
getIrqCount:
	pha
	txa
	pha
	
	ldx testNumber
	ldy log_irqcount,x	
	
	pla
	tax
	pla
	rts
	
;Input: a = expected IRQ counter value
validateValue:
	jsr getIrqCount
	sty compareValue
	cmp compareValue
	beq :+
	;value doesn't match, fail the test
	jsr failTest
:	
	rts

labels: 
	.asciiz "No Irq At Start"
	.asciiz "Trigger 1 IRQ"
	.asciiz "IRQ when rvalue = 0"
	.asciiz "Reload val not reset"
	.asciiz "Disable DiskReg Test"
	.asciiz "No Disk Reg = No IRQ"
	.asciiz "Can't Ack w/ $4020"
	.asciiz "Can't Ack w/ $4021"
	.asciiz "Can Ack w/ W:$4022:0"
	.asciiz "Cant w/ W:$4022:2"
	.asciiz "Can Ack w/ W:$4023:0"
	.asciiz "Cant w/ W:$4023:1"
	.asciiz "2x W:4022 Delays IRQ"
	.asciiz "Enbl DiskR after 4022"
	.asciiz "Set RelVal DskRg Off"
	.asciiz "4022:0 stops irq ctr"
	.asciiz "4022:0 not reset rval"
	.asciiz "RVal=0 4x W:$4022:2"
	.asciiz "Irq w/ Repeat test"
	.asciiz "Irq repeat stop test"
	.asciiz "RVal=0 w/ Repeat"

datestring:
	.asciiz "V7 2017-09-22"
	
runTests:
	jsr test1_NoIrqsAtStart
	jsr test2_NoRepeat_TriggerSingleIrq
	jsr test3_NoRepeat_IrqTriggersWhenReloadIsZero
	jsr test4_NoRepeat_ReloadValueKeptBetweenIrqs
	jsr test5_DisableDiskRegsDisablesIrq
	jsr test6_NoIrqIfDiskRegDisabled
	jsr test7_CantAckWith4020
	jsr test8_CantAckWith4021
	jsr test9_CanAckWith4022Write_Val0
	jsr test10_CantAckWith4022Write_Val2
	jsr test11_CanAckWith4023Write_Val0
	jsr test12_CantAckWith4023Write_Val1
	jsr test13_WriteTo4022DelaysIrq
	jsr test14_EnableDiskRegAfter4022WriteDoesntTriggerIrq
	jsr test15_CanSetReloadValueWithDiskRegsOff
	jsr test16_Write0To4022StopsPendingIrqCountdown
	jsr test17_Write0To4022DoesntResetReloadValue
	jsr test18_TriggerMultipleIrqsWith0ReloadValue
	jsr test19_IrqWithRepeat
	jsr test20_IrqWithRepeatStopsOn4022Write
	jsr test21_0ReloadValueAndRepeatFlag
	
	rts

test1_NoIrqsAtStart:
	jsr inittest
	delay 2000
	lda #00
	jsr endTest
	rts
	
test2_NoRepeat_TriggerSingleIrq:
	jsr inittest
	
	;Trigger an IRQ in 200 cycles
	lda #02
	sta $4022
	
	;make sure irq is not early
	delay 50
	lda #00
	jsr validateValue
	
	delay 150
	lda #01
	jsr endTest
	rts
	
test3_NoRepeat_IrqTriggersWhenReloadIsZero:
	jsr inittest
	
	;Try to trigger an IRQ in 0 cycles
	lda #00
	sta $4021
	sta $4020	
	lda #02
	sta $4022

	lda #01
	jsr endTest
	rts		
	
test4_NoRepeat_ReloadValueKeptBetweenIrqs:
	jsr inittest
	
	;Trigger an IRQ in 200 cycles
	lda #02
	sta $4022
	
	;wait for first irq
	delay 220
	lda #01
	jsr validateValue
	
	;Trigger another IRQ in 200 cycles, without setting reload value
	lda #02
	sta $4022
	
	;Make sure the 2nd irq doesn't trigger super fast (would happen if reload value was reset)
	delay 50
	lda #01
	jsr validateValue
	
	;Wait for second irq to trigger
	delay 200	
	
	lda #02
	jsr endTest
	rts
	
test5_DisableDiskRegsDisablesIrq:
	jsr inittest
	
	;Trigger an IRQ in 200 cycles
	lda #02
	sta $4022
	
	;disable disk regs
	lda #00
	sta $4023
	
	;wait for irq - should not occur
	delay 200
	lda #00
	jsr endTest
	rts	
	
test6_NoIrqIfDiskRegDisabled:
	jsr inittest
	
	;disable disk regs
	lda #00
	sta $4023

	;try to trigger an IRQ in 200 cycles
	lda #02
	sta $4022
	
	;wait for irq - should not occur?
	delay 220
	lda #00
	jsr endTest
	rts
	
.macro acktestmacro irqhandler, expectedvalue
	jsr inittest

	lda #<irqhandler
	sta $DFFE
	lda #>irqhandler
	sta $DFFF

	;try to trigger an IRQ in 200 cycles
	lda #02
	sta $4022
	
	;wait for irq - it will be acked by the safety mechanism after 10 irqs
	delay 220
		
	lda #expectedvalue
	jsr endTest
.endmacro		
		
test7_CantAckWith4020:
	acktestmacro irq_4020, $20
	rts
	
test8_CantAckWith4021:
	acktestmacro irq_4021, $20
	rts		

test9_CanAckWith4022Write_Val0:
	acktestmacro irq_4022_0, $01
	rts	
	
test10_CantAckWith4022Write_Val2:
	acktestmacro irq_4022_2, $20
	rts		
	
test11_CanAckWith4023Write_Val0:
	acktestmacro irq_4023_0, $01
	rts		
	
test12_CantAckWith4023Write_Val1:
	acktestmacro irq_4023_1, $20
	rts		
	
test13_WriteTo4022DelaysIrq:
	jsr inittest
	
	;Trigger an IRQ in 200 cycles
	lda #02
	sta $4022

	;Wait a bit, and then write to $4022 again - this should reset the timer?
	delay 100
	lda #02
	sta $4022
	
	;Wait until the irq would have triggered
	delay 100
	lda #00
	jsr validateValue

	;wait for delayed irq to trigger
	delay 200
	lda #01
	jsr endTest
	rts	
	
test14_EnableDiskRegAfter4022WriteDoesntTriggerIrq:
	jsr inittest

	;disable disk regs
	lda #00
	sta $4023	
	
	;Attempt to trigger an IRQ in 200 cycles
	lda #02
	sta $4022

	;enable disk regs
	lda #03
	sta $4023	
	
	;wait for irq - should not occur
	delay 220
	lda #00
	jsr endTest
	rts	
	
test15_CanSetReloadValueWithDiskRegsOff:
	jsr inittest

	;disable disk regs
	lda #00
	sta $4023
	
	lda #10
	sta $4021

	;enable disk regs
	lda #03
	sta $4023	
	
	;Attempt to trigger an IRQ in ~4296 cycles
	lda #02
	sta $4022
	
	;check if irq triggered after 200 cycles
	delay 220
	lda #00
	jsr validateValue
	
	delay 4100	
	lda #01
	jsr endTest
	rts

test16_Write0To4022StopsPendingIrqCountdown:
	jsr inittest

	;Attempt to trigger an IRQ in 200 cycles
	lda #02
	sta $4022
	
	;Stop the irq counter
	delay 50
	lda #00
	sta $4022
	
	delay 200
	lda #00
	jsr endTest
	rts
	
test17_Write0To4022DoesntResetReloadValue:
	jsr inittest

	;Attempt to trigger an IRQ in 200 cycles
	lda #02
	sta $4022
	
	;Stop the irq counter
	delay 50
	lda #00
	sta $4022

	;Start it again, reload counter should still be 200
	delay 50
	lda #02
	sta $4022
	
	;No irq should have occurred yet
	delay 50
	lda #00
	jsr validateValue
	
	;An irq should have occurred by now
	delay 220
	lda #01
	jsr endTest
	rts
	
test18_TriggerMultipleIrqsWith0ReloadValue:
	jsr inittest
	
	;Try to trigger an IRQ in 0 cycles
	lda #00
	sta $4021
	sta $4020	
	lda #02
	sta $4022
	delay 10
	sta $4022
	delay 10
	sta $4022
	delay 10
	sta $4022
	delay 10

	lda #04
	jsr endTest
	rts		
	
test19_IrqWithRepeat:
	jsr inittest
	
	;Trigger an IRQ every 200 cycles, with repeat mode one
	lda #03
	sta $4022
	
	delay 650
	lda #04
	jsr endTest
	rts
	
test20_IrqWithRepeatStopsOn4022Write:
	jsr inittest
	
	;Trigger an IRQ every 200 cycles, with repeat mode one
	lda #03
	sta $4022
	
	;Wait for 4 irqs to fire
	delay 650
	
	;Stop irqs and make sure no more irqs fire
	lda #00
	sta $4022
	delay 2000
	
	lda #04
	jsr endTest
	rts			
	
test21_0ReloadValueAndRepeatFlag:
	jsr inittest
	
	;Try to trigger multiple IRQs with repeat + 0 reload value
	lda #00
	sta $4021
	sta $4020	
	lda #03
	sta $4022
	
	delay 20

	lda #$40
	jsr endTest
	rts		
	
draw_hex:
	pha
	lsr
	lsr
	lsr
	lsr
	clc
	adc #$A0
	sta $2007
	pla
	and #$0F
	clc
	adc #$A0
	sta $2007
	rts

displayResults:
	; set FDS to use vertical mirroring
	lda $FA
	and #%11110111
	sta $4025
	; clear screen
	lda #0
	sta $2000
	sta $2001
	bit $2002
	lda #$20
	sta $2006
	lda #$00
	sta $2006
	tax
	ldy #16
	:
		sta $2007
		inx
		bne :-
		dey
		bne :-
	; draw log
	lda #$20
	sta $2006
	lda #$62
	sta $2006
	
	lda #<labels
	sta pointer
	lda #>labels
	sta pointer+1
	
	ldx #1
testresultloop:		
	;draw test number
	txa
	jsr draw_hex
	
	;space
	lda #00
	sta $2007
	
	;draw test result
	lda log_testresult,x
	sta $2007

	;space
	lda #00
	sta $2007	
	
	lda log_irqcount,x
	jsr draw_hex

	;space
	lda #00
	sta $2007

	txa
	pha
	ldx #00
	ldy #00	
looptext:	
	lda (pointer),y
	inc pointer
	bne :+
	inc pointer+1
:
	sta $2007
	inx
	cmp #00
	bne looptext	

	;space until next row	
	txa
	clc
	sbc #23
	tay
	
	pla
	tax
	
	lda #00
:	
	sta $2007	
	iny
	cpy #00	
	bne :-
	
	inx
	cpx #22
	bne testresultloop	
	
	
	lda #$23
	sta $2006
	lda #$44
	sta $2006	
	lda #<datestring
	sta pointer
	lda #>datestring
	sta pointer+1	
	ldy #00	
looptext2:	
	lda (pointer),y
	inc pointer
	bne :+
	inc pointer+1
:
	sta $2007
	cmp #00
	bne looptext2
	
	; set palette
	lda #$3F
	sta $2006
	lda #$00
	sta $2006
	lda #$0F
	sta $2007
	lda #$00
	sta $2007
	lda #$10
	sta $2007
	lda #$20
	sta $2007
	; finish
	lda #0
	sta $2000
	sta $2005
	sta $2005
	lda #%00001010
	sta $2001
	rts
	
;
; end of file
;
