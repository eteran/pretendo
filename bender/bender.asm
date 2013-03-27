; bender -  a 6502/2A03 emulator written in x86 assembly
;			with plenty of help from others
; 			Eli Dayan 2012-2013

bits 32


section .text


%define BENDER_2A03	(1)	; 1 for NES, 0 for stock 6502 (with decimal mode)


; on win32, symbols are prefixed by an underscore.  i don't know.  also thanks
; to the NASM manual for the next few macros.
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


; these are neat.  again, from the NASM manual.  pushes a list of registers
; onto the stack, or pops them off without having to reverse the order.
%macro  multipush 1-* 
  %rep  %0 
        push    %1 
  %rotate 1 
  %endrep 
%endmacro

%macro  multipop 1-* 
  %rep %0 
  %rotate -1 
        pop     %1
  %endrep 
%endmacro


%define MAX_PAGES 64	; pages 1K-64K supported, in powers of two.


; important addresses
; usually standard across normal 6502 systems
%define ZERO_PAGE		0x0
%define STACK 			0x100
%define NMI_VECTOR 		0xfffa
%define RESET_VECTOR 	0xfffc
%define IRQ_VECTOR 		0xfffe


; bitmasks for the P (flags) register
%define N_MASK 0x80
%define V_MASK 0x40
%define R_MASK 0x20		; antisocial flag... always 1
%define B_MASK 0x10
%define D_MASK 0x8
%define I_MASK 0x4
%define Z_MASK 0x2
%define C_MASK 0x1


; reset and interrupt latencies.  i didn't know where to put these.
%define RESET_CYCLES 		6
%define INTERRUPT_CYCLES	7


; register map
; free registers: ecx/cx/ch/cl, edx/dx/dh/dl, edi
; note S is not cached in a register (but is usually accessed through edi)
%define PC esi
%define P al
%define A ah
%define X bl
%define Y bh
%define S rS


; 6502 registers (as available to the programmer)
globsym rPC		; program counter
globsym rP		; P (flags) register
globsym rA		; accumulator
globsym rX		; x index
globsym rY		; y index
globsym rS		; stack


; cpu data (available to the programmer)
globsym bender_memory				; pointer to paged memory (64K)
globsym bender_needs_interrupt		; necessity for interrupt
globsym bender_is_jammed			; jam status
globsym bender_remaining_cycles		; # cycles left
globsym bender_total_cycles			; # cycles executed
globsym bender_cycles_to_eat		; # cycles to eat
globsym bender_read_hook			; io read hook
globsym bender_write_hook			; io write hook 
globsym bender_jam_hook				; jam hook


; exported functions (provided to programmer)
globsym bender_abort			; abort execution
globsym bender_clearirq			; clear cpu interrupt flag and return
globsym bender_dma				; dma a byte from memory and return
globsym bender_eatcycles		; eat # of cycles and return
globsym bender_elapsedcycles	; return # of cycles run so far
globsym bender_init 			; initialise.  must be called before doing anything
globsym bender_irq				; software interrupt
globsym bender_nmi				; non-maskable interrupt
globsym bender_reset			; set all regs to zero, maintenance, and go
globsym bender_run				; run cpu # of cycles
globsym bender_step				; run one instruction cycle and return


; init data
section .data

align 4
	rPC dd 0
	rA 	db 0
	rX 	db 0
	rY  db 0 
	rP  db 0
	rS 	db 0
	
align 4
	bender_memory times MAX_PAGES	dd 0
	bender_needs_interrupt			db 0
	bender_is_jammed				db 0
	bender_remaining_cycles			dd 0
	bender_total_cycles				dd 0
	bender_cycles_to_eat			dd 0
	bender_read_hook				dd 0
	bender_write_hook				dd 0
	bender_jam_hook					dd 0

align 4
	nFlag db 0
	vFlag db 0
	bFlag db 0
	dFlag db 0
	iFlag db 0
	zFlag db 0
	cFlag db 0


; the (in)famous jumptable, complete with pointers to emulated 6502 instructions
align 4
instruction_set:
	;   x0  x1  x2  x3  x4  x5  x6  x7  x8  x9  xA  xB  xC  xD  xE  xF  
	dd _00,_01,_02,_03,_04,_05,_06,_07,_08,_09,_0A,_0B,_0C,_0D,_0E,_0F ; 0x
	dd _10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_1A,_1B,_1C,_1D,_1E,_1F ; 1x
	dd _20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_2A,_2B,_2C,_2D,_2E,_2F ; 2x
	dd _30,_31,_32,_33,_34,_35,_36,_37,_38,_39,_3A,_3B,_3C,_3D,_3E,_3F ; 3x
	dd _40,_41,_42,_43,_44,_45,_46,_47,_48,_49,_4A,_4B,_4C,_4D,_4E,_4F ; 4x
	dd _50,_51,_52,_53,_54,_55,_56,_57,_58,_59,_5A,_5B,_5C,_5D,_5E,_5F ; 5x
	dd _60,_61,_62,_63,_64,_65,_66,_67,_68,_69,_6A,_6B,_6C,_6D,_6E,_6F ; 6x
	dd _70,_71,_72,_73,_74,_75,_76,_77,_78,_79,_7A,_7B,_7C,_7D,_7E,_7F ; 7x
	dd _80,_81,_82,_83,_84,_85,_86,_87,_88,_89,_8A,_8B,_8C,_8D,_8E,_8F ; 8x
	dd _90,_91,_92,_93,_94,_95,_96,_97,_98,_99,_9A,_9B,_9C,_9D,_9E,_9F ; 9x
	dd _A0,_A1,_A2,_A3,_A4,_A5,_A6,_A7,_A8,_A9,_AA,_AB,_AC,_AD,_AE,_AF ; Ax
	dd _B0,_B1,_B2,_B3,_B4,_B5,_B6,_B7,_B8,_B9,_BA,_BB,_BC,_BD,_BE,_BF ; Bx
	dd _C0,_C1,_C2,_C3,_C4,_C5,_C6,_C7,_C8,_C9,_CA,_CB,_CC,_CD,_CE,_CF ; Cx
	dd _D0,_D1,_D2,_D3,_D4,_D5,_D6,_D7,_D8,_D9,_DA,_DB,_DC,_DD,_DE,_DF ; Dx
	dd _E0,_E1,_E2,_E3,_E4,_E5,_E6,_E7,_E8,_E9,_EA,_EB,_EC,_ED,_EE,_EF ; Ex
	dd _F0,_F1,_F2,_F3,_F4,_F5,_F6,_F7,_F8,_F9,_FA,_FB,_FC,_FD,_FE,_FF ; Fx


section .text		


; get the next instruction and execute it		
%macro _next 0
	cmp [bender_remaining_cycles], dword 0	; any cycles left
	jle _run_end							; nope, we're out of here		
	
	readmem byte, PC						; next instruction sits at PC
	inc PC									; PC++
	jmp [instruction_set+ecx*4]				; goto the instruction via jumptable
%endmacro


; write a byte to memory
; %1: address
; %2: data
%macro writemem 2	
	multipush eax, edx, ecx	
	movzx eax, %2					; mov low byte into eax (al)
	push eax						; push to stack
	push %1							; push address to stack
	call dword [bender_write_hook] 	; call the hook
	add esp, 8						; adjust stack (4*2)
	multipop eax, edx, ecx
%endmacro


; read from memory
; %1: size
; %2: address
; returns: ecx/cl
; clobbers: ECX (return value)
%macro readmem 2
%ifidn %1, byte
	push eax						; save for later
	push edx 						; preserve due to C function call
	push %2							; address
	call dword [bender_read_hook]	; call the read hook
	add esp, 4					    ; adjust stack 1 param
	movzx ecx, al					; we need our byte in cl
	pop edx
	pop eax							; restore eax
%elifidn %1, word
	multipush eax, ebx, edx	
	push %2 ; address
	call dword [bender_read_hook]
	mov bl, al		; low byte
	inc dword [esp]	; addresss already on stack!
	call dword [bender_read_hook]
	add esp, 4
	mov bh, al		; high byte
	movzx ecx, bx 	; address (bx) goes in ecx
	multipop eax, ebx, edx	
%else
	%error "readmem: size not defined"
	%endif
%endmacro


; write a byte to zero page.  pretty straightforward
; %1: address
; %2: byte to write
%macro zpwrite 2
	and %1, 0xff				; ensure the address is 8-bits
	mov ebp, [bender_memory]
	mov [ebp+ZERO_PAGE+%1], %2	; ebp already has a pointer to paged memory
%endmacro


; read a byte or word from zero page.    
; %1: size
; %2: address to read from
; returns: ecx/cl
%macro zpread 2
	mov ebp, [bender_memory]
	and %2, 0xff						; ensure address is 8-bits
%ifidn %1, byte							; is it a bytewise read
	and ecx, 0xff
	mov cl, byte [ebp+ZERO_PAGE+%2]		; yes, retrieve data
