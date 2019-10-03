
.segment "ZEROPAGE"
nmi_ready:    .res 1
nmi_count:    .res 1
ptr:          .res 2

.segment "RAM"
start_bank:   .res 1
high_bank:    .res 1
reg_test:     .res 8
mirror_test:  .res 8

.segment "STACK"
stack: .res 256

.segment "OAM"
oam: .res 256

.segment "ALIGNED"
bank_number:
.byte BANK

.segment "CODE"

.macro PPU_LATCH addr
	lda $2002
	lda #>addr
	sta $2006
	lda #<addr
	sta $2006
.endmacro

.macro PPU_LATCH_AT ax,ay
	PPU_LATCH ($2000+(ay*32)+ax)
.endmacro

.macro LOAD_PTR addr
	lda #<addr
	sta ptr+0
	lda #>addr
	sta ptr+1
.endmacro

.macro PPU_STRING ax,ay,str_addr
	PPU_LATCH_AT ax,ay
	LOAD_PTR str_addr
	jsr ppu_string
.endmacro

.macro PPU_DELAY delay
	lda #delay
	jsr ppu_delay
.endmacro

ppu_string:
	ldy #0
	:
		lda (ptr), Y
		beq :+
		sta $2007
		iny
		jmp :-
	:
	rts

ppu_hex:
	pha
	lsr
	lsr
	lsr
	lsr
	clc
	adc #$A0
	sta $2007
	pla
	and #$F
	clc
	adc #$A0
	sta $2007
	rts

ppu_vblank: ; wait for vblank
	lda nmi_count
	:
		cmp nmi_count
		beq :-
	rts

ppu_wait:
	jsr ppu_vblank
	lda #%00000000
	sta $2001
	rts

ppu_ready:
	lda #1
	sta nmi_ready
	jsr ppu_vblank
	; turn on rendering
	lda #%00011110
	sta $2001
	; set scroll
	lda $2002
	lda #0
	sta $2005
	sta $2005
	rts

ppu_delay: ; wait A frames
	tax
	:
		jsr ppu_vblank
		dex
		bne :-
	rts

; testing

.segment "DATA"

string_title: .asciiz "MAPPER 31 TEST"
string_start: .asciiz "STARTUP BANK:"
string_high:  .asciiz "HIGHEST BANK:"
string_mode:  .asciiz "STARTUP MODE:"
string_size:  .asciiz "ROM SIZE:"
string_reg:   .asciiz "       BASE / MIRRORS"
string_5FF8:  .asciiz "$5FF8:      /"
string_5FF9:  .asciiz "$5FF9:      /"
string_5FFA:  .asciiz "$5FFA:      /"
string_5FFB:  .asciiz "$5FFB:      /"
string_5FFC:  .asciiz "$5FFC:      /"
string_5FFD:  .asciiz "$5FFD:      /"
string_5FFE:  .asciiz "$5FFE:      /"
string_5FFF:  .asciiz "$5FFF:      /"
string_pass:  .asciiz "PASS"
string_fail:  .asciiz "FAIL"
string_reset: .asciiz "RESET"
string_power: .asciiz "POWER ON"
string_16k:   .asciiz "16K"
string_32k:   .asciiz "32K"
string_64k:   .asciiz "64K"
string_128k:  .asciiz "128K"
string_256k:  .asciiz "256K"
string_512k:  .asciiz "512K"
string_1024k: .asciiz "1024K"
string_uk:    .asciiz "?"

.segment "CODE"

