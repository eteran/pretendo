; N163 Ttst
; Brad Smith, 7/10/2013
;
; This ROM is intended to test the N163 sound chip
; via hotswapping.
;
; these tests mainly pertain to volume and DAC activity

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
.byte "N163 swap test",           0,0
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
.byte $10 ; expansion bits
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
	LDA (SONG_READ), Y ; Y=0
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
	CMP #$00 ; 0 = load waveform
	BNE @do_reg
	; load 128 bytes of waveform data
	JSR inc_read
	; TODO
	LDX #0
@wav_loop:
	STX $F800
	LDA wave, X
	STA $4800
	INX
	CPX #$80
	BNE @wav_loop
	JMP @read_loop ; note Y is unmodified
	; write value to N163 register
@do_reg:
	JSR inc_read
	LDA (SONG_READ), Y ; Y=0
	STA $F800
	JSR inc_read
	LDA (SONG_READ), Y
	STA $4800
	JSR inc_read
	JMP @read_loop ; note Y is unmodified
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
; wave data
; note that waveform nibble pairs are reversed in each byte

wave:
.byte $00, $11, $22, $33, $44, $55, $66, $77, $88, $99, $AA, $BB, $CC, $DD, $EE, $FF ; saw up
.byte $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $00, $00, $00, $00, $00, $00, $00, $00 ; square
.byte $FF, $EE, $DD, $CC, $BB, $AA, $99, $88, $77, $66, $55, $44, $33, $22, $11, $00 ; saw dn
.byte $0F, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00 ; narrow
.byte $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00 ; blank
.byte $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00 ; blank
.byte $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00 ; blank
.byte $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00 ; blank

;
; song data
;

; variable length code:
; FF ab = return, skip the next ab frames
; FE = loop
; 00 = load waveform (128 bytes)
; 01 ab cd = write cd to N163 register ab
; ab cd ef = write ef to memory ab cd (note big endian address)

song_test:

; quick buzz
.byte $40, $11, $7F ; 4011 = 7F
.byte $FF, $00      ; end frame
.byte $40, $11, $00 ; 4011 = 00
.byte $FF, $00      ; end frame
.byte $40, $11, $7F ; 4011 = 7F
.byte $FF, $00      ; end frame
.byte $40, $11, $00 ; 4011 = 00
.byte $FF, $00      ; end frame

.ifndef NSF
; give 5 seconds to swap
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
.endif

.byte $FF, 29

; N163 setup
.byte $E0, $00, %00000000 ; enable sound
.byte $00                 ; load waveforms and write 0 to all registers
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

; 1 channel test
.byte $01, $7E, 32        ; select square wave (address 32)
.byte $01, $7A, $1E       ; A 440 mid
.byte $01, $78, $36       ; A 440 low
.byte $01, $7C, (64-8)<<2 ; 32 samples long
.byte $01, $7F, $0F       ; full volume 1 channel
.byte $FF, 59
; 2 channels
.byte $01, $7A, $3C       ; A 440 mid
.byte $01, $78, $6B       ; A 440 low
.byte $01, $7F, $1F       ; full volume
.byte $FF, 59
; 3 channels
.byte $01, $7A, $5A       ; A 440 mid
.byte $01, $78, $A1       ; A 440 low
.byte $01, $7F, $2F       ; full volume
.byte $FF, 59
; 4 channels
.byte $01, $7A, $78       ; A 440 mid
.byte $01, $78, $D6       ; A 440 low
.byte $01, $7F, $3F       ; full volume
.byte $FF, 59
; 5 channels
.byte $01, $7A, $97       ; A 440 mid
.byte $01, $78, $0C       ; A 440 low
.byte $01, $7F, $4F       ; full volume
.byte $FF, 59
; 6 channels
.byte $01, $7A, $B5       ; A 440 mid
.byte $01, $78, $41       ; A 440 low
.byte $01, $7F, $5F       ; full volume
.byte $FF, 59
; 7 channels
.byte $01, $7A, $D3       ; A 440 mid
.byte $01, $78, $77       ; A 440 low
.byte $01, $7F, $6F       ; full volume
.byte $FF, 59
; 8 channels
.byte $01, $7A, $F1       ; A 440 mid
.byte $01, $78, $AC       ; A 440 low
.byte $01, $7F, $7F       ; full volume
.byte $FF, 59
; 9 channels
.byte $01, $7A, $0F       ; A 440 mid
.byte $01, $78, $E2       ; A 440 low
.byte $01, $7C, ((64-8)<<2)+$01 ; high bit set for A440
.byte $01, $7F, $8F       ; full volume
.byte $FF, 59
.byte $01, $7F, $80       ; silence
.byte $FF, 29

