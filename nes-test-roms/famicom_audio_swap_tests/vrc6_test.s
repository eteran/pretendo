; VRC6 Swap Test
; Brad Smith, 8/12/2012
;
; This ROM is intended to test the VRC6 chip
; via hotswapping. It compares the 2A03 square
; channels to the VRC6 ones, and tests various
; chip features.
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
.byte $40, $02, $A9 ; E-4 (timer low)
.byte $40, $06, $FD ; A-3
.byte V9002, $80 ; timer high, enable vrc6 square 1
.byte VA002, $80 ; timer high, enable vrc6 square 2
.byte V9001, $A9 ; E-4
.byte VA001, $FD ; A-3
; test APU
.byte $40, $15, %00000001 ; enable square 1
.byte $40, $00, $7F ; square 1 on, duty 1, full volume
.byte $40, $03, $00
.byte $FF, 29
.byte $40, $15, %00000010 ; enable square 2
.byte $40, $04, $BF ; square 2 on, duty 2, full volume
.byte $40, $07, $00
.byte $FF, 29
.byte $40, $15, %00000000 ; disable squares
; echo VRC6
.byte V9000, $3F ; square 1 on, duty 3, full volume
.byte $FF, 29
.byte V9000, $00 ; square 1 off
.byte VA000, $7F ; square 2 on, duty 7, full volume
.byte $FF, 29
.byte VA000, $00 ; square 2 off
; end of test
.byte $FF, 59 ; wait 1s

; TEST 2 phase reset
; test APU
.byte $40, $15, %00000001 ; enable square 1
.byte $40, $00, $BF ; square 1 on, duty 2, full volume
.byte $40, $02, $A9 ; E-4
.byte $40, $03, $00
.byte $FF, 19
.byte $40, $03, $00 ; reset phase
.byte $FF, 19
.byte $40, $03, $00
.byte $FF, 19
.byte $40, $03, $00
.byte $FF, 19
.byte $40, $03, $00
.byte $FF, 19
.byte $40, $03, $00
.byte $FF, 19
.byte $40, $15, %000000010 ; enable square 2
.byte $40, $04, $3F ; square 2 on, duty 0, full volume
.byte $40, $06, $FD ; A-3
.byte $40, $07, $00
.byte $FF, 19
.byte $40, $07, $00 ; reset phase
.byte $FF, 19
.byte $40, $07, $00
.byte $FF, 19
.byte $40, $07, $00
.byte $FF, 19
.byte $40, $07, $00
.byte $FF, 19
.byte $40, $07, $00
.byte $FF, 19
.byte $40, $15, %00000000 ; disable squares
.byte $FF, 59 ; wait 1s
; echo vrc6
.byte V9000, $7F ; square 1 on, duty 7, full volume
.byte $FF, 19
.byte V9002, $80 ; reset phase?
.byte $FF, 19
.byte V9002, $80
.byte $FF, 19
.byte V9002, $80
.byte $FF, 19
.byte V9002, $80
.byte $FF, 19
.byte V9002, $80
.byte $FF, 19
.byte V9000, $00 ; mute square 1
.byte VA000, $1F ; square 2 on, duty 1, full volume
.byte $FF, 19
.byte VA002, $80 ; reset phase?
.byte $FF, 19
.byte VA002, $80
.byte $FF, 19
.byte VA002, $80
.byte $FF, 19
.byte VA002, $80
.byte $FF, 19
.byte VA002, $80
.byte $FF, 19
.byte VA000, $00 ; mute square 2
; end of test
.byte $FF, 59 ; wait 1s

