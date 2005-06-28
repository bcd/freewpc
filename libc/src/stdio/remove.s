	.module remove.s
	.area sysrom
	.globl _remove	; int remove(const char *filename);
_remove:
	jmp	_unlink
