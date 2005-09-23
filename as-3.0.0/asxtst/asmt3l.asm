	.title	Machine Independent Assembler Test
	.module Asmtst

	;  The file 'asmt3l.asm' must be assembled with
	; a LO/HI assembler (in file ___ext.c, hilo = 0).
	;
	;  The file 'asmt3h.asm' must be assembled with
	; a HI/LO assembler (in file ___ext.c, hilo = 1).
	
	.sbttl	Memory Allocation Directives

	.radix	O			; set default to octal
	.24bit				; 24-bit addressing

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
byte3:	.3byte	.+2			;r9As00R00
	.3byte	.-2			;r99s00R00
	.3byte	2+.			;rA0s00R00
	.3byte	.-(byte3+3)		; 06 00 00
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
	n =	 1 +  1			; 0x000001 + 0x000001 ; 0x000002
	.byte	>n,<n			; 00 02
	n =	-1 +  1			; 0xFFFFFF + 0x000001 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-1 + -1			; 0xFFFFFF + 0xFFFFFF ; 0xFFFFFE
	.byte	>n,<n			; FF FE
	n =	 32768 +  32768		; 0x008000 + 0x008000 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-32768 +  32768		; 0xFF8000 + 0x008000 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-32768 + -32768		; 0xFF8000 + 0xFF8000 ; 0xFF0000
	.byte	>n,<n			; 00 00
	n =	 65535 +  1		; 0x00FFFF + 0x000001 ; 0x010000
	.byte	>n,<n			; 00 00
	n =	 65535 + -1		; 0x00FFFF + 0xFFFFFF ; 0x00FFFE
	.byte	>n,<n			; FF FE
	.sbttl	Subtraction
	n =	 1 -  1			; 0x000001 - 0x000001 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-1 -  1			; 0xFFFFFF - 0x000001 ; 0xFFFFFE
	.byte	>n,<n			; FF FE
	n =	-1 - -1			; 0xFFFFFF - 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	 32768 -  32768		; 0x008000 - 0xFF8000 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-32768 -  32768		; 0xFF8000 - 0x008000 ; 0xFF0000
	.byte	>n,<n			; 00 00
	n =	-32768 - -32768		; 0xFF8000 - 0xFF8000 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	 65535 -  1		; 0x00FFFF - 0x000001 ; 0x00FFFE
	.byte	>n,<n			; FF FE
	n =	 65535 - -1		; 0x00FFFF - 0xFFFFFF ; 0x010000
	.byte	>n,<n			; 00 00
	.sbttl	Multiplication
	n =	 1 *  1			; 0x000001 * 0x000001 ; 0x000001
	.byte	>n,<n			; 00 01
	n =	 1 * -1			; 0x000001 * 0xFFFFFF ; 0xFFFFFF
	.byte	>n,<n			; FF FF
	n =	-1 * -1			; 0xFFFFFF * 0xFFFFFF ; 0x000001
	.byte	>n,<n			; 00 01
	n =	 256 *  256		; 0x000100 * 0x000100 ; 0x010000
	.byte	>n,<n			; 00 00
	n =	 256 * -256		; 0x000100 * 0xFFFF00 ; 0xFF0000
	.byte	>n,<n			; 00 00
	n =	-256 * -256		; 0xFFFF00 * 0xFFFF00 ; 0xFF0000
	.byte	>n,<n			; 00 00
	.sbttl	Division
	n =	 1 /  1			; 0x000001 / 0x000001 ; 0x000001
	.byte	>n,<n			; 00 01
	n =	10 /  2			; 0x00000A / 0x000002 ; 0x000005
	.byte	>n,<n			; 00 05
	n =	512 / 4			; 0x000200 / 0x000004 ; 0x000080
	.byte	>n,<n			; 00 80
	n =	32768 / 2		; 0x008000 / 0x000002 ; 0x004000
	.byte	>n,<n			; 40 00
	n =	65535 / 2		; 0x00FFFF / 0x000002 ; 0x007FFF
	.byte	>n,<n			; 7F FF
	n = 	 1 / -1			; 0x000001 / 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	 32767 / -1		; 0x007FFF / 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	 32768 / -1		; 0x008000 / 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	 65534 / -1		; 0x00FFFE / 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	 65535 / -1		; 0x00FFFF / 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-1 / -1			; 0xFFFFFF / 0xFFFFFF ; 0x000001
	.byte	>n,<n			; 00 01
	n =	-2 / -1			; 0xFFFFFE / 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-32768 / -1		; 0xFF8000 / 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-32769 / -1		; 0xFF7FFF / OxFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-65535 / -1		; 0xFF0001 / 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-65536 / -1		; 0xFF0000 / 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-256 /   -1		; 0xFFFF00 / 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-256 / -255		; 0xFFFF00 / 0xFFFF01 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-256 / -256		; 0xFFFF00 / 0xFFFF00 ; 0x000001
	.byte	>n,<n			; 00 01
	n =	-256 / -257		; 0xFFFF00 / 0xFFFEFF ; 0x000001
	.byte	>n,<n			; 00 01
	n =	-256 / -32767		; 0xFFFF00 / 0xFF8001 ; 0x000001
	.byte	>n,<n			; 00 01
	n =	-256 / -32768		; 0xFFFF00 / 0xFF8000 ; 0x000001
	.byte	>n,<n			; 00 01
	n =	-256 /  32768		; 0xFFFF00 / 0x008000 ; 0x0001FF
	.byte	>n,<n			; 01 FF
	n =	-256 /  65280		; 0xFFFF00 / 0x00FF00 ; 0x000101
	.byte	>n,<n			; 01 01
	n =	-256 /  65281		; 0xFFFF00 / 0x00FF01 ; 0x000100
	.byte	>n,<n			; 01 00
	n =	-256 /  65535		; 0xFFFF00 / 0x00FFFF ; 0x000100
	.byte	>n,<n			; 01 00
	.sbttl	Modulus
	n =	 1 %  1			; 0x000001 % 0x000001 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	10 %  2			; 0x00000A % 0x000002 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	512 % 4			; 0x000200 % 0x000004 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	32768 % 2		; 0x008000 % 0x000002 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	65535 % 2		; 0x00FFFF % 0x000002 ; 0x000001
	.byte	>n,<n			; 00 01
	n = 	 1 % -1			; 0x000001 % 0xFFFFFF ; 0x000001
	.byte	>n,<n			; 00 01
	n =	 32767 % -1		; 0x007FFF % 0xFFFFFF ; 0x007FFF
	.byte	>n,<n			; 7F FF
	n =	 32768 % -1		; 0x008000 % 0xFFFFFF ; 0x008000
	.byte	>n,<n			; 80 00
	n =	 65534 % -1		; 0x00FFFE % 0xFFFFFF ; 0x00FFFE
	.byte	>n,<n			; FF FE
	n =	 65535 % -1		; 0x00FFFF % 0xFFFFFF ; 0x00FFFF
	.byte	>n,<n			; FF FF
	n =	-1 % -1			; 0xFFFFFF % 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-2 % -1			; 0xFFFFFE % 0xFFFFFF ; 0xFFFFFE
	.byte	>n,<n			; FF FE
	n =	-32768 % -1		; 0xFF8000 % 0xFFFFFF ; 0xFF8000
	.byte	>n,<n			; 80 00
	n =	-32769 % -1		; 0xFF7FFF % 0xFFFFFF ; 0xFF7FFF
	.byte	>n,<n			; 7F FF
	n =	-65535 % -1		; 0xFF0001 % 0xFFFFFF ; 0xFF0001
	.byte	>n,<n			; 00 01
	n =	-65536 % -1		; 0xFF0000 % 0xFFFFFF ; 0xFF0000
	.byte	>n,<n			; 00 00
	n =	-256 %   -1		; 0xFFFF00 % 0xFFFFFF ; 0XFFFFF0
	.byte	>n,<n			; FF 00
	n =	-256 % -255		; 0xFFFF00 % 0xFFFF01 ; 0xFFFF00
	.byte	>n,<n			; FF 00
	n =	-256 % -256		; 0xFFFF00 % 0xFFFF00 ; 0X000000
	.byte	>n,<n			; 00 00
	n =	-256 % -257		; 0xFFFF00 % 0XFFFF01 ; 0xFF0001
	.byte	>n,<n			; 00 01
	n =	-256 % -32767		; 0xFFFF00 % 0xFF8001 ; 0x007EFF
	.byte	>n,<n			; 7E FF
	n =	-256 % -32768		; 0xFFFF00 % 0xFF8000 ; 0xFF7F00
	.byte	>n,<n			; 7F 00
	n =	-256 %  32768		; 0xFFFF00 % 0x008000 ; 0x007F00
	.byte	>n,<n			; 7F 00
	n =	-256 %  65280		; 0xFFFF00 % 0x00FF00 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-256 %  65281		; 0xFFFF00 % 0x00FF01 ; 0x00FE00
	.byte	>n,<n			; FE 00
	n =	-256 %  65535		; 0xFFFF00 % 0x00FFFF ; 0x000000
	.byte	>n,<n			; 00 00
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
	.word	0$,1$			;rAEs01rB0s01
	.word	2$,3$			;rB2s01rB4s01
	.word	4$,5$			;rB6s01rB8s01
	.word	6$,7$			;rBAs01rBCs01
	.word	8$,9$			;rBEs01rC0s01
