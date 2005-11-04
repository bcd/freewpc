
#ifndef _SYS_IRQ_H
#define _SYS_IRQ_H

/* How to enable/disable the IRQ */
#define disable_irq()	asm ("orcc\t#" STR(CC_IRQ))
#define enable_irq()		asm ("andcc\t#" STR(~CC_IRQ))

#define disable_firq()	asm ("orcc\t#" STR(CC_FIRQ))
#define enable_firq()	asm ("andcc\t#" STR(~CC_FIRQ))

#endif /* _SYS_IRQ_H */
