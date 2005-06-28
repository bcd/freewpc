	.module putchar.s
	.area sysrom
	.globl _putchar	; int putchar(unsigned char c)
_putchar:
	ldb	2,s	; c -> B
put:
	jsr	0xE803	; call "PUTC" monitor routine
;	MO version
;	.byte	2
;	swi
; LF (\n) is LF/CR (\n\r) in the C language
	cmpb	#10	; LF
	bne	end
	ldb	#13	; CR
	bra	put
end:
	clra
	clrb
	rts