; TEST 3
; test channel enable/disable
.byte VA000, $7F ; square 2 on, duty 7, full volume
.byte $FF, 19
.byte VA002, $00
.byte $FF, 19
.byte VA002, $80
.byte $FF, 19
.byte VA002, $00
.byte $FF, 19
.byte VA002, $80
.byte $FF, 19
.byte VA002, $00
.byte $FF, 19
.byte VA002, $80
.byte $FF, 19
.byte VA002, $00
.byte VA002, $80
.byte $FF, 19
.byte VA002, $00
.byte VA002, $80
.byte $FF, 19
.byte VA002, $00
.byte VA002, $80
.byte $FF, 19
.byte VA000, $00 ; square 2 mute
; end of test
.byte $FF, 59 ; wait 1s

; TEST 4
; test duty ignore bit
.byte VA000, $FF ; square 2 on, duty F, full volume
.byte $FF, 19
.byte VA000, $7F ; duty 7
.byte $FF, 19
.byte VA000, $FF ; duty F
.byte $FF, 19
.byte VA000, $7F
.byte $FF, 19
.byte VA000, $FF
.byte $FF, 19
.byte VA000, $7F
.byte $FF, 19
.byte VA000, $FF
.byte $FF, 19
.byte VA000, $7F
.byte $FF, 19
.byte VA000, $FF
.byte $FF, 19
.byte VA000, $00 ; square 2 mute
; end of test
.byte $FF, 59 ; wait 1s

; TEST 5
; test volume in duty ignore mode
.byte V9000, $80
.byte $FF, 9
.byte V9000, $81
.byte $FF, 9
.byte V9000, $82
.byte $FF, 9
.byte V9000, $83
.byte $FF, 9
.byte V9000, $84
.byte $FF, 9
.byte V9000, $85
.byte $FF, 9
.byte V9000, $86
.byte $FF, 9
.byte V9000, $87
.byte $FF, 9
.byte V9000, $88
.byte $FF, 9
.byte V9000, $89
.byte $FF, 9
.byte V9000, $8A
.byte $FF, 9
.byte V9000, $8B
.byte $FF, 9
.byte V9000, $8C
.byte $FF, 9
.byte V9000, $8D
.byte $FF, 9
.byte V9000, $8E
.byte $FF, 9
.byte V9000, $8F
.byte $FF, 9
.byte V9000, $F0
.byte $FF, 9
.byte V9000, $F1
.byte $FF, 9
.byte V9000, $F2
.byte $FF, 9
.byte V9000, $F3
.byte $FF, 9
.byte V9000, $F4
.byte $FF, 9
.byte V9000, $F5
.byte $FF, 9
.byte V9000, $F6
.byte $FF, 9
.byte V9000, $F7
.byte $FF, 9
.byte V9000, $F8
.byte $FF, 9
.byte V9000, $F9
.byte $FF, 9
.byte V9000, $FA
.byte $FF, 9
.byte V9000, $FB
.byte $FF, 9
.byte V9000, $FC
.byte $FF, 9
.byte V9000, $FD
.byte $FF, 9
.byte V9000, $FE
.byte $FF, 9
.byte V9000, $FF
.byte $FF, 9
.byte V9000, $00 ; square 1 mute
; end of test
.byte $FF, 59 ; wait 1s

; TEST 6
; test saw
.byte VB002, $80
.byte VB001, $AB
.byte VB000, $1F
.byte $FF, 29
.byte VB002, $85
.byte VB001, $89
.byte $FF, 29
.byte VB002, $89
.byte VB001, $67
.byte $FF, 29
.byte VB000, $00 ; mute
; end of test
.byte $FF, 59 ; wait 1s

