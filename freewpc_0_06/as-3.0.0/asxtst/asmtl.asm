	.title	Machine Independent Assembler Test
	.module Asmtst

	;  The file 'asmtl.asm' must be assembled with
	; a LO/HI assembler (in file ___ext.c, hilo = 0).
	;
	;  The file 'asmth.asm' must be assembled with
	; a HI/LO assembler (in file ___ext.c, hilo = 1).
	
	.sbttl	Memory Allocation Directives

	.radix	O			; set default to octal

					; binary constants
	.byte	0b11000000		; C0
	.byte	0B1110			; 0E
	.byte	$%11000000		; C0

					; octal constants
	.byte	24			; 14
	.byte	024			; 14
	.byte	0q024			; 14
	.byte	0Q024			; 14
	.byte	0o024			; 14
	.byte	0O024			; 14
	.byte	$&24			; 14
	
					; decimal constant
	.byte	0d024			; 18
	.byte	0D024			; 18
	.byte	$#24			; 18

					; hexidecimal constants
	.byte	0h024			; 24
	.byte	0H024			; 24
	.byte	0x024			; 24
	.byte	0X024			; 24
	.byte	$$24			; 24

	.db	0			; 00
	.dw	0			; 00 00

	.radix	D			; set default to decimal

	.byte	1,2,3			; 01 02 03
	.byte	4,5,6			; 04 05 06
	.byte	7,8,9			; 07 08 09
	.word	1,2,3			; 01 00 02 00 03 00
	.word	4,5,6			; 04 00 05 00 06 00
	.word	7,8,9			; 07 00 08 00 09 00

	.blkb	16
	.ds	16

	.blkw	16
	.ds	16*2