%elifidn %1, word						; no, we need to fetch a word
	mov ecx, %2
	mov dl, byte[ebp+ZERO_PAGE+ecx]		; low byte
	inc cl
	mov dh, byte[ebp+ZERO_PAGE+ecx]		; high byte
	movzx ecx, dx
%else
	%error "zpread: size not defined"
%endif
%endmacro


; save all 6502 registers and unpack the flags
; quite self-explanatory
; clobbers: EDX (usage of unpackflags)
%macro save 0
	mov A, [rA]
	mov X, [rX]
	mov Y, [rY]
	mov P, [rP]
	unpackflags
	mov PC, [rPC]
%endmacro


; restore all 6502 registers and pack the flags back up
; quite self-explanatory
%macro restore 0
	mov [rA], A
	mov [rX], X
	mov [rY], Y
	packflags
	mov [rP], P
	mov [rPC], PC
%endmacro


; this is what happens when a jam instruction is encountered
%macro jam 0
	multipush eax, ebx, ecx, edx, edi, esi, ebp	; save x86 regs
	save										; and 6502
	call [bender_jam_hook]						; call the hook
	multipop eax, ebx, ecx, edx, edi, esi, ebp	; restore x86 regs
	mov [bender_remaining_cycles], dword 0		; we're jammed, so nothing to do
%endmacro


; takes individual flags and 'packs' them into a suitable 'P' (flags) register for the 
; 6502.  fairly straightforward.
%macro packflags 0
	mov P, R_MASK	; make sure we set R
	
	test [nFlag], byte 0x1
	jz %%vflag
	or P, N_MASK
	
%%vflag:
	test [vFlag], byte 0x1
	jz %%dflag
	or P, V_MASK
	
%%dflag:
	test [dFlag], byte 0x1
	jz %%iflag
	or P, D_MASK

%%iflag:
	test [iFlag], byte 0x1
	jz %%zflag
	or P, I_MASK
	
%%zflag:
	test [zFlag], byte 0x1
	jz %%cflag
	or P, Z_MASK
	
%%cflag:	
	test [cFlag], byte 0x1
	jz %%done
	or P, C_MASK
%%done:
%endmacro


; this does the oppposite of packflags.  it will take a properly formed 6502 'P'
; register, and separate it into individual flags, as our operations expect
; them to be.  each flag is shifted into the x86 carry flag, then set/cleared by setc.
;
; clobbers: EDX
%macro unpackflags 0
	mov dl, P
	shr dl, 1
	setc [cFlag]	; C
	
	shr dl, 1
	setc [zFlag]	; Z
	
	shr dl, 1
	setc [iFlag]	; I
	
	shr dl, 1
	setc [dFlag]	; D
	
	shr dl, 3		; skip B,R
	setc [vFlag]	; V
	
	shr dl, 1
	setc [nFlag]	; N		
%endmacro


; pushes a byte onto the 6502 stack.
; %1: byte to push
%macro pushstack 1
	mov ebp, [bender_memory]
	movzx edi, byte [S]
	mov [ebp+STACK+edi], %1 
	dec byte [S]
%endmacro


; pops a byte off the 6502 stack.
; %1: byte to pop into
%macro popstack 1
	inc byte [S]
 	movzx edi, byte [S]  
 	mov ebp, [bender_memory]
 	mov %1, [ebp+STACK+edi]
%endmacro


; use this for counting cycles.  not much fun to be had here.
%macro clock 1
	add [bender_total_cycles], dword %1
	sub [bender_remaining_cycles], dword %1
%endmacro


; use for se* instructions
%macro setflag 1
	mov %1, byte 0x1
	clock 2
%endmacro


; use for cl* instructions
%macro clearflag 1
	mov %1, byte 0x0
	clock 2
%endmacro


; some instructions set both n and z.  this macro does the same, based on the x86 flags
%macro setnzflags 1
	test %1, 0xff	; needed this since some operations (mov) do not affect flags
	sets [nFlag]
	setz [zFlag]
%endmacro


;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       addressing         ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; absolute address in ecx
%macro absoluteaddress 0
	readmem word, PC
	add PC, 2
%endmacro


; immediate byte in cl
%macro immediate 0
	readmem byte, PC
	inc PC
%endmacro


; absolute-X address in ecx 
%macro absolutexaddress 0
	absoluteaddress
	add cl, X	; add X offset 
	jnc %%done	; check for page cross (if we got a carry)
	
	inc ch		; icrement address high byte
	clock 1		; add a cycle

%%done: 		
%endmacro


; absolute-X byte in cl
%macro absolutexbyte 0
	absolutexaddress
	readmem byte, ecx
%endmacro


; absolute byte in cl
%macro absolutebyte 0
	absoluteaddress
	readmem byte, ecx
%endmacro


; absolute read for RMW operations
; cl: byte to write
; edx: address written to
%macro readabsolute 0
	absoluteaddress
	mov edx, ecx
	readmem byte, edx
%endmacro


; absolutex read for RMW operations
; cl: byte to write
; edx: address written to
%macro readabsolutex 0
	readmem word, PC
	mov edx, ecx
	add dl, X		; add X offset
	jnc %%done		; check for page cross (if we got a carry)
	
	inc dh			; increment address high byte

%%done:
	readmem byte, edx		; read a byte at the address
	add PC, 2
%endmacro


; absolutey read for RMW operations
; cl: byte to write
; edx: address written to
%macro readabsolutey 0
	readmem word, PC
	mov edx, ecx
	add dl, Y		; add Y 
	jnc %%done		; check for page cross (if we got a carry)
	
	inc dh			; icrement address high byte

%%done:
	readmem byte, edx		; read a byte at the address
	add PC, 2
%endmacro


; zero page address in cl
%macro zpaddress 0
	immediate	; this works out nicely
%endmacro


; read zero page byte in cl
%macro zpbyte 0
	zpaddress
	zpread byte, ecx
%endmacro


; absolute-y address in ecx
%macro absoluteyaddress 0
	absoluteaddress
	add cl, Y		; add Y offset
	jnc %%done		; check for page cross
	
	inc ch			; increment high byte
	clock 1			; one extra cycle

%%done:
%endmacro


; absolute-y byte in cl
%macro absoluteybyte 0
	absoluteyaddress
	readmem byte, ecx
%endmacro


; indirect-y address in ecx
%macro indirectyaddress 0
	zpaddress
	zpread word, ecx
	add cl, Y
	jnc %%done
	
	inc ch
	clock 1
%%done:
%endmacro


; indirect-y byte in cl
%macro indirectybyte 0
	indirectyaddress
	readmem byte, ecx
%endmacro


; indirect-x address in ecx
%macro indirectxaddress 0
	zpaddress
	add cl, X
	and ecx, 0xff
	zpread word, ecx
	;and ecx, 0xffff
%endmacro


; indirect-x read for RMW operations
; cl: byte to write
; edx: address written to
%macro readindirectx 0
	indirectxaddress
	mov edx, ecx
	readmem byte, ecx
%endmacro


; indirect-y read for RMW operations
; cl: byte to write
; edx: address written to
%macro readindirecty 0
	indirectyaddress
	mov edx, ecx
	readmem byte, ecx
%endmacro


; indirect-x byte in cl
%macro indirectxbyte 0
	indirectxaddress
	readmem byte, ecx
%endmacro


; zeropage read for RMW operations
; cl: byte to write
; edx: address written to
%macro readzp 0
	zpaddress
	mov edx, ecx
	zpread byte, edx
%endmacro


; zeropage-x address in ecx
%macro zpxaddress 0
	zpaddress
	add cl, X
%endmacro


; zeropage-x byte in cl
%macro zpxbyte 0
	zpxaddress
	zpread byte, ecx
%endmacro


; zeropagex read for RMW operations
; cl: byte to write
; edx: address written to
%macro readzpx 0
	zpxaddress
	mov edx, ecx
	zpread byte, edx
%endmacro


; zeropage-y address in cl
%macro zpyaddress	0
	zpaddress
	add cl, Y
%endmacro


; zeropage-y read for RMW operations
; cl: byte to write
; edx: address written to
%macro readzpy 0
	zpyaddress
	mov edx, ecx
	zpread byte, edx
%endmacro


; zeropage-y byte in cl
%macro zpybyte 0
	zpyaddress
	zpread byte, ecx
%endmacro


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;        instructions        ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


%macro addc 2
%if BENDER_2A03
	%1
	mov P, byte 0xff		
	add P, byte [cFlag]		; prepare flags
	adc A, cl				; do the add
	sets [nFlag]
	seto [vFlag]			; adjust the flags accordingly
	setz [zFlag]
	setc [cFlag]
	jmp %%done
%else ; !BENDER_2A03	
	test [dFlag], byte 0x1
	jnz %%dmode
	; it is fairly simple without decimal moode!
	%1
	mov P, byte 0xff
	add P, byte [cFlag]
	adc A, cl
	sets [nFlag]
	seto [vFlag]
	setz [zFlag]
	setc [cFlag]
	jmp %%done
