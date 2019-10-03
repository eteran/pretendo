; VRC7 Swap Test
; Brad Smith, 5/15/2012
;
; This ROM is intended to test the Konami VRC7 chip
; via hotswapping. It compares a proposed set of
; patches against the internal patch set.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.segment "HEADER"

.ifndef NSF

; NES header
.byte 'N', 'E', 'S', $1A ; ID
.byte $02 ; 16k PRG bank count (2 = 32kb)
.byte $02 ; 8k CHR bank count (2 = 8kb)
.byte $00, $00 ; mapper 0 (hotswap to a VRC7)
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
.byte "VRC7 swap test",           0,0
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
.byte %00000010 ; expansion bits
.byte 0,0,0,0 ; pad to $80

.endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.segment "CODE"

; zero page variables
PATCH         = $00 ; current patch
TONE          = $01 ; current tone
VOLUME        = $02 ; current volume
SKIP          = $03 ; frames to skip
TONE_LOW      = $04 ; temporary tone variable
TONE_HIGH     = $05 ; temporary tone variable
STORE_X       = $0082 ; used by Lagrange Point register wait code

VRC7_REG = $9010
VRC7_DAT = $9030

.import __SWAP_LOAD__
.import __SWAP_RUN__

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
	; initialize variables
	LDA #0
	STA TONE
	STA SKIP
	LDA #$08
	STA VOLUME
	LDA #16
	STA PATCH
	RTS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.segment "SWAP"

swap_nmi:
swap_irq:
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
	BEQ @main_loop
	DEY
	STY SKIP
	RTS
@main_loop:
	LDA PATCH
	CMP #16
	BCS vrc7_reset ; if PATCH >= 16 do reset sequence (3 buzzes, time to swap in between)
	JMP vrc7_play_tones ; else do tone sequence

vrc7_reset:
	; 10 frames of $4011 buzz
	LDA TONE
	CMP #10
	BCS @buzz_finished
	; determine polarity based on LSB
	ROR
	BCS @buzz_down
	; buzz up
	LDA #$7F
	JMP @buzz_end
@buzz_down:
	LDA #$00
@buzz_end:
	STA $4011
	LDX TONE
	INX
	STX TONE
	; wait 1 frame
	LDA #0
	STA SKIP
	RTS
@buzz_finished:
	; clear all registers
	LDX #$FF
	LDA #0
@clear_loop:
	JSR vrc7_write
	CPX #0
	BEQ @registers_clear
	DEX
	JMP @clear_loop
@registers_clear:
	LDX PATCH
	CPX #18 ; PATCH = 16, 17 do a buzz, reset and wait, 18 buzzes, resets, and continues
	BCS @finish_reset
	INX
	STX PATCH
	LDA #0
	STA TONE
	; wait 254 frames to give time to swap
	LDA #253
	STA SKIP
	RTS
@finish_reset:
	; reset patch and tone loop
	LDA #0
	STA TONE
	LDA #1
	STA PATCH
	; toggle between loud and quiet volume
	LDA VOLUME
	EOR #$08
	STA VOLUME
	; wait 1s
	LDA #59
	STA SKIP
	RTS

vrc7_play_tones:
	; sequence of 1s tone tests
	; 0 load patch, start low note
	; . 1 do nothing (reach sustain)
	; . 2 begin release
	; . 3 silence channel
	; 4 start medium note (w/sustain bit)
	; 8 start high note
	; 12 use preset patch, start low note
	; 16 start medium note (w/sustain bit)
	; 20 start high note
	; todo
	JSR vrc7_set_instrument
	JSR vrc7_play_tone
	; increment tone
	LDA #59 ; default 1s delay
	LDX TONE
	INX
	CPX #24
	BCC @next_tone ; increment patch if tone count is exceeded
	LDY PATCH
	INY
	STY PATCH
	LDX #0 ; reset tone
	LDA #119 ; 2s delay after end of tone loop
@next_tone:
	STX TONE
	STA SKIP
	RTS

vrc7_set_instrument:
	; only reload patch on tone step 3 (1s before first use in  tone 4)
	LDA TONE
	CMP #3
	BNE @no_load
	JSR vrc7_load_patch
@no_load:
	LDA TONE
	AND #$03
	BEQ @set_instrument
	RTS ; only set on tone 0, 4, 8, 12, 16, 20
@set_instrument:
	LDY #0
	LDA TONE
	AND #$04
	BNE @set_patch ; use patch 0 for tone 4, 12, 20
	LDY PATCH ; use PATCH for tone 0, 8, 16
@set_patch:
	LDX #$30 ; IIIIVVVV register channel 0
	TYA
	ASL        ; shift patch number into high 4 bits
	ASL
	ASL
	ASL
	ORA VOLUME ; set volume
	JSR vrc7_write
	RTS

vrc7_load_patch:
	LDA PATCH
	ASL
	ASL
	ASL
	TAY ; Y = PATCH * 8
	LDX #0
@patch_loop:
	LDA patch_set, Y
	JSR vrc7_write
	INY
	INX
	CPX #8
	BNE @patch_loop
	RTS

vrc7_tone_low: ; 3 randomly chosen scale pitches
.byte $43, $43
.byte $65, $65
.byte $76, $76

vrc7_tone_high: ; sustain O3, normal O4, normal O5
.byte $36, $36
.byte $18, $18
.byte $1A, $1A

vrc7_play_tone:
	; set pitch, store $20 data in Y register, to be masked
	LDA TONE
	LSR
	LSR
	TAX ; X = 0...5
	; load pitches
	LDA vrc7_tone_low, X
	STA TONE_LOW
	LDA vrc7_tone_high, X
	STA TONE_HIGH
	; 4 stage sound of tone
	LDA TONE
	AND #03
	; A = 0 start tone
	CMP #0
	BEQ @begin_tone
	; A = 1 do nothing
	CMP #1
	BEQ @tone_done
	; A = 2 release
	CMP #2
	BEQ @release_tone
	; else A == 3, stop tone
	JMP @stop_tone
@begin_tone:
	; set low pitch
	LDX #$10
	LDA TONE_LOW
	JSR vrc7_write
	; begin note
	LDX #$20
	LDA TONE_HIGH
	AND #%00111111 ; keep all bits
	JSR vrc7_write
	JMP @tone_done
@release_tone:
	LDX #$20
	LDA TONE_HIGH
	AND #%00101111 ; clear trigger bit
	JSR vrc7_write
	JMP @tone_done
@stop_tone:
	LDX #$20
	LDA #0
	JSR vrc7_write
@tone_done:
	RTS

vrc7_write: ; writes A to register X
	STX VRC7_REG
	JSR vrc7_wait_reg
	STA VRC7_DAT
	JSR vrc7_wait_dat
	RTS

; vrc7 register write delay code borrowed from Lagrange Point
vrc7_wait_dat:
	STX $0082 ; note this address is arbitrary, just what LP uses, and doesn't conflict for me
	LDX #$08
@wait_loop:
	DEX
	BNE @wait_loop
	LDX $0082
vrc7_wait_reg:
	RTS

; proposed patch set
patch_set:
.incbin "rainwarrior.vrc7"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.segment "VECTORS"
	.word swap_nmi    ; vblank interrupt
	.word swap_reset  ; reset interrupt
	.word swap_irq    ; irq

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.segment "TILES"
	; fills with zeroes

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;
; end of file
;