; TEST 7
; test saw volumes
.byte VB002, $86
.byte VB001, $FE
.byte VB000, $00
.byte $FF, 9
.byte VB000, $01
.byte $FF, 9
.byte VB000, $02
.byte $FF, 9
.byte VB000, $03
.byte $FF, 9
.byte VB000, $04
.byte $FF, 9
.byte VB000, $05
.byte $FF, 9
.byte VB000, $06
.byte $FF, 9
.byte VB000, $07
.byte $FF, 9
.byte VB000, $08
.byte $FF, 9
.byte VB000, $09
.byte $FF, 9
.byte VB000, $0A
.byte $FF, 9
.byte VB000, $0B
.byte $FF, 9
.byte VB000, $0C
.byte $FF, 9
.byte VB000, $0D
.byte $FF, 9
.byte VB000, $0E
.byte $FF, 9
.byte VB000, $0F
.byte $FF, 9
.byte VB000, $10
.byte $FF, 9
.byte VB000, $11
.byte $FF, 9
.byte VB000, $12
.byte $FF, 9
.byte VB000, $13
.byte $FF, 9
.byte VB000, $14
.byte $FF, 9
.byte VB000, $15
.byte $FF, 9
.byte VB000, $16
.byte $FF, 9
.byte VB000, $17
.byte $FF, 9
.byte VB000, $18
.byte $FF, 9
.byte VB000, $19
.byte $FF, 9
.byte VB000, $1A
.byte $FF, 9
.byte VB000, $1B
.byte $FF, 9
.byte VB000, $1C
.byte $FF, 9
.byte VB000, $1D
.byte $FF, 9
.byte VB000, $1E
.byte $FF, 9
.byte VB000, $1F
.byte $FF, 9
.byte VB000, $20
.byte $FF, 9
.byte VB000, $21
.byte $FF, 9
.byte VB000, $22
.byte $FF, 9
.byte VB000, $23
.byte $FF, 9
.byte VB000, $24
.byte $FF, 9
.byte VB000, $25
.byte $FF, 9
.byte VB000, $26
.byte $FF, 9
.byte VB000, $27
.byte $FF, 9
.byte VB000, $28
.byte $FF, 9
.byte VB000, $29
.byte $FF, 9
.byte VB000, $2A
.byte $FF, 9
.byte VB000, $2B
.byte $FF, 9
.byte VB000, $2C
.byte $FF, 9
.byte VB000, $2D
.byte $FF, 9
.byte VB000, $2E
.byte $FF, 9
.byte VB000, $2F
.byte $FF, 9
.byte VB000, $30
.byte $FF, 9
.byte VB000, $31
.byte $FF, 9
.byte VB000, $32
.byte $FF, 9
.byte VB000, $33
.byte $FF, 9
.byte VB000, $34
.byte $FF, 9
.byte VB000, $35
.byte $FF, 9
.byte VB000, $36
.byte $FF, 9
.byte VB000, $37
.byte $FF, 9
.byte VB000, $38
.byte $FF, 9
.byte VB000, $39
.byte $FF, 9
.byte VB000, $3A
.byte $FF, 9
.byte VB000, $3B
.byte $FF, 9
.byte VB000, $3C
.byte $FF, 9
.byte VB000, $3D
.byte $FF, 9
.byte VB000, $3E
.byte $FF, 9
.byte VB000, $3F
.byte $FF, 9
.byte VB000, $00 ; mute
; end of test
.byte $FF, 59 ; wait 1s

