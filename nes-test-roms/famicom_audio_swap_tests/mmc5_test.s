; MMC5 Swap Test
; Brad Smith, 5/03/2012
;
; This ROM is intended to test the MMC5 chip
; via hotswapping. It tests the existance of
; envelope, length counter, sweep on its square
; channels, its nonexistant triangle/noise,
; its PCM channel in write mode, the frame
; frame sequencer register, and length counters.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.segment "HEADER"

.ifndef NSF

; NES header
.byte 'N', 'E', 'S', $1A ; ID
.byte $02 ; 16k PRG bank count (2 = 32kb)
.byte $02 ; 8k CHR bank count (2 = 8kb)
.byte $00, $00 ; mapper 0 (hotswap to an MMC5)
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
.byte "mmc5 swap test",           0,0
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

song_data = song_mmc5_test

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

song_mmc5_test:
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
.byte $50, $15, $00 ; wipe MMC5
.byte $50, $00, $00
.byte $50, $01, $00
.byte $50, $02, $00
.byte $50, $03, $00
.byte $50, $04, $00
.byte $50, $05, $00
.byte $50, $06, $00
.byte $50, $07, $00
.byte $50, $08, $00
.byte $50, $09, $00
.byte $50, $0A, $00
.byte $50, $0B, $00
.byte $50, $0C, $00
.byte $50, $0D, $00
.byte $50, $0E, $00
.byte $50, $0F, $00
.byte $FF, 59       ; wait 1s

; TEST 1 compare solid pulse
; setup pitches
.byte $40, $02, $A9 ; E-4 (timer low)
.byte $40, $06, $FD ; A-3
.byte $50, $02, $A9 ; E-4
.byte $50, $06, $FD ; A-3
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
; echo MMC5
.byte $50, $15, %00000001 ; enable square 1
.byte $50, $00, $7F ; square 1 on, duty 1, full volume
.byte $50, $03, $00
.byte $FF, 29
.byte $50, $15, %00000010 ; enable square 2
.byte $50, $04, $BF ; square 2 on, duty 2, full volume
.byte $50, $07, $00
.byte $FF, 29
.byte $50, $15, %00000000 ; disable squares
; end of test
.byte $FF, 59 ; wait 1s

; TEST 2 compare envelopes
; setup pitches
.byte $40, $02, $A9 ; E-4 (timer low)
.byte $40, $06, $FD ; A-3
.byte $50, $02, $96 ; F#4
.byte $50, $06, $E1 ; B-3
; test APU
.byte $40, $15, %00000001 ; enable square 1
.byte $40, $00, $65 ; square 1 on, duty 1, medium envelope looped
.byte $40, $03, $00
.byte $FF, 29
.byte $40, $15, %00000010 ; enable square 2
.byte $40, $04, $A0 ; square 2 on, duty 2, short envelope looped
.byte $40, $07, $00
.byte $FF, 29
.byte $40, $15, %00000001 ; enable square 1
.byte $40, $00, $4F ; square 1 on, duty 1, longest envelope unlooped
.byte $40, $03, $08
.byte $FF, 179
.byte $40, $15, %00000000 ; disable squares
; echo MMC5
.byte $50, $15, %00000001 ; enable square 1
.byte $50, $00, $65 ; square 1 on, duty 1, medium envelope looped
.byte $50, $03, $00
.byte $FF, 29
.byte $50, $15, %00000010 ; enable square 2
.byte $50, $04, $A0 ; square 2 on, duty 2, short envelope looped
.byte $50, $07, $00
.byte $FF, 29
.byte $50, $15, %00000001 ; enable square 1
.byte $50, $00, $4F ; square 1 on, duty 1, longest envelope unlooped
.byte $50, $03, $08
.byte $FF, 179
.byte $50, $15, %00000000 ; disable squares
; end of test
.byte $FF, 59 ; wait 1s

