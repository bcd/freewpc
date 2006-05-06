/* r65mch.c */

/*
 * (C) Copyright 1995-2000
 * All Rights Reserved
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 */

/*
 * With Contributions from
 *
 * Marko Makela
 * Sillitie 10 A
 * 01480 Vantaa
 * Finland
 * Internet: Marko.Makela@Helsinki.Fi
 * EARN/BitNet: msmakela@finuh
 */

#include <stdio.h>
#include <setjmp.h>
#include "asxxxx.h"
#include "r6500.h"

int r6500;
int r65f11;
int r65c00;
int r65c02;

/*
 * Process a machine op.
 */
VOID
machine(mp)
struct mne *mp;
{
	register int op, t1;
	struct expr e1,e2;
	struct area *espa;
	char id[NCPS];
	int c, v1, v2;

	clrexpr(&e1);
	clrexpr(&e2);
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

	case S_R6500:
		r65f11 =  0;
		r65c00 =  0;
		r65c02 =  0;
		break;

	case S_R65F11:
		r65f11 = 1;
		r65c00 = 0;
		r65c02 = 0;
		break;

	case S_R65C00:
		r65f11 = 1;
		r65c00 = 1;
		r65c02 = 0;
		break;

	case S_R65C02:
		r65f11 = 1;
		r65c00 = 1;
		r65c02 = 1;
		break;

	case S_INH3:
		if (r65c02) {
			err('o');
			break;
		}
		
	case S_INH2:
		if (!r65c00) {
			err('o');
			break;
		}
		
	case S_INH1:
		outab(op);
		break;

	case S_BRA2:
		if (!r65c00) {
			err('o');
			break;
		}
		
	case S_BRA1:
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

	case S_JSR:
		t1 = addr(&e1);
		outab(op);
		outrw(&e1, R_USGN);
		if (t1 != S_DIR && t1 != S_EXT)
			aerr();
		break;

	case S_JMP:
		t1 = addr(&e1);
		switch (t1) {
		case S_DIR:
		case S_EXT:
	                outab(op);
			outrw(&e1, R_USGN);
			break;
		case S_IND:
			outab(op + 0x20);
			outrw(&e1, R_USGN);
			break;
		default:
			if (r65c02) {	/* Check 65C02 Extensions */
				switch(t1) {
				case S_IPREX:
					outab(op + 0x30);
					outrw(&e1, R_USGN);
					break;
				default:
					outab(op);
					outaw(0);
					aerr();
					break;
				}
			} else {
				outab(op);
				outaw(0);
				aerr();
			}
			break;
		}
		break;

	case S_DOP:
		t1 = addr(&e1);
		switch (t1) {
		case S_IPREX:
			outab(op + 0x01);
			outrb(&e1, R_PAG0);
			if ((!e1.e_flag)
			    && (e1.e_base.e_ap==NULL)
				&& (e1.e_addr & ~0xFF))
				    err('d');
			break;
		case S_DIR:
			outab(op + 0x05);
			outrb(&e1, R_PAG0);
			break;
		case S_IMMED:
			outab(op + 0x09);
			outrb(&e1, R_USGN);
			if (op == 0x80)
				aerr();
			break;
		case S_EXT:
			outab(op + 0x0D);
			outrw(&e1, R_USGN);
			break;
		case S_IPSTY:
			outab(op + 0x11);
			outrb(&e1, R_PAG0);
			if ((!e1.e_flag)
			    && (e1.e_base.e_ap==NULL)
				&& (e1.e_addr & ~0xFF))
				    err('d');
			break;
		case S_DINDX:
			outab(op + 0x15);
			outrb(&e1, R_PAG0);
			break;
		case S_DINDY:
		case S_INDY:
			outab(op + 0x19);
			outrw(&e1, R_USGN);
			break;
		case S_INDX:
			outab(op + 0x1D);
			outrw(&e1, R_USGN);
			break;
		default:
			if (r65c02) {	/* Check 65C02 Extensions */
				switch(t1) {
				case S_IND:
					outab(op + 0x12);
					outrb(&e1, R_PAG0);
					break;
				default:
					outab(op + 0x05);
					outab(0);
					aerr();
					break;
				}
			} else {
				outab(op + 0x05);
				outab(0);
				aerr();
			}
			break;
		}
		break;

	case S_SOP:
		t1 = addr(&e1);
		switch (t1) {
		case S_DIR:
			outab(op + 0x06);
			outrb(&e1, R_PAG0);
			break;
		case S_EXT:
			outab(op + 0x0E);
			outrw(&e1, R_USGN);
			break;
		case S_ACC:
			if (op == 0xC0) {	/* 65C02 Extension */
				outab(0x3A);
				if (!r65c02)
					aerr();
			} else
			if (op == 0xE0) {	/* 65C02 Extension */
				outab(0x1A);
				if (!r65c02)
					aerr();
			} else {
				outab(op + 0x0A);
			}
			break;
		case S_DINDX:
			outab(op + 0x16);
			outrb(&e1, R_PAG0);
			break;
		case S_INDX:
			outab(op + 0x1E);
			outrw(&e1, R_USGN);
			break;
		default:
			outab(op + 0x06);
			outab(0);
			aerr();
			break;
		}
		break;