%endif	
%%dmode:
	%1
	xor edx, edx
	xor edi, edi
	
	mov dl, A					; dl = A
	and dl, 0xf					; dl = dl & 0xf	
	mov edi, edx				; edi = (A & 0xf)
	
	mov dl, cl					; dl = data
	and dl, 0xf					; dl = dl & 0xf
	add edi, edx 				; edi += (data & 0xf)
	movzx edx, byte [cFlag]
	add edi, edx 				;edi = (A & 0xf) + (data & 0xf) + C	
	
	cmp edi, 9					; past 9?
	jg %%adjust					; yes, add 6
	jmp %%continue
	
%%adjust:
	add edi, 6
	
%%continue:
	push ebx
	xor edx, edx
	xor ebx, ebx
	
	mov ebx, edi	; ebx = tmp
	and bl, 0xf		; ebx = tmp & 0x0f
	movzx edx, A	; edx = A
	and edx, 0xf0	; edx = A & 0xf0
	add ebx, edx	; edx += (A & 0xf0)
	mov dl, cl		; dl = data
	and dl, 0xf0	; dl = data & 0xf0
	add ebx, edx	; tmp = (tmp & 0xf) + (A & 0xf0) + (data & 0xf0)
	
	; check for carry
	cmp edi, 0xf
	jg %%fix
	
	jmp %%dozflag
	
%%fix:
	; apply carry from before
	add ebx, 0x10
	
%%dozflag:	
	mov edi, ebx	; ebx = tmp
	mov dl, A		; A
	movzx ebx, cl	; data
	add ebx, edx	; A+data
	add ebx, [cFlag] ; ebx = A+data+C
	and ebx, 0xff
	setz [zFlag]	; set Z
	
	test edi, 0x80
	setne [nFlag]	; set N
	
    mov ebx, edi
    movzx edx, A    ; edx = A
    xor edx, ebx     ; A = A ^ tmp
    and edx, 0x80    ; ((A ^ tmp) & 0x80)

    mov dh, A
    xor dh, cl        ; (A ^ data)
    and dh, 0x80    ; ((A ^ data) & 0x80))
    xor dh, 0x80
    and dh, dl
    setnz [vFlag]	; set V

	mov ebx, edi
	and ebx, 0x1f0
	cmp ebx, 0x90	; check upper nybble
	jg %%fixh
	jmp %%checkh
	
%%fixh:
	add edi, 0x60	; adjust (add 0x60)
	
%%checkh:
	mov ebx, edi
	and ebx, 0xff0
	cmp ebx, 0x90	; check for past 9 overflow to set carry
	setg [cFlag]
	
	mov ebx, edi
	mov A, bl
	pop ebx

%%done:
	clock %2
%endmacro


; bit (bit test)
%macro bit 2
	%1
	mov dl, cl		; save operand
	shr cl, 7		; get d6 into carry
	setc [vFlag]	; v is set according to d6
	shr cl, 1		; get d7 into carry
	setc [nFlag]	; set sign
	and dl, A		; and A with operand
	setz [zFlag]	; set Z
	clock %2
%endmacro


; branch (b**)
; %1: type of jump (jz,jnz)
; %2: flag
%macro branch 2
	test %2, byte 0x1	; test the flag
	%1 %%branch			; take selected branch
	
	inc PC				; no branch, move along and clock
	clock 2				
	jmp %%done

%%branch:
	immediate			; fetch offset
	movsx ecx, cl		; sign extend it to ecx
	mov edx, PC			; edx gets PC
	add dl, cl			; apply offset
	jnc %%nopenalty		; 1-cycle penalty for crossing a page
	clock 1

%%nopenalty:
	add ecx, PC			; get our PC back
	movzx PC, cx		; normalise it
	clock 3
%%done:
%endmacro


; brk (break / (software interrupt))
%macro brk 0
	mov edx, PC						; save PC
	inc dx							; increment it
	pushstack dh					; push pc high
	pushstack dl					; push pc low
	packflags						
	mov [bFlag], byte 0x1
	pushstack P						; push flags with B set
	mov [iFlag], byte 0x1			; set I
	readmem word, IRQ_VECTOR	
	mov PC, ecx						; read from irq vector
	clock INTERRUPT_CYCLES
%endmacro


; compare macro for cmp,cpx,cpy
%macro compare 3
	%2				; load operand
	cmp %1, cl		; perform the compare
	sets [nFlag]	; n flag
	setz [zFlag]	; z flag
	setnc [cFlag]	; c flag (inverted)
	clock %3
%endmacro


; dec (decrement memory)
; %1: read
; %2: write
; %3: cycles
%macro decm 3
	%1				; load operand	
	dec cl			; do the decrement
	setnzflags cl	; set n and z
	%2 edx, cl		; write it back
	clock %3
%endmacro


; dex (decrement X)
%macro dex 0
	dec X
	setnzflags X
	clock 2
%endmacro


; dey (decrement Y)
%macro dey 0
	dec Y
	setnzflags Y
	clock 2
%endmacro


; inc (increment memory)
; %1: read
; %2: write
; %3: cycles
%macro incm 3
	%1				; read a byte
	inc cl			; increment it
	setnzflags cl	; flags
	%2 edx, cl		; write it back (destaddr in edx)
	clock %3
%endmacro


; inx (increment X)
%macro inx 0
	inc X
	setnzflags X
	clock 2
%endmacro


; iny (increment Y)
%macro iny 0
	inc Y
	setnzflags Y
	clock 2
%endmacro


; jmp $nnnn (jump to address)
%macro jmpa 0
	readmem word, PC	 	; read a word at PC
	mov PC, ecx				; jump there
	clock 3
%endmacro


; jmp ($nnnn) (jump to address pointed to by $nnnn).  as a result of hardware design and
; optimisation, there is an apparent 'bug' in this instruction.  any destination address 
; with a low byte of $ff, the 6502 will not jump to $xxff and $xxff+1, but rather $xxff 
; and $xx00.  this behaviour is the result of the MOS engineers not wanting to waste gates
; to increment the high byte of the pointer.
%macro jmpi 0
	readmem word, PC
	mov edx, ecx			; check low byte
	and dl, 0xff
	cmp dl, 0xff			; is it $ff?
	je %%fix				; yep, let's go adjust it
	
	; otherwise normal treatment
	readmem word, ecx		; read a word
	mov PC, ecx				; set PC
	jmp %%done				; we're done

%%fix:
	readmem byte, ecx 		; read low byte
	push ecx				; save it for later	
	and edx, 0xff00			; clear out PC low byte
	readmem byte, edx		; read a byte, and store in edx
	shl cx, 8				; shift address into upper byte
	movzx PC, cx			; get PC back
	pop ecx					; pop it
	or PC, ecx				; merge

%%done:
	clock 5
%endmacro


; jsr $nnnn	(jump to subroutine at $nnnn)
%macro jsr 0
	mov ecx, PC
	inc ecx
	pushstack ch			; push high byte
	pushstack cl			; and low byte
	readmem word, PC		; read a word at PC
	mov PC, ecx				; get PC back
	clock 6
%endmacro


; load macro for lda/ldx/ldy
; %1: byte to read (A,X,Y)
; %2: read
; %3: cycles
%macro load 3
	%2					
	mov %1, cl			 
	setnzflags %1
	clock %3
%endmacro


; store macro for sta/stx/sty
; %1: byte to write (A,X,Y)
; %2: read
; %3: write
; %4: cycles
%macro store 4
	%2
	%3 ecx, %1
	clock %4
%endmacro


; shift macro for asl/lsr
; %1: direction
; %2: read
; %3: write
; %4: clock
%macro shift 2-4
%if %0 == 2 					; if 2 arguments were supplied, then A is implied
	%ifidn %1, right			; get shift direction
		shr A, 1				; shift
		setc [cFlag]			; flags
		setnzflags A
		clock 2
	%else
		shl A, 1				; same as above but left shift
		setc [cFlag]
		setnzflags A
		clock 2
	%endif
%else							; four arguments, operand must be fetched
	%ifidn %1, right			; get shift direction
		%2						; read
		shr cl, 1
		setc [cFlag]			; set carry
		mov [nFlag], byte 0x0	; clear N
		setz [zFlag]			; set Z
		%3 edx, cl				; write shifted byte
		clock %4
	%else 						; same as above but left shift
		%2
		shl cl, 1
		setc [cFlag]
		sets [nFlag]			; but here we set N
		setz [zFlag]
		%3 edx, cl
		clock %4
	%endif
%endif			
%endmacro


; nop (no operation)
; does nothing, but PC and clock offsets can be supplied
%macro nop 2
	add PC, %1
	clock %2
%endmacro


; pha (push accumulator to stack)
%macro pha 0
	pushstack A
	clock 3
%endmacro


; php (push P (flags) to stack)
%macro php 0
	packflags
	or P, B_MASK	; ensure B is set
	pushstack P
	clock 3
%endmacro