; TEST 3 compare length counter
; setup pitches
.byte $40, $02, $A9 ; E-4 (timer low)
.byte $40, $06, $FD ; A-3
.byte $50, $02, $96 ; F#4
.byte $50, $06, $E1 ; B-3
; test APU
.byte $40, $15, %00000001 ; enable square 1
.byte $40, $00, $5F ; square 1 on, duty 1, full volume
.byte $40, $03, $10 ; length counter $02 > 20 steps
.byte $FF, 29
.byte $40, $15, %00000010 ; enable square 2
.byte $40, $04, $9F ; square 2 on, duty 2, full volume
.byte $40, $07, $30 ; length counter $06 > 80 steps
.byte $FF, 59
.byte $40, $15, %00000000 ; disable squares
; echo MMC5
.byte $50, $15, %00000001 ; enable square 1
.byte $50, $00, $5F ; square 1 on, duty 1, full volume
.byte $50, $03, $10 ; length counter $02 > 20 steps (double-speed on MMC5)
.byte $FF, 29
.byte $50, $15, %00000010 ; enable square 2
.byte $50, $04, $9F ; square 2 on, duty 2, full volume
.byte $50, $07, $30 ; length counter $06 > 80 steps (double-speed on MMC5)
.byte $FF, 59
.byte $50, $15, %00000000 ; disable squares
; end of test
.byte $FF, 59 ; wait 1s

; TEST 4 compare sweep
; test APU
.byte $40, $15, %00000001 ; enable square 1
.byte $40, $00, $7F ; square 1 on, duty 1, full volume
.byte $40, $02, $A9 ; E-4
.byte $40, $03, $00
.byte $40, $01, $8B ; sweep up
.byte $FF, 29
.byte $40, $15, %00000010 ; enable square 2
.byte $40, $04, $BF ; square 2 on, duty 2, full volume
.byte $40, $06, $A9 ; E-4
.byte $40, $07, $00
.byte $40, $05, $8B ; sweep up
.byte $FF, 29
.byte $40, $15, %00000001 ; enable square 1
.byte $40, $00, $7F ; square 1 on, duty 1, full volume
.byte $40, $02, $A9 ; E-4
.byte $40, $03, $00
.byte $40, $01, $83 ; sweep down
.byte $FF, 29
.byte $40, $15, %00000010 ; enable square 2
.byte $40, $04, $BF ; square 2 on, duty 2, full volume
.byte $40, $06, $A9 ; E-4
.byte $40, $07, $00
.byte $40, $05, $83 ; sweep down
.byte $FF, 29
.byte $40, $15, %00000001 ; enable square 1
.byte $40, $00, $7F ; square 1 on, duty 1, full volume
.byte $40, $02, $A9 ; E-4
.byte $40, $03, $00
.byte $40, $01, $87 ; sweep down slow
.byte $FF, 29
.byte $40, $15, %00000010 ; enable square 2
.byte $40, $04, $BF ; square 2 on, duty 2, full volume
.byte $40, $06, $FD ; A-3
.byte $40, $07, $00
.byte $40, $05, $89 ; sweep up fast
.byte $FF, 29
.byte $40, $15, %00000000 ; disable squares
.byte $40, $01, $00 ; sweep off
.byte $40, $05, $00
; echo MMC5
.byte $50, $15, %00000001 ; enable square 1
.byte $50, $00, $7F ; square 1 on, duty 1, full volume
.byte $50, $02, $A9 ; E-4
.byte $50, $03, $00
.byte $50, $01, $8B ; sweep up
.byte $FF, 29
.byte $50, $15, %00000010 ; enable square 2
.byte $50, $04, $BF ; square 2 on, duty 2, full volume
.byte $50, $06, $A9 ; E-4
.byte $50, $07, $00
.byte $50, $05, $8B ; sweep up
.byte $FF, 29
.byte $50, $15, %00000001 ; enable square 1
.byte $50, $00, $7F ; square 1 on, duty 1, full volume
.byte $50, $02, $A9 ; E-4
.byte $50, $03, $00
.byte $50, $01, $83 ; sweep down
.byte $FF, 29
.byte $50, $15, %00000010 ; enable square 2
.byte $50, $04, $BF ; square 2 on, duty 2, full volume
.byte $50, $06, $A9 ; E-4
.byte $50, $07, $00
.byte $50, $05, $83 ; sweep down
.byte $FF, 29
.byte $50, $15, %00000001 ; enable square 1
.byte $50, $00, $7F ; square 1 on, duty 1, full volume
.byte $50, $02, $A9 ; E-4
.byte $50, $03, $00
.byte $50, $01, $87 ; sweep down slow
.byte $FF, 29
.byte $50, $15, %00000010 ; enable square 2
.byte $50, $04, $BF ; square 2 on, duty 2, full volume
.byte $50, $06, $FD ; A-3
.byte $50, $07, $00
.byte $50, $05, $89 ; sweep up fast
.byte $FF, 29
.byte $50, $15, %00000000 ; disable squares
.byte $50, $01, $00 ; sweep off
.byte $50, $05, $00
; end of test
.byte $FF, 59 ; wait 1s

