; FDS Swap Test
; Brad Smith, 6/29/2013
;
; This ROM is intended to test the FDS sound chip
; via hotswapping.
;
; This tests a square wave output at all volume
; settings, and also a saw ramp (increasing value).

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
.byte "FDS swap test",          0,0,0
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
	; multiply A by 64 to select waveform pos
	ASL
	ASL
	ASL
	ASL
	ASL
	ASL
	TAY
	LDX #00
	; enable waveform write
	LDA #%10000000
	STA $4089
	; write waveform to $4040-407F
@wave_loop:
	LDA wave, Y
	STA $4040, X
	INY
	INX
	CPX #64
	BNE @wave_loop
	; disable waveform write
	LDA #%00000000
	STA $4089
	; fill the mod table too
	LDX #0
@mod_loop:
	LDA mod, X
	STA $4088
	INX
	CPX #32
	BNE @mod_loop
	; done
	LDY #0
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
; waveforms
;

wave:
; 0-63 = square
.byte 63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63
.byte 63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63
.byte  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
.byte  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
; 64-127 = saw up
.byte  0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15
.byte 16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
.byte 32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47
.byte 48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63
; 128-191 = ???
; 192-255 = ???

mod:
.byte  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
.byte  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

;
; song data
;

; variable length code:
; FF ab = return, skip the next ab frames
; FE = loop
; 0a = load waveform (a=0 square, 1 saw)
; ab cd ef = write ef to memory ab cd (note big endian address)

song_test:

; FDS setup
.byte $40, $23, $00       ; master I/O reset
.byte $40, $23, $83
.byte $40, $83, %10000000 ; disable FDS sound
.byte $00                 ; load square wave
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

; half second A440 FDS square
.byte $40, $83, $44
.byte $FF, 29
.byte $40, $83, $C4
.byte $FF, 29

; volume tests

.byte $40, $89, 0         ; max volume
.byte $40, $83, $44       ; sound on
.byte $40, $80, $C0 + 63, $FF, 9
.byte $40, $80, $C0 + 62, $FF, 9
.byte $40, $80, $C0 + 61, $FF, 9
.byte $40, $80, $C0 + 60, $FF, 9
.byte $40, $80, $C0 + 59, $FF, 9
.byte $40, $80, $C0 + 58, $FF, 9
.byte $40, $80, $C0 + 57, $FF, 9
.byte $40, $80, $C0 + 56, $FF, 9
.byte $40, $80, $C0 + 55, $FF, 9
.byte $40, $80, $C0 + 54, $FF, 9
.byte $40, $80, $C0 + 53, $FF, 9
.byte $40, $80, $C0 + 52, $FF, 9
.byte $40, $80, $C0 + 51, $FF, 9
.byte $40, $80, $C0 + 50, $FF, 9
.byte $40, $80, $C0 + 49, $FF, 9
.byte $40, $80, $C0 + 48, $FF, 9
.byte $40, $80, $C0 + 47, $FF, 9
.byte $40, $80, $C0 + 46, $FF, 9
.byte $40, $80, $C0 + 45, $FF, 9
.byte $40, $80, $C0 + 44, $FF, 9
.byte $40, $80, $C0 + 43, $FF, 9
.byte $40, $80, $C0 + 42, $FF, 9
.byte $40, $80, $C0 + 41, $FF, 9
.byte $40, $80, $C0 + 40, $FF, 9
.byte $40, $80, $C0 + 39, $FF, 9
.byte $40, $80, $C0 + 38, $FF, 9
.byte $40, $80, $C0 + 37, $FF, 9
.byte $40, $80, $C0 + 36, $FF, 9
.byte $40, $80, $C0 + 35, $FF, 9
.byte $40, $80, $C0 + 34, $FF, 9
.byte $40, $80, $C0 + 33, $FF, 9
.byte $40, $80, $C0 + 32, $FF, 9
.byte $40, $80, $C0 + 31, $FF, 9
.byte $40, $80, $C0 + 30, $FF, 9
.byte $40, $80, $C0 + 29, $FF, 9
.byte $40, $80, $C0 + 28, $FF, 9
.byte $40, $80, $C0 + 27, $FF, 9
.byte $40, $80, $C0 + 26, $FF, 9
.byte $40, $80, $C0 + 25, $FF, 9
.byte $40, $80, $C0 + 24, $FF, 9
.byte $40, $80, $C0 + 23, $FF, 9
.byte $40, $80, $C0 + 22, $FF, 9
.byte $40, $80, $C0 + 21, $FF, 9
.byte $40, $80, $C0 + 20, $FF, 9
.byte $40, $80, $C0 + 19, $FF, 9
.byte $40, $80, $C0 + 18, $FF, 9
.byte $40, $80, $C0 + 17, $FF, 9
.byte $40, $80, $C0 + 16, $FF, 9
.byte $40, $80, $C0 + 15, $FF, 9
.byte $40, $80, $C0 + 14, $FF, 9
.byte $40, $80, $C0 + 13, $FF, 9
.byte $40, $80, $C0 + 12, $FF, 9
.byte $40, $80, $C0 + 11, $FF, 9
.byte $40, $80, $C0 + 10, $FF, 9
.byte $40, $80, $C0 +  9, $FF, 9
.byte $40, $80, $C0 +  8, $FF, 9
.byte $40, $80, $C0 +  7, $FF, 9
.byte $40, $80, $C0 +  6, $FF, 9
.byte $40, $80, $C0 +  5, $FF, 9
.byte $40, $80, $C0 +  4, $FF, 9
.byte $40, $80, $C0 +  3, $FF, 9
.byte $40, $80, $C0 +  2, $FF, 9
.byte $40, $80, $C0 +  1, $FF, 9
.byte $40, $80, $C0 +  0, $FF, 9
.byte $40, $83, $C4       ; sound off
.byte $FF, 59             ; 1 second silence