	case S_BIT:
		t1 = addr(&e1);
		switch (t1) {
		case S_DIR:
			outab(op + 0x04);
			outrb(&e1, R_PAG0);
			break;
		case S_EXT:
			outab(op + 0x0C);
			outrw(&e1, R_USGN);
			break;
		default:
			if (r65c02) {	/* Check 65C02 Extensions */
				switch(t1) {
				case S_DINDX:
					outab(op + 0x14);
					outrb(&e1, R_PAG0);
					break;
				case S_INDX:
					outab(op + 0x1C);
					outrw(&e1, R_USGN);
					break;
				case S_IMMED:
					outab(0x89);
					outrb(&e1, R_USGN);
					break;
				default:
					outab(op + 0x04);
					outab(0);
					aerr();
					break;
				}
			} else {
				outab(op + 0x04);
				outab(0);
				aerr();
			}
			break;
		}
		break;

	case S_CP:
		t1 = addr(&e1);
		switch (t1) {
		case S_DIR:
			outab(op + 0x04);
			outrb(&e1, R_PAG0);
			break;
		case S_EXT:
			outab(op+0x0C);
			outrw(&e1, R_USGN);
			break;
		default:
			if (r65c02) {	/* Check 65C02 Extensions */
				switch(t1) {
				case S_IMMED:
					outab(op);
					outrb(&e1, R_USGN);
					break;
				default:
					outab(op);
					outab(0);
					aerr();
					break;
				}
			} else {
				outab(op);
				outab(0);
				aerr();
			}
			break;
		}
		break;

	case S_LDSTX:
		t1 = addr(&e1);
		switch (t1) {
		case S_IMMED:
			outab(op + 0x02);
			outrb(&e1, R_USGN);
			if (op == 0x80)
				aerr();
			break;
		case S_DIR:
			outab(op + 0x06);
			outrb(&e1, R_PAG0);
			break;
		case S_EXT:
			outab(op + 0x0E);
			outrw(&e1, R_USGN);
			break;
		case S_DINDY:
			outab(op + 0x16);
			outrb(&e1, R_PAG0);
			break;
		case S_INDY:
			outab(op + 0x1E);
			outrw(&e1, R_USGN);
			break;
		default:
			outab(op + 0x06);
			outab(0);
			aerr();
			break;
		}
		break;

	case S_LDSTY:
		t1 = addr(&e1);
		switch (t1) {
		case S_IMMED:
			outab(op);
			outrb(&e1, R_USGN);
			if (op == 0x80)
				aerr();
			break;
		case S_DIR:
			outab(op + 0x04);
			outrb(&e1, R_PAG0);
			break;
		case S_EXT:
			outab(op + 0x0C);
			outrw(&e1, R_USGN);
			break;
		case S_DINDX:
			outab(op + 0x14);
			outrb(&e1, R_PAG0);
			break;
		case S_INDX:
			outab(op + 0x1C);
			outrw(&e1, R_USGN);
			break;
		default:
			outab(op + 0x04);
			outab(0);
			aerr();
			break;
		}
		break;

	case S_BB:
		if (!r65f11) {
			err('o');
			break;
		}
		if ((c = getnb()) != '*')
			unget(c);
		expr(&e1, 0);
		comma();
		expr(&e2, 0);
		outab(op);
		outrb(&e1, R_PAG0);
		if (mchpcr(&e2)) {
			v2 = e2.e_addr - dot.s_addr - 1;
			if ((v2 < -128) || (v2 > 127))
				aerr();
			outab(v2);
		} else {
			outrb(&e2, R_PCR);
		}
		if ((!e1.e_flag)
		    && (e1.e_base.e_ap==NULL)
			&& (e1.e_addr & ~0xFF))
			    aerr();
		if (e2.e_mode != S_USER)
			rerr();
		break;

	case S_MB:
		if (!r65f11) {
			err('o');
			break;
		}
		t1 = addr(&e1);
		outab(op);
		outrb(&e1, R_PAG0);
		if (t1 != S_DIR && t1 != S_EXT)
			aerr();
		if ((!e1.e_flag)
		    && (e1.e_base.e_ap==NULL)
			&& (e1.e_addr & ~0xFF))
			    aerr();
		break;

	case S_STZ:
		if (!r65c02) {
			err('o');
			break;
		}
		switch (addr(&e1)) {
		case S_DIR:
			outab(op + 0x04);
			outrb(&e1, R_PAG0);
			break;
		case S_DINDX:
			outab(op + 0x14);
			outrb(&e1, R_PAG0);
			break;
		case S_EXT:
			outab(op + 0x3C);
			outrw(&e1, R_USGN);
			break;
		case S_INDX:
			outab(op + 0x3E);
			outrw(&e1, R_USGN);
			break;
		default:
			outab(op + 0x04);
			outab(0);
			aerr();
			break;
	        }
		break;

	case S_TB:
		if (!r65c02) {
			err('o');
			break;
		}
		switch (addr(&e1)) {
		case S_DIR:
			outab(op + 0x04);
			outrb(&e1, R_PAG0);
			break;
		case S_EXT:
			outab(op+0x0C);
			outrw(&e1, R_USGN);
			break;
		default:
			outab(op);
			outab(0);
			aerr();
			break;
		}
		break;

	default:
		err('o');
		break;
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
VOID
comma()
{
	if (getnb() != ',')
		qerr();
}

/*
 * Machine dependent initialization
 */
VOID
minit()
{
	r65c02 = 0;
}