; TEST 5 triangle and noise
; test APU
.byte $40, $15, %00000100 ; enable triangle
.byte $40, $08, $81
.byte $40, $0A, $42
.byte $40, $0B, $00
.byte $FF, 59
.byte $40, $15, %00001000 ; enable noise
.byte $40, $0C, $FF ; full volume
.byte $40, $0E, $06 ; regular noise
.byte $40, $0F, $00
.byte $FF, 59
.byte $40, $0E, $86 ; periodic noise
.byte $FF, 59
.byte $40, $15, %00000000 ; disable noise
; echo MMC5
.byte $50, $15, %00000100 ; enable triangle
.byte $50, $08, $81
.byte $50, $0A, $42
.byte $50, $0B, $00
.byte $FF, 59
.byte $50, $15, %00001000 ; enable noise
.byte $50, $0C, $FF ; full volume
.byte $50, $0E, $06 ; regular noise
.byte $50, $0F, $00
.byte $FF, 59
.byte $50, $0E, $86 ; periodic noise
.byte $FF, 59
.byte $50, $15, %00000000 ; disable noise
; end of test
.byte $FF, 59 ; wait 1s

; TEST 6 phase reset
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
; echo MMC5
.byte $50, $15, %00000001 ; enable square 1
.byte $50, $00, $BF ; square 1 on, duty 2, full volume
.byte $50, $02, $A9 ; E-4
.byte $50, $03, $00
.byte $FF, 19
.byte $50, $03, $00 ; reset phase
.byte $FF, 19
.byte $50, $03, $00
.byte $FF, 19
.byte $50, $03, $00
.byte $FF, 19
.byte $50, $03, $00
.byte $FF, 19
.byte $50, $03, $00
.byte $FF, 19
.byte $50, $15, %000000010 ; enable square 2
.byte $50, $04, $3F ; square 2 on, duty 0, full volume
.byte $50, $06, $FD ; A-3
.byte $50, $07, $00
.byte $FF, 19
.byte $50, $07, $00 ; reset phase
.byte $FF, 19
.byte $50, $07, $00
.byte $FF, 19
.byte $50, $07, $00
.byte $FF, 19
.byte $50, $07, $00
.byte $FF, 19
.byte $50, $07, $00
.byte $FF, 19
.byte $50, $15, %00000000 ; disable squares
; end of test
.byte $FF, 59 ; wait 1s

