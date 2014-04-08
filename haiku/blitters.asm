; optimised blitters
; initial code thanks to Caz Jones

bits 32

global blit_windowed_dirty_mmx
global blit_2x_mmx
global blit_2x_dirty_mmx
global blit_2x_windowed_dirty_mmx
global blit_overlay


align 16
blit_2x_mmx:
	push edi
	push esi
	
	mov edi, [esp+12]	; dest
	mov esi, [esp+16]	; source
	mov ecx, [esp+20]	; size
	shr ecx, 3
	
.loop:
	movq mm0, [esi]
	movq mm1, mm0
	punpcklbw mm0, mm0
	punpckhbw mm1, mm1
	movq [edi], mm0
	movq [edi+8], mm1
	add edi, 16
	add esi, 8
	sub ecx, 1
	jnz .loop
	
	pop esi
	pop edi
	
	emms
ret


align 16
blit_2x_dirty_mmx:
	push ebp
	mov ebp, esp
	
	push edi
	push esi
	push ebx
	push edx
	
	mov edi, [ebp+8]	; dest
	mov esi, [ebp+12]	; source
	mov eax, [ebp+16]	; dirty
	mov edx, [ebp+20]	; rowbytes
	mov ecx, [ebp+24]	; size
	shr ecx, 3

.loop:
	movq mm0, [esi]
	movq mm1, [eax]
	
	pcmpeqb mm1, mm0
	packsswb mm1, mm1
	movd ebx, mm1
	cmp ebx, 0xffffffff
	jz .skip
	
	movq [eax], mm0
	movq mm1, mm0
	punpcklbw mm0, mm0
	punpckhbw mm1, mm1
	movq [edi], mm0
	movq [edi+8], mm1
	
	; omit these two for scanlines
	movq [edi+edx], mm0 	; next line
	movq [edi+edx+8], mm1

.skip:
	add edi, 16
	add esi, 8
	add eax, 8
	sub ecx, 1
	jnz .loop
	
	pop edx
	pop ebx
	pop esi
	pop edi
	
	mov esp, ebp
	pop ebp
	
	emms
ret


blit_windowed_dirty_mmx:
	push ebp
	mov ebp, esp
	push edi
	push esi
	push ebx

	mov esi, [ebp+8]	; src
	mov eax, [ebp+12]	; dirty
	mov edi, [ebp+16]	; dst
	mov ecx, [ebp+20]	; size
	
	shr ecx, 4
	jz near .finish
	
	cmp [ebp+24], dword 2 ; 16 bit
	jz .loop16
	
	cmp [ebp+24], dword 1 ; 8 bit
	jz near .loop8
	
.loop32:
	movq mm0, [esi]
	movq mm1, [eax]
	movq mm2, [esi+8]
	movq mm3, [eax+8]
	
	pcmpeqd mm1, mm0
	pcmpeqd mm3, mm2
	
	psrld mm1, 31 ; ffffffffffffffff -> 0000000100000001
	psrld mm3, 31 ; ffffffffffffffff -> 0000000100000001
	pslld mm3, 1 ; 0000000100000001 -> 0000000200000002
	por mm1, mm3 ; 0000000300000003
	
	packsswb mm1, mm1 ; 0000000300000003 -> 00030003
	movd ebx, mm1
	cmp ebx, 0x00030003
	jz .skip32
	
	movq [eax], mm0
	movq [eax+8], mm2
	movq [edi], mm0
	movq [edi+8], mm2
	
.skip32:
	add esi, 16
	add eax, 16
	add edi, 16
	dec ecx
	jnz .loop32
	jz near .finish

.loop16:
	movq mm0, [esi]
	movq mm1, [eax]
	movq mm2, [esi+8]
	movq mm3, [eax+8]
	
	pcmpeqw mm1, mm0
	pcmpeqw mm3, mm2
	
	psrlw mm1, 15
	psrlw mm3, 15
	psllw mm3, 1
	por mm1, mm3
	
	packsswb mm1, mm1
	movd ebx, mm1
	cmp ebx, 0x03030303
	jz .skip16
	
	movq [eax], mm0
	movq [eax+8], mm2
	movq [edi], mm0
	movq [edi+8], mm2
	