word:	.word	.+2			;r92s00
	.word	.-2			;r90s00
	.word	2+.			;r96s00
	.word	.-(word+2)		; 04 00


	.sbttl	Boundary Directives

	.even
	.byte	0			; 00
	.even
	.byte	1			; 01
	.even
	.odd
	.byte	2			; 02
	.odd
	.byte	3			; 03
	.odd
	.even
	.word	4			; 04 00
	.odd
	.word	5			; 05 00
	.even
	.word	6			; 06 00


	.sbttl	String Directives

	.ascii	"abcde"			; 61 62 63 64 65
	.asciz	"abcde"			; 61 62 63 64 65 00
	.ascis	"abcde"			; 61 62 63 64 E5
	

	.sbttl	Expression Evaluation

	n0x00	=	0x00
	n0x01	=	0x01
	n0x10	=	0x10
	n0xff	=	0xff
	n0xeeff =	0xeeff


	n	=	< n0xeeff	; 0xff		low byte
	.byte	>n,<n			; 00 FF
	n	=	> n0xeeff	; 0xee		high byte
	.byte	>n,<n			; 00 EE


	n	=	'A		; 0x41		single character
	.byte	>n,<n			; 00 41
	n	=	"AB		; 0x4142	double character
	.byte	>n,<n			; 42 41
	n	=	n0x01		; 0x01		assignment
	.byte	>n,<n			; 00 01
	n	=	n + n0x01	; 0x02		addition
	.byte	>n,<n			; 00 02
	n	=	n - n0x01	; 0x01		subtraction
	.byte	>n,<n			; 00 01
	n	=	n * 0x05	; 0x05		multiplication
	.byte	>n,<n			; 00 05
	n	=	n / 0x02	; 0x02		division
	.byte	>n,<n			; 00 02
	n	=	n0x10 % 0x05	; 0x01		modulus
	.byte	>n,<n			; 00 01
	n	=	n0x10 | n0x01	; 0x11		or
	.byte	>n,<n			; 00 11
	n	=	n0xff & n0x01	; 0x01		and
	.byte	>n,<n			; 00 01
	n	=	n0x01 << 4	; 0x10		left shift
	.byte	>n,<n			; 00 10
	n	=	n0x10 >> 4	; 0x01		right shift
	.byte	>n,<n			; 00 01
	n	=	n0xff ^ n0x10	; 0xef		xor
	.byte	>n,<n			; 00 EF
	n	=	n ^ n0x10	; 0xff		xor
	.byte	>n,<n			; 00 FF
	n	=	~n0x10		; 0xffef	1's complement
	.byte	>n,<n			; FF EF
	n	=	-n0x10		; 0xfff0	2's complement
	.byte	>n,<n			; FF F0



	n	=	n0xeeff & 0xff	; 0xff		low byte
	.byte	>n,<n			; 00 FF
	n = (n0xeeff & 0xff00)/0x100	; 0xee		high byte
	.byte	>n,<n			; 00 EE


	n	=	n0xeeff % 0x100 ; 0xff		low byte
	.byte	>n,<n			; 00 FF
	n	=	n0xeeff / 0x100 ; 0xee		high byte
	.byte	>n,<n			; 00 EE


	n	=	3*(2 + 4*(6))	; 0x4e		expression evaluation
	.byte	>n,<n			; 00 4E
	n	=	2*(0x20 + <~n0x10)	; 0x21e
	.byte	>n,<n			; 02 1E


	.sbttl	arithmatic tests

	;
	; The following series of tests verify that
	; the arithmetic is 16-Bit and unsigned.


	.sbttl	Addition

	n =	 1 +  1			; 0x0001 + 0x0001 ; 0x0002
	.byte	>n,<n			; 00 02
	n =	-1 +  1			; 0xFFFF + 0x0001 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-1 + -1			; 0xFFFF + 0xFFFF ; 0xFFFE
	.byte	>n,<n			; FF FE

	n =	 32768 +  32768		; 0x8000 + 0x8000 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-32768 +  32768		; 0x8000 + 0x8000 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-32768 + -32768		; 0x8000 + 0x8000 ; 0x0000
	.byte	>n,<n			; 00 00

	n =	 65535 +  1		; 0xFFFF + 0x0001 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	 65535 + -1		; 0xFFFF + 0xFFFF ; 0xFFFE
	.byte	>n,<n			; FF FE


	.sbttl	Subtraction

	n =	 1 -  1			; 0x0001 - 0x0001 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-1 -  1			; 0xFFFF - 0x0001 ; 0xFFFE
	.byte	>n,<n			; FF FE
	n =	-1 - -1			; 0xFFFF - 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00

	n =	 32768 -  32768		; 0x8000 - 0x8000 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-32768 -  32768		; 0x8000 - 0x8000 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-32768 - -32768		; 0x8000 - 0x8000 ; 0x0000
	.byte	>n,<n			; 00 00

	n =	 65535 -  1		; 0xFFFF - 0x0001 ; 0xFFFE
	.byte	>n,<n			; FF FE
	n =	 65535 - -1		; 0xFFFF - 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00


	.sbttl	Multiplication

	n =	 1 *  1			; 0x0001 * 0x0001 ; 0x0001
	.byte	>n,<n			; 00 01
	n =	 1 * -1			; 0x0001 * 0xFFFF ; 0xFFFF
	.byte	>n,<n			; FF FF
	n =	-1 * -1			; 0xFFFF * 0xFFFF ; 0x0001
	.byte	>n,<n			; 00 01

	n =	 256 *  256		; 0x0100 * 0x0100 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	 256 * -256		; 0x0100 * 0xFF00 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-256 * -256		; 0xFF00 * 0xFF00 ; 0x0000
	.byte	>n,<n			; 00 00


	.sbttl	Division

	n =	 1 /  1			; 0x0001 / 0x0001 ; 0x0001
	.byte	>n,<n			; 00 01
	n =	10 /  2			; 0x000A / 0x0002 ; 0x0005
	.byte	>n,<n			; 00 05
	n =	512 / 4			; 0x0200 / 0x0004 ; 0x0080
	.byte	>n,<n			; 00 80
	n =	32768 / 2		; 0x8000 / 0x0002 ; 0x4000
	.byte	>n,<n			; 40 00
	n =	65535 / 2		; 0xFFFF / 0x0002 ; 0x7FFF
	.byte	>n,<n			; 7F FF

	n = 	 1 / -1			; 0x0001 / 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00
	n =	 32767 / -1		; 0x7FFF / 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00
	n =	 32768 / -1		; 0x8000 / 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00
	n =	 65534 / -1		; 0xFFFE / 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00
	n =	 65535 / -1		; 0xFFFF / 0xFFFF ; 0x0001
	.byte	>n,<n			; 00 01
	n =	-1 / -1			; 0xFFFF / 0xFFFF ; 0x0001
	.byte	>n,<n			; 00 01
	n =	-2 / -1			; 0xFFFE / 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-32768 / -1		; 0x8000 / 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-32769 / -1		; 0x7FFF / 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-65535 / -1		; 0x0001 / 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-65536 / -1		; 0x0000 / 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00

	n =	-256 /   -1		; 0xFF00 / 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-256 / -255		; 0xFF00 / 0xFF01 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-256 / -256		; 0xFF00 / 0xFF00 ; 0x0001
	.byte	>n,<n			; 00 01
	n =	-256 / -257		; 0xFF00 / 0xFEFF ; 0x0001
	.byte	>n,<n			; 00 01
	n =	-256 / -32767		; 0xFF00 / 0x8001 ; 0x0001
	.byte	>n,<n			; 00 01
	n =	-256 / -32768		; 0xFF00 / 0x8000 ; 0x0001
	.byte	>n,<n			; 00 01
	n =	-256 /  32768		; 0xFF00 / 0x8000 ; 0x0001
	.byte	>n,<n			; 00 01
	n =	-256 /  65280		; 0xFF00 / 0xFF00 ; 0x0001
	.byte	>n,<n			; 00 01
	n =	-256 /  65281		; 0xFF00 / 0xFF01 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-256 /  65535		; 0xFF00 / 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00


	.sbttl	Modulus

	n =	 1 %  1			; 0x0001 % 0x0001 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	10 %  2			; 0x000A % 0x0002 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	512 % 4			; 0x0200 % 0x0004 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	32768 % 2		; 0x8000 % 0x0002 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	65535 % 2		; 0xFFFF % 0x0002 ; 0x0001
	.byte	>n,<n			; 00 01

	n = 	 1 % -1			; 0x0001 % 0xFFFF ; 0x0001
	.byte	>n,<n			; 00 01
	n =	 32767 % -1		; 0x7FFF % 0xFFFF ; 0x7FFF
	.byte	>n,<n			; 7F FF
	n =	 32768 % -1		; 0x8000 % 0xFFFF ; 0x8000
	.byte	>n,<n			; 80 00
	n =	 65534 % -1		; 0xFFFE % 0xFFFF ; 0xFFFE
	.byte	>n,<n			; FF FE
	n =	 65535 % -1		; 0xFFFF % 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-1 % -1			; 0xFFFF % 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-2 % -1			; 0xFFFE % 0xFFFF ; 0xFFFE
	.byte	>n,<n			; FF FE
	n =	-32768 % -1		; 0x8000 % 0xFFFF ; 0x8000
	.byte	>n,<n			; 80 00
	n =	-32769 % -1		; 0x7FFF % 0xFFFF ; 0x7FFF
	.byte	>n,<n			; 7F FF
	n =	-65535 % -1		; 0x0001 % 0xFFFF ; 0x0001
	.byte	>n,<n			; 00 01
	n =	-65536 % -1		; 0x0000 % 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00

	n =	-256 %   -1		; 0xFF00 % 0xFFFF ; 0xFF00
	.byte	>n,<n			; FF 00
	n =	-256 % -255		; 0xFF00 % 0xFF01 ; 0xFF00
	.byte	>n,<n			; FF 00
	n =	-256 % -256		; 0xFF00 % 0xFF00 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-256 % -257		; 0xFF00 % 0xFEFF ; 0x0001
	.byte	>n,<n			; 00 01
	n =	-256 % -32767		; 0xFF00 % 0x8001 ; 0x7EFF
	.byte	>n,<n			; 7E FF
	n =	-256 % -32768		; 0xFF00 % 0x8000 ; 0x7F00
	.byte	>n,<n			; 7F 00
	n =	-256 %  32768		; 0xFF00 % 0x8000 ; 0x7F00
	.byte	>n,<n			; 7F 00
	n =	-256 %  65280		; 0xFF00 % 0xFF00 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-256 %  65281		; 0xFF00 % 0xFF01 ; 0xFF00
	.byte	>n,<n			; FF 00
	n =	-256 %  65535		; 0xFF00 % 0xFFFF ; 0xFF00
	.byte	>n,<n			; FF 00


	.sbttl	IF, ELSE, and ENDIF

	n = 0
	m = 0

	.if	0
		n = 1
		.if	0
		m = 1
		.else
		m = 2
		.endif
	.else
		n = 2
	.endif

					; n = 2, m = 0
	.byte	n,m			; 02 00

	;*******************************************************

	n = 0
	m = 0

	.if	1
		n = 1
		.if	0
		m = 1
		.else
		m = 2
		.endif
	.else
		n = 2
	.endif

					; n = 1, m = 2
	.byte	n,m			; 01 02



	n = 0
	m = 0

	.if	0
		n = 1
		.if	1
		m = 1
		.else
		m = 2
		.endif
	.else
		n = 2
	.endif

					; n = 2, m = 0
	.byte	n,m			; 02 00

	;*******************************************************

	n = 0
	m = 0

	.if	1
		n = 1
		.if	1
		m = 1
		.else
		m = 2
		.endif
	.else
		n = 2
	.endif

					; n = 1, m = 1
	.byte	n,m			; 01 01



	n = 0
	m = 0

	.if	0
		n = 1
	.else
		.if	0
		m = 1
		.else
		m = 2
		.endif
		n = 2
	.endif

					; n = 2, m = 2
	.byte	n,m			; 02 02

	;*******************************************************

	n = 0
	m = 0

	.if	1
		n = 1
	.else
		.if	0
		m = 1
		.else
		m = 2
		.endif
		n = 2
	.endif

					; n = 1, m = 0
	.byte	n,m			; 01 00



	n = 0
	m = 0

	.if	0
		n = 1
	.else
		.if	1
		m = 1
		.else
		m = 2
		.endif
		n = 2
	.endif

					; n = 2, m = 1
	.byte	n,m			; 02 01

	;*******************************************************

	n = 0
	m = 0

	.if	1
		n = 1
	.else
		.if	1
		m = 1
		.else
		m = 2
		.endif
		n = 2
	.endif

					; n = 1, m = 0
	.byte	n,m			; 01 00


	.sbttl	Local Symbols