; TEST 7 frame sequencer
; setup pitches
.byte $40, $02, $A9 ; E-4 (timer low)
.byte $40, $06, $FD ; A-3
.byte $50, $02, $A9 ; E-4
.byte $50, $06, $FD ; A-3
; test APU
.byte $40, $17, $40 ; 4-step sequence
.byte $40, $15, %00000001 ; enable square 1
.byte $40, $00, $63 ; square 1 on, duty 1, looped envelope
.byte $40, $03, $00
.byte $FF, 29
.byte $40, $17, $C0 ; 5-step sequence
.byte $40, $15, %00000010 ; enable square 2
.byte $40, $04, $A3 ; square 2 on, duty 2, looped envelope
.byte $40, $07, $00
.byte $FF, 29
.byte $40, $15, %00000001 ; enable square 1
.byte $40, $00, $63 ; square 1 on, duty 1, looped envelope
.byte $40, $03, $00
.byte $FF, 0
.byte $40, $17, $C0 ; trigger sequence
.byte $FF, 0
.byte $40, $17, $C0
.byte $FF, 0
.byte $40, $17, $C0
.byte $FF, 0
.byte $40, $17, $C0
.byte $FF, 0
.byte $40, $17, $C0
.byte $FF, 0
.byte $40, $17, $C0
.byte $FF, 0
.byte $40, $17, $C0
.byte $FF, 0
.byte $40, $17, $C0
.byte $FF, 0
.byte $40, $17, $C0
.byte $FF, 0
.byte $40, $17, $C0
.byte $FF, 19
.byte $40, $15, %00000000 ; disable squares
.byte $40, $17, $40 ; 4-step sequence
; echo MMC5
.byte $50, $17, $40 ; 4-step sequence
.byte $50, $15, %00000001 ; enable square 1
.byte $50, $00, $63 ; square 1 on, duty 1, looped envelope
.byte $50, $03, $00
.byte $FF, 29
.byte $50, $17, $C0 ; 5-step sequence
.byte $50, $15, %00000010 ; enable square 2
.byte $50, $04, $A3 ; square 2 on, duty 2, looped envelope
.byte $50, $07, $00
.byte $FF, 29
.byte $50, $15, %00000001 ; enable square 1
.byte $50, $00, $63 ; square 1 on, duty 1, looped envelope
.byte $50, $03, $00
.byte $FF, 0
.byte $50, $17, $C0 ; trigger sequence
.byte $FF, 0
.byte $50, $17, $C0
.byte $FF, 0
.byte $50, $17, $C0
.byte $FF, 0
.byte $50, $17, $C0
.byte $FF, 0
.byte $50, $17, $C0
.byte $FF, 0
.byte $50, $17, $C0
.byte $FF, 0
.byte $50, $17, $C0
.byte $FF, 0
.byte $50, $17, $C0
.byte $FF, 0
.byte $50, $17, $C0
.byte $FF, 0
.byte $50, $17, $C0
.byte $FF, 19
.byte $50, $15, %00000000 ; disable squares
.byte $50, $17, $40 ; 4-step sequence
; echo MMC5 again but using $4017 instead of $5017
.if 0 ; removed, too large for ROM
; NOTE: MMC5 did not respond to 4017 changes when I tried this test,
;       (didn't seem plausible anyway)
.byte $40, $17, $40 ; 4-step sequence
.byte $50, $15, %00000001 ; enable square 1
.byte $50, $00, $63 ; square 1 on, duty 1, looped envelope
.byte $50, $03, $00
.byte $FF, 29
.byte $40, $17, $C0 ; 5-step sequence
.byte $50, $15, %00000010 ; enable square 2
.byte $50, $04, $A3 ; square 2 on, duty 2, looped envelope
.byte $50, $07, $00
.byte $FF, 29
.byte $50, $15, %00000001 ; enable square 1
.byte $50, $00, $63 ; square 1 on, duty 1, looped envelope
.byte $50, $03, $00
.byte $FF, 0
.byte $40, $17, $C0 ; trigger sequence
.byte $FF, 0
.byte $40, $17, $C0
.byte $FF, 0
.byte $40, $17, $C0
.byte $FF, 0
.byte $40, $17, $C0
.byte $FF, 0
.byte $40, $17, $C0
.byte $FF, 0
.byte $40, $17, $C0
.byte $FF, 0
.byte $40, $17, $C0
.byte $FF, 0
.byte $40, $17, $C0
.byte $FF, 0
.byte $40, $17, $C0
.byte $FF, 0
.byte $40, $17, $C0
.byte $FF, 19
.byte $50, $15, %00000000 ; disable squares
.byte $40, $17, $40 ; 4-step sequence
.endif
; end of test
.byte $FF, 59 ; wait 1s

; TEST 8 length counter table
; test APU
.if 0 ; removed for size; length table on 2A03 is known anyway
.byte $40, $15, %00000001 ; enable square 1
.byte $40, $02, $96 ; F#4
.byte $40, $00, $9F ; square 1 on, duty 2, full volume
.byte $40, $03, $08 ; length counter $01
.byte $FF, 179
.byte $40, $03, $18 ; length counter $03
.byte $FF, 179
.byte $40, $03, $28 ; length counter $05
.byte $FF, 179
.byte $40, $03, $38 ; length counter $07
.byte $FF, 179
.byte $40, $03, $48 ; length counter $09
.byte $FF, 179
.byte $40, $03, $58 ; length counter $0B
.byte $FF, 179
.byte $40, $03, $68 ; length counter $0D
.byte $FF, 179
.byte $40, $03, $78 ; length counter $0F
.byte $FF, 179
.byte $40, $03, $88 ; length counter $11
.byte $FF, 179
.byte $40, $03, $98 ; length counter $13
.byte $FF, 179
.byte $40, $03, $A8 ; length counter $15
.byte $FF, 179
.byte $40, $03, $B8 ; length counter $17
.byte $FF, 179
.byte $40, $03, $C8 ; length counter $19
.byte $FF, 179
.byte $40, $03, $D8 ; length counter $1B
.byte $FF, 179
.byte $40, $03, $E8 ; length counter $1D
.byte $FF, 179
.byte $40, $03, $F8 ; length counter $1F
.byte $FF, 179
.byte $40, $03, $00 ; length counter $00
.byte $FF, 179
.byte $40, $03, $10 ; length counter $02
.byte $FF, 179
.byte $40, $03, $20 ; length counter $04
.byte $FF, 179
.byte $40, $03, $30 ; length counter $06
.byte $FF, 179
.byte $40, $03, $40 ; length counter $08
.byte $FF, 179
.byte $40, $03, $50 ; length counter $0A
.byte $FF, 179
.byte $40, $03, $60 ; length counter $0C
.byte $FF, 179
.byte $40, $03, $70 ; length counter $0E
.byte $FF, 179
.byte $40, $03, $80 ; length counter $10
.byte $FF, 179
.byte $40, $03, $90 ; length counter $12
.byte $FF, 179
.byte $40, $03, $A0 ; length counter $14
.byte $FF, 179
.byte $40, $03, $B0 ; length counter $16
.byte $FF, 179
.byte $40, $03, $C0 ; length counter $18
.byte $FF, 179
.byte $40, $03, $D0 ; length counter $1A
.byte $FF, 179
.byte $40, $03, $E0 ; length counter $1C
.byte $FF, 179
.byte $40, $03, $F0 ; length counter $1E
.byte $FF, 179
.byte $40, $15, %00000000 ; disable squares
.endif
; echo MMC5
.byte $50, $15, %00000001 ; enable square 1
.byte $50, $00, $9F ; square 1 on, duty 2, full volume
.byte $50, $03, $08 ; length counter $01
.byte $FF, 179
.byte $50, $03, $18 ; length counter $03
.byte $FF, 179
.byte $50, $03, $28 ; length counter $05
.byte $FF, 179
.byte $50, $03, $38 ; length counter $07
.byte $FF, 179
.byte $50, $03, $48 ; length counter $09
.byte $FF, 179
.byte $50, $03, $58 ; length counter $0B
.byte $FF, 179
.byte $50, $03, $68 ; length counter $0D
.byte $FF, 179
.byte $50, $03, $78 ; length counter $0F
.byte $FF, 179
.byte $50, $03, $88 ; length counter $11
.byte $FF, 179
.byte $50, $03, $98 ; length counter $13
.byte $FF, 179
.byte $50, $03, $A8 ; length counter $15
.byte $FF, 179
.byte $50, $03, $B8 ; length counter $17
.byte $FF, 179
.byte $50, $03, $C8 ; length counter $19
.byte $FF, 179
.byte $50, $03, $D8 ; length counter $1B
.byte $FF, 179
.byte $50, $03, $E8 ; length counter $1D
.byte $FF, 179
.byte $50, $03, $F8 ; length counter $1F
.byte $FF, 179
.byte $50, $03, $00 ; length counter $00
.byte $FF, 179
.byte $50, $03, $10 ; length counter $02
.byte $FF, 179
.byte $50, $03, $20 ; length counter $04
.byte $FF, 179
.byte $50, $03, $30 ; length counter $06
.byte $FF, 179
.byte $50, $03, $40 ; length counter $08
.byte $FF, 179
.byte $50, $03, $50 ; length counter $0A
.byte $FF, 179
.byte $50, $03, $60 ; length counter $0C
.byte $FF, 179
.byte $50, $03, $70 ; length counter $0E
.byte $FF, 179
.byte $50, $03, $80 ; length counter $10
.byte $FF, 179
.byte $50, $03, $90 ; length counter $12
.byte $FF, 179
.byte $50, $03, $A0 ; length counter $14
.byte $FF, 179
.byte $50, $03, $B0 ; length counter $16
.byte $FF, 179
.byte $50, $03, $C0 ; length counter $18
.byte $FF, 179
.byte $50, $03, $D0 ; length counter $1A
.byte $FF, 179
.byte $50, $03, $E0 ; length counter $1C
.byte $FF, 179
.byte $50, $03, $F0 ; length counter $1E
.byte $FF, 179
.byte $50, $15, %00000000 ; disable squares
; end of test
.byte $FF, 59 ; wait 1s

; TEST 9 MMC5 PCM
.byte $50, $10, $00 ; set PCM to write mode
; buzz 5011 7F/01
.byte $50, $11, $7F ; 5011 = 7F
.byte $FF, $00      ; end frame
.byte $50, $11, $01 ;
.byte $FF, $00      ;
.byte $50, $11, $7F ;
.byte $FF, $00      ;
.byte $50, $11, $01 ;
.byte $FF, $00      ;
.byte $50, $11, $7F ;
.byte $FF, $00      ;
.byte $50, $11, $01 ;
.byte $FF, $00      ;
.byte $50, $11, $7F ;
.byte $FF, $00      ;
.byte $50, $11, $01 ;
.byte $FF, $00      ;
.byte $50, $11, $7F ;
.byte $FF, $00      ;
.byte $50, $11, $01 ;
.byte $FF, 59       ; wait 1s
; buzz 5011, FF/01
.byte $50, $11, $FF ; 5011 = FF
.byte $FF, $00      ; end frame
.byte $50, $11, $01 ;
.byte $FF, $00      ;
.byte $50, $11, $FF ;
.byte $FF, $00      ;
.byte $50, $11, $01 ;
.byte $FF, $00      ;
.byte $50, $11, $FF ;
.byte $FF, $00      ;
.byte $50, $11, $01 ;
.byte $FF, $00      ;
.byte $50, $11, $FF ;
.byte $FF, $00      ;
.byte $50, $11, $01 ;
.byte $FF, $00      ;
.byte $50, $11, $FF ;
.byte $FF, $00      ;
.byte $50, $11, $01 ;
.byte $FF, 59       ; wait 1s
; buzz 5011 7F/00
.byte $50, $11, $7F ; 5011 = 7F
.byte $FF, $00      ; end frame
.byte $50, $11, $00;
.byte $FF, $00      ;
.byte $50, $11, $7F ;
.byte $FF, $00      ;
.byte $50, $11, $00;
.byte $FF, $00      ;
.byte $50, $11, $7F ;
.byte $FF, $00      ;
.byte $50, $11, $00;
.byte $FF, $00      ;
.byte $50, $11, $7F ;
.byte $FF, $00      ;
.byte $50, $11, $00;
.byte $FF, $00      ;
.byte $50, $11, $7F ;
.byte $FF, $00      ;
.byte $50, $11, $00;
.byte $FF, 59       ; wait 1s
; buzz 5011, FF/00
.byte $50, $11, $FF ; 5011 = FF
.byte $FF, $00      ; end frame
.byte $50, $11, $00;
.byte $FF, $00      ;
.byte $50, $11, $FF ;
.byte $FF, $00      ;
.byte $50, $11, $00;
.byte $FF, $00      ;
.byte $50, $11, $FF ;
.byte $FF, $00      ;
.byte $50, $11, $00;
.byte $FF, $00      ;
.byte $50, $11, $FF ;
.byte $FF, $00      ;
.byte $50, $11, $00;
.byte $FF, $00      ;
.byte $50, $11, $FF ;
.byte $FF, $00      ;
.byte $50, $11, $00;
.byte $FF, 59       ; wait 1s

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
