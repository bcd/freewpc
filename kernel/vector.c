
/*
 * \file
 * \brief 6809 interrupt vector table
 *
 * This module defines the contents of the vector table and places the
 * structure at a fixed location, named "vector", which is mapped at
 * link-time to address 0xFFF0.
 */

extern void do_reset (void);
extern void do_swi3 (void);
extern void do_swi2 (void);
extern void do_firq (void);
extern void do_irq (void);
extern void do_swi (void);
extern void do_nmi (void);


/** The 6809 vector table structure */
typedef struct
{
	void (*unused) (void);
	void (*swi3) (void);
	void (*swi2) (void);
	void (*firq) (void);
	void (*irq) (void);
	void (*swi) (void);
	void (*nmi) (void);
	void (*reset) (void);
} m6809_vector_table_t;


/** The interrupt vector table */
__attribute__((section("vector"))) m6809_vector_table_t vectors = {
	.unused = do_reset,
	.swi3 = do_swi3,
	.swi2 = do_swi2,
	.firq = do_firq,
	.irq = do_irq,
	.swi = do_swi,
	.nmi = do_nmi,
	.reset = do_reset,
};

