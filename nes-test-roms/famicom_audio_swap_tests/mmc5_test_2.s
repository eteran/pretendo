; MMC5 Swap Test 2
; Brad Smith, 5/10/2012
;
; This ROM is intended to test the MMC5 chip
; via hotswapping. It tests the extreme frequency
; ranges of the MMC5 and 2A03 square channels,
; as well as the PCM channel in read and write
; modes.

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
.byte "mmc5 swap test 2"
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

ADDR_5B_REG  = $C000
ADDR_5B_DATA = $E000

.import __SWAP_LOAD__
.import __SWAP_RUN__

song_data = song_mmc5_test

; 256 bytes of sample data at $8000
; These are the first 256 bytes of the Just Breed ROM (after iNES header),
; replace with data from the cartridge you intend to hotswap.
.byte $00, $0A, $73, $20, $D3, $49, $59, $67, $F4, $CE, $E5, $F1, $8D, $94, $7F, $B4
.byte $38, $96, $66, $E2, $12, $88, $D1, $42, $CD, $FF, $8C, $8F, $EE, $B4, $C0, $36
.byte $CB, $41, $55, $61, $6D, $79, $47, $EC, $F8, $06, $30, $28, $74, $83, $AB, $72
.byte $D1, $9D, $A0, $38, $92, $97, $39, $9F, $FF, $FF, $99, $94, $66, $E8, $7F, $7F
.byte $4F, $AC, $2C, $66, $66, $66, $66, $66, $66, $66, $66, $66, $66, $66, $66, $66
.byte $14, $98, $F2, $B7, $B7, $DB, $35, $35, $24, $24, $92, $D2, $D2, $D2, $D2, $D2
.byte $D2, $D2, $D2, $FA, $FA, $FA, $6B, $A9, $E3, $E3, $E3, $E3, $E3, $E3, $E3, $E3
.byte $D5, $D5, $A9, $7C, $30, $6F, $6F, $6F, $6F, $6F, $3B, $68, $AB, $E0, $E0, $E0
.byte $E0, $E0, $E0, $E0, $A9, $49, $49, $49, $49, $49, $49, $49, $49, $DF, $2A, $93
.byte $E6, $1F, $63, $9F, $E4, $1F, $6B, $A3, $A3, $A3, $A3, $A3, $A3, $A3, $A3, $A3
.byte $A3, $A3, $8D, $D0, $05, $48, $7D, $26, $C4, $06, $38, $38, $7E, $A4, $C2, $FB
.byte $44, $79, $98, $DA, $2D, $4B, $AF, $FC, $49, $6C, $AB, $11, $41, $72, $89, $A8
.byte $D7, $F9, $F9, $F9, $F9, $F9, $F9, $3E, $60, $6A, $D9, $1A, $CC, $69, $8C, $8C
.byte $8C, $E8, $E0, $98, $78, $A8, $A8, $A8, $A8, $A8, $A8, $A8, $26, $75, $76, $76
.byte $76, $76, $4F, $A1, $0B, $13, $13, $13, $68, $F6, $CB, $CB, $CB, $70, $70, $70
.byte $70, $69, $69, $69, $69, $69, $69, $69, $69, $69, $AE, $B1, $B1, $B1, $B1, $B1

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
	CMP #$FD ; $FD - pcm test
	BEQ @do_pcm_test
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
	; pcm test instruction
@do_pcm_test:
	JSR inc_read
	JSR pcm_test
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

; test of MMC5 PCM in both modes
pcm_test:
	; TEST 1 output PCM data from $80XX to DMC
	LDX #0
	LDY #30 ; loop this many times
@test_1_loop:
	LDA $8000, X
	LSR ; reduce 8-bit data to 7-bit sample
	STA $4011
	JSR pcm_delay
	INX
	BNE @test_1_loop
	DEY
	BNE @test_1_loop
	JSR pcm_long_delay
	; TEST 2 output MMC5 PCM data, write mode
	LDA #0
	STA $5010 ; MMC5 PCM write mode
	LDX #0
	LDY #30 ; loop this many times
