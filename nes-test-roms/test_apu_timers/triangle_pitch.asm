; NES APU Triangle Pitch Test

; Tests triangle's frequency timer. A tone of unchanging timbre
; should play; if it continually shifts, check timer period
; handling, then CPU instruction timing.

; Shay Green <mail.com@blargg>

main:
	sei
	
	ldx	#0		; wait for hardware
hw_delay:
	dey
	bne	hw_delay
	dex
	bne	hw_delay
	
	lda	#0		; disable nmi
	sta	$2000
	
	lda	#$04		; enable triangle
	sta	$4015
	lda	#$ff		; linear counter suspended
	sta	$4008
	lda	#$91		; period = $49
	sta	$400a
	lda	#$00
	sta	$400b
	
	lda	#0
tone_loop:
	sta	0
	pha
	pla
	ldx	#193
tone_delay_loop:
	pha
	pla
	dex
	bne	tone_delay_loop
	sta	$4011
	eor	#20
	jmp	tone_loop
	
irq:
nmi:	rti

	.org $fffa
	.word	nmi
	.word	main
	.word	irq
