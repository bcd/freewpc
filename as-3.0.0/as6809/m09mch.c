/* M09MCH:C */

/*
 * (C) Copyright 1989-2000
 * All Rights Reserved
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 */

#include <stdio.h>
#include <setjmp.h>
#include "asxxxx.h"
#include "m6809.h"

#define	NB	512

int	*bp;
int	bm;
int	bb[NB];

/*
 * Process a machine op.
 */
VOID
machine(mp)
struct mne *mp;
{
	register int op, rf, cpg, c;
	struct expr e1;
	int t1, v1, v2;
	struct area *espa;
	char id[NCPS];

	cpg = 0;
	clrexpr(&e1);
	op = mp->m_valu;
	switch (rf = mp->m_type) {

	case S_SDP:
		espa = NULL;
		if (more()) {
			expr(&e1, 0);
			if (e1.e_flag == 0 && e1.e_base.e_ap == NULL) {
				if (e1.e_addr & 0xFF) {
					err('b');
				}
			}
			if ((c = getnb()) == ',') {
				getid(id, -1);
				espa = alookup(id);
				if (espa == NULL) {
					err('u');
				}
			} else {
				unget(c);
			}
		}
		if (espa) {
			outdp(espa, &e1);
		} else {
			outdp(dot.s_area, &e1);
		}
		lmode = SLIST;
		break;

	case S_INH2:
		cpg += 0x01;

	case S_INH1:
		cpg += 0x10;

	case S_INH:
		if (cpg)
			outab(cpg);
		outab(op);
		break;

	case S_BRA:
		expr(&e1, 0);
		outab(op);
		if (mchpcr(&e1)) {
			v1 = e1.e_addr - dot.s_addr - 1;
			if ((v1 < -128) || (v1 > 127))
				aerr();
			outab(v1);
		} else {
			outrb(&e1, R_PCR);
		}
		if (e1.e_mode != S_USER)
			rerr();
		break;

	case S_LBRA:
		cpg += 0x10;

	case S_LBSR:
		expr(&e1, 0);
		if (cpg)
			outab(cpg);
		outab(op);
		if (mchpcr(&e1)) {
			v1 = e1.e_addr - dot.s_addr - 2;
			outaw(v1);
		} else {
			outrw(&e1, R_PCR);
		}
		if (e1.e_mode != S_USER)
			aerr();
		break;

	case S_PULS:
		v1 = 0;
		do {
			if ((t1 = admode(stks)) == 0 || v1 & t1)
				aerr();
			v1 |= t1;
		} while (more() && comma());
		outab(op);
		outab(v1);
		break;

	case S_PULU:
		v1 = 0;
		do {
			if ((t1 = admode(stku)) == 0 || v1 & t1)
				aerr();
			v1 |= t1;
		} while (more() && comma());
		outab(op);
		outab(v1);
		break;

	case S_EXG:
		v1 = admode(regs);
		comma();
		v2 = admode(regs);
		if ((v1 & 0x08) != (v2 & 0x08))
			aerr();
		outab(op);
		outab((v1<<4)|v2);
		break;

	case S_ACC:
		t1 = addr(&e1);
		if (t1 == S_IMMED)
			e1.e_mode = S_IMB;
		genout(cpg, op, rf, &e1);
		break;

	case S_STR1:
		cpg += 0x10;

	case S_SOP:
	case S_STR:
		t1 = addr(&e1);
		if (t1 == S_IMMED)
			e1.e_mode = S_IMER;
		genout(cpg, op, rf, &e1);
		break;

	case S_LR2:
		cpg += 0x01;

	case S_LR1:
		cpg += 0x10;

	case S_LR:
		t1 = addr(&e1);
		if (t1 == S_IMMED)
			e1.e_mode = S_IMW;
		genout(cpg, op, rf, &e1);
		break;

	case S_LEA:
		t1 = addr(&e1);
		if (aindx) {
			genout(cpg, op, rf, &e1);
			break;
		}
		aerr();
		break;

	case S_CC:
		t1 = addr(&e1);
		if (t1 == S_IMMED) {
			e1.e_mode = S_IMB;
			genout(cpg, op, rf, &e1);
			break;
		}
		aerr();
		break;

	case S_6800:
		m68out(op);
		break;

	default:
		err('o');
	}
}

/*
 * General Output Routine
 */
