/* N2A03.h */

#ifndef _BENDER_H_
#define _BENDER_H_

#include <stdint.h>

#define MAX_PAGES 	64

#ifdef __cplusplus
	extern "C" {
#endif

extern uint32_t rPC;
extern uint8_t rA;
extern uint8_t rX;
extern uint8_t rY;
extern uint8_t rP;
extern uint8_t rS;

extern uint8_t *bender_memory[MAX_PAGES];
extern uint8_t bender_needs_interrupt;
extern uint8_t bender_is_jammed;
extern int32_t bender_remaining_cycles;
extern int32_t bender_total_cycles;
extern int32_t bender_cycles_to_eat;

extern uint8_t (*bender_read_hook)(uint32_t address);
extern void	(*bender_write_hook)(uint32_t address, uint8_t data);
extern void (*bender_jam_hook)(void);

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

#ifdef __cplusplus
}
#endif

#endif /* _BENDER_H_ */