; pla (pull accumulator from stack)
%macro pla 0
	popstack A
	setnzflags A
	clock 4
%endmacro


; plp (pull P (flags) from stack)
; clobbers: EDX (usage of unpackflags)
%macro plp 0
	popstack P
	or P, R_MASK	; ensure R is set
	unpackflags
	clock 4
%endmacro


; rotate macro for rol (rotate left), ror (rotate right)
; %1: direction
; %2: read
; %3: write
; %4: clocks
%macro rotate 2-4
	%if %0 > 2	; !A
		%2		; fetch operand
	%endif
	
	mov P, byte 0xff
	add P, byte [cFlag]	; fix carry flag
	
	%ifidn %1, right	; find rotate direction
		%if %0 == 2		; two params means A
			rcr A, 1	; rotate A
		%else
			rcr cl, 1	; otherwise rotate operand
		%endif
	%elifidn %1, left
		%if %0 == 2		; two params means A
			rcl A, 1	; rotate A
		%else
			rcl cl, 1	; otherwise rotate operand
		%endif
	%endif
	
	setc [cFlag]		; take care of carry flag
	
	%if %0 == 2 ; A
		setnzflags A	; set flags based on A
		clock 2
	%else
		setnzflags cl	; set flags based on operand
		%3 edx, cl		; write it back
		clock %4
	%endif
%endmacro


; rti (return from interrupt)
; clobbers: EDX (usage of unpackflags)
%macro rti 0
	popstack P
	unpackflags
	or P, R_MASK
	xor edx, edx	
	popstack dl		; pop PC low byte
	popstack dh		; pop PC high byte
	mov PC, edx		; save it
	clock 6
%endmacro


; rts (return from subroutine)
%macro rts 0
	xor edx, edx
	popstack dl		; pop PC low byte
	popstack dh		; pop PC high byte
	inc dx			; increment it
	mov PC, edx		; save it
	clock 6
%endmacro

	
; sbc (subtract with carry (borrow))
; %1: read
; %2: clock
%macro sbc 2
	%1
%if BENDER_2A03
	mov P, byte [cFlag]			
	sub P, 1				; set up P as 0 or -1 to get the carry logic right					
	sbb A, cl				; subtract	
	sets [nFlag]			; flags
	seto [vFlag]
	setz [zFlag]
	setnc [cFlag]
	jmp %%done
%else
	test [dFlag], byte 0x1	; same as above, but with D flag logic   
	jnz %%decimal                

	mov P, byte [cFlag]			
	sub P, 1
	sbb A, cl
	sets [nFlag]
	seto [vFlag]
	setz [zFlag]
	setnc [cFlag]	
	jmp %%done
%endif
%%decimal:
	push ebx
	mov P, byte [cFlag]
	sub P, 1			; set up P as 0 or -1 to get the carry logic right

	xor edi, edi		; clear out regs and get A and arg copied in and split up
	xor edx, edx
	
	mov bh, cl
	mov bl, cl
	and ebx, dword 0xf00f
	mov edi, ebx
	mov bh, A
	mov bl, A
	and ebx, dword 0xf00f
	sub ebx, edi
	; if the low nybble borrows, it will do the - 0x10 on the high nybble for us
	movsx edi, P
	add ebx, edi
	; applies the borrow bit, so lda #0 / sbc #0 gets to be #$ff before the fixup
	test ebx, dword 0x10		; check for low borrow
	jz %%nofixl
	
	sub bl, 6					; adjust lower nybble accordingly

%%nofixl:
	test ebx, dword 0x10000		; check for high borrow
	jz %%nofixh
	sub bh, 0x60				; adjust upper nybble accordingly

%%nofixh:
	and ebx, 0xf00f				; mask to the result
	or bl, bh					; combine the nybbles back together
	mov P, byte [cFlag]			; redo the subtract without fixups to do flags,
	sub P, 1					; and throw the result away
	
	mov dl, A
	sbb dl, cl
	sets [nFlag]
	seto [vFlag]
	setz [zFlag]
	setnc [cFlag]
	mov A, bl
	pop ebx
%%done:
	clock %2
%endmacro

; transfer macro for all t** operations
%macro transfer 2
%ifidn %1, S	; tsx
	mov X, [S]
	setnzflags X
%elifidn %2, S ; txs
	mov [S], X
%else
	; any other transfer, tax,tay,etc...
	mov %2, %1
	setnzflags %2
%endif	
	clock 2
%endmacro


; alu operation (and/or/xor)
%macro aluop 3
	%2				; fetch operand
	%1 A, cl		; and/or/xor A with cl
	setnzflags A	; set n and z
	clock %3
%endmacro



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; undocumented instructions ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; asr: this opcode ANDs the contents of the A register with an immediate value and 
; then LSRs the result.
%macro asr 2
	%1				; get immediate
	and cl, A		; and with A
	shr cl, 1		; now shift right
	setc [cFlag]
	mov A, cl		; get result
	setnzflags A	; flags
	clock %2
%endmacro


; slo: this opcode ASLs the contents of a memory location and then ORs the result 
; with the accumulator.
%macro slo 3
	%1					; read from memory
	shl cl, 1			; shift left
	setc [cFlag]
	or A, cl			; or the result
	setnzflags A
	%2 edx, cl			; write it back
	clock %3
%endmacro


; sax: ANDs the contents of the A and X registers (without changing the 
; contents of either register) and stores the result in memory.
; does not affect any flags in the processor status register
%macro sax 3
	%1			; read a byte
	mov cl, A	; get A
	and cl, X	; and with X
	%2 edx, cl	; write it
	; no flags
	clock %3
%endmacro


; dcp: this opcode DECs the contents of a memory location and then CMPs the result 
; with the A register.
%macro dcp 3
	%1				; get a byte
	dec cl			; dec it
	cmp A, cl		; compare with A
	sets [nFlag]	; set flags
	setz [zFlag]
	setnc [cFlag]	; carry is inverted
	%2 edx, cl		; write it back
	clock %3
%endmacro


; isc: this opcode INCs the contents of a memory location and then SBCs the result 
; from the A register.
%macro isc 3
	%1						; read a byte	
	inc cl					; increment it
	%2 edx, cl
	
%if BENDER_2A03
	mov P, byte [cFlag]			
	sub P, 1				
	sbb A, cl					
	sets [nFlag]
	seto [vFlag]
	setz [zFlag]
	setnc [cFlag]	
	jmp %%done
%else
	test [dFlag], byte 0x1
	jnz %%dmode
	
	mov P, byte [cFlag]			
	sub P, 1				
	sbb A, cl					
	sets [nFlag]
	seto [vFlag]
	setz [zFlag]
	setnc [cFlag]	
	jmp %%done
%endif
%%dmode:
	push ebx
	mov P, byte [cFlag]
	sub P, 1		; set up P as 0 or -1 to get the carry logic right
	
	xor edi, edi	; clear out regs and get A and arg copied in and split up
	xor edx, edx
	
	mov bh, cl
	mov bl, cl
	and ebx, dword 0xf00f
	mov edi, ebx
	mov bh, A
	mov bl, A
	and ebx, dword 0xf00f
	sub ebx, edi
	; if the low nybble borrows, it will do the - 0x10 on the high nybble for us
	movsx edi, P
	add ebx, edi
	; applies the borrow bit, so lda #0 / sbc #0 gets to be #$ff before the fixup
	test ebx, dword 0x10		; check for low borrow
	jz %%nofixl
	
	sub bl, 6					; adjust lower nybble accordingly

%%nofixl:
	test ebx, dword 0x10000		; check for high borrow
	jz %%nofixh
	sub bh, 0x60				; adjust upper nybble accordingly

%%nofixh:
	and ebx, 0xf00f				; mask to the result
	or bl, bh					; combine the nybbles back together
	
	mov P, byte [cFlag]			; redo the subtract without fixups to do flags,
	sub P, 1					; and throw the result away
	
	mov dl, A
	sbb dl, cl
	sets [nFlag]
	seto [vFlag]
	setz [zFlag]
	setnc [cFlag]
	mov A, bl
	pop ebx

%%done:
	clock %3	
%endmacro



; lax: this opcode loads both the accumulator and the X register with the contents 
; of a memory location.
%macro lax 2
	%1				; read a byte
	mov A, cl		; into A
	mov X, A		; and X as well
	setnzflags A
	clock %2
%endmacro


; sre: LSRs the contents of a memory location and then EORs the result with 
; the accumulator.
%macro sre 3
	%1				; get a byte
	shr cl, 1		; shift it right
	setc [cFlag]
	xor A, cl		; xor it with A
	setnzflags A
	%2 edx, cl		; write it back
	clock %3
%endmacro


; rla: ROLs the contents of a memory location and then ANDs the result with 
; the accumulator
%macro rla 3
	%1					; get a byte
	mov P, byte 0xff	; prepare C flag
	add P, byte [cFlag]
	rcl cl, 1			; rotate it
	setc [cFlag]
	and A, cl			; do the and
	setnzflags A
	%2 edx, cl			; write it back
	clock %3