; "I'm alive!" APU chirp
.byte $40, $15, %00000001
.byte $40, $03, $00
.byte $FF, 9
.byte $40, $15, %00000000
.byte $FF, 9

.byte $40, $89, 1         ; 2/3 volume ?
.byte $40, $83, $44       ; sound on
.byte $40, $80, $C0 + 63, $FF, 9
.byte $40, $80, $C0 + 61, $FF, 9
.byte $40, $80, $C0 + 59, $FF, 9
.byte $40, $80, $C0 + 57, $FF, 9
.byte $40, $80, $C0 + 55, $FF, 9
.byte $40, $80, $C0 + 53, $FF, 9
.byte $40, $80, $C0 + 51, $FF, 9
.byte $40, $80, $C0 + 49, $FF, 9
.byte $40, $80, $C0 + 47, $FF, 9
.byte $40, $80, $C0 + 45, $FF, 9
.byte $40, $80, $C0 + 43, $FF, 9
.byte $40, $80, $C0 + 41, $FF, 9
.byte $40, $80, $C0 + 39, $FF, 9
.byte $40, $80, $C0 + 37, $FF, 9
.byte $40, $80, $C0 + 35, $FF, 9
.byte $40, $80, $C0 + 33, $FF, 9
.byte $40, $80, $C0 + 31, $FF, 9
.byte $40, $80, $C0 + 29, $FF, 9
.byte $40, $80, $C0 + 27, $FF, 9
.byte $40, $80, $C0 + 25, $FF, 9
.byte $40, $80, $C0 + 23, $FF, 9
.byte $40, $80, $C0 + 21, $FF, 9
.byte $40, $80, $C0 + 19, $FF, 9
.byte $40, $80, $C0 + 17, $FF, 9
.byte $40, $80, $C0 + 15, $FF, 9
.byte $40, $80, $C0 + 13, $FF, 9
.byte $40, $80, $C0 + 11, $FF, 9
.byte $40, $80, $C0 +  9, $FF, 9
.byte $40, $80, $C0 +  7, $FF, 9
.byte $40, $80, $C0 +  5, $FF, 9
.byte $40, $80, $C0 +  3, $FF, 9
.byte $40, $80, $C0 +  1, $FF, 9
.byte $40, $80, $C0 +  0, $FF, 9
.byte $40, $83, $C4       ; sound off
.byte $FF, 59             ; 1 second silence

; "I'm alive!" APU chirp
.byte $40, $15, %00000001
.byte $40, $03, $00
.byte $FF, 9
.byte $40, $15, %00000000
.byte $FF, 9

.byte $40, $89, 2         ; 2/4 volume ?
.byte $40, $83, $44       ; sound on
.byte $40, $80, $C0 + 63, $FF, 9
.byte $40, $80, $C0 + 61, $FF, 9
.byte $40, $80, $C0 + 59, $FF, 9
.byte $40, $80, $C0 + 57, $FF, 9
.byte $40, $80, $C0 + 55, $FF, 9
.byte $40, $80, $C0 + 53, $FF, 9
.byte $40, $80, $C0 + 51, $FF, 9
.byte $40, $80, $C0 + 49, $FF, 9
.byte $40, $80, $C0 + 47, $FF, 9
.byte $40, $80, $C0 + 45, $FF, 9
.byte $40, $80, $C0 + 43, $FF, 9
.byte $40, $80, $C0 + 41, $FF, 9
.byte $40, $80, $C0 + 39, $FF, 9
.byte $40, $80, $C0 + 37, $FF, 9
.byte $40, $80, $C0 + 35, $FF, 9
.byte $40, $80, $C0 + 33, $FF, 9
.byte $40, $80, $C0 + 31, $FF, 9
.byte $40, $80, $C0 + 29, $FF, 9
.byte $40, $80, $C0 + 27, $FF, 9
.byte $40, $80, $C0 + 25, $FF, 9
.byte $40, $80, $C0 + 23, $FF, 9
.byte $40, $80, $C0 + 21, $FF, 9
.byte $40, $80, $C0 + 19, $FF, 9
.byte $40, $80, $C0 + 17, $FF, 9
.byte $40, $80, $C0 + 15, $FF, 9
.byte $40, $80, $C0 + 13, $FF, 9
.byte $40, $80, $C0 + 11, $FF, 9
.byte $40, $80, $C0 +  9, $FF, 9
.byte $40, $80, $C0 +  7, $FF, 9
.byte $40, $80, $C0 +  5, $FF, 9
.byte $40, $80, $C0 +  3, $FF, 9
.byte $40, $80, $C0 +  1, $FF, 9
.byte $40, $80, $C0 +  0, $FF, 9
.byte $40, $83, $C4       ; sound off
.byte $FF, 59             ; 1 second silence

