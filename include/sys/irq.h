
#ifndef _SYS_IRQ_H
#define _SYS_IRQ_H

/* How to enable/disable the IRQ */
#define disable_irq()	asm ("orcc\t#" STR(CC_IRQ))
#define enable_irq()		asm ("andcc\t#" STR(~CC_IRQ))

/* How to enable/diable the FIRQ */
#define disable_firq()	asm ("orcc\t#" STR(CC_FIRQ))
#define enable_firq()	asm ("andcc\t#" STR(~CC_FIRQ))

/* How to enable/disable all interrupts */
#define disable_interrupts()	asm ("orcc\t#" STR(CC_IRQ|CC_FIRQ))
#define enable_interrupts()	asm ("andcc\t#" STR(~(CC_IRQ|CC_FIRQ)))

#endif /* _SYS_IRQ_H */
