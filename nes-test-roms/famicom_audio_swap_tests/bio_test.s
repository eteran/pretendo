; FDS Swap Test, Bio Test
; Brad Smith, 7/07/2013
;
; This ROM is intended to test the FDS sound chip
; via hotswapping.
;
; This plays the opening of Bio Miracle track 2,
; only the FDS channel.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.segment "HEADER"

.ifndef NSF

; NES header
.byte 'N', 'E', 'S', $1A ; ID
.byte $02 ; 16k PRG bank count (2 = 32kb)
.byte $02 ; 8k CHR bank count (2 = 8kb)
.byte $00, $00 ; mapper (00 = NROM)
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
.byte "FDS Bio Test",         0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
; artist
.byte "bradsmith",      0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
; copyright
.byte "2013", 0,0,0,0,0,0,0,0,0,0,0,0
.byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
; other
.word 16666 ; NTSC speed
.byte 0,0,0,0,0,0,0,0 ; bankswitch init
.word 20000 ; PAL speed
.byte $00 ; PAL/NTSC bits
.byte $04 ; expansion bits
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

song_data = song_test

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
	LDA #$02
	STA $4014
	STA $4014
	; test for NMI
	BIT $2002
	BPL @wait_nmi
	; play the thing
	JSR swap_play
	; do it again
	JMP swap_loop

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
	CMP #$10 ; < $10 = special command
	BCC @do_special
	; else memory write
	JMP @do_write
	; special command
@do_special:
	JSR inc_read
	; do nothing
	JMP @read_loop
	; write command
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
@do_loop:
	LDA SONG_LOOP+0
	STA SONG_READ+0
	LDA SONG_LOOP+1
	STA SONG_READ+1
	JMP @read_loop
	; skip frames command
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
; 0a = load waveform (a=0 square, 1 saw)
; ab cd ef = write ef to memory ab cd (note big endian address)

song_test:

; quick buzz and then give 5 seconds to swap
.byte $40, $11, $7F ; 4011 = 7F
.byte $FF, $00      ; end frame
.byte $40, $11, $00 ; 4011 = 00
.byte $FF, $00      ; end frame
.byte $40, $11, $7F ; 4011 = 7F
.byte $FF, $00      ; end frame
.byte $40, $11, $00 ; 4011 = 00
.byte $FF, $00      ; end frame

.byte $FF, 59
.byte $FF, 59
.byte $FF, 59
.byte $FF, 59
.byte $FF, 59

.byte $40, $11, $7F ; 4011 = 7F
.byte $FF, $00      ; end frame
.byte $40, $11, $00 ; 4011 = 00
.byte $FF, $00      ; end frame
.byte $40, $11, $7F ; 4011 = 7F
.byte $FF, $00      ; end frame
.byte $40, $11, $00 ; 4011 = 00
.byte $FF, $00      ; end frame

.byte $FF, 29

; FDS setup
.byte $40, $23, $00       ; master I/O reset
.byte $40, $23, $83
.byte $40, $83, %10000000 ; disable FDS sound
;.byte $00                 ; load square wave
.byte $40, $80, %11111111 ; max volume
.byte $40, $82, $07       ; A440 low
.byte $40, $83, $C4       ; sound off, disable vol/sweep, A440 high
.byte $40, $84, %10000000 ; sweep gain to 0
.byte $40, $85, $00       ; reset sweep bias
.byte $40, $86, $00       ; mod frequency low to 0
.byte $40, $87, $80       ; mod frequency high to 0, disable mod
.byte $40, $89, %00000000 ; max volume, write protect
.byte $40, $8A, $FF       ; reset envelope speed
; APU setup
.byte $40, $15, %00000000 ; disable APU sound
.byte $40, $00, %00111111 ; APU sq0 max volume, duty 0
.byte $40, $01, %00000000 ; disable sweep
.byte $40, $02, $FD       ; APU sq0 A440
.byte $40, $03, $00
; setup complere, wait 15 frames
.byte $FF, 14