; "I'm alive!" APU chirp
.byte $40, $15, %00000001
.byte $40, $03, $00
.byte $FF, 9
.byte $40, $15, %00000000
.byte $FF, 9

.byte $40, $89, 3         ; 2/5 volume ?
.byte $40, $83, $44       ; sound on
.byte $40, $80, $C0 + 63, $FF, 9
.byte $40, $80, $C0 + 61, $FF, 9
.byte $40, $80, $C0 + 59, $FF, 9
.byte $40, $80, $C0 + 57, $FF, 9
.byte $40, $80, $C0 + 55, $FF, 9
.byte $40, $80, $C0 + 53, $FF, 9
.byte $40, $80, $C0 + 51, $FF, 9
.byte $40, $80, $C0 + 49, $FF, 9
.byte $40, $80, $C0 + 47, $FF, 9
.byte $40, $80, $C0 + 45, $FF, 9
.byte $40, $80, $C0 + 43, $FF, 9
.byte $40, $80, $C0 + 41, $FF, 9
.byte $40, $80, $C0 + 39, $FF, 9
.byte $40, $80, $C0 + 37, $FF, 9
.byte $40, $80, $C0 + 35, $FF, 9
.byte $40, $80, $C0 + 33, $FF, 9
.byte $40, $80, $C0 + 31, $FF, 9
.byte $40, $80, $C0 + 29, $FF, 9
.byte $40, $80, $C0 + 27, $FF, 9
.byte $40, $80, $C0 + 25, $FF, 9
.byte $40, $80, $C0 + 23, $FF, 9
.byte $40, $80, $C0 + 21, $FF, 9
.byte $40, $80, $C0 + 19, $FF, 9
.byte $40, $80, $C0 + 17, $FF, 9
.byte $40, $80, $C0 + 15, $FF, 9
.byte $40, $80, $C0 + 13, $FF, 9
.byte $40, $80, $C0 + 11, $FF, 9
.byte $40, $80, $C0 +  9, $FF, 9
.byte $40, $80, $C0 +  7, $FF, 9
.byte $40, $80, $C0 +  5, $FF, 9
.byte $40, $80, $C0 +  3, $FF, 9
.byte $40, $80, $C0 +  1, $FF, 9
.byte $40, $80, $C0 +  0, $FF, 9
.byte $40, $83, $C4       ; sound off
.byte $FF, 59             ; 1 second silence

; "I'm alive!" APU chirp
.byte $40, $15, %00000001
.byte $40, $03, $00
.byte $FF, 9
.byte $40, $15, %00000000
.byte $FF, 9

; full second A440 FDS saw
.byte $01                 ; load saw wave
.byte $40, $89, 0         ; full global volume
.byte $40, $80, $C0 +  63 ; full volume
.byte $40, $83, $44       ; turn on
.byte $FF, 59
.byte $40, $83, $C4       ; turn off
.byte $FF, 59

; buzz 4011 to signify end of loop
.byte $40, $11, $7F ; 4011 = 7F
.byte $FF, $00      ; end frame
.byte $40, $11, $00 ; 4011 = 00
.byte $FF, $00      ; end frame
.byte $40, $11, $7F ; 4011 = 7F
.byte $FF, $00      ; end frame
.byte $40, $11, $00 ; 4011 = 00
.byte $FF, $00      ; end frame
.byte $40, $11, $7F ; 4011 = 7F
.byte $FF, $00      ; end frame
.byte $40, $11, $00 ; 4011 = 00
.byte $FF, $00      ; end frame
.byte $40, $11, $7F ; 4011 = 7F
.byte $FF, $00      ; end frame
.byte $40, $11, $00 ; 4011 = 00
.byte $FF, $00      ; end frame
.byte $40, $11, $7F ; 4011 = 7F
.byte $FF, $00      ; end frame
.byte $40, $11, $00 ; 4011 = 00
.byte $FF, $59      ; end frame, wait 1s

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
