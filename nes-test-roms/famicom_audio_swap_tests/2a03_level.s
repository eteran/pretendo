; 2A03 Test
; Brad Smith, 7/16/2013
;
; This ROM is intended to test the 2A03 APU.
; it verifies the output levels

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.segment "HEADER"

.ifndef NSF

; NES header
.byte 'N', 'E', 'S', $1A ; ID
.byte $02 ; 16k PRG bank count (2 = 32kb)
.byte $02 ; 8k CHR bank count (2 = 8kb)
.byte $00, $00 ; mapper 0
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
.byte "2a03 level",       0,0,0,0,0,0
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
.byte %00001000 ; expansion bits
.byte 0,0,0,0 ; pad to $80

.endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.segment "CODE"

SONG_READ  = $00 ; current data read pointer
SONG_LOOP  = $02 ; initial data read pointer
WRITE_ADDR = $04 ; memory address to write
SKIP       = $06 ; frames to skip

.import __SWAP_LOAD__
.import __SWAP_RUN__

song_data = song_2a03_test

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

song_2a03_test:

; APU setup
.byte $40, $15, %00000000 ; disable APU sound
.byte $40, $00, %00111111 ; APU sq0 max volume, duty 0
.byte $40, $01, %00000000 ; disable sweep
.byte $40, $02, $FD       ; APU sq0 A440
.byte $40, $03, $00
; setup complere, wait 15 frames
.byte $FF, 14

; half second A440 APU sq0 duty 0 (for polarity reference)
.byte $40, $15, %00000001 ; APU sq0 enable
.byte $40, $03, $00       ; reset length counter
.byte $FF, 29
.byte $40, $15, %00000000 ; APU sq0 off
.byte $FF, 29
; half second A440 APU sq0 duty 2 (square, for comparison)
.byte $40, $00, %10111111 ; max volume, duty 2
.byte $40, $15, %00000001 ; APU sq0 enable
.byte $40, $03, $00       ; reset length counter
.byte $FF, 29
.byte $40, $15, %00000000 ; APU sq0 off
.byte $FF, 29

; test noise
.byte $40, $0E, $0F       ; lowest noise period
.byte $40, $0C, $3F       ; max volume
.byte $40, $15, %00001000 ; APU noise enable 
.byte $40, $0F, $00       ; reset length counter
.byte $FF, 29
.byte $40, $15, %00000000 ; APU off
.byte $FF, 29

; test triangle
.byte $40, $0A, $7E       ; A440
.byte $40, $08, $FF       ; disable length counter
.byte $40, $15, %00000100 ; APU tri enable 
.byte $40, $0B, $00       ; reset length counter
.byte $FF, 29
.byte $40, $15, %00000000 ; APU off
.byte $FF, 29

; END of test loop
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
.byte $FF, $59      ; wait 1s

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
