	.byte	0x00				; inctst.asm
	.include	"inc0a"			; inc0a.asm
	.include	".\inc1\inc1a"		; inc1a.asm
	.include	".\inc2\inc2a"		; inc2a.asm
	.include	".\inc3\inc3a"		; inc3a.asm
	.include	".\inc1\inc1"		; inc1.asm,
						; inc1a.asm, inc0.asm
	.include	".\inc2\inc2"		; inc2.asm,
						; inc0a.asm, inc1a.asm,
						; inc2a.asm
						; inc1.asm,
						; inc0a.asm, inc1a.asm
	.include	".\inc3\inc3"	        ; inc3.asm,
						; inc0a.asm, inc1a.asm,
						; inc3a.asm
						; inc1.asm,
						; inc0a.asm, inc1a.asm
						; inc2.asm,
						; inc0a.asm, inc1a.asm,
						; inc2a.asm
						; inc1.asm,
						; inc0a.asm, inc1a.asm