; "I'm alive!" APU chirp
.byte $40, $15, %00000001
.byte $40, $03, $00
.byte $FF, 9
.byte $40, $15, %00000000
.byte $FF, 9

; volume ramp test
.byte $01, $7A, $1E       ; A 440 mid
.byte $01, $78, $36       ; A 440 low
.byte $01, $7C, (64-8)<<2 ; 32 samples long
.byte $01, $7F, $0F, $FF, 19
.byte $01, $7F, $0E, $FF, 19
.byte $01, $7F, $0D, $FF, 19
.byte $01, $7F, $0C, $FF, 19
.byte $01, $7F, $0B, $FF, 19
.byte $01, $7F, $0A, $FF, 19
.byte $01, $7F, $09, $FF, 19
.byte $01, $7F, $08, $FF, 19
.byte $01, $7F, $07, $FF, 19
.byte $01, $7F, $06, $FF, 19
.byte $01, $7F, $05, $FF, 19
.byte $01, $7F, $04, $FF, 19
.byte $01, $7F, $03, $FF, 19
.byte $01, $7F, $02, $FF, 19
.byte $01, $7F, $01, $FF, 19
.byte $01, $7F, $00, $FF, 19

; "I'm alive!" APU chirp
.byte $40, $15, %00000001
.byte $40, $03, $00
.byte $FF, 9
.byte $40, $15, %00000000
.byte $FF, 9

; volume ramp test with saw up
.byte $01, $7E, 0         ; select saw wave (address 0)
.byte $01, $7A, $1E       ; A 440 mid
.byte $01, $78, $36       ; A 440 low
.byte $01, $7C, (64-8)<<2 ; 32 samples long
.byte $01, $7F, $0F, $FF, 19
.byte $01, $7F, $0E, $FF, 19
.byte $01, $7F, $0D, $FF, 19
.byte $01, $7F, $0C, $FF, 19
.byte $01, $7F, $0B, $FF, 19
.byte $01, $7F, $0A, $FF, 19
.byte $01, $7F, $09, $FF, 19
.byte $01, $7F, $08, $FF, 19
.byte $01, $7F, $07, $FF, 19
.byte $01, $7F, $06, $FF, 19
.byte $01, $7F, $05, $FF, 19
.byte $01, $7F, $04, $FF, 19
.byte $01, $7F, $03, $FF, 19
.byte $01, $7F, $02, $FF, 19
.byte $01, $7F, $01, $FF, 19
.byte $01, $7F, $00, $FF, 19

; "I'm alive!" APU chirp
.byte $40, $15, %00000001
.byte $40, $03, $00
.byte $FF, 9
.byte $40, $15, %00000000
.byte $FF, 9

; test wave length
.byte $01, $7F, $0F       ; on
.byte $01, $7C, %00000000, $FF, 59
.byte $01, $7C, %10000000, $FF, 59
.byte $01, $7C, %11000000, $FF, 59
.byte $01, $7C, %11100000, $FF, 59
.byte $01, $7C, %11110000, $FF, 59
.byte $01, $7C, %11111000, $FF, 59
.byte $01, $7C, %11111100, $FF, 59
.byte $01, $7C, %01010100, $FF, 59
.byte $01, $7C, %10101000, $FF, 59
.byte $01, $7F, $00       ; off
.byte $FF, 29

; "I'm alive!" APU chirp
.byte $40, $15, %00000001
.byte $40, $03, $00
.byte $FF, 9
.byte $40, $15, %00000000
.byte $FF, 9

; test phase reset
.byte $01, $7E, 64        ; select saw down wave (address 64)
.byte $01, $7A, $1E       ; A 440 mid
.byte $01, $78, $36       ; A 440 low
.byte $01, $7C, (64-8)<<2 ; 32 samples long
.byte $01, $7F, $0F
.byte $01, $7D, 0, $FF, 9 ; does $7D reset phase?
.byte $01, $7D, 0, $FF, 9
.byte $01, $7D, 0, $FF, 9
.byte $01, $7D, 0, $FF, 9
.byte $01, $7D, 0, $FF, 9
.byte $01, $7D, 0, $FF, 9
.byte $01, $7B, 0, $FF, 9 ; does $7B reset phase?
.byte $01, $7B, 0, $FF, 9
.byte $01, $7B, 0, $FF, 9
.byte $01, $7B, 0, $FF, 9
.byte $01, $7B, 0, $FF, 9
.byte $01, $7B, 0, $FF, 9
.byte $01, $79, 0, $FF, 9 ; does $79 reset phase?
.byte $01, $79, 0, $FF, 9
.byte $01, $79, 0, $FF, 9
.byte $01, $79, 0, $FF, 9
.byte $01, $79, 0, $FF, 9
.byte $01, $79, 0, $FF, 9
.byte $01, $7F, $00
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