VOID
genout(cpg, op, rf, esp)
register int cpg, op, rf;
register struct expr *esp;
{
	int espv;
	struct area *espa;
	int disp, flag;

	espv = esp->e_addr;
	espa = esp->e_base.e_ap;
	switch (esp->e_mode) {

	case S_IMB:
		if (cpg)
			outab(cpg);
		outab(op);
		outrb(esp, R_NORM);
		break;

	case S_IMW:
		if (cpg)
			outab(cpg);
		outab(op);
		outrw(esp, R_NORM);
		break;

	case S_DIR:
		if (cpg)
			outab(cpg);
		if (rf == S_SOP) {
			outab(op&0x0F);
		} else {
			outab(op|0x10);
		}
		outrb(esp, R_PAG);
		break;

	case S_EXT:
		if (cpg)
			outab(cpg);
		if (aindx) {
			outab(op|0x20);
			outab(aindx|0x0F);
			outrw(esp, R_NORM);
			break;
		}
		outab(op|0x30);
		outrw(esp, R_NORM);
		break;

	case S_IND:
		if (cpg)
			outab(cpg);
		outab(op|0x20);
		outab(aindx);
		break;

	case S_PC:
		if (espa) {
			aerr();
			break;
		}
		if (cpg)
			outab(cpg);
		outab(op|0x20);
		if (pass == 0) {
			dot.s_addr += 3;
		} else
		if (pass == 1) {
			if (esp->e_addr >= dot.s_addr)
				esp->e_addr -= fuzz;
			dot.s_addr += 2;
			disp = esp->e_addr;
			flag = 0;
			if (disp < -128 || disp > 127)
				++flag;
			if (setbit(flag))
				++dot.s_addr;
		} else {
			if (getbit()) {
				outab(aindx|0x01);
				outaw(espv);
			} else {
				outab(aindx);
				outab(espv);
			}
		}
		break;

	case S_PCR:
		if (cpg)
			outab(cpg);
		outab(op|0x20);
		if (pass == 0) {
			dot.s_addr += 3;
		} else
		if (espa && espa != dot.s_area) {
			outab(aindx|0x01);
			outrw(esp, R_PCR);
		} else
		if (pass == 1) {
			if (esp->e_addr >= dot.s_addr)
				esp->e_addr -= fuzz;
			dot.s_addr += 2;
			disp = esp->e_addr - dot.s_addr;
			flag = 0;
			if (disp < -128 || disp > 127)
				++flag;
			if (setbit(flag))
				++dot.s_addr;
		} else {
			if (getbit()) {
				outab(aindx|0x01);
				disp = espv - dot.s_addr - 2;
				outaw(disp);
			} else {
				outab(aindx);
				disp = espv - dot.s_addr - 1;
				outab(disp);
			}
		}
		break;

	case S_OFST:
		if (cpg)
			outab(cpg);
		outab(op|0x20);
		if (pass == 0) {
			dot.s_addr += 3;
		} else
		if (espa) {
			outab(aindx|0x09);
			outrw(esp, R_NORM);
		} else
		if (pass == 1) {
			if (esp->e_addr >= dot.s_addr)
				esp->e_addr -= fuzz;
			dot.s_addr += 1;
			flag = 0;
			if (espv < -128 || espv > 127)
				++flag;
			if (setbit(flag)) {
				dot.s_addr += 2;
			} else {
				flag = aindx & 0x10;
				if (espv < -16 || espv > 15)
					++flag;
				if (setbit(flag))
					++dot.s_addr;
			}
		} else {
			if (getbit()) {
				outab(aindx|0x09);
				outaw(espv);
			} else {
				if (getbit()) {
					outab(aindx|0x08);
					outab(espv);
				} else {
					outab((aindx & 0x60) | (espv & 0x1F));
				}
			}
		}
		break;

	case S_IMER:
	default:
		aerr();
	}
}

/*
 * mc6800 compatibility output routine
 */
VOID
m68out(i)
int i;
{
	register char *ptr;
	register int j;

	ptr = (char *) &mc6800[i];
	for (j=0; j<4 ; j++) {
		if ((i = *ptr++) != 0) {
			outab(i);
		} else {
			break;
		}
	}
}

/*
 * Branch/Jump PCR Mode Check
 */
int
mchpcr(esp)
register struct expr *esp;
{
	if (esp->e_base.e_ap == dot.s_area) {
		return(1);
	}
	if (esp->e_flag==0 && esp->e_base.e_ap==NULL) {
		/*
		 * Absolute Destination
		 *
		 * Use the global symbol '.__.ABS.'
		 * of value zero and force the assembler
		 * to use this absolute constant as the
		 * base value for the relocation.
		 */
		esp->e_flag = 1;
		esp->e_base.e_sp = &sym[1];
	}
	return(0);
}

/*
 * Machine specific initialization.
 * Set up the bit table.
 */
VOID
minit()
{
	bp = bb;
	bm = 1;
}

/*
 * Store `b' in the next slot of the bit table.
 * If no room, force the longer form of the offset.
 */
int
setbit(b)
int b;
{
	if (bp >= &bb[NB])
		return(1);
	if (b)
		*bp |= bm;
	bm <<= 1;
	if (bm == 0) {
		bm = 1;
		++bp;
	}
	return(b);
}

/*
 * Get the next bit from the bit table.
 * If none left, return a `1'.
 * This will force the longer form of the offset.
 */
int
getbit()
{
	register int f;

	if (bp >= &bb[NB])
		return (1);
	f = *bp & bm;
	bm <<= 1;
	if (bm == 0) {
		bm = 1;
		++bp;
	}
	return (f);
}

/*
 * The next character must be a
 * comma.
 */
int
comma()
{
	if (getnb() != ',')
		qerr();
	return(1);
}