@test_2_loop:
	LDA $8000, X
	STA $5011
	LSR ; dummy operation
	JSR pcm_delay
	INX
	BNE @test_2_loop
	DEY
	BNE @test_2_loop
	JSR pcm_long_delay
	; TEST 3 output MMC5 PCM data, read mode
	LDA #1
	STA $5010 ; MMC5 PCM read mode
	LDX #0
	LDY #30 ; loop this many times
@test_3_loop:
	LDA $8000, X
	STA $4002 ; dummy write
	LSR ; dummy operation
	JSR pcm_delay
	INX
	BNE @test_3_loop
	DEY
	BNE @test_3_loop
	; end of test
	LDA #0
	STA $5010 ; MMC5 PCM write mode
	; END OF TESTS
	;JSR pcm_long_delay ; no need for extra delay
	RTS

pcm_delay:
	; preserve X
	TXA
	PHA
	; waste time
	LDX #50 ; time to waste between samples
@delay_loop:
	DEX
	BNE @delay_loop
	; restore X
	PLA
	TAX
	RTS

pcm_long_delay:
	LDY #$FF
@long_delay_loop:
	JSR pcm_delay
	DEY
	BNE @long_delay_loop
	RTS

;
; song data
;

; variable length code:
; FF ab = return, skip the next ab frames
; FE = loop
; FD = MMC5 pcm test
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
; setup MMC5 bank to ensure data is present (works with Just Breed data)
.byte $51, $00, $00 ; PRG mode 0 (one 32kb bank)
.byte $51, $17, $00 ; select 32kb bank 0
; end of setup
.byte $FF, 59       ; wait 1s

; TEST 1 compare very high pitches
; test APU
.byte $40, $15, %00000001 ; enable square 1
.byte $40, $00, $7F ; square 1 on, duty 1, full volume
.byte $40, $02, $0F ; frequency 15
.byte $40, $03, $00
.byte $FF, 19
.byte $40, $02, $0E
.byte $FF, 19
.byte $40, $02, $0D
.byte $FF, 19
.byte $40, $02, $0C
.byte $FF, 19
.byte $40, $02, $0B
.byte $FF, 19
.byte $40, $02, $0A
.byte $FF, 19
.byte $40, $02, $09
.byte $FF, 19
.byte $40, $02, $08
.byte $FF, 19
.byte $40, $02, $07
.byte $FF, 19
.byte $40, $02, $06
.byte $FF, 19
.byte $40, $02, $05
.byte $FF, 19
.byte $40, $02, $04
.byte $FF, 19
.byte $40, $02, $03
.byte $FF, 19
.byte $40, $02, $02
.byte $FF, 19
.byte $40, $02, $01
.byte $FF, 19
.byte $40, $02, $00
.byte $FF, 19
.byte $40, $15, %00000000 ; disable squares
; echo MMC5
.byte $50, $15, %00000001 ; enable square 1
.byte $50, $00, $7F ; square 1 on, duty 1, full volume
.byte $50, $02, $0F ; frequency 15
.byte $50, $03, $00
.byte $FF, 19
.byte $50, $02, $0E
.byte $FF, 19
.byte $50, $02, $0D
.byte $FF, 19
.byte $50, $02, $0C
.byte $FF, 19
.byte $50, $02, $0B
.byte $FF, 19
.byte $50, $02, $0A
.byte $FF, 19
.byte $50, $02, $09
.byte $FF, 19
.byte $50, $02, $08
.byte $FF, 19
.byte $50, $02, $07
.byte $FF, 19
.byte $50, $02, $06
.byte $FF, 19
.byte $50, $02, $05
.byte $FF, 19
.byte $50, $02, $04
.byte $FF, 19
.byte $50, $02, $03
.byte $FF, 19
.byte $50, $02, $02
.byte $FF, 19
.byte $50, $02, $01
.byte $FF, 19
.byte $50, $02, $00
.byte $FF, 19
.byte $50, $15, %00000000 ; disable squares
; end of test
.byte $FF, 59 ; wait 1s