.byte $40, $80, $80
.byte $40, $83, $80
.byte $40, $84, $80
.byte $40, $87, $80
.byte $40, $80, $80
.byte $40, $81, $7f
.byte $FF, 0
.byte $40, $89, $80
.byte $40, $40, $0c
.byte $40, $41, $26
.byte $40, $42, $0d
.byte $40, $43, $35
.byte $40, $44, $26
.byte $40, $45, $18
.byte $40, $46, $1e
.byte $40, $47, $10
.byte $40, $48, $0c
.byte $40, $49, $20
.byte $40, $4a, $0d
.byte $40, $4b, $35
.byte $40, $4c, $24
.byte $40, $4d, $22
.byte $40, $4e, $14
.byte $40, $4f, $15
.byte $40, $50, $0c
.byte $40, $51, $23
.byte $40, $52, $0d
.byte $40, $53, $36
.byte $40, $54, $27
.byte $40, $55, $17
.byte $40, $56, $1d
.byte $40, $57, $12
.byte $40, $58, $0c
.byte $40, $59, $24
.byte $40, $5a, $0e
.byte $40, $5b, $35
.byte $40, $5c, $24
.byte $40, $5d, $21
.byte $40, $5e, $15
.byte $40, $5f, $14
.byte $40, $60, $0c
.byte $40, $61, $26
.byte $40, $62, $0d
.byte $40, $63, $35
.byte $40, $64, $26
.byte $40, $65, $18
.byte $40, $66, $1e
.byte $40, $67, $10
.byte $40, $68, $0c
.byte $40, $69, $20
.byte $40, $6a, $0d
.byte $40, $6b, $35
.byte $40, $6c, $24
.byte $40, $6d, $22
.byte $40, $6e, $14
.byte $40, $6f, $15
.byte $40, $70, $0c
.byte $40, $71, $23
.byte $40, $72, $0d
.byte $40, $73, $36
.byte $40, $74, $27
.byte $40, $75, $17
.byte $40, $76, $1d
.byte $40, $77, $12
.byte $40, $78, $0c
.byte $40, $79, $24
.byte $40, $7a, $0e
.byte $40, $7b, $35
.byte $40, $7c, $24
.byte $40, $7d, $21
.byte $40, $7e, $15
.byte $40, $7f, $14
.byte $40, $89, $00
.byte $40, $83, $80
.byte $40, $84, $80
.byte $40, $87, $80
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $86, $00
.byte $40, $87, $08
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $87, $02
.byte $40, $83, $01
.byte $40, $86, $65
.byte $40, $82, $32
.byte $FF, 0
.byte $40, $84, $6f
.byte $40, $80, $9a
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $FF, 0
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $89
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $86, $65
.byte $40, $87, $02
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $87, $02
.byte $40, $83, $01
.byte $40, $86, $65
.byte $40, $82, $32
.byte $FF, 0
.byte $40, $84, $6f
.byte $40, $80, $9a
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $FF, 0
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $86, $65
.byte $40, $87, $02
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $87, $02
.byte $40, $83, $01
.byte $40, $86, $22
.byte $40, $82, $11
.byte $FF, 0
.byte $40, $84, $6f
.byte $40, $80, $9a
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $FF, 0
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $86, $22
.byte $40, $87, $02
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $87, $02
.byte $40, $83, $01
.byte $40, $86, $03
.byte $40, $82, $01
.byte $FF, 0
.byte $40, $84, $6f
.byte $40, $80, $9a
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $FF, 0
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $86, $03
.byte $40, $87, $02
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $87, $01
.byte $40, $83, $00
.byte $40, $86, $cb
.byte $40, $82, $e5
.byte $FF, 0
.byte $40, $84, $6f
.byte $40, $80, $9a
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $FF, 0
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $80, $80
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.if 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $86, $cb
.byte $40, $87, $01
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $87, $02
.byte $40, $83, $01
.byte $40, $86, $65
.byte $40, $82, $32
.byte $FF, 0
.byte $40, $84, $6f
.byte $40, $80, $9a
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $FF, 0
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $89
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $86, $65
.byte $40, $87, $02
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $87, $02
.byte $40, $83, $01
.byte $40, $86, $65
.byte $40, $82, $32
.byte $FF, 0
.byte $40, $84, $6f
.byte $40, $80, $9a
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $FF, 0
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $86, $65
.byte $40, $87, $02
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $87, $02
.byte $40, $83, $01
.byte $40, $86, $22
.byte $40, $82, $11
.byte $FF, 0
.byte $40, $84, $6f
.byte $40, $80, $9a
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $FF, 0
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $86, $22
.byte $40, $87, $02
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $87, $02
.byte $40, $83, $01
.byte $40, $86, $03
.byte $40, $82, $01
.byte $FF, 0
.byte $40, $84, $6f
.byte $40, $80, $9a
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $FF, 0
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $86, $03
.byte $40, $87, $02
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $87, $01
.byte $40, $83, $00
.byte $40, $86, $cb
.byte $40, $82, $e5
.byte $FF, 0
.byte $40, $84, $6f
.byte $40, $80, $9a
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $FF, 0
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $86, $cb
.byte $40, $87, $01
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $87, $01
.byte $40, $83, $00
.byte $40, $86, $6c
.byte $40, $82, $b6
.byte $FF, 0
.byte $40, $84, $6f
.byte $40, $80, $9a
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $FF, 0
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $86, $6c
.byte $40, $87, $01
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $87, $01
.byte $40, $83, $00
.byte $40, $86, $82
.byte $40, $82, $c1
.byte $FF, 0
.byte $40, $84, $6f
.byte $40, $80, $9a
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $FF, 0
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $86, $82
.byte $40, $87, $01
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $87, $02
.byte $40, $83, $01
.byte $40, $86, $65
.byte $40, $82, $32
.byte $FF, 0
.byte $40, $84, $6f
.byte $40, $80, $9a
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $FF, 0
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $89
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $86, $65
.byte $40, $87, $02
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $87, $02
.byte $40, $83, $01
.byte $40, $86, $65
.byte $40, $82, $32
.byte $FF, 0
.byte $40, $84, $6f
.byte $40, $80, $9a
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $FF, 0
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $86, $65
.byte $40, $87, $02
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $87, $02
.byte $40, $83, $01
.byte $40, $86, $22
.byte $40, $82, $11
.byte $FF, 0
.byte $40, $84, $6f
.byte $40, $80, $9a
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $FF, 0
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $86, $22
.byte $40, $87, $02
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $87, $02
.byte $40, $83, $01
.byte $40, $86, $03
.byte $40, $82, $01
.byte $FF, 0
.byte $40, $84, $6f
.byte $40, $80, $9a
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $FF, 0
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $86, $03
.byte $40, $87, $02
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $87, $01
.byte $40, $83, $00
.byte $40, $86, $cb
.byte $40, $82, $e5
.byte $FF, 0
.byte $40, $84, $6f
.byte $40, $80, $9a
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $FF, 0
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $86, $cb
.byte $40, $87, $01
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $87, $01
.byte $40, $83, $00
.byte $40, $86, $6c
.byte $40, $82, $b6
.byte $FF, 0
.byte $40, $84, $6f
.byte $40, $80, $9a
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $FF, 0
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $86, $6c
.byte $40, $87, $01
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $87, $01
.byte $40, $83, $00
.byte $40, $86, $82
.byte $40, $82, $c1
.byte $FF, 0
.byte $40, $84, $6f
.byte $40, $80, $9a
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $FF, 0
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $86, $82
.byte $40, $87, $01
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $87, $01
.byte $40, $83, $00
.byte $40, $86, $32
.byte $40, $82, $99
.byte $FF, 0
.byte $40, $84, $6f
.byte $40, $80, $9a
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $FF, 0
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $89
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $86, $32
.byte $40, $87, $01
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $87, $01
.byte $40, $83, $00
.byte $40, $86, $6c
.byte $40, $82, $b6
.byte $FF, 0
.byte $40, $84, $6f
.byte $40, $80, $9a
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $FF, 0
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $86, $6c
.byte $40, $87, $01
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $87, $01
.byte $40, $83, $00
.byte $40, $86, $82
.byte $40, $82, $c1
.byte $FF, 0
.byte $40, $84, $6f
.byte $40, $80, $9a
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $FF, 0
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $86, $82
.byte $40, $87, $01
.byte $40, $84, $86
.byte $40, $85, $00
.byte $40, $87, $01
.byte $40, $83, $00
.byte $40, $86, $32
.byte $40, $82, $99
.byte $FF, 0
.byte $40, $84, $6f
.byte $40, $80, $9a
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $FF, 0
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $80, $80
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $89, $80
.byte $40, $40, $00
.byte $40, $41, $0d
.byte $40, $42, $28
.byte $40, $43, $33
.byte $40, $44, $3d
.byte $40, $45, $37
.byte $40, $46, $2a
.byte $40, $47, $09
.byte $40, $48, $00
.byte $40, $49, $0c
.byte $40, $4a, $28
.byte $40, $4b, $34
.byte $40, $4c, $3d
.byte $40, $4d, $37
.byte $40, $4e, $2a
.byte $40, $4f, $0a
.byte $40, $50, $00
.byte $40, $51, $0b
.byte $40, $52, $28
.byte $40, $53, $35
.byte $40, $54, $3e
.byte $40, $55, $36
.byte $40, $56, $29
.byte $40, $57, $0a
.byte $40, $58, $00
.byte $40, $59, $0a
.byte $40, $5a, $28
.byte $40, $5b, $35
.byte $40, $5c, $3e
.byte $40, $5d, $36
.byte $40, $5e, $29
.byte $40, $5f, $0a
.byte $40, $60, $00
.byte $40, $61, $0d
.byte $40, $62, $28
.byte $40, $63, $33
.byte $40, $64, $3d
.byte $40, $65, $37
.byte $40, $66, $2a
.byte $40, $67, $09
.byte $40, $68, $00
.byte $40, $69, $0c
.byte $40, $6a, $28
.byte $40, $6b, $34
.byte $40, $6c, $3d
.byte $40, $6d, $37
.byte $40, $6e, $2a
.byte $40, $6f, $0a
.byte $40, $70, $00
.byte $40, $71, $0b
.byte $40, $72, $28
.byte $40, $73, $35
.byte $40, $74, $3e
.byte $40, $75, $36
.byte $40, $76, $29
.byte $40, $77, $0a
.byte $40, $78, $00
.byte $40, $79, $0a
.byte $40, $7a, $28
.byte $40, $7b, $35
.byte $40, $7c, $3e
.byte $40, $7d, $36
.byte $40, $7e, $29
.byte $40, $7f, $0a
.byte $40, $89, $00
.byte $40, $83, $80
.byte $40, $84, $80
.byte $40, $87, $80
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $32
.byte $40, $87, $01
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $02
.byte $40, $83, $01
.byte $40, $86, $65
.byte $40, $82, $32
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $65
.byte $40, $87, $02
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $02
.byte $40, $83, $01
.byte $40, $86, $65
.byte $40, $82, $32
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $65
.byte $40, $87, $02
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $04
.byte $40, $83, $02
.byte $40, $86, $44
.byte $40, $82, $22
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $44
.byte $40, $87, $04
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $04
.byte $40, $83, $02
.byte $40, $86, $44
.byte $40, $82, $22
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $44
.byte $40, $87, $04
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $04
.byte $40, $83, $02
.byte $40, $86, $07
.byte $40, $82, $03
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $80, $80
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $07
.byte $40, $87, $04
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $03
.byte $40, $83, $01
.byte $40, $86, $96
.byte $40, $82, $cb
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $80, $80
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $96
.byte $40, $87, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $04
.byte $40, $83, $02
.byte $40, $86, $07
.byte $40, $82, $03
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $07
.byte $40, $87, $04
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $03
.byte $40, $83, $01
.byte $40, $86, $96
.byte $40, $82, $cb
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $96
.byte $40, $87, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $02
.byte $40, $83, $01
.byte $40, $86, $d9
.byte $40, $82, $6c
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $d9
.byte $40, $87, $02
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $03
.byte $40, $83, $01
.byte $40, $86, $04
.byte $40, $82, $82
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $04
.byte $40, $87, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $02
.byte $40, $83, $01
.byte $40, $86, $65
.byte $40, $82, $32
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $65
.byte $40, $87, $02
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $02
.byte $40, $83, $01
.byte $40, $86, $65
.byte $40, $82, $32
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $65
.byte $40, $87, $02
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $03
.byte $40, $83, $01
.byte $40, $86, $04
.byte $40, $82, $82
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $04
.byte $40, $87, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $04
.byte $40, $83, $02
.byte $40, $86, $44
.byte $40, $82, $22
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $44
.byte $40, $87, $04
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $04
.byte $40, $83, $02
.byte $40, $86, $44
.byte $40, $82, $22
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $44
.byte $40, $87, $04
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $04
.byte $40, $83, $02
.byte $40, $86, $07
.byte $40, $82, $03
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $80, $80
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $07
.byte $40, $87, $04
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $03
.byte $40, $83, $01
.byte $40, $86, $96
.byte $40, $82, $cb
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $80, $80
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $96
.byte $40, $87, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $04
.byte $40, $83, $02
.byte $40, $86, $07
.byte $40, $82, $03
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $07
.byte $40, $87, $04
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $04
.byte $40, $83, $02
.byte $40, $86, $44
.byte $40, $82, $22
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $44
.byte $40, $87, $04
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $04
.byte $40, $83, $02
.byte $40, $86, $ca
.byte $40, $82, $65
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $ca
.byte $40, $87, $04
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $05
.byte $40, $83, $02
.byte $40, $86, $60
.byte $40, $82, $b0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $60
.byte $40, $87, $05
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $06
.byte $40, $83, $03
.byte $40, $86, $64
.byte $40, $82, $32
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $64
.byte $40, $87, $06
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $06
.byte $40, $83, $03
.byte $40, $86, $08
.byte $40, $82, $04
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $08
.byte $40, $87, $06
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $04
.byte $40, $83, $02
.byte $40, $86, $ca
.byte $40, $82, $65
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $ca
.byte $40, $87, $04
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $08
.byte $40, $83, $04
.byte $40, $86, $0e
.byte $40, $82, $07
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $0e
.byte $40, $87, $08
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $07
.byte $40, $83, $03
.byte $40, $86, $9a
.byte $40, $82, $cd
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $9a
.byte $40, $87, $07
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $07
.byte $40, $83, $03
.byte $40, $86, $2d
.byte $40, $82, $96
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $2d
.byte $40, $87, $07
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $06
.byte $40, $83, $03
.byte $40, $86, $64
.byte $40, $82, $32
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $64
.byte $40, $87, $06
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $04
.byte $40, $83, $02
.byte $40, $86, $ca
.byte $40, $82, $65
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $ca
.byte $40, $87, $04
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $06
.byte $40, $83, $03
.byte $40, $86, $64
.byte $40, $82, $32
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $80, $80
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $64
.byte $40, $87, $06
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $06
.byte $40, $83, $03
.byte $40, $86, $08
.byte $40, $82, $04
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $80, $80
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $08
.byte $40, $87, $06
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $05
.byte $40, $83, $02
.byte $40, $86, $60
.byte $40, $82, $b0
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $60
.byte $40, $87, $05
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $06
.byte $40, $83, $03
.byte $40, $86, $08
.byte $40, $82, $04
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $08
.byte $40, $87, $06
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $04
.byte $40, $83, $02
.byte $40, $86, $ca
.byte $40, $82, $65
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $ca
.byte $40, $87, $04
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $03
.byte $40, $83, $01
.byte $40, $86, $96
.byte $40, $82, $cb
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $96
.byte $40, $87, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $03
.byte $40, $83, $01
.byte $40, $86, $04
.byte $40, $82, $82
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $04
.byte $40, $87, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $03
.byte $40, $83, $01
.byte $40, $86, $32
.byte $40, $82, $99
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $32
.byte $40, $87, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $03
.byte $40, $83, $01
.byte $40, $86, $63
.byte $40, $82, $b1
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $63
.byte $40, $87, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $03
.byte $40, $83, $01
.byte $40, $86, $96
.byte $40, $82, $cb
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $96
.byte $40, $87, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $03
.byte $40, $83, $01
.byte $40, $86, $96
.byte $40, $82, $cb
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $96
.byte $40, $87, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $03
.byte $40, $83, $01
.byte $40, $86, $04
.byte $40, $82, $82
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $04
.byte $40, $87, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $03
.byte $40, $83, $01
.byte $40, $86, $32
.byte $40, $82, $99
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $32
.byte $40, $87, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $03
.byte $40, $83, $01
.byte $40, $86, $63
.byte $40, $82, $b1
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $63
.byte $40, $87, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $03
.byte $40, $83, $01
.byte $40, $86, $96
.byte $40, $82, $cb
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $96
.byte $40, $87, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $03
.byte $40, $83, $01
.byte $40, $86, $96
.byte $40, $82, $cb
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $96
.byte $40, $87, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $03
.byte $40, $83, $01
.byte $40, $86, $04
.byte $40, $82, $82
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $04
.byte $40, $87, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $03
.byte $40, $83, $01
.byte $40, $86, $04
.byte $40, $82, $82
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $04
.byte $40, $87, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $02
.byte $40, $83, $01
.byte $40, $86, $d9
.byte $40, $82, $6c
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $d9
.byte $40, $87, $02
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $02
.byte $40, $83, $01
.byte $40, $86, $b0
.byte $40, $82, $58
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $b0
.byte $40, $87, $02
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $02
.byte $40, $83, $01
.byte $40, $86, $65
.byte $40, $82, $32
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $89, $80
.byte $40, $40, $07
.byte $40, $41, $0d
.byte $40, $42, $10
.byte $40, $43, $20
.byte $40, $44, $33
.byte $40, $45, $0a
.byte $40, $46, $35
.byte $40, $47, $21
.byte $40, $48, $15
.byte $40, $49, $12
.byte $40, $4a, $0a
.byte $40, $4b, $2e
.byte $40, $4c, $16
.byte $40, $4d, $1f
.byte $40, $4e, $37
.byte $40, $4f, $35
.byte $40, $50, $32
.byte $40, $51, $1e
.byte $40, $52, $11
.byte $40, $53, $2e
.byte $40, $54, $09
.byte $40, $55, $17
.byte $40, $56, $1f
.byte $40, $57, $26
.byte $40, $58, $34
.byte $40, $59, $0b
.byte $40, $5a, $2f
.byte $40, $5b, $1e
.byte $40, $5c, $0a
.byte $40, $5d, $0b
.byte $40, $5e, $0b
.byte $40, $5f, $23
.byte $40, $60, $07
.byte $40, $61, $0d
.byte $40, $62, $10
.byte $40, $63, $20
.byte $40, $64, $33
.byte $40, $65, $0a
.byte $40, $66, $35
.byte $40, $67, $21
.byte $40, $68, $15
.byte $40, $69, $12
.byte $40, $6a, $0a
.byte $40, $6b, $2e
.byte $40, $6c, $16
.byte $40, $6d, $1f
.byte $40, $6e, $37
.byte $40, $6f, $35
.byte $40, $70, $32
.byte $40, $71, $1e
.byte $40, $72, $11
.byte $40, $73, $2e
.byte $40, $74, $09
.byte $40, $75, $17
.byte $40, $76, $1f
.byte $40, $77, $26
.byte $40, $78, $34
.byte $40, $79, $0b
.byte $40, $7a, $2f
.byte $40, $7b, $1e
.byte $40, $7c, $0a
.byte $40, $7d, $0b
.byte $40, $7e, $0b
.byte $40, $7f, $23
.byte $40, $89, $00
.byte $40, $83, $80
.byte $40, $84, $80
.byte $40, $87, $80
.byte $40, $80, $94
.byte $40, $80, $80
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $01
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $84, $88
.byte $40, $85, $00
.byte $40, $86, $65
.byte $40, $87, $02
.byte $40, $84, $88
.byte $40, $85, $00
.byte $40, $87, $01
.byte $40, $83, $00
.byte $40, $86, $32
.byte $40, $82, $99
.byte $FF, 0
.byte $40, $84, $01
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $90
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $82
.byte $FF, 0
.byte $40, $80, $82
.byte $FF, 0
.byte $40, $80, $94
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $01
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $84, $88
.byte $40, $85, $00
.byte $40, $86, $32
.byte $40, $87, $01
.byte $40, $84, $88
.byte $40, $85, $00
.byte $40, $87, $01
.byte $40, $83, $00
.byte $40, $86, $32
.byte $40, $82, $99
.byte $FF, 0
.byte $40, $84, $01
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $90
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $82
.byte $FF, 0
.byte $40, $80, $82
.byte $FF, 0
.byte $40, $80, $94
.byte $40, $80, $80
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $01
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $84, $88
.byte $40, $85, $00
.byte $40, $86, $32
.byte $40, $87, $01
.byte $40, $84, $88
.byte $40, $85, $00
.byte $40, $87, $01
.byte $40, $83, $00
.byte $40, $86, $32
.byte $40, $82, $99
.byte $FF, 0
.byte $40, $84, $01
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $90
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $82
.byte $FF, 0
.byte $40, $80, $82
.byte $FF, 0
.byte $40, $80, $94
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $01
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $84, $88
.byte $40, $85, $00
.byte $40, $86, $32
.byte $40, $87, $01
.byte $40, $84, $88
.byte $40, $85, $00
.byte $40, $87, $01
.byte $40, $83, $00
.byte $40, $86, $32
.byte $40, $82, $99
.byte $FF, 0
.byte $40, $84, $01
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $90
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $82
.byte $FF, 0
.byte $40, $80, $82
.byte $FF, 0
.byte $40, $80, $94
.byte $40, $80, $80
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $94
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $01
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $84, $88
.byte $40, $85, $00
.byte $40, $86, $32
.byte $40, $87, $01
.byte $40, $84, $88
.byte $40, $85, $00
.byte $40, $87, $01
.byte $40, $83, $00
.byte $40, $86, $32
.byte $40, $82, $99
.byte $FF, 0
.byte $40, $84, $01
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $90
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $82
.byte $FF, 0
.byte $40, $80, $82
.byte $FF, 0
.byte $40, $80, $94
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $01
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $84, $88
.byte $40, $85, $00
.byte $40, $86, $32
.byte $40, $87, $01
.byte $40, $84, $88
.byte $40, $85, $00
.byte $40, $87, $01
.byte $40, $83, $00
.byte $40, $86, $32
.byte $40, $82, $99
.byte $FF, 0
.byte $40, $84, $01
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $90
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $82
.byte $FF, 0
.byte $40, $80, $82
.byte $FF, 0
.byte $40, $80, $94
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $01
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $84, $88
.byte $40, $85, $00
.byte $40, $86, $32
.byte $40, $87, $01
.byte $40, $84, $88
.byte $40, $85, $00
.byte $40, $87, $01
.byte $40, $83, $00
.byte $40, $86, $6c
.byte $40, $82, $b6
.byte $FF, 0
.byte $40, $84, $01
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $90
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $82
.byte $FF, 0
.byte $40, $80, $82
.byte $FF, 0
.byte $40, $80, $94
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $01
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $84, $88
.byte $40, $85, $00
.byte $40, $86, $6c
.byte $40, $87, $01
.byte $40, $84, $88
.byte $40, $85, $00
.byte $40, $87, $01
.byte $40, $83, $00
.byte $40, $86, $82
.byte $40, $82, $c1
.byte $FF, 0
.byte $40, $84, $01
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $90
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $82
.byte $FF, 0
.byte $40, $80, $82
.byte $FF, 0
.byte $40, $80, $94
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $01
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $01
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $00
.byte $40, $88, $07
.byte $40, $88, $07
.byte $40, $84, $88
.byte $40, $85, $00
.byte $40, $86, $82
.byte $40, $87, $01
.byte $40, $84, $88
.byte $40, $85, $00
.byte $40, $87, $01
.byte $40, $83, $00
.byte $40, $86, $32
.byte $40, $82, $99
.byte $FF, 0
.byte $40, $84, $01
.byte $40, $80, $92
.byte $FF, 0
.byte $40, $80, $90
.byte $FF, 0
.byte $40, $80, $8e
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $82
.byte $FF, 0
.byte $40, $80, $82
.byte $FF, 0
.byte $40, $80, $94
.byte $40, $80, $80
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $89, $80
.byte $40, $40, $00
.byte $40, $41, $0d
.byte $40, $42, $28
.byte $40, $43, $33
.byte $40, $44, $3d
.byte $40, $45, $37
.byte $40, $46, $2a
.byte $40, $47, $09
.byte $40, $48, $00
.byte $40, $49, $0c
.byte $40, $4a, $28
.byte $40, $4b, $34
.byte $40, $4c, $3d
.byte $40, $4d, $37
.byte $40, $4e, $2a
.byte $40, $4f, $0a
.byte $40, $50, $00
.byte $40, $51, $0b
.byte $40, $52, $28
.byte $40, $53, $35
.byte $40, $54, $3e
.byte $40, $55, $36
.byte $40, $56, $29
.byte $40, $57, $0a
.byte $40, $58, $00
.byte $40, $59, $0a
.byte $40, $5a, $28
.byte $40, $5b, $35
.byte $40, $5c, $3e
.byte $40, $5d, $36
.byte $40, $5e, $29
.byte $40, $5f, $0a
.byte $40, $60, $00
.byte $40, $61, $0d
.byte $40, $62, $28
.byte $40, $63, $33
.byte $40, $64, $3d
.byte $40, $65, $37
.byte $40, $66, $2a
.byte $40, $67, $09
.byte $40, $68, $00
.byte $40, $69, $0c
.byte $40, $6a, $28
.byte $40, $6b, $34
.byte $40, $6c, $3d
.byte $40, $6d, $37
.byte $40, $6e, $2a
.byte $40, $6f, $0a
.byte $40, $70, $00
.byte $40, $71, $0b
.byte $40, $72, $28
.byte $40, $73, $35
.byte $40, $74, $3e
.byte $40, $75, $36
.byte $40, $76, $29
.byte $40, $77, $0a
.byte $40, $78, $00
.byte $40, $79, $0a
.byte $40, $7a, $28
.byte $40, $7b, $35
.byte $40, $7c, $3e
.byte $40, $7d, $36
.byte $40, $7e, $29
.byte $40, $7f, $0a
.byte $40, $89, $00
.byte $40, $83, $80
.byte $40, $84, $80
.byte $40, $87, $80
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $32
.byte $40, $87, $01
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $02
.byte $40, $83, $01
.byte $40, $86, $65
.byte $40, $82, $32
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $65
.byte $40, $87, $02
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $02
.byte $40, $83, $01
.byte $40, $86, $65
.byte $40, $82, $32
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $65
.byte $40, $87, $02
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $04
.byte $40, $83, $02
.byte $40, $86, $44
.byte $40, $82, $22
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $44
.byte $40, $87, $04
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $04
.byte $40, $83, $02
.byte $40, $86, $44
.byte $40, $82, $22
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $44
.byte $40, $87, $04
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $04
.byte $40, $83, $02
.byte $40, $86, $07
.byte $40, $82, $03
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $80, $80
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $07
.byte $40, $87, $04
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $03
.byte $40, $83, $01
.byte $40, $86, $96
.byte $40, $82, $cb
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $40, $80, $86
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $84
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $80, $80
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $FF, 0
.byte $40, $80, $8e
.byte $40, $87, $80
.byte $40, $85, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $05
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $00
.byte $40, $88, $03
.byte $40, $88, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $86, $96
.byte $40, $87, $03
.byte $40, $84, $a0
.byte $40, $85, $00
.byte $40, $87, $04
.byte $40, $83, $02
.byte $40, $86, $07
.byte $40, $82, $03
.byte $FF, 0
.byte $40, $80, $8c
.byte $FF, 0
.byte $40, $80, $8a
.byte $FF, 0
.byte $40, $84, $3f
.byte $40, $80, $88
.byte $FF, 0
.byte $40, $80, $86
.endif

; loop
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
