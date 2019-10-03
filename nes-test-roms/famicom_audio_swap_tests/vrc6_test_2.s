; VRC6 Swap Test 2
; Brad Smith, 8/13/2012
;
; This ROM is intended to test the VRC6 chip
; via hotswapping. It tests the phase reset
; feature of the channel enable flag.
;
; This is intended for Esper Dream 2 or Madara,
; but can easily be rebuilt for Akumajou Densetsu,
; see below.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.segment "HEADER"

.ifndef NSF

; NES header
.byte 'N', 'E', 'S', $1A ; ID
.byte $02 ; 16k PRG bank count (2 = 32kb)
.byte $02 ; 8k CHR bank count (2 = 8kb)
;.byte $A0, $10 ; mapper 1A (Esper Dream 2)
.byte $00, $00 ; mapper 0 (hotswap to a VRC6 like Esper Dream 2)
.byte $0, $0, $0, $0, $0, $0, $0, $0

.else

; NSF header
.byte 'N', 'E', 'S', 'M', $1A ; ID
.byte $01 ; version
.byte 1 ; songs
.byte 1 ; starting song
.word $8000 ; load address
.word nsf_init ; init function
.word nsf_play ; play function
; title
.byte "vrc6 swap test",           0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
; artist
.byte "bradsmith",      0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
; copyright
.byte "2012", 0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
; other
.word 16666 ; NTSC speed
.byte 0,0,0,0,0,0,0,0 ; bankswitch init
.word 20000 ; PAL speed
.byte $00 ; PAL/NTSC bits
.byte %00000001 ; expansion bits
.byte 0,0,0,0 ; pad to $80

.endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.segment "CODE"

SONG_READ  = $00 ; current data read pointer
SONG_LOOP  = $02 ; initial data read pointer
WRITE_ADDR = $04 ; memory address to write
SKIP       = $06 ; frames to skip

.ifndef NSF

; Esper Dream 2 / Madara style VRC6 lines
.define V9000 $90, $00
.define VA000 $A0, $00
.define VB000 $B0, $00
.define V9001 $90, $02
.define VA001 $A0, $02
.define VB001 $B0, $02
.define V9002 $90, $01
.define VA002 $A0, $01
.define VB002 $B0, $01

.else

; Akumajou Densetsu style VRC6 lines
.define V9000 $90, $00
.define VA000 $A0, $00
.define VB000 $B0, $00
.define V9001 $90, $01
.define VA001 $A0, $01
.define VB001 $B0, $01
.define V9002 $90, $02
.define VA002 $A0, $02
.define VB002 $B0, $02

.endif

.out .sprintf("V9001: %02X%02X", V9001);

.import __SWAP_LOAD__
.import __SWAP_RUN__

song_data = song_vrc6_test

.ifdef NSF

nsf_init:
	JSR swap_init
	RTS

nsf_play:
	JMP swap_play
	; swap_play will RTS

.endif

swap_reset:
	; reset stuff
	SEI        ; disable IRQ
	CLD        ; disable decimal mode
	LDX #$40
	STX $4017  ; disable APU frame counter IRQ
	LDX #$FF
	TXS        ; reset stack pointer to $01FF
	INX        ; X = 0
	STX $2000  ; disable NMI
	STX $2001  ; disable all rendering
	STX $4010  ; disable DMC IRQ
	; playback init
	JSR swap_init
	; run SWAP loop from RAM
	JMP swap_loop

swap_init:
	; load SWAP into RAM
	LDY #0
	; read address
	LDA #<__SWAP_LOAD__
	STA $0
	LDA #>__SWAP_LOAD__
	STA $1
	; write address
	LDA #<__SWAP_RUN__
	STA $2
	LDA #>__SWAP_RUN__
	STA $3
	; copy
swap_copy:
	LDA ($0),Y
	STA ($2),Y
	INC $0
	BNE @load_inc
	INC $1
@load_inc:
	INC $2
	BNE @run_inc
	INC $3
@run_inc:
	LDA $3
	; when write address == $0800 stop
	CMP #$08
	BNE swap_copy
swap_copy_done:
	; initialize song
	LDA #<song_data
	STA SONG_READ+0
	STA SONG_LOOP+0
	LDA #>song_data
	STA SONG_READ+1
	STA SONG_LOOP+1
	LDA #0
	STA SKIP
	RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.segment "SWAP"

swap_nmi:
	RTI

swap_loop:
@wait_nmi:
	; waste time to reduce inaccuracy (BIT $2002 skips a frame if it has bad timing)
	LDX #$FF
@waste_time:
	DEX
	BNE @waste_time
	; test for NMI
	BIT $2002
	BPL @wait_nmi
	; play the thing
	JSR swap_play
	; do it again
	JMP swap_loop

swap_play:
	; if SKIP > 0 skip a frame and decrement
	LDY SKIP
	BEQ @read_loop
	DEY
	STY SKIP
	RTS
	; main loop
	; note Y = 0
@read_loop:
	LDA (SONG_READ), Y
	CMP #$FF ; $FF - end frame
	BEQ @do_skip
	CMP #$FE ; $FE - loop
	BEQ @do_loop
	; else memory write
	JMP @do_write
	; write instruction
@do_write:
	STA WRITE_ADDR+1 ; high byte
	JSR inc_read
	LDA (SONG_READ), Y
	STA WRITE_ADDR+0 ; low byte
	JSR inc_read
	LDA (SONG_READ), Y
	STA (WRITE_ADDR), Y
	JSR inc_read
	JMP @read_loop
	; loop instruction
