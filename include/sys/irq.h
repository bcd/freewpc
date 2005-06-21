
#ifndef _SYS_IRQ_H
#define _SYS_IRQ_H

/* How to enable/disable the IRQ */
#define disable_irq()	asm ("orcc #" STR(CC_IRQ))
#define enable_irq()		asm ("andcc #" STR(~CC_IRQ))

#endif /* _SYS_IRQ_H */