; TEST 8
; test of $9003 register
.byte V9002, $8F ; square low frequency
.byte V9000, $7F ; square on
.byte $90, $03, $00
.byte $FF, 59
.byte $90, $03, $01
.byte $FF, 59
.byte $90, $03, $02
.byte $FF, 59
.byte $90, $03, $03
.byte $FF, 59
.byte $90, $03, $04
.byte $FF, 59
.byte $90, $03, $05
.byte $FF, 59
.byte $90, $03, $06
.byte $FF, 59
.byte $90, $03, $07
.byte $FF, 59
.byte $90, $03, $08
.byte $FF, 59
.byte $90, $03, $09
.byte $FF, 59
.byte $90, $03, $0A
.byte $FF, 59
.byte $90, $03, $0B
.byte $FF, 59
.byte $90, $03, $0C
.byte $FF, 59
.byte $90, $03, $0D
.byte $FF, 59
.byte $90, $03, $0E
.byte $FF, 59
.byte $90, $03, $0F
.byte $FF, 59
.byte $90, $03, %00010000
.byte $FF, 59
.byte $90, $03, %00100000
.byte $FF, 59
.byte $90, $03, %01000000
.byte $FF, 59
.byte $90, $03, %10000000
.byte $FF, 59
.byte V9000, $00 ; square off
.byte VB002, $8F ; saw low frequency
.byte VB000, $1E ; saw on
.byte $90, $03, $00
.byte $FF, 59
.byte $90, $03, $01
.byte $FF, 59
.byte $90, $03, $02
.byte $FF, 59
.byte $90, $03, $03
.byte $FF, 59
.byte $90, $03, $04
.byte $FF, 59
.byte $90, $03, $05
.byte $FF, 59
.byte $90, $03, $06
.byte $FF, 59
.byte $90, $03, $07
.byte $FF, 59
.byte $90, $03, $08
.byte $FF, 59
.byte $90, $03, $09
.byte $FF, 59
.byte $90, $03, $0A
.byte $FF, 59
.byte $90, $03, $0B
.byte $FF, 59
.byte $90, $03, $0C
.byte $FF, 59
.byte $90, $03, $0D
.byte $FF, 59
.byte $90, $03, $0E
.byte $FF, 59
.byte $90, $03, $0F
.byte $FF, 59
.byte $90, $03, %00010000
.byte $FF, 59
.byte $90, $03, %00100000
.byte $FF, 59
.byte $90, $03, %01000000
.byte $FF, 59
.byte $90, $03, %10000000
.byte $FF, 59
.byte VB000, $00 ; saw off
.byte $90, $03, $00
; end of test
.byte $FF, 59 ; wait 1s

; TEST 9
; test of freq writes with $9003 register in effect
.byte VB000, $1E ; saw on
.byte VB001, $FF
.byte VB002, $8F
.byte $90, $03, %00000000 ; normal
.byte VB001, $FF
.byte $FF, 19
.byte VB001, $F8
.byte $FF, 19
.byte VB001, $FF
.byte $FF, 19
.byte VB001, $8F
.byte $FF, 19
.byte VB002, $8F
.byte $FF, 19
.byte VB002, $88
.byte $FF, 19
.byte $90, $03, %00000010 ; 16x
.byte VB001, $FF
.byte $FF, 19
.byte VB001, $F8
.byte $FF, 19
.byte VB001, $FF
.byte $FF, 19
.byte VB001, $8F
.byte $FF, 19
.byte VB002, $8F
.byte $FF, 19
.byte VB002, $88
.byte $FF, 19
.byte $90, $03, %00000100 ; 256x
.byte VB001, $FF
.byte $FF, 19
.byte VB001, $F8
.byte $FF, 19
.byte VB001, $FF
.byte $FF, 19
.byte VB001, $8F
.byte $FF, 19
.byte VB002, $8F
.byte $FF, 19
.byte VB002, $88
.byte $FF, 19
.byte VB000, $00 ; mute
.byte $90, $03, $00 ; reset frequency control
; end of test
.byte $FF, 59 ; wait 1s

; TEST 10
; test channel enable/disable on saw
.byte VB000, $1F ; saw on
.byte $FF, 19
.byte VB002, $0B
.byte $FF, 19
.byte VB002, $8B
.byte $FF, 19
.byte VB002, $0B
.byte $FF, 19
.byte VB002, $8B
.byte $FF, 19
.byte VB002, $0B
.byte $FF, 19
.byte VB002, $8B
.byte $FF, 19
.byte VB002, $0B
.byte VB002, $8B
.byte $FF, 19
.byte VB002, $0B
.byte VB002, $8B
.byte $FF, 19
.byte VB002, $0B
.byte VB002, $8B
.byte $FF, 19
.byte VB000, $00 ; mute
; end of test
.byte $FF, 59 ; wait 1s

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