lclsym0:
					; forward references
	.word	0$,1$			;rA2s01rA4s01
	.word	2$,3$			;rA6s01rA8s01
	.word	4$,5$			;rAAs01rACs01
	.word	6$,7$			;rAEs01rB0s01
	.word	8$,9$			;rB2s01rB4s01

0$:	.word	9$			;rB4s01
1$:	.word	8$			;rB2s01
2$:	.word	7$			;rB0s01
3$:	.word	6$			;rAEs01
4$:	.word	5$			;rACs01
5$:	.word	4$			;rAAs01
6$:	.word	3$			;rA8s01
7$:	.word	2$			;rA6s01
8$:	.word	1$			;rA4s01
9$:	.word	0$			;rA2s01
10$:

					; backward references
	.word	0$,1$			;rA2s01rA4s01
	.word	2$,3$			;rA6s01rA8s01
	.word	4$,5$			;rAAs01rACs01
	.word	6$,7$			;rAEs01rB0s01
	.word	8$,9$			;rB2s01rB4s01

lclsym1:
					; forward references
	.word	0$,1$			;rDEs01rE0s01
	.word	2$,3$			;rE2s01rE4s01
	.word	4$,5$			;rE6s01rE8s01
	.word	6$,7$			;rEAs01rECs01
	.word	8$,9$			;rEEs01rF0s01