; TEST 2 compare very low pitches
; test APU
.byte $40, $15, %00000010 ; enable square 2
.byte $40, $04, $7F ; square 2 on, duty 1, full volume
.byte $40, $05, $08 ; disable/negate sweep
.byte $40, $06, $F0 ; frequency $7FF-15
.byte $40, $07, $07
.byte $FF, 19
.byte $40, $06, $F1
.byte $FF, 19
.byte $40, $06, $F2
.byte $FF, 19
.byte $40, $06, $F3
.byte $FF, 19
.byte $40, $06, $F4
.byte $FF, 19
.byte $40, $06, $F5
.byte $FF, 19
.byte $40, $06, $F6
.byte $FF, 19
.byte $40, $06, $F7
.byte $FF, 19
.byte $40, $06, $F8
.byte $FF, 19
.byte $40, $06, $F9
.byte $FF, 19
.byte $40, $06, $FA
.byte $FF, 19
.byte $40, $06, $FB
.byte $FF, 19
.byte $40, $06, $FC
.byte $FF, 19
.byte $40, $06, $FD
.byte $FF, 19
.byte $40, $06, $FE
.byte $FF, 19
.byte $40, $06, $FF
.byte $FF, 19
.byte $40, $15, %00000000 ; disable squares
.byte $FF, 29 ; brief pause
; echo MMC5
.byte $50, $15, %00000010 ; enable square 2
.byte $50, $04, $7F ; square 2 on, duty 1, full volume
.byte $50, $05, $08 ; disable/negate sweep
.byte $50, $06, $F0 ; frequency $7FF-15
.byte $50, $07, $07
.byte $FF, 19
.byte $50, $06, $F1
.byte $FF, 19
.byte $50, $06, $F2
.byte $FF, 19
.byte $50, $06, $F3
.byte $FF, 19
.byte $50, $06, $F4
.byte $FF, 19
.byte $50, $06, $F5
.byte $FF, 19
.byte $50, $06, $F6
.byte $FF, 19
.byte $50, $06, $F7
.byte $FF, 19
.byte $50, $06, $F8
.byte $FF, 19
.byte $50, $06, $F9
.byte $FF, 19
.byte $50, $06, $FA
.byte $FF, 19
.byte $50, $06, $FB
.byte $FF, 19
.byte $50, $06, $FC
.byte $FF, 19
.byte $50, $06, $FD
.byte $FF, 19
.byte $50, $06, $FE
.byte $FF, 19
.byte $50, $06, $FF
.byte $FF, 19
.byte $50, $15, %00000000 ; disable squares
; end of test
.byte $FF, 59 ; wait 1s

; TEST 3 compare sweep unit silencing
; test APU
.byte $40, $15, %00000001 ; enable square 1
.byte $40, $00, $7F ; square 1 on, duty 1, full volume
.byte $40, $01, $00 ; disable but do not negate sweep (should silence low frequency here)
.byte $40, $02, $F0 ; frequency $7F0
.byte $40, $03, $07
.byte $FF, 59
.byte $40, $15, %00000000 ; disable squares
; echo MMC5
.byte $50, $15, %00000001 ; enable square 1
.byte $50, $00, $7F ; square 1 on, duty 1, full volume
.byte $50, $01, $00 ; disable but do not negate sweep (would silence low frequency here, if sweep unit exists)
.byte $50, $02, $F0 ; frequency $7F0
.byte $50, $03, $07
.byte $FF, 59
.byte $50, $15, %00000000 ; disable squares
; end of test
.byte $FF, 59 ; wait 1s

; TEST 4 MMC5 PCM tests
.byte $FD           ; branch to PCM test
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