0$:	.word	9$			;rC0s01
1$:	.word	8$			;rBEs01
2$:	.word	7$			;rBCs01
3$:	.word	6$			;rBAs01
4$:	.word	5$			;rB8s01
5$:	.word	4$			;rB6s01
6$:	.word	3$			;rB4s01
7$:	.word	2$			;rB2s01
8$:	.word	1$			;rB0s01
9$:	.word	0$			;rAEs01
10$:
					; backward references
	.word	0$,1$			;rAEs01rB0s01
	.word	2$,3$			;rB2s01rB4s01
	.word	4$,5$			;rB6s01rB8s01
	.word	6$,7$			;rBAs01rBCs01
	.word	8$,9$			;rBEs01rC0s01
lclsym1:
					; forward references
	.word	0$,1$			;rEAs01rECs01
	.word	2$,3$			;rEEs01rF0s01
	.word	4$,5$			;rF2s01rF4s01
	.word	6$,7$			;rF6s01rF8s01
	.word	8$,9$			;rFAs01rFCs01
0$:	.word	9$			;rFCs01
1$:	.word	8$			;rFAs01
2$:	.word	7$			;rF8s01
3$:	.word	6$			;rF6s01
4$:	.word	5$			;rF4s01
5$:	.word	4$			;rF2s01
6$:	.word	3$			;rF0s01
7$:	.word	2$			;rEEs01
8$:	.word	1$			;rECs01
9$:	.word	0$			;rEAs01
10$:
					; backward references
	.word	0$,1$			;rEAs01rECs01
	.word	2$,3$			;rEEs01rF0s01
	.word	4$,5$			;rF2s01rF4s01
	.word	6$,7$			;rF6s01rF8s01
	.word	8$,9$			;rFAs01rFCs01
	.sbttl	Offset calculations
	ofsbyte	=	(10$-0$)	; 0x0014
	ofsword	=	ofsbyte/2	; 0x000A
					; 1$ + 0x001E
	.word	1$+ofsbyte+ofsword	;r0As02
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
	.sbttl	Assembler Output File asmt3_.sym
;       assembled by:
;                asxxxx -gloxff asmt3_.asm
;
;Symbol Table
;
;    .__.ABS.       =   000000 G   |     AreaB              ****** GX
;  1 a0                 000000 GR  |     a1                 ****** GX
;    a2                 ****** GX  |   2 abcdabcd           00004A GR
;  0 byte3              000098 GR  |     cnstnt0        =   00ABCD G
;    cnstnt1        =   001234 G   |     cnstnt2        =   005678 G
;  0 code0              000218 GR  |     extern             ****** GX
;  0 lclsym0            00019A GR  |   0 lclsym1            0001D6 GR
;    m              =   000000 G   |     n              =   000001 G
;    n0x00          =   000000 G   |     n0x01          =   000001 G
;    n0x10          =   000010 G   |     n0xeeff        =   00EEFF G
;    n0xff          =   0000FF G   |     ofsbyte        =   000014 G
;    ofsword        =   00000A G   |   0 word               000090 GR
;
;Area Table
;
;   0 _CODE            size    21C   flags   0
;   1 AreaA            size     24   flags   4
;   2 AreaB            size     4A   flags   C