%endmacro


; rra: RORs the contents of a memory location and then ADCs the result with 
; the accumulator.
%macro rra 3
	%1
	mov P, byte 0xff	; prepare carry
	add P, byte [cFlag]
	rcr cl, 1			; do the rotate
	setc [cFlag]
	%2 edx, cl
	
%if	BENDER_2A03
	mov P, 0xff			; take care of carry (again)
	add P, byte [cFlag]
	adc A, cl			; perform the add
	sets [nFlag]
	seto [vFlag]
	setz [zFlag]
	setc [cFlag]
	jmp %%done
%else
	test [dFlag], byte 0x1
	jnz %%dmode
	
	mov P, 0xff			; take care of carry (again)
	add P, byte [cFlag]
	adc A, cl			; perform the add
	sets [nFlag]
	seto [vFlag]
	setz [zFlag]
	setc [cFlag]
	jmp %%done
%endif	
%%dmode:
	xor edx, edx
	xor edi, edi
	
	mov dl, A					; dl = A
	and dl, 0xf					; dl = dl & 0xf	
	mov edi, edx				; edi = (A & 0xf)
	
	mov dl, cl					; dl = data
	and dl, 0xf					; dl = dl & 0xf
	add edi, edx 				; edi += (data & 0xf)
	movzx edx, byte [cFlag]
	add edi, edx 				;edi = (A & 0xf) + (data & 0xf) + C	
	
	cmp edi, 9
	jg %%adjust
	jmp %%continue
	
%%adjust:
	add edi, 6
	
%%continue:
	push ebx
	xor edx, edx
	xor ebx, ebx
	
	mov ebx, edi	; ebx = tmp
	and bl, 0xf		; ebx = tmp & 0x0f
	movzx edx, A	; edx = A
	and edx, 0xf0	; edx = A & 0xf0
	add ebx, edx	; edx += (A & 0xf0)
	mov dl, cl		; dl = data
	and dl, 0xf0	; dl = data & 0xf0
	add ebx, edx	; tmp = (tmp & 0xf) + (A & 0xf0) + (data & 0xf0)
	
	cmp edi, 0xf
	jg %%fix
	
	jmp %%dozflag
	
%%fix:
	add ebx, 0x10
	
%%dozflag:	
	mov edi, ebx	; ebx = tmp
	mov dl, A		; A
	movzx ebx, cl	; data
	add ebx, edx	; A+data
	add ebx, [cFlag] ; ebx = A+data+C
	and ebx, 0xff
	setz [zFlag]	; set Z
	
	test edi, 0x80
	setne [nFlag]	; set N
	
    mov ebx, edi
    movzx edx, A    ; edx = A
    xor edx, ebx     ; A = A ^ tmp
    and edx, 0x80    ; ((A ^ tmp) & 0x80)

    mov dh, A
    xor dh, cl        ; (A ^ data)
    and dh, 0x80    ; ((A ^ data) & 0x80))
    xor dh, 0x80
    and dh, dl
    setnz [vFlag]	; set V

	mov ebx, edi
	and ebx, 0x1f0
	cmp ebx, 0x90
	jg %%fixh
	jmp %%almost
	
%%fixh:
	add edi, 0x60
	
%%almost:
	mov ebx, edi
	and ebx, 0xff0
	cmp ebx, 0x90
	setg [cFlag]
	
	mov ebx, edi
	mov A, bl
	pop ebx

%%done:
	clock %3
%endmacro


; sbx: ANDs the contents of the A and X registers (leaving the contents of A 
; intact), subtracts an immediate value, and then stores the result in X.
%macro sbx 0
	immediate		; get a byte
	mov dl, A		; save A in dl
	and dl, X		; and it with X
	sub dl, cl		; subtract immediate value
	setnc [cFlag]
	mov X, dl		; store result in X
	setnzflags X
	clock 2
%endmacro


; anc: ANDs the contents of the A register with an immediate value and then 
; moves bit 7 of A into the Carry flag.
%macro anc 0
	immediate		; fetch a byte
	and A, cl		; and it with A
	setnzflags A
	mov dl, [nFlag]	; move bit 7 to carry flag
	mov [cFlag], dl
%endmacro


; arr: this is a rather funky instruction, which also has decimal mode.  
; here is how it was described in documentation:
;	
; 	AND byte with accumulator, then rotate one bit right in accumulator 
;	and check bit 5 and 6:
;	if both bits are 1: set C, clear V.
;	if both bits are 0: clear C and V.
;	if only bit 5 is 1: set V, clear C.
;	if only bit 6 is 1: set C and V.
;   Status flags: N,V,Z,C
%macro arr 0
	immediate
%if BENDER_2A03	
	and cl, A
	shr cl, 1
	mov A, cl
	mov dl, byte [cFlag]
	shl dl, 7
	or cl, dl
	mov A, cl
	setnzflags A
	test A, 0x40
	setnz [cFlag]
	mov dl, A
	mov dh, A
	shr dl, 6
	shr dh, 5
	xor dl, dh
	and dl, 0x1
	setnz [vFlag]
	jmp %%done
%else
	test byte [dFlag], 0x1 
	jnz %%decimal

	and cl, A
	shr cl, 1
	mov A, cl
	mov dl, byte [cFlag]
	shl dl, 7
	or cl, dl
	mov A, cl
	setnzflags A
	test A, 0x40
	setnz [cFlag]
	mov dl, A
	mov dh, A
	shr dl, 6
	shr dh, 5
	xor dl, dh
	and dl, 0x1
	setnz [vFlag]
	jmp %%done
%endif
%%decimal:
	push ebx
	
	xor edx, edx
	xor edi, edi
	movzx edi, A
	movzx edx, cl
	and edx, edi				; t = edx
	mov bl, byte [cFlag]
	mov byte [nFlag], bl
	movzx ebx, byte [cFlag]
	shl ebx, 8
	or ebx, edx					; rotate through carry
	shr ebx, 1
	test bl, byte 0xff
	setz byte [zFlag]
	mov A, bl
	xor bl, dl
	test bl, 0x40				; set V accordingly, (t ^ A) & 64
	setnz byte [vFlag]
	mov dh, dl
	and edx, dword 0xf00f		; split the nybbles up
	mov bl, dl
	and bl, 0x1
	add dl, bl
	cmp dl, 5
	jbe %%nofixl
	
	mov bl, A
	add bl, 6
	and bl, 0xf
	and A, 0xf0
	or A, bl

%%nofixl:
	shr dh, 4
	mov bl, dh
	and bl, 1
	add dh, bl
	pop ebx
	cmp dl, 5
	seta byte [cFlag]			; C = AH + (AH & 1) > 5
	jbe %%done
	add A, 0x60

%%done:
%endmacro


; ane*: transfers the contents of the X register to the A register and then 
; ANDs the A register with an immediate value.
%macro ane 0
	immediate
	mov A, X
	and A, cl
	setnzflags A
%endmacro


; las*: This opcode ANDs the contents of a memory location with the contents of the 
; stack pointer register and stores the result in the accumulator, the X 
; register, and the stack pointer.  Affected flags: N Z.
%macro las 0
	absoluteybyte	
	mov dl, byte [S]
	and cl, dl
	mov A, cl
	mov X, cl
	mov [S], cl
	setnzflags cl	
%endmacro


; lxa*: This opcode ORs the A register with #$EE, ANDs the result with an immediate 
; value, and then stores the result in both A and X.
%macro lxa 0
	immediate
	or A, 0xee
	and A, cl
	mov X, A
	setnzflags X
  	clock 2
%endmacro


; shaiy*: this opcode stores the result of A AND X AND the high byte of the target 
; address of the operand +1 in memory.
%macro shaiy 0
	zpaddress
	zpread word, ecx
	mov edx, ecx
	inc ch
	and ch, X
	and ch, A
	add dl, Y
	jnc %%done
	
	mov dh, ch
	clock 1

%%done:
	writemem edx, ch			; PC inc already done by zpaddress
	clock 6
%endmacro


%macro shaay 0
	readmem word, PC
	mov edx, ecx
	inc ch
	and ch, X
	and ch, A
	add dl, Y
	jnc %%done
	
	mov dh, ch
	clock 1

%%done:
	writemem edx, ch
	add PC, 2
	clock 5
%endmacro



; shs*: this opcode ANDs the contents of the A and X registers (without changing 
; the contents of either register) and transfers the result to the stack 
; pointer.  It then ANDs that result with the contents of the high byte of 
; the target address of the operand +1 and stores that final result in 
; memory.  
%macro shs 0
	readmem word, PC
	mov edx, ecx
	inc ch				; build T in ch, X & HI+1
	and ch, X
	and ch, A
	mov [S], ch			; transfer to stack
	
	add dl, Y			; add Y
	jnc %%done			; check for page cross (if we got a carry)
	
	mov dh, ch			; if there's a page cross, we write T to T:LO+Y
	clock 1

