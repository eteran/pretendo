bender 0.1
32-bit 6502 CPU emulator
Eli Dayan 2012, 2013
------------------------

what is it? 
-----------
bender is a 6502 CPU emulator written in 32-bit x86 assembly.  It builds with
NASM, and emulates the entire MOS 6502 instruction set.  This includes all
undocumented instructions as well as decimal mode.  bender aims to be fast, 
clean, and easy to maintain and use.  Note, because bender is written in 32-bit
assembly, it will not work under a 64-bit OS.  Please consider this if you 
choose to use bender in your program.  bender took a several months of hard 
work and debugging in order for it to get in the state it's in.  bender was
painstakingly debugged using Wolfgang Lorentz C64 emulator test suite, as well
as by throwing real NES code at it.  Needless to say, aside from a few 
undocumented instructions that I cannot test, bender preforms pretty much like
a stock 6502/2A03.


how do i use it?
----------------

The first thing you must do when using bender is to tell it whether or not to 
use decimal mode.  This is controlled by a define in the assembly file:
	%define BENDER_2A03	(1)
Changing the one to a zero will enable decimal mode, and cause bender to
operate like a normal 6502 would.  bender has been designed from day one for
minimal intervention, and ease of use.  We will take a look at the exports, to
see what we're dealing with:

	extern uint32_t rPC;
	extern uint8_t rA;
	extern uint8_t rX;
	extern uint8_t rY;
	extern uint8_t rP;
	extern uint8_t rS;
	
These are the global 6502 reigsters, you can access them by name at any time.

	extern uint8_t *bender_memory[MAX_PAGES];
	extern uint8_t bender_needs_interrupt;
	extern uint8_t bender_is_jammed;
	extern int32_t bender_remaining_cycles;
	extern int32_t bender_total_cycles;
	extern int32_t bender_cycles_to_eat;
	
	extern uint8_t *bender_memory[MAX_PAGES] - this is a pointer to bender's
	64K emulated space.  it can range in size from 1K to 64K, in powers of
	two.  bender defaults to 64x1K max pages.  In an 8x8k setup, for example,
	you would use the first 8 pages.  if a page is not used, it will point to
	NULL.  The programmer is responsible for allocation and maintenance of the
	pages.
	
	extern uint8_t bender_needs_interrupt - this is 1 when an interrupt is
	pending, 0 otherwise
	
	extern uint8_t bender_is_jammed - this will tell us if we're jammed or not
	
	extern int32_t bender_remaining_cycles - this will give us the remaining
	cycles since run was called
	
	extern int32_t bender_total_cycles - this gives the total number of cycles
	executed since run was called
	
	extern int32_t bender_cycles_to_eat - this lets bender keep track of how
	many cycles to 'waste' during execution
	
bender requires the user to provide three function pointers in order to work
properly.  They are:	
	
	extern uint8_t (*bender_read_hook)(uint32_t address);
	extern void	(*bender_write_hook)(uint32_t address, uint8_t data);
	extern void (*bender_jam_hook)(void);
	
bender_read_hook and bender_write_hook are called when external memory is
accessed, and bender_jam_hook is called when a JAM instruction is encountered.
Make sure your hooks follow the above calling model or else bad things will
happen.


bender provides a number of control funcitons so you can govern its execution.

	extern void	bender_abort (void);
	extern void	bender_clearirq (void);
	extern uint8_t bender_dma (uint32_t address);
	extern void	bender_eatcycles (int32_t cycles);
	extern uint32_t bender_elapsedcycles (uint8_t reset);
	extern void bender_init (void *read_hook, void *write_hook, void *jam_hook);
	extern void bender_irq (void);
	extern void bender_nmi (void);
	extern void bender_reset (void);
	extern int32_t bender_run (int32_t cycles);
	extern void bender_step (void);
	
bender_abort will abort execution immediately and return after the current
fetch is finished.
bender_clear_irq will clear any pending interrupts
bender_dma will fetch a byte directly from memory, useful on NES
bender_eat_cycles will waste the passed in amount of cycles, and continue
bender_elapsed_cycles will return the number of cycles elapsed since run was
called, and gives you the option to reset the counter.
bender_init gets the core ready.  you must pass in your read/write/jam hooks to
this function.
bender_irq will cause an irq to occur
bender_nmi will cause an nmi to occur
bender_reset will reset the cpu, and its registers
bender_run will run the cpu for the specified number of cycles
bender_step will run the cpu one instruction at a time


thanks
------
I would like to thank the following people for their generosity in time,
helping me make bender what it is today.  I couldn't have done it without
their help.  Presented alphabetically:
	John Allensworth
	Neil Bradley
	Alastair Bridgewater
	Matthew Conte
	Evan Teran
	
EOF