@do_loop:
	LDA SONG_LOOP+0
	STA SONG_READ+0
	LDA SONG_LOOP+1
	STA SONG_READ+1
	JMP @read_loop
	; skip frames instruction
@do_skip:
	JSR inc_read
	LDA (SONG_READ), Y
	STA SKIP
	JSR inc_read
	RTS

; increment SONG_READ pointer
inc_read:
	INC SONG_READ+0
	BNE @done
	INC SONG_READ+1
@done:
	RTS

;
; song data
;

; variable length code:
; FF ab = return, skip the next ab frames
; FE = loop
; ab cd ef = write ef to memory ab cd (note big endian address)

song_vrc6_test:

; buzz 4011
.byte $40, $11, $7F ; 4011 = 7F
.byte $FF, $00      ; end frame
.byte $40, $11, $00 ;
.byte $FF, $00      ;
.byte $40, $11, $7F ;
.byte $FF, $00      ;
.byte $40, $11, $00 ;
.byte $FF, $00      ;
.byte $40, $11, $7F ;
.byte $FF, $00      ;
.byte $40, $11, $00 ;
.byte $FF, 179      ; wait 3s

; buzz 4011
.byte $40, $11, $7F ; 4011 = 7F
.byte $FF, $00      ; end frame
.byte $40, $11, $00 ;
.byte $FF, $00      ;
.byte $40, $11, $7F ;
.byte $FF, $00      ;
.byte $40, $11, $00 ;
.byte $FF, $00      ;
.byte $40, $11, $7F ;
.byte $FF, $00      ;
.byte $40, $11, $00 ;
.byte $FF, $00      ;
.byte $40, $11, $7F ;
.byte $FF, $00      ;
.byte $40, $11, $00 ;
.byte $FF, $00      ;
.byte $40, $11, $7F ;
.byte $FF, $00      ;
.byte $40, $11, $00 ;
.byte $FF, 59       ; wait 1s

; frame 0
.byte $40, $15, $00 ; wipe APU
.byte $40, $00, $00
.byte $40, $01, $00
.byte $40, $02, $00
.byte $40, $03, $00
.byte $40, $04, $00
.byte $40, $05, $00
.byte $40, $06, $00
.byte $40, $07, $00
.byte $40, $08, $00
.byte $40, $09, $00
.byte $40, $0A, $00
.byte $40, $0B, $00
.byte $40, $0C, $00
.byte $40, $0D, $00
.byte $40, $0E, $00
.byte $40, $0F, $00
.byte $80, $00, $00 ; VRC6 mapper init
.byte $C0, $00, $02
.byte $B0, $03, $00
.byte $F0, $00, $00
.byte $F0, $01, $00
.byte $F0, $02, $00
.byte $90, $03, $00 ; VRC6 ???
.byte $A0, $03, $00 ; VRC6 ???
.byte $B0, $03, $00 ; VRC6 mirroring
.byte V9000, $00 ; wipe VRC6
.byte V9001, $00
.byte V9002, $00
.byte VA000, $00
.byte VA001, $00
.byte VA002, $00
.byte VB000, $00
.byte VB001, $00
.byte VB002, $00
.byte $FF, 59       ; wait 1s

; TEST 1 compare solid pulse
; setup pitches
.byte VA001, $34
.byte VA002, $83
.byte VA000, $5F
.byte $FF, 19
.byte VA002, $0D
.byte $FF, 19
.byte $40, $11, $7F
.byte VA002, $83
.byte $FF, 19
.byte VA002, $03
.byte $40, $11, $00
.byte $FF, 19
.byte $40, $11, $7F
.byte VA002, $83
.byte $FF, 19
.byte VA002, $03
.byte $40, $11, $00
.byte $FF, 19
.byte VA002, $83
.byte $FF, 19
.byte VA002, $03
.byte $FF, 19
.byte VA002, $83
.byte $FF, 19
.byte VA002, $03
.byte $FF, 19
.byte VA002, $03
.byte VA002, $83
.byte $FF, 19
.byte VA002, $03
.byte VA002, $83
.byte $FF, 19
.byte VA002, $03
.byte VA002, $83
.byte $FF, 19
.byte VA002, $03
.byte VA002, $83
.byte $FF, 19
.byte VA002, $03
.byte VA002, $83
.byte $FF, 19
.byte VA002, $03
.byte VA002, $83
.byte $FF, 19
.byte VA002, $03
.byte VA002, $83
.byte $FF, 19
.byte VA002, $03
.byte VA002, $83
.byte $FF, 19
.byte VA002, $03
.byte VA002, $83
.byte $FF, 19
.byte VA002, $03
.byte VA002, $83
.byte $FF, 19
.byte VA002, $03
.byte VA002, $83
.byte $FF, 19
.byte VA002, $03
.byte VA002, $83
.byte $FF, 19
.byte VA002, $03
.byte VA002, $83
.byte $FF, 19
.byte VA002, $03
.byte VA002, $83
.byte $FF, 19
.byte VA002, $03
.byte VA002, $83
.byte $FF, 19
.byte VA002, $03
.byte VA002, $83
.byte $FF, 19
.byte VA000, $00
; end of test
.byte $FF, 59

; END of test loop
; LOOP
.byte $FE

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.segment "VECTORS"
	.word swap_nmi    ; vblank interrupt
	.word swap_reset  ; reset interrupt
	.word 0           ; irq

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.segment "TILES"
	; fills with zeroes

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;
; end of file
;
