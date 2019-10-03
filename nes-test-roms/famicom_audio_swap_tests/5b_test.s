; 5B Swap Test
; Brad Smith, 5/01/2012
;
; This ROM is intended to test the Sunsoft 5B chip
; via hotswapping. It verifies the presence of noise
; and envelope generators on the chip, and some of
; their operation.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.segment "HEADER"

.ifndef NSF

; NES header
.byte 'N', 'E', 'S', $1A ; ID
.byte $02 ; 16k PRG bank count (2 = 32kb)
.byte $02 ; 8k CHR bank count (2 = 8kb)
.byte $50, $40 ; mapper (69 = $45 = FME7)
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
.byte "5b swap test",         0,0,0,0
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
.byte $20 ; expansion bits
.byte 0,0,0,0 ; pad to $80

.endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.segment "CODE"

SONG_READ  = $00 ; current data read pointer
SONG_LOOP  = $02 ; initial data read pointer
WRITE_ADDR = $04 ; memory address to write
SKIP       = $06 ; frames to skip

ADDR_5B_REG  = $C000
ADDR_5B_DATA = $E000

.import __SWAP_LOAD__
.import __SWAP_RUN__

song_data = song_ay_test

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
	CMP #$10 ; < $10 = 5B register
	BCC @do_5b
	; else memory write
	JMP @do_write
	; loop instruction
@do_5b:
	STA ADDR_5B_REG
	JSR inc_read
	LDA (SONG_READ), Y
	STA ADDR_5B_DATA
	JSR inc_read
	JMP @read_loop
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
; 0a bc = write bc to 5B register a (if ab < $10)
; ab cd ef = write ef to memory ab cd (note big endian address)

song_ay_test:
; frame 0
.byte $07, %00111111 ; disable all sound
.byte $08, $0F ; A disable envelope, full volume
.byte $09, $0F ; B disable envelope, full volume
.byte $0A, $0F ; C disable envelope, full volume
.byte $FF, 19 ; end frame, wait 20 frames
; test channel A
.byte $00, $7F, $01, $00 ; A frequency = 440hz
.byte $07, %00111110 ; enable tone A
.byte $FF, 59 ; wait 60 frames
.byte $07, %00111111 ; disable sound
.byte $FF, 59
; test channel B w/noise
.byte $02, $A9, $03, $00 ; B = 330hz (unheard)
.byte $06, $1F ; noise = 1800hz
.byte $07, %00101111 ; enable noise B
.byte $FF, 59
.byte $07, %00111111
.byte $FF, 59
; test channel C w/both
.byte $04, $FE, $05, $00 ; C = 220hz
.byte $07, %00011011 ; enable noise/tone C
.byte $FF, 59
.byte $07, %00111111
.byte $FF, 59
; test envelope A
.byte $0B, $0E, $0C, $00 ; env = 249hz
.byte $08, $1F ; enable envelope A
.byte $0D, $0C ; reset envelope, shape 0C (saw up)
.byte $FF, 59
.byte $08, $0F ; disable envelope A
.byte $FF, 59
; test envelope B
.byte $09, $1F ; enable envelope B
.byte $0D, $0A ; reset envelope, shape 0A (tri down)
.byte $FF, 59
.byte $09, $0F
.byte $FF, 59
; test envelope C
.byte $0B, $BB, $0C, $02 ; env = 5hz
.byte $0A, $1F ; enable envelope C
.byte $0D, $08 ; reset envelope, shape 08 (saw down)
.byte $07, %00111011 ; enable tone C
.byte $FF, 59
.byte $07, %00111111 ; disable tone C
.byte $0A, $0F ; disable envelope C
.byte $FF, 59
; 5 second noise envelope A
.byte $0B, $46, $0C, $44 ; env = 0.2hz
.byte $09, $1F ; enable envelope B
.byte $0D, $08 ; reset envelope, shape 08 (saw down)
.byte $07, %00101111 ; enable noise B
.byte $FF, 59
.byte $FF, 59
.byte $FF, 59
.byte $FF, 59
.byte $FF, 59
.byte $07, %00111111
.byte $09, $0F
.byte $FF, 59
; volume test of tone B
.byte $09, $00 ; volume 0 B
.byte $07, %00111101 ; enable tone B
.byte $FF, 1 ; wait 2 frames
.byte $09, $01, $FF, 1
.byte $09, $02, $FF, 1
.byte $09, $03, $FF, 1
.byte $09, $04, $FF, 1
.byte $09, $05, $FF, 1
.byte $09, $06, $FF, 1
.byte $09, $07, $FF, 1
.byte $09, $08, $FF, 1
.byte $09, $09, $FF, 1
.byte $09, $0A, $FF, 1
.byte $09, $0B, $FF, 1
.byte $09, $0C, $FF, 1
.byte $09, $0D, $FF, 1
.byte $09, $0E, $FF, 1
.byte $09, $0F, $FF, 1
.byte $09, $0E, $FF, 0
.byte $09, $0D, $FF, 0
.byte $09, $0C, $FF, 0
.byte $09, $0B, $FF, 0
.byte $09, $0A, $FF, 0
.byte $09, $09, $FF, 0
.byte $09, $08, $FF, 0
.byte $09, $07, $FF, 0
.byte $09, $06, $FF, 0
.byte $09, $05, $FF, 0
.byte $09, $04, $FF, 0
.byte $09, $03, $FF, 0
.byte $09, $02, $FF, 0
.byte $09, $01, $FF, 0
.byte $09, $00, $FF, 0
.byte $07, %00111111 ; disable all tones
.byte $FF, 59
; test channel A
.byte $00, $FF, $01, $0F ; A frequency = 13hz
.byte $07, %00111110 ; enable tone A
.byte $FF, 59 ; wait 60 frames
.byte $07, %00111111 ; disable sound
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
