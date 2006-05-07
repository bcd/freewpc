
RETADDR=5
WPC_ROM_PAGE_REG=0x3FFC

	.area sysrom
	.globl __far_call_handler
__far_call_handler:
	pshs	u                  ; Save U reg like prologue

	pshs	b,x
	ldu	RETADDR,s
	ldx	,u++
	ldb	,u+
	stu	RETADDR,s
	tfr	x,u
	lda	WPC_ROM_PAGE_REG
	stb	WPC_ROM_PAGE_REG
	puls	b,x

	pshs	a
	jsr	,u
	puls	a

	sta	WPC_ROM_PAGE_REG

	puls	u,pc               ; Restore U reg, like epilogue, and return


