bits 32

section .text

%ifdef WIN32
	%macro globsym 1 
  		global  _%1 
  		%define %1 _%1 
	%endmacro

	%macro extsym 1 
  		extern  _%1 
  		%define %1 _%1 
	%endmacro
%else ;!WIN32
	%define globsym global
	%define extsym extern
%endif ; WIN32


globsym revbits
 
	

revbits:	
; we will develop the following in x86 assembly
; 
;unsigned char b = x;
; b = ((b * 0x0802LU & 0x22110LU) | (b * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;
;
	mov eax, [esp+4]
	mov edx, eax
	imul edx, 0x802
	and edx, 0x22110
	imul eax, 0x8020
	and eax, 0x88440
	or edx, eax
	imul edx, 0x10101
	shr edx, 16
	mov al, dl
	ret

