	.module puts.s
	.area sysrom
	.globl _puts	; void puts(const char *s)
_puts:
	pshs	x
	ldx	4,s	; s -> X
loop:
	ldb	,x+
	beq	end
put:
	jsr	0xE803	; call "PUTC" monitor routine
;	MO version
;	.byte	2
;	swi
; LF (\n) is LF/CR (\n\r) in the C language
	cmpb	#10	; LF
	bne	loop
	ldb	#13	; CR
	bra	put
end:
	clra
	clrb
	puls	x,pc
