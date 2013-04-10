; optimised copies

bits 32

global mmx_copy
global mmx2_copy
global sse_copy

align 16
mmx_copy:	
	push edi
	push esi
	
	mov edi, [esp+12]	; dest
	mov esi, [esp+16]	; source
	mov ecx, [esp+20]	; size
	xor eax, eax
	shr ecx, 3

.loop:
	movq mm0, [esi+eax*8]
	movq [edi+eax*8], mm0
	add eax, 1
	cmp ecx, eax
	jg .loop
	
	pop esi
	pop edi
	emms
ret

align 16
mmx2_copy:
	push edi
	push esi
	
	mov edi, [esp+12]	; dest
	mov esi, [esp+16]	; source
	mov ecx, [esp+20]	; size
	
	prefetchnta [esi]
	prefetchnta [esi+64]
	prefetchnta [esi+128]
	prefetchnta [esi+192]
	prefetchnta [esi+256]
	
	mov eax, edi
	and eax, 0x7
	jz .noalign
	
	mov edx, 0x7
	sub edx, eax
	sub ecx, edx
	mov eax, ecx
	mov ecx, edx
	rep movsb
	mov ecx, eax
	
.noalign:
	mov edx, ecx
	shr edx, 6
	and ecx, 0x3f
	
.loop:
	prefetchnta [esi+320]
	
	movq mm0, [esi]
	movq mm1, [esi+8]
	movq mm2, [esi+16]
	movq mm3, [esi+24]
	movq mm4, [esi+32]
	movq mm5, [esi+40]
	movq mm6, [esi+48]
	movq mm7, [esi+56]
	
	movntq [edi], mm0
	movntq [edi+8], mm1
	movntq [edi+16], mm2
	movntq [edi+24], mm3
	movntq [edi+32], mm4
	movntq [edi+40], mm5
	movntq [edi+48], mm6
	movntq [edi+56], mm7
	
	add esi, 64
	add edi, 64
	sub edx, 1
	jnz .loop
	
	sfence
	emms
	
	shr ecx, 2
	rep movsd
	and ecx, 0x3
	rep movsb
	
	pop esi
	pop edi
ret

align 16
sse_copy:
	push edi
	push esi
	
	mov edi, [esp+12]	; dest
	mov esi, [esp+16]	; source
	mov ecx, [esp+20]	; size
	
	prefetchnta [esi]
	prefetchnta [esi+64]
	prefetchnta [esi+128]
	prefetchnta [esi+192]
	
	mov edx, ecx
	shr edx, 3
	
.loop:
	prefetchnta [esi+256]
	movq mm0, [esi]
	movntq [edi], mm0
	add esi, 8
	add edi, 8
	sub edx, 1
	jnz .loop
	
	and ecx, 0x7
	jz .done
	
	rep movsb
		
.done:
	pop esi
	pop edi
	
	emms
ret
