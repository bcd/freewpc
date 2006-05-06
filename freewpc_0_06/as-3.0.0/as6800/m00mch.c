/* m00mch.c */

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
#include "m6800.h"

/*
 * Process a machine op.
 */
VOID
machine(mp)
struct mne *mp;
{
	register int op, t1;
	struct expr e1;
	struct area *espa;
	char id[NCPS];
	int c, v1, reg;

	clrexpr(&e1);
	reg = 0;
	op = mp->m_valu;
	switch (mp->m_type) {

	case S_SDP:
		espa = NULL;
		if (more()) {
			expr(&e1, 0);
			if (e1.e_flag == 0 && e1.e_base.e_ap == NULL) {
				if (e1.e_addr) {
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

	case S_INH:
		outab(op);
		break;

	case S_PUL:
		v1 = admode(abx);
		if (v1 == S_A) {
			outab(op);
			break;
		}
		if (v1 == S_B) {
			outab(op+1);
			break;
		}
		aerr();
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

	case S_TYP1:
		t1 = addr(&e1);
		if (t1 == S_A) {
			outab(op|A);
			break;
		}
		if (t1 == S_B) {
			outab(op|B);
			break;
		}
		if (t1 == S_DIR) {
			outab(op|0x30);
			outrw(&e1, 0);
			aerr();
			break;
		}
		if (t1 == S_EXT) {
			outab(op|0x30);
			outrw(&e1, 0);
			break;
		}
		if (t1 == S_INDX) {
			outab(op|X);
			outrb(&e1, R_USGN);
			break;
		}
		aerr();
		break;

	case S_TYP2:
		if ((reg = admode(abx)) == 0)
			aerr();

	case S_TYP3:
		if (!reg) {
			reg = op & 0x40;
		} else if (reg == S_A) {
			reg = 0x00;
		} else if (reg == S_B) {
			reg = 0x40;
		} else {
			aerr();
			reg = 0x00;
		}
		t1 = addr(&e1);
		if (t1 == S_IMMED) {
			if ((op|0x40) == 0xC7)
				aerr();
			outab(op|reg);
			outrb(&e1, 0);
			break;
		}
		if (t1 == S_EXT) {
			outab(op|reg|0x30);
			outrw(&e1, 0);
			break;
		}
		if (t1 == S_DIR) {
			outab(op|reg|0x10);
			outrb(&e1, R_PAG0);
			break;
		}
		if (t1 == S_INDX) {
			outab(op|reg|0x20);
			outrb(&e1, R_USGN);
			break;
		}
		aerr();
		break;

	case S_TYP4:
		t1 = addr(&e1);
		if (t1 == S_IMMED) {
			if ((op&0x0D) == 0x0D)
				aerr();
			outab(op);
			outrw(&e1, 0);
			break;
		}
		if (t1 == S_EXT) {
			outab(op|0x30);
			outrw(&e1, 0);
			break;
		}
		if (t1 == S_DIR) {
			outab(op|0x10);
			outrb(&e1, R_PAG0);
			break;
		}
		if (t1 == S_INDX) {
			outab(op|0x20);
			outrb(&e1, R_USGN);
			break;
		}
		aerr();
		break;

	case S_TYP5:
		t1 = addr(&e1);
		if (t1 == S_DIR) {
			outab(op|0x10);
			outrw(&e1, 0);
			aerr();
			break;
		}
		if (t1 == S_EXT) {
			outab(op|0x10);
			outrw(&e1, 0);
			break;
		}
		if (t1 == S_INDX) {
			outab(op);
			outrb(&e1, R_USGN);
			break;
		}
		aerr();
		break;

	default:
		err('o');
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

/*
 * Machine dependent initialization
 */
VOID
minit()
{
}