%%done:
	writemem edx, ch	; write t to the address
	add PC, 2

	clock 5
%endmacro


; shx*: 
; Set T := X AND (HI+1)
;
; If LO+Y < 256:
;     store T at [HI : LO+Y]
; otherwise:
;     store T at [T : LO+Y]
%macro shx 0
	readmem word, PC
	mov edx, ecx
	inc ch			; build T in ch, X & HI+1
	and ch, X
	add dl, Y		; add Y
	jnc %%done		; check for page cross (if we got a carry)
	
	mov dh, ch		; if there's a page cross, we write T to T:LO+Y
	clock 1

%%done:
	writemem edx, ch		; write t to the address
	add PC, 2

	clock 5
%endmacro


; shy*:
; Set T := Y AND (HI+1)
;
; If LO+Y < 256:
;     store T at [HI : LO+X]
; otherwise:
;     store T at [T : LO+X]
%macro shy 0
	readmem word, PC
	mov edx, ecx
	inc ch			; build T in ch, X & HI+1
	and ch, Y
	add dl, X		; add X
	jnc %%done		; check for page cross (if we got a carry)
	
	mov dh, ch		; if there's a page cross, we write T to T:LO+X
	clock 1

%%done:
	writemem edx, ch ; write t to the address
	add PC, 2

	clock 5
%endmacro

; * = not thoroughly tested but should behave as described
;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;        instructions        ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_00:	brk 									; brk
_next

_01:	aluop or, indirectxbyte, 6				; ora ($nn, X)
_next
	
_02: 	jam										; jam
_next

_03:	slo readindirectx, writemem, 8			; slo ($nn, X)
_next
	
_04:	nop 1, 3								; nop $nn
_next
	
_05:	aluop or, zpbyte, 3						; ora $nn
_next
	
_06:	shift left, readzp, zpwrite, 5			; asl $nn
_next

_07:	slo readzp, zpwrite, 5					; slo $nn		
_next

_08:	php										; php
_next
	
_09:	aluop or, immediate, 2					; ora #$nn
_next
	
_0A: 	shift left, A							; asl
_next

_0B:	anc										; anc #$nn
_next
	
_0C:	nop 2, 4								; nop $nnnn
_next

_0D:	aluop or, absolutebyte, 4				; ora $nnnn
_next
	
_0E:	shift left, readabsolute, writemem, 6	; asl $nnnn
_next

_0F:	slo readabsolute, writemem, 6			; slo $nnnn
_next
	
_10:	branch jz, [nFlag]						; bpl $nnnn
_next
	
_11:	aluop or, indirectybyte, 5				; ora ($nn), Y
_next
	
_12:	jam										; jam
_next

_13:	slo readindirecty, writemem, 8			; slo ($nn), Y
_next
	
_14:	nop 1, 4								; nop $nn, X
_next

_15:	aluop or, zpxbyte, 4					; ora $nn, X
_next

_16:	shift left, readzpx, zpwrite, 6			; asl $nn, X
_next

_17:	slo readzpx, zpwrite, 6					; slo $nn, X
_next

_18:	clearflag [cFlag]						; clc
_next

_19:	aluop or, absoluteybyte, 4				; ora $nnnn, Y
_next

_1A:	nop 0, 2								; nop
_next

_1B:	slo readabsolutey, writemem, 7			; slo $nnnn, Y
_next	
	
_1C:	nop 2, 4								; nop $nnnn, X
_next
	
_1D:	aluop or, absolutexbyte, 4				; ora $nnnn, X
_next
	
_1E:	shift left, readabsolutex, writemem, 7 	; asl $nnnn,X
_next

_1F:	slo readabsolutex, writemem, 7			; slo $nnnn, X
_next

_20:	jsr										; jsr $nnnn
_next
	
_21:	aluop and, indirectxbyte, 6				; and ($nn,X)
_next

_22: 	jam										; jam
_next

_23:	rla readindirectx, writemem, 8			; rla ($nn, X)
_next
	
_24:	bit zpbyte, 3							; bit $nn
_next
	
_25:	aluop and, zpbyte, 3					; and $nn
_next

_26:	rotate left, readzp, zpwrite, 5			; rol $nn	
_next

_27:	rla readzp, zpwrite, 5					; rla $nn
_next
	
_28:	plp										; plp
_next
	
_29:	aluop and, immediate, 2					; and #$nn
_next	
	
_2A:	rotate left, A							; rol
_next

_2B:	anc										; anc #$nn
_next	
	
_2C:	bit absolutebyte, 4						; bit $nnnn	
_next
	
_2D:	aluop and, absolutebyte, 4				; and $nnnn
_next
	
_2E:	rotate left, readabsolute, writemem, 6 	; rol $nnnn
_next

_2F:	rla readabsolute, writemem, 6			; rla $nnnn
_next
	
_30:	branch jnz, [nFlag]						; bmi $nnnn
_next	
	
_31:	aluop and, indirectybyte, 5				; and ($nn), Y
_next	

_32: 	jam										; jam
_next

_33:	rla readindirecty, writemem, 8			; rla ($nn), Y
_next
	
_34:	nop 1, 4								; nop
_next	
	
_35:	aluop and, zpxbyte, 4					; and $nn, X
_next

_36:	rotate left, readzpx, zpwrite,6			; rol $nn, X
_next

_37:	rla readzpx, zpwrite, 6					; rla $nn, X
_next
	
_38:	setflag [cFlag]							; sec
_next	
	
_39:	aluop and, absoluteybyte, 4				; and $nnnn, Y
_next		

_3A:	nop 0, 2								; nop
_next

_3B:	rla readabsolutey, writemem, 7			; rla $nnnn, Y
_next

_3C:	nop 2, 4								; nop $nnnn, X
_next

_3D:	aluop and, absolutexbyte, 4				; and $nnnn, X
_next

_3E:	rotate left, readabsolutex, writemem, 7	; rol $nnnn, X
_next

_3F:	rla readabsolutex, writemem,7			; rla $nnnn, X
_next
	
_40:	rti										; rti
_next
	
_41:	aluop xor, indirectxbyte, 6				; eor ($nn,X)
_next

_42: 	jam										; jam
_next

_43:	sre readindirectx, writemem, 8			; sre ($nn, X)
_next
	
_44:	nop 1, 3								; nop $nn
_next
	
_45:	aluop xor, zpbyte, 3					; eor $nn
_next
	
_46:	shift right, readzp, zpwrite, 3			; lsr $nn
_next

_47:	sre readzp, zpwrite, 8					; sre ($nn, X)
_next
	
_48:	pha										; pha
_next
	
_49:	aluop xor, immediate, 2					; eor #$nn
_next
	
_4A:	shift right, A							; lsr
_next

_4B:	asr immediate, 2						; asr #$nn
_next
	
_4C:	jmpa 									; jmp $nnnn
_next
	
_4D:	aluop xor, absolutebyte, 4				; eor $nnnn
_next
	
_4E:	shift right, readabsolute, writemem, 6	; lsr $nnnn
_next

_4F:	sre readabsolute, writemem, 6			; sre $nnnn
_next
	
_50:	branch jz, [vFlag]						; bvc $nnnn
_next	
	
_51:	aluop xor, indirectybyte, 5				; eor ($nn),Y
_next

_52: 	jam										; jam
_next

_53:	sre readindirecty, writemem, 8			; sre ($nn), Y
_next
	
_54:	nop 1, 4								; nop $nn, X
_next
	
_55:	aluop xor, zpxbyte, 4					; eor $nn, X
_next
	
_56:	shift right, readzpx, zpwrite, 6		; lsr $nn, X
_next

_57:	sre readzpx, zpwrite, 6					; sre $nn, X
_next
		
_58:	clearflag [iFlag]						; cli
_next
	
_59:	aluop xor, absoluteybyte, 4				; eor $nnnn,Y
_next
	
_5A:	nop 0, 2								; nop
_next

_5B:	sre readabsolutey,writemem,7			; sre $nnnn, Y
_next	

_5C:	nop 2,4									; nop $nnnn, X
_next		
	
_5D:	aluop xor, absolutexbyte, 4				; eor $nnnn,X
_next
	
_5E:	shift right, readabsolutex, writemem ,7	; lsr $nnnn, X
_next

_5F:	sre readabsolutex, writemem, 7			; sre $nnnn, X
_next	
	
_60:	rts										; rts
_next

_61:	addc indirectxbyte, 6					; add ($nn, X)
_next

_62: 	jam										; jam
_next

_63:	rra readindirectx, writemem, 8			; rra ($nn, X)
_next
	
_64:	nop 1, 3								; nop $nn
_next

_65:	addc zpbyte, 3							; adc $nn
_next
	
_66:	rotate right, readzp, zpwrite,5			; ror $nn
_next

_67:	rra readzp, zpwrite, 5					; rra $nn
_next

_68:	pla										; pla
_next
	
_69:	addc immediate, 2						; adc #$nn
_next
	
_6A:	rotate right, A							; ror
_next

