
.area vector (ABS)

	vector_unused:
		.dw	sys_reset
	vector_swi3:
		.dw	_do_swi3
	vector_swi2:
		.dw	_do_swi2
	vector_firq:
		.dw	sys_firq
	vector_irq:
		.dw	sys_irq
	vector_swi:
		.dw	_do_swi
	vector_nmi:
		.dw	_do_nmi
	vector_reset:
		.dw	sys_reset