0$:	.word	9$			;rF0s01
1$:	.word	8$			;rEEs01
2$:	.word	7$			;rECs01
3$:	.word	6$			;rEAs01
4$:	.word	5$			;rE8s01
5$:	.word	4$			;rE6s01
6$:	.word	3$			;rE4s01
7$:	.word	2$			;rE2s01
8$:	.word	1$			;rE0s01
9$:	.word	0$			;rDEs01
10$:

					; backward references
	.word	0$,1$			;rDEs01rE0s01
	.word	2$,3$			;rE2s01rE4s01
	.word	4$,5$			;rE6s01rE8s01
	.word	6$,7$			;rEAs01rECs01
	.word	8$,9$			;rEEs01rF0s01

	.sbttl	Offset calculations

	ofsbyte	=	(10$-0$)	; 0x0014
	ofsword	=	ofsbyte/2	; 0x000A

					; 1$ + 0x001E
	.word	1$+ofsbyte+ofsword	;rFEs01

	.sbttl	Lower/Upper Byte Selections

	.globl	extern

					; low byte
	.byte	< (extern + 0x0001)	;r01

					; low byte
	.byte	< (extern + 0x0200)	;r00

					; high byte 
	.byte	> (extern + 0x0003)	;s00

					; high byte
	.byte	> (extern + 0x0400)	;s04


	.sbttl	Area Definitions

	.globl	code0
	.globl	cnstnt1,cnstnt2

	cnstnt0 == 0xabcd		; global equate

code0:	.word	a0			;r00s00
	.word	cnstnt0			; CD AB

	.area	AreaA (OVR)

	cnstnt1 = 0x1234

a0:	.word	0x00ff			; FF 00

	.area	AreaB (ABS,OVR)

	cnstnt2 = 0x5678

	.word	a1			;r00s00

	.area	AreaA

	.=.+0x0020
	.word	a2			;r00s00

	.area	AreaB
	.org	0x40

	.word	a0,a1,a2		;r00s00r00s00r00s00
	.word	AreaB,OVR		;r00s00r00s00

abcdabcd::				; global symbol

	.sbttl	Assembler Output File asmt_.sym

;       assembled by:
;                asxxxx -glosxff asmt_.asm
;
; symbol table
;
;    .__.ABS.=  0000 G   |     AreaB      **** GX  |     OVR        **** GX
;  1 a0         0000 R   |     a1         **** GX  |     a2         **** GX
;  2 abcdabcd   004A GR  |     cnstnt0 =  ABCD G   |     cnstnt1 =  1234 G
;    cnstnt2 =  5678 G   |   0 code0      020C GR  |     extern     **** GX
;  0 lclsym0    018E R   |   0 lclsym1    01CA R   |     m       =  0000 
;    n       =  0001     |     n0x00   =  0000     |     n0x01   =  0001 
;    n0x10   =  0010     |     n0xeeff =  EEFF     |     n0xff   =  00FF 
;    ofsbyte =  0014     |     ofsword =  000A     |   0 word       0090 R
;
; area table
;
;   0 _CODE      size  210   flags 0
;   1 A          size   24   flags 4
;   2 B          size   4A   flags C
;