_6B:	arr										; arr #$nn
_next
	
_6C:	jmpi									; jmp ($nnnn)
_next
	
_6D:	addc absolutebyte, 4					; adc $nnnn	
_next
	
_6E:	rotate right, readabsolute, writemem, 4	; ror $nnnn
_next

_6F:	rra readabsolute, writemem, 6			; rra $nnnn
_next
	
_70:	branch jnz, [vFlag]						; bvs $nnnn
_next

_71:	addc indirectybyte, 5					; adc ($nn), Y
_next

_72: 	jam										; jam
_next

_73:	rra readindirecty, writemem, 8			; rra ($nn), Y
_next
	
_74:	nop 1, 4								; nop $nn, X
_next

_75:	addc zpxbyte, 4;						; adc $nn, X
_next
	
_76:	rotate right, readzpx, zpwrite,6		; ror $nn, X
_next

_77:	rra readzpx, zpwrite, 6					; rra $nn, X
_next
	
_78:	setflag [iFlag]							; sei
_next

_79:	addc absoluteybyte, 4					; adc $nnnn, Y
_next
	
_7A:	nop 0, 2								; nop
_next

_7B:	rra readabsolutey, writemem, 7			; rra $nnnn, X
_next

_7C:	nop 2, 4								; nop $nnnn, X
_next

_7D:	addc absolutexbyte, 4					; adc $nnnn, X
_next
	
_7E:	rotate right, readabsolutex, writemem, 7; ror $nnnn, X
_next

_7F:	rra readabsolutex, writemem, 7			; rra $nnnn, X
_next
	
_80:	nop 1, 2								; nop #$nn
_next
	
_81:	store A, indirectxaddress, writemem,6	; sta ($nn, X)
_next

_82:	nop 1, 2								; nop #$nn
_next

_83:	sax readindirectx, writemem, 6			; sax ($nn, X)
_next
	
_84:	store Y, zpaddress, zpwrite, 3			; sty $nn
_next
	
_85:	store A, zpaddress, zpwrite,3			; sta $nn
_next
	
_86:	store X, zpaddress, zpwrite,3			; stx $nn
_next

_87:	sax readzp, zpwrite, 3					; sax $nn
_next
	
_88:	dey										; dey
_next
	
_89:	nop 1,2									; nop #$nn
_next
	
_8A:	transfer X, A							; txa	
_next

_8B:	ane										; ane #$nn
_next
	
_8C:	store Y, absoluteaddress, writemem, 4	; sty $nnnn
_next

_8D:	store A, absoluteaddress, writemem, 4	; sta $nnnn
_next
	
_8E:	store X, absoluteaddress, writemem, 4	; stx $nnnn
_next

_8F:	sax readabsolute, writemem, 4			; sax $nnnn
_next
	
_90:	branch jz, [cFlag]						; bcc $nnnn
_next
	
_91:	store A, indirectyaddress, writemem, 6	; sta ($nn), Y
_next

_92: 	jam										; jam
_next

_93:	shaiy									; sha ($nn), Y
_next
	
_94:	store Y, zpxaddress, zpwrite, 4			; sty $nn, X
_next
	
_95:	store A, zpxaddress, zpwrite, 4			; sta $nn, X
_next
	
_96:	store X, zpyaddress, zpwrite, 4			; stx $nn, Y
_next

_97:	sax readzpy, zpwrite, 4					; sax $nn, Y
_next
	
_98:	transfer Y, A							; tya
_next
	
_99:	store A, absoluteyaddress, writemem, 5	; sta $nnnn, Y
_next
	
_9A:	transfer X, S							; txs
_next

_9B:	shs										; shs $nnnn, Y
_next

_9C:	shy										; shy $nnnn, X
_next	

_9D:	store A, absolutexaddress, writemem, 5	; sta $nnnn, X
_next	

_9E:	shx										; shx $nnnn, Y
_next

_9F:	shaay									; sha $nnnn, Y
_next

_A0:	load Y, immediate, 2					; ldy #$nn
_next

_A1:	load A, indirectxbyte, 6				; lda ($nn, X)	
_next
	
_A2:	load X, immediate, 2					; ldx #$nn
_next

_A3:	lax readindirectx, 6					; lax ($nn, X)
_next
	
_A4:	load Y, zpbyte, 3						; ldy $nn
_next
	
_A5:	load A, zpbyte, 3						; lda $nn
_next

_A7:	lax readzp, 3							; lax $nn
_next

_A6:	load X, zpbyte, 3						; ldx $nn
_next
	
_A8:	transfer A, Y							; tay
_next

_A9:	load A, immediate, 2					; lda #$nn
_next
	
_AA:	transfer A, X							; tax
_next

_AB:	lxa										; lxa #$nn
_next

_AC:	load Y, absolutebyte,4					; ldy $nnnn
_next
	
_AD:	load A, absolutebyte,4					; lda $nnnn
_next
	
_AE:	load X, absolutebyte, 4					; ldx $nnnn
_next

_AF:	lax readabsolute, 4						; lax $nnnn
_next
	
_B0:	branch jnz, [cFlag]						; bcs $nnnn
_next
	
_B1:	load A, indirectybyte, 5				; lda ($nn), Y
_next

_B2: 	jam										; jam
_next

_B3:	lax readindirecty, 5					; lax ($nn), Y
_next
	
_B4:	load Y, zpxbyte, 4						; ldy $nn, X
_next
	
_B5:	load A, zpxbyte, 4						; lda $nn, X
_next
	
_B6:	load X, zpybyte, 4						; ldx $nn, Y
_next

_B7:	lax readzpy, 4							; lax ($nn), Y
_next
	
_B8:	clearflag [vFlag]						; clv
_next
	
_B9:	load A, absoluteybyte, 4				; lda $nnnn,Y
_next
	
_BA:	transfer S, X							; tsx
_next

_BB:	las										; las $nnnn, Y
_next
	
_BC:	load Y, absolutexbyte, 4				; ldy $nnnn, X
_next
	
_BD:	load A, absolutexbyte, 4				; lda $nnnn,X
_next

_BE:	load X, absoluteybyte, 4				; ldx $nnnn,Y
_next

_BF:	lax readabsolutey, 4					; lax $nnnn, Y
_next
	
_C0:	compare Y, immediate, 2					; cpy #$nn
_next
	
_C1:	compare A, indirectxbyte, 6				; cmp ($nn, X)
_next
	
_C2:	nop 1, 2								; nop #$nn
_next

_C3:	dcp readindirectx, writemem, 8			; dcp ($nn, X)
_next
	
_C4:	compare Y, zpbyte, 3					; cpy $nn
_next
	
_C5:	compare A, zpbyte, 3					; cmp $nn
_next
	
_C6:	decm readzp, zpwrite, 5					; dec $nn
_next

_C7:	dcp readzp, zpwrite, 6					; dcp $nn
_next
	
_C8:	iny										; iny
_next
	
_C9:	compare A, immediate, 2					; cmp #$nn
_next
	
_CA:	dex										; dex
_next

_CB:	sbx 									; sbx #$nn	
_next
	
_CC:	compare Y, absolutebyte, 4				; cpy $nnnn
_next
	
_CD:	compare A, absolutebyte, 4				; cmp $nnnn
_next
	
_CE:	decm readabsolute, writemem, 6			; dec $nnnn
_next
			
_CF:	dcp readabsolute, writemem, 6			; dcp $nnnn
_next
	
_D0:	branch jz, [zFlag]						; bne $nnnn
_next
	
_D1:	compare A, indirectybyte, 5				; cmp ($nn), Y
_next
	
_D2: 	jam										; jam
_next

_D3:	dcp readindirecty, writemem, 8			; dcp ($nn), Y
_next
	
_D4:	nop 1, 4								; nop $nn, X
_next
	
_D5:	compare A, zpxbyte, 4					; cmp $nn, X
_next
	
_D6:	decm readzpx, zpwrite, 6				; dec $nn, X
_next

_D7:	dcp readzpx, zpwrite, 6					; dcp $nn, X
_next
	
_D8:	clearflag [dFlag]						; cld
_next
	
_D9:	compare A, absoluteybyte, 4				; cmp $nnnn, Y
_next
	
_DA:	nop 0, 2								; nop
_next

_DB:	dcp readabsolutey, writemem, 7			; dcp $nnnn, Y
_next
	
_DC:	nop 2, 4								; nop $nnnn, X
_next
		
_DD:	compare A, absolutexbyte, 4				; cmp $nnnn,X
_next
	
_DE:	decm readabsolutex, writemem, 7			; dec $nnnn, X
_next

_DF:	dcp readabsolutex, writemem, 7			; dcp $nnnn, X
_next
	
_E0:	compare X, immediate, 2					; cpx #$nn
_next

_E1:	sbc indirectxbyte, 6					; sbc ($nn, X)
_next
	
_E2:	nop 1, 2								; nop #$nn
_next

_E3:	isc readindirectx, writemem, 8			; isc ($nn, X)
_next
	
