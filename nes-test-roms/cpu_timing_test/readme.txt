NES 6502 Timing Test
--------------------
This iNES file tests a NES emulator's 6502 instruction timing for most
instructions. The tests rely on correct behavior of instructions. It
does not test the timing of the branch instructions, nor undocumented
opcodes, except for the NOP instructions listed below. When the test is
started, it prints the title and a note that it takes about 12 seconds
to pass. When it passes, a message is printed on screen and two beeps
sound. If it fails, it prints an error message.

1-byte NOPs: $1A $3A $5A $7A $DA $FA
2-byte NOPs: $04 $14 $34 $44 $54 $64 $74 $80 $82 $89 $C2 $D4 $E2 $F4
3-byte NOPs: $0C
3-byte NOPs that use absolute,x timing: $1C $3C $5C $7C $DC $FC

Source code forthcoming.


Fail op
-------
The indicated opcode failed. If it was being timed where a page crossing
should occur, that will be noted. The number of clocks will be shown
that the emulator used, and the correct number of clocks it should have
used.


Unknown error
-------------
Occurs if the instruction timing fails or NMI unexpectedly returns. Post
to the Nesdev forum if you get this error.


Basic timing wrong
------------------
If you get this error, then the loop that tests NMI and basic
instruction timing (below) ran too many/too few times. If this occurs,
verify the timing of these instructions and your PPU's NMI interrupt
timing.

loop:
	cpx zero-page
	bne stop
	inc zero-page
	bne loop
	inc zero-page
	jmp loop
stop:

-- 
Shay Green <gblargg@gmail.com>
