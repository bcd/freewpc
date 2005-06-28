	.module div.s
	.area sysrom
	.globl _div	; div_t div(int numer, int denom);
_div:
	ldd	2,s	; numer -> D
	pshs	b,a
	ldd	6,s	; denom -> D
	jsr	_seuclid
	std	6,s	; D -> numer
	puls	a,b
	std	2,s	; D -> denom
	tfr	s,d
	addd	#2	; stack+2 -> D
	rts
