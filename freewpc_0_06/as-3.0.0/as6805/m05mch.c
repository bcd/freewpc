/* m05mch.c */

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
#include "m6805.h"

/*
 * Process a machine op.
 */
VOID
machine(mp)
struct mne *mp;
{
	register int op, t1, t2, type;
	struct expr e1, e2, e3;
	addr_t espv;
	struct area *espa;
	char id[NCPS];
	int c, v1;

	clrexpr(&e1);
	clrexpr(&e2);
	clrexpr(&e3);
	op = mp->m_valu;
	type = mp->m_type;
	switch (type) {

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

	case S_BRA:
		expr(&e1, 0);
		outab(op);
		if (e1.e_base.e_ap == dot.s_area) {
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
			outab(op+0x10);
			break;
		}
		if (t1 == S_X) {
			outab(op+0x20);
			break;
		}
		if (t1 == S_DIR || t1 == S_EXT) {
			outab(op);
			outrb(&e1, R_PAG0);
			break;
		}
		if (t1 == S_IX) {
			outab(op+0x40);
			break;
		}
		if (t1 == S_I8X || t1 == S_INDX) {
			outab(op+0x30);
			outrb(&e1, R_USGN);
			break;
		}
		aerr();
		break;

	case S_TYP2:
		t1 = addr(&e1);
		espv = e1.e_addr;
		espa = e1.e_base.e_ap;
		if (t1 == S_IMMED) {
			if ((op == 0xA7) ||
			    (op == 0xAC) ||
			    (op == 0xAF))
				aerr();
			outab(op);
			outrb(&e1, 0);
			break;
		}
		if (t1 == S_DIR) {
			outab(op+0x10);
			outrb(&e1, R_PAG0);
			break;
		}
		if (t1 == S_EXT) {
			outab(op+0x20);
			outrw(&e1, 0);
			break;
		}
		if (t1 == S_IX) {
			outab(op+0x50);
			break;
		}
		if (t1 == S_I8X) {
			outab(op+0x40);
			outrb(&e1, R_USGN);
			break;
		}
		if (t1 == S_INDX) {
			outab(op+0x30);
			outrw(&e1, 0);
			break;
		}
		aerr();
		break;

	case S_TYP3:
		t1 = addr(&e1);
		espv = e1.e_addr;
		if (t1 != S_IMMED || espv & ~0x07)
			aerr();
		comma();
		t2 = addr(&e2);
		if (t2 != S_DIR)
			aerr();
		outab(op + 2*(espv&0x07));
		outrb(&e2, R_PAG0);
		break;

	case S_TYP4:
		t1 = addr(&e1);
		espv = e1.e_addr;
		if (t1 != S_IMMED || espv & ~0x07)
			aerr();
		comma();
		t2 = addr(&e2);
		if (t2 != S_DIR)
			aerr();
		comma();
		expr(&e3, 0);
		outab(op + 2*(espv&0x07));
		outrb(&e2, R_PAG0);
		if (mchpcr(&e3)) {
			v1 = e3.e_addr - dot.s_addr - 1;
			if ((v1 < -128) || (v1 > 127))
				aerr();
			outab(v1);
		} else {
			outrb(&e3, R_PCR);
		}
		if (e3.e_mode != S_USER)
			rerr();
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
 * Machine specific initialization.
 */
VOID
minit()
{
}