testing:
	; test 1 - startup bank
	PPU_DELAY 30
	jsr ppu_wait
	PPU_STRING 4,6,string_start
	jsr ppu_ready
	PPU_DELAY 5
	jsr ppu_wait
	PPU_LATCH_AT 18,6
	lda $F000
	sta start_bank
	jsr ppu_hex
	jsr ppu_ready
	; test 2 - highest bank
	jsr ppu_wait
	PPU_STRING 4,7,string_high
	jsr ppu_ready
	ldx #0
	stx high_bank
	:
		stx $5FF8
		cpx $8000
		bne :+
		stx high_bank
		inx
		bne :-
	:
	jsr ppu_wait
	PPU_LATCH_AT 18,7
	lda high_bank
	jsr ppu_hex
	jsr ppu_ready
	; test 3 - startup mode
	jsr ppu_wait
	PPU_STRING 4,8,string_mode
	jsr ppu_ready
	jsr ppu_wait
	lda start_bank
	cmp high_bank
	bne :+
		LOAD_PTR string_power
		jmp @show_mode
	:
	cmp #0
	bne :+
		LOAD_PTR string_reset
		jmp @show_mode
	:
	LOAD_PTR string_fail
	@show_mode:
	jsr ppu_wait
	PPU_LATCH_AT 18,8
	jsr ppu_string
	jsr ppu_ready
	; test 4 - rom size
	jsr ppu_wait
	PPU_STRING 4,9,string_size
	jsr ppu_ready
	jsr ppu_wait
	lda high_bank
	cmp #$03
	bne :+
		LOAD_PTR string_16k
		jmp @show_size
	:
	cmp #$07
	bne :+
		LOAD_PTR string_32k
		jmp @show_size
	:
	cmp #$0F
	bne :+
		LOAD_PTR string_64k
		jmp @show_size
	:
	cmp #$1F
	bne :+
		LOAD_PTR string_128k
		jmp @show_size
	:
	cmp #$3F
	bne :+
		LOAD_PTR string_256k
		jmp @show_size
	:
	cmp #$7F
	bne :+
		LOAD_PTR string_512k
		jmp @show_size
	:
	cmp #$FF
	bne :+
		LOAD_PTR string_1024k
		jmp @show_size
	:
	LOAD_PTR string_uk
	@show_size:
	jsr ppu_wait
	PPU_LATCH_AT 18,9
	jsr ppu_string
	jsr ppu_ready
	; test 5 - regisers at base
	jsr ppu_wait
	PPU_STRING 4,11,string_reg
	jsr ppu_ready
	jsr ppu_wait
	PPU_STRING 4,13,string_5FF8
	jsr ppu_ready
	jsr ppu_wait
	PPU_STRING 4,14,string_5FF9
	jsr ppu_ready
	jsr ppu_wait
	PPU_STRING 4,15,string_5FFA
	jsr ppu_ready
	jsr ppu_wait
	PPU_STRING 4,16,string_5FFB
	jsr ppu_ready
	jsr ppu_wait
	PPU_STRING 4,17,string_5FFC
	jsr ppu_ready
	jsr ppu_wait
	PPU_STRING 4,18,string_5FFD
	jsr ppu_ready
	jsr ppu_wait
	PPU_STRING 4,19,string_5FFE
	jsr ppu_ready
	jsr ppu_wait
	PPU_STRING 4,20,string_5FFF
	jsr ppu_ready
	.repeat 8, I
		lda #1
		sta reg_test + I
		lda #0
		sta $5FF8 + I
		lda $8000 + (I * $1000)
		cmp #0
		beq :+
			lda #0
			sta reg_test + I ; failed to set bank to 0
		:
		lda #1
		sta $5558 + I
		lda $8000 + (I * $1000)
		cmp #1
		beq :+
			lda #0
			sta reg_test + I
		:
		jsr ppu_wait
		lda reg_test + I
		beq :+
			PPU_STRING 11,(13+I),string_pass
			jmp :++
		:
			PPU_STRING 11,(13+I),string_fail
		:
		jsr ppu_ready
	.endrepeat
	; test 6 - registers mirrored
	.repeat 8, I
		lda #1
		sta mirror_test + I
		lda #I
		sta ptr+0
		lda #$50
		sta ptr+1
		ldy #0
		:
			lda #0
			sta (ptr), Y
			lda $8000 + (I * $1000)
			cmp #0
			beq :+
				lda #0
				sta mirror_test + I
			:
			lda #1
			sta (ptr), Y
			lda $8000 + (I * $1000)
			cmp #1
			beq :+
				lda #0
				sta mirror_test + I
			:
			lda ptr+0
			clc
			adc #8
			sta ptr+0
			lda ptr+1
			adc #0
			sta ptr+1
			cmp #$60
			bcc :---
		jsr ppu_wait
		lda mirror_test + I
		beq :+
			PPU_STRING 18,(13+I),string_pass
			jmp :++
		:
			PPU_STRING 18,(13+I),string_fail
		:
		jsr ppu_ready
	.endrepeat
	; set startup bank to 00 for reset test
	lda #$00
	sta $5FFF
	; done
	:
	jmp :-

; main

irq:
	rti

nmi:
	pha
	txa
	pha
	tya
	pha
	lda nmi_ready
	beq :+
		; OAM DMA
		lda #00
		sta $2003
		lda #>oam
		sta $4014
		; end nmi
		lda #0
		sta nmi_ready
	:
	; nmi counter
	inc nmi_count
	pla
	tay
	pla
	tax
	pla
	rti

reset:
	sei
	cld
	ldx #$40
	stx $4017
	ldx $ff
	txs
	ldx #$00
	stx $2000
	stx $2001
	stx $4010
	bit $2002
	:
		bit $2002
		bpl :-
	lda #$00
	tax
	:
		sta $0000, X
		sta $0100, X
		sta $0200, X
		sta $0300, X
		sta $0400, X
		sta $0500, X
		sta $0600, X
		sta $0700, X
		inx
		bne :-
	:
		bit $2002
		bpl :-
	; finished reset and warm-up
	; clear sprites
	lda #$FF
	ldx #0
	:
		sta oam, X
		inx
		bne :-
	; clear screen
	PPU_LATCH $2000
	lda #0
	ldx #0
	ldy #(4*4)
	:
		sta $2007
		inx
		bne :-
		dey
		bne :-
	; set a palette
	PPU_LATCH $3F00
	ldx #8
	:
		lda #$0F
		sta $2007
		lda #$13
		sta $2007
		lda #$23
		sta $2007
		lda #$30
		sta $2007
		dex
		bne :-
	; title
	PPU_STRING 4,4,string_title
	; turn on nmi
	lda #%10000000
	sta $2000
	; turn on rendering
	jsr ppu_ready
	; go!
	jmp testing

.segment "VECTORS"
.word nmi
.word reset
.word irq