_E4:	compare X, zpbyte, 3					; cpx $nn
_next

_E5:	sbc zpbyte, 3							; sbc $nn
_next
	
_E6:	incm readzp, zpwrite, 5					; inc $nn
_next

_E7:	isc readzp, zpwrite, 5					; isc $nn
_next
	
_E8:	inx										; inx
_next
	
_E9:	sbc immediate, 2						; sbc #$nn
_next
	
_EA:	nop 0, 2								; nop
_next

_EB:	sbc immediate, 2						; usbc #$nn
_next
	
_EC:	compare X, absolutebyte, 4				; cpx $nnnn
_next
	
_ED:	sbc absolutebyte, 4						; sbc $nnnn
_next
	
_EE:	incm readabsolute, writemem, 6			; inc $nnnn
_next

_EF:	isc readabsolute, writemem, 6			; isc $nnnn
_next

_F0:	branch jnz, [zFlag]						; beq $nnnn
_next

_F1:	sbc indirectybyte, 5					; sbc ($nn), Y
_next

_F2:	jam										; jam
_next

_F3:	isc readindirecty, writemem, 8			; isc ($nn), Y
_next
	
_F4:	nop 1, 4								; nop $nn, X
_next

_F5:	sbc zpxbyte, 4							; sbc $nn, X
_next
	
_F6:	incm readzpx, zpwrite, 6				; inc $nn,X
_next

_F7:	isc readzpx, zpwrite, 6					; isc $nn, X
_next
	
_F8:	setflag [dFlag]							; sed
_next

_F9:	sbc absoluteybyte, 4					; sbc $nnnn, Y
_next
	
_FA:	nop 0, 2								; nop
_next

_FB:	isc readabsolutey, writemem, 7			; isc $nnnn, Y
_next
	
_FC:	nop 2, 4								; nop $nnnn, X
_next

_FD:	sbc absolutexbyte, 4					; sbc $nnnn, X
_next
	
_FE:	incm readabsolutex, writemem, 7			; inc $nnnn,X
_next

_FF:	isc readabsolutex, writemem, 7			; isc $nnnn, X
_next

	
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;	
;        functions        ;	
;;;;;;;;;;;;;;;;;;;;;;;;;;;

align 4

; clear any pending interrupt(s)
; pretty straightforward	
bender_clearirq:
	mov [bender_needs_interrupt], byte 0
	ret


; more or less a read from memory, this routine allows
; you to DMA a byte from memory.  it is useful on the NES 
; directly, and perhaps on other systems for other purposes	
bender_dma:
	mov eax, [esp+4]
	push ecx
	push edi
	readmem byte, eax
	mov eax, ecx
	pop edi
	pop ecx
	ret

	
; some systems, such as the NES require the ability to waste cycles.
; when the NES does an OAM DMA transfer, 513 cpu cycles are "stolen"
; from otherwise cpu time.
bender_eatcycles:
	mov edx, [esp+4]
	add [bender_cycles_to_eat], edx
	ret


; this will return the number of cycles elapsed since the last call.
; if you need to reset the clock, pass anything other than 0 in, or
; else it will continue counting.
bender_elapsedcycles:
	; check to see if we need to reset the count
	mov edx, [esp+4]
	or edx, edx
	jnz .reset
	
	; nope, just return total cycles
	mov eax, [bender_total_cycles]
	ret
	
.reset:
	; reset the count
	mov [bender_total_cycles], dword 0
	xor eax, eax
	ret


; initialise the cpu.  clear out registers, assign i/o hooks, and return.
; good idea to soon call reset() soon afterward.
; clobbers: EAX, ECX, EDX
bender_init:
	xor eax, eax			; clear out eax
	mov [rPC], eax			; clear 6502 registers
	mov [rA], al
	mov [rX], al
	mov [rY], al
	mov [rP], al
	mov [rS], al
	
	mov [bender_needs_interrupt], al	; and interrupt & jam
	mov [bender_is_jammed], al
	
	mov eax, [esp+4]	; read hook
	mov edx, [esp+8]	; write hook
	mov ecx, [esp+12]	; jam hook
	
	; setup hooks
	mov [bender_read_hook], eax
	mov [bender_write_hook], edx
	mov [bender_jam_hook], ecx
	
	; the rest is taken care of when reset is called
	ret
	
	
; software interrupt
bender_irq:
	; make sure we aren't jammed
	cmp [bender_is_jammed], byte 0x0
	jz .irq
	ret
	
.irq:
	multipush ebx, ecx, edx, edi, esi, ebp	; save x86 regs
	save									; and 6502
	
	test [iFlag], byte 0x1					; check for interupt
	jnz .pending	

	mov edx, PC								; otherwise push pch & pcl to stack
	pushstack dh
	pushstack dl
	mov [bFlag], byte 0x0					; make sure B is not set
	packflags
	pushstack P								; push flags to stack
	mov [iFlag], byte 0x1
	readmem word, IRQ_VECTOR
	mov PC, ecx
	add [bender_cycles_to_eat], byte INTERRUPT_CYCLES
	jmp .done

.pending:
	mov [bender_needs_interrupt], byte 0x1
	
.done:
	restore
	multipop ebx, ecx, edx,edi, esi, ebp	; save these
	ret
	
	
; non-maskable interrupt.  this happens on the NES, for example, at 60 times/sec.	
bender_nmi:
	; if we're jammed, we don't need to take this interrupt.  
	; we drop it, and move along
	cmp [bender_is_jammed], byte 0x1
	jz .done

	multipush ebx, ecx, edx, edi, esi, ebp	; save x86 regs
	save									; and 6502

	mov edx, PC		; get current PC
	pushstack dh	; push high-byte to stack
	pushstack dl	; push low-byte to stack
	
	mov [bFlag], byte 0x0		; don't want B set
	packflags
	pushstack P					; flags go on the stack as well

	readmem word, NMI_VECTOR	; read from nmi vector
	mov PC, ecx					; stash it in PC and go

	add [bender_cycles_to_eat], byte INTERRUPT_CYCLES
	restore									; restore 6502 regs
	multipop ebx, ecx, edx, edi, esi, ebp	; and x86
		
.done:
	ret
	
	
; cut out of execution early
; essentially, any remaining cycles are dumped and we should in theory return 
; from execution after the current fetch finshes
bender_abort:
	mov [bender_total_cycles], dword 0
	ret


; reset the cpu, and various related cpu data.  most importantly, fetch the
; word at the reset vector and return.
;
; clobbers: eax, ecx, edx (unpackflags)
bender_reset:
	xor eax, eax							; clear out variables
	mov [bender_is_jammed], al
	mov [bender_needs_interrupt], al
	mov [bender_total_cycles], eax
	mov [bender_remaining_cycles], eax
	mov [bender_cycles_to_eat], eax
	mov [rS], byte 0xfd
	mov [rP], byte (B_MASK|I_MASK|R_MASK)
	unpackflags
	;mov [bender_cycles_to_eat], byte RESET_CYCLES ; weird things.
	readmem word, RESET_VECTOR
	mov [rPC], ecx
	ret


; run the cpu for (esp+4) cycles, then return
bender_run:
	mov edx, [esp+4]
	mov [bender_remaining_cycles], edx
	mov edx, [bender_total_cycles]
	push edx ; stash cycle count here
		
	multipush eax, ebx, ecx, edi, esi, ebp
	save
	xor edx, edx
	xor edi, edi
	
	cmp [bender_remaining_cycles], dword 0
	jle .next ;jle .check
	
	mov ecx, [bender_cycles_to_eat]
	or ecx, ecx
	jz .check
	
	mov edx, ecx
	cmp [bender_remaining_cycles], ecx
	ja .keepgoing
	mov edx, [bender_remaining_cycles]
	
.keepgoing:
	sub [bender_remaining_cycles], edx
	add [bender_total_cycles], edx
	sub [bender_cycles_to_eat], edx
	
.check:
	cmp [bender_remaining_cycles], dword 0
	jle .next
	
	cmp [bender_needs_interrupt], byte 0
	jz .next
	
	test [iFlag], byte 0x1
	jnz .next
	
	; do the irq
	mov [bender_needs_interrupt], byte 0
	mov edx, PC
	pushstack dh
	pushstack dl
	mov [bFlag], byte 0x0
	packflags
	pushstack P
	mov [iFlag], byte 0x1
	readmem word, IRQ_VECTOR
	mov PC, ecx
	add [bender_cycles_to_eat], byte INTERRUPT_CYCLES
	
.next:
	_next	; fetch next instruction
	
_run_end:
	restore					; restore 6502 regs
	multipop eax, ebx, ecx, edi, esi, ebp	; and x86
 	pop eax	; pickup cycle count from before
 	sub eax, [bender_total_cycles]	; subtract from stashed
 	neg eax
	ret


; execute one instruction and return
; naturally (from bender_run) returns the number of cycles executed	
bender_step:
	push dword 1		; one cycle
	call bender_run		; go
	add esp, 4
	ret