.skip16:
	add esi, 16
	add eax, 16
	add edi, 16
	dec ecx
	jnz .loop16
	jz .finish

.loop8:
	movq mm0, [esi]
	movq mm1, [eax]
	movq mm2, [esi+8]
	movq mm3, [eax+8]
	
	pcmpeqw mm1, mm0
	pcmpeqw mm3, mm2
	
	psrlw mm1, 8
	psllw mm3, 8
	por mm1, mm3
	
	packsswb mm1, mm1
	movd ebx, mm1
	cmp ebx, 0xffffffff
	jz .skip8
	
	movq [eax], mm0
	movq [eax+8], mm2
	movq [edi], mm0
	movq [edi+8], mm2
	
.skip8:
	add esi, 16
	add eax, 16
	add edi, 16
	dec ecx
	jnz .loop8
	
.finish:
	pop ebx
	pop esi
	pop edi
	mov esp, ebp
	pop ebp
	emms
ret



blit_2x_windowed_dirty_mmx:
	push ebp
	mov ebp, esp
	push edi
	push esi
	push ebx
	push edx
	
	mov esi, [ebp+8]	; src
	mov eax, [ebp+12]	; dirty
	mov edi, [ebp+16]	; dst
	mov ecx, [ebp+20]	; size
	mov edx, [ebp+28]	; bytes_per_row
	
	shr ecx, 4
	jz near .finish
	
	cmp [ebp+24], dword 2 ; 16 bit
	jz .loop16
	
	cmp [ebp+24], dword 1 ; 8 bit
	jz near .loop8
	
.loop32:
	movq mm0, [esi]
	movq mm1, [eax]
	
	pcmpeqd mm1, mm0
	packsswb mm1, mm1
	movd ebx, mm1
	cmp ebx, 0xffffffff
	jz .skip32
	
	movq [eax], mm0
	movq mm1, mm0
	
	punpckldq mm0, mm0
	punpckhdq mm1, mm1
	movq [edi], mm0
	movq [edi+8], mm1
	
	movq [edi+edx], mm0 ; remove for scanlines
	movq [edi+edx+8], mm1 ; remove for scanlines

.skip32:
	add esi, 8
	add eax, 8
	add edi, 16
	dec ecx
	jnz .loop32
	jz near .finish

.loop16:
	movq mm0, [esi]
	movq mm1, [eax]
	
	pcmpeqw mm1, mm0
	packsswb mm1, mm1
	movd ebx, mm1
	cmp ebx, 0xffffffff
	jz .skip16
	
	movq [eax], mm0
	movq mm1, mm0
	
	punpcklwd mm0, mm0
	punpckhwd mm1, mm1
	movq [edi], mm0
	movq [edi+8], mm1

	movq [edi+edx], mm0 ; remove for scanlines
	movq [edi+edx+8], mm1 ; remove for scanlines
	
.skip16:
	add esi, 8
	add eax, 8
	add edi, 16
	dec ecx
	jnz .loop16
	jz .finish

.loop8:
	movq mm0, [esi]
	movq mm1, [eax]
	
	pcmpeqb mm1, mm0
	packsswb mm1, mm1
	movd ebx, mm1
	cmp ebx, 0xffffffff
	jz .skip8
	
	movq [eax], mm0
	movq mm1, mm0
	
	punpcklbw mm0, mm0
	punpckhbw mm1, mm1
	movq [edi], mm0
	movq [edi+8], mm1

	movq [edi+edx], mm0 ; remove for scanlines
	movq [edi+edx+8], mm1 ; remove for scanlines
	
.skip8:
	add esi, 8
	add eax, 8
	add edi, 16
	dec ecx
	jnz .loop8
	
.finish:
	pop edx
	pop ebx
	pop esi
	pop edi
	mov esp, ebp
	pop ebp
	emms
ret


blit_overlay:
	push edi
	push esi

	mov edi, [esp+12]	; dest
	mov esi, [esp+16]	; source
	mov ecx, [esp+20]	; size
	mov eax, [esp+24]	; y
	mov edx, [esp+28]	; ycbcr
	
	push ecx
	

.loop:	
	sub ecx, 1
	jnz .loop
	
	pop esi
	pop edi
ret
