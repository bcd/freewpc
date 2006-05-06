/* M16MCH:C */

/*
 * (C) Copyright 1991-2000
 * All Rights Reserved
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 */

#include <stdio.h>
#include <setjmp.h>
#include "asxxxx.h"
#include "m6816.h"

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
	register int op, rf, cpg;
	struct expr e1, e2, e3;
	char id[NCPS];
	struct area *espa;
	int c, pc, t1, t2, vn;

	clrexpr(&e1);
	clrexpr(&e2);
	clrexpr(&e3);
	pc = dot.s_addr;
	cpg = 0;
	op = mp->m_valu;
	switch (rf = mp->m_type) {

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

	case S_IMMA:
		if (addr(&e1) == T_IMM) {
			if (mchimm(&e1)) {
				outab(PAGE3);
				outab(op);
				outrw(&e1, R_NORM);
			} else {
				outab(op);
				outrb(&e1, R_NORM);
			}
		} else {
			dot.s_addr += 4;
			aerr();
		}
		break;

	case S_IM16:
		if (addr(&e1) == T_IMM) {
			outab(PAGE3);
			outab(op);
			outrw(&e1, R_NORM);
		} else {
			dot.s_addr += 4;
			aerr();
		}
		break;

	case S_BIT:
		t1 = addr(&e1);
		comma();
		t2 = addr(&e2);
		if ((t2 != T_IMM) && (t2 != T_EXT)) {
			aerr();
		}
		if (t1 == T_EXT) {
			outab(op|0x30);
			mchubyt(&e2);
			outrb(&e2, R_USGN);
			outrw(&e1, R_NORM);
		} else
		if (t1 & T_INDX) {
			if (mchindx(t1, &e1)) {
				outab(op|(t1 & 0x30));
				mchubyt(&e2);
				outrb(&e2, R_USGN);
				outrw(&e1, R_NORM);
			} else {
				outab(PAGE1);
				outab(op|(t1 & 0x30));
				mchubyt(&e2);
				outrb(&e2, R_USGN);
				mchubyt(&e1);
				outrb(&e1, R_USGN);
			}
		} else {
			dot.s_addr += 4;
			aerr();
		}
		break;

	case S_BITW:
		t1 = addr(&e1);
		comma();
		t2 = addr(&e2);
		if ((t2 != T_IMM) && (t2 != T_EXT)) {
			aerr();
		}
		if (t1 == T_EXT) {
			t1 |= 0x30;
		} else
		if (t1 & T_INDX) {
			if (t1 & T_IND8) {
				aerr();
			}
		} else {
			aerr();
		}
		outab(PAGE2);
		outab(op|(t1 & 0x30));
		outrw(&e2, R_NORM);
		outrw(&e1, R_NORM);
		break;

	case S_BRBT:
		t1 = addr(&e1);
		comma();
		t2 = addr(&e2);
		comma();
		addr(&e3);
		if ((t2 != T_IMM) && (t2 != T_EXT)) {
			aerr();
		}
		if (t1 == T_EXT) {
			outab(op|0x30);
			mchubyt(&e2);
			outrb(&e2, R_USGN);
			outrw(&e1, R_NORM);
			if (mchpcr(&e3)) {
				/*
				 * pc     = address following instruction - 6
				 *        = (dot.s_addr + 2) - 6
				 *
				 * offset = e3.e_addr - (pc + 6)
				 *        = e3.e_addr - (((dot.s_addr + 2) - 6) + 6)
				 *        = e3.e_addr - dot.s_addr - 2 + 6 - 6
				 *        = e3.e_addr - dot.s_addr - 2
				 */
				vn = e3.e_addr - dot.s_addr - 2;
				outaw(vn);
			} else {
				/* R_PCR is calculated relative to the
				 * PC value after the R_PCR word. This
				 * accounts for 6 of the 6 byte offset
				 * required.  Thus no offset adjustment
				 * is required.
				 */
				outrw(&e3, R_PCR);
			}
		} else
		if (t1 & T_INDX) {
			if ((t1 & T_IND8) || (t1 & T_IND16)) {
				;
			} else
			if (mchindx(t1, &e1)) {
				t1 |= T_IND16;
			} else {
				if (mchpcr(&e3)) {
					vn = e3.e_addr - dot.s_addr - 4;
					if ((vn < -128) || (vn > 127)) {
						t1 |= T_IND16;
					} else {
						t1 |= T_IND8;
					}
				} else {
					t1 |= T_IND16;
				}
			}
			if (t1 & T_IND8) {
				if (op == 0x0A)
					op = 0xCB;
				if (op == 0x0B)
					op = 0x8B;
				outab(op|(t1 & 0x30));
				mchubyt(&e2);
				outrb(&e2, R_USGN);
				mchubyt(&e1);
				outrb(&e1, R_USGN);
				if (mchpcr(&e3)) {
					/*
					 * pc     = address following instruction - 4
					 *        = (dot.s_addr + 1) - 4
					 *
					 * offset = e3.e_addr - (pc + 6)
					 *        = e3.e_addr - (((dot.s_addr + 1) - 4) + 6)
					 *        = e3.e_addr - dot.s_addr - 1 + 4 - 6
					 *        = e3.e_addr - dot.s_addr - 3
					 */
					vn = e3.e_addr - dot.s_addr - 3;
					if ((vn < -128) || (vn > 127))
						aerr();
					outab(vn);
				} else {
					/* R_PCR is calculated relative to the
					 * PC value after the R_PCR byte. This
					 * accounts for 4 of the 6 byte offset
					 * required.  Thus a 2 byte adjustment
					 * is required.
					 */
					e3.e_addr -= 2;
					outrb(&e3, R_PCR);
				}
			} else
			if (t1 & T_IND16) {
				outab(op|(t1 & 0x30));
				mchubyt(&e2);
				outrb(&e2, R_USGN);
				outrw(&e1, R_NORM);
				if (mchpcr(&e3)) {
					/*
					 * pc     = address following instruction - 6
					 *        = (dot.s_addr + 2) - 6
					 *
					 * offset = e3.e_addr - (pc + 6)
					 *        = e3.e_addr - (((dot.s_addr + 2) - 6) + 6)
					 *        = e3.e_addr - dot.s_addr - 2 + 6 - 6
					 *        = e3.e_addr - dot.s_addr - 2
					 */
					vn = e3.e_addr - dot.s_addr - 2;
					outaw(vn);
				} else {
					/* R_PCR is calculated relative to the
					 * PC value after the R_PCR word. This
					 * accounts for 6 of the 6 byte offset
					 * required.  Thus no offset adjustment
					 * is required.
					 */
					outrw(&e3, R_PCR);
				}
			}
		} else {
			dot.s_addr += 6;
			aerr();
		}
		break;

	case S_LDED:
		t1 = addr(&e1);
		if (t1 == T_EXT) {
			outab(PAGE2);
			outab(op);
			outrw(&e1, R_NORM);
		} else {
			dot.s_addr += 4;
			aerr();
		}
		break;

	case S_MAC:
		t1 = addr(&e1);
		if (more()) {
			comma();
			t2 = addr(&e2);
			if ((t1 != T_IMM) || !mchcon(&e1) ||
			    (t2 != T_IMM) || !mchcon(&e2))
				aerr();
			outab(op);
			outab(((e1.e_addr << 4) & 0xF0) | (e2.e_addr & 0x0F));
		} else {
			if (t1 != T_IMM)
				aerr();
			outab(op);
			outrb(&e1, R_NORM);
		}
		break;

	case S_PSHM:
		vn = 0;
		do {
			if ((t1 = admode(pshm)) == 0 || vn & t1)
				aerr();
			vn |= t1;
		} while (more() && comma());
		outab(op);
		outab(vn);
		break;

	case S_PULM:
		vn = 0;
		do {
			if ((t1 = admode(pulm)) == 0 || vn & t1)
				aerr();
			vn |= t1;
		} while (more() && comma());
		outab(op);
		outab(vn);
		break;

	case S_JXX:
		t1 = addr(&e1);
		comma();
		t2 = addr(&e2);
		if ((t1 != T_IMM) && (t1 != T_EXT)) {
			aerr();
		}
		if (t2 == T_EXT) {
			if (op == 0x4B)
				outab(0x70);
			if (op == 0x89)
				outab(0xFA);
			mchubyt(&e1);
			outrb(&e1, R_USGN);
			outrw(&e2, R_NORM);
		} else
		if ((t2 & T_INDX) && (t2 != (T_INDX|T_IND8))) {
			outab(op|(t2 & 0x30));
			mchubyt(&e1);
			outrb(&e1, R_USGN);
			outrw(&e2, R_NORM);
		} else {
			dot.s_addr += 4;
			aerr();
		}
		break;

	case S_MOVB:
	case S_MOVW:
		t1 = addr(&e1);
		comma();
		t2 = addr(&e2);
		if((t1 == T_EXT) && (t2 == T_EXT)) {
			outab(PAGE3);
			outab(op|0xFE);
			outrw(&e1, R_NORM);
			outrw(&e2, R_NORM);
		} else
		if((t1 & T_INDX) && (t2 == T_EXT)) {
			outab(op|(t1 & 0x30));
			mchubyt(&e1);
			outrb(&e1, R_USGN);
			outrw(&e2, R_NORM);
		} else
		if((t1 == T_EXT) && (t2 & T_INDX)) {
			outab(op|0x02|(t2 & 0x30));
			mchubyt(&e2);
			outrb(&e2, R_USGN);
			outrw(&e1, R_NORM);
		} else {
			dot.s_addr += 6;
			aerr();
		}
		break;

	case S_CMP:
	case S_LOAD:
	case S_STOR:
		t1 = addr(&e1);
		if (t1 == T_EXT) {
			outab(PAGE1);
			outab(op|0x30);
			outrw(&e1, R_NORM);
		} else
		if ((t1 == T_IMM) && (rf != S_STOR)) {
			outab(PAGE3);
			if (rf == S_CMP)
				outab(op|0x30);
			if (rf == S_LOAD)
				outab((op|0x30) & 0xBF);
			outrw(&e1, R_NORM);
		} else
		if (t1 & T_INDX) {
			if (mchindx(t1, &e1)) {
				outab(PAGE1);
				outab(op|(t1 & 0x30));
				outrw(&e1, R_NORM);
			} else {
				outab(op|(t1 & 0x30));
				mchubyt(&e1);
				outrb(&e1, R_USGN);
			}
		} else {
			dot.s_addr += 4;
			aerr();
		}
		break;

	case S_SOPW:
		t1 = addr(&e1);
		if (t1 == T_EXT) {
			t1 |= 0x30;
		} else
		if (t1 & T_INDX) {
			if (t1 & T_IND8) {
				aerr();
			}
		} else {
			aerr();
		}
		outab(PAGE2);
		outab(op|(t1 & 0x30));
		outrw(&e1, R_NORM);
		break;

	case S_SOP:
		t1 = addr(&e1);
		if (t1 == T_EXT) {
			outab(PAGE1);
			outab(op|0x30);
			outrw(&e1, R_NORM);
		} else
		if (t1 & T_INDX) {
			if (mchindx(t1, &e1)) {
				outab(PAGE1);
				outab(op|(t1 & 0x30));
				outrw(&e1, R_NORM);
			} else {
				outab(op|(t1 & 0x30));
				mchubyt(&e1);
				outrb(&e1, R_USGN);
			}
		} else {
			dot.s_addr += 4;
			aerr();
		}
		break;

	case S_DOPE:
		t1 = addr(&e1);
		if (t1 == T_EXT) {
			outab(PAGE3);
			outab(op|0x30);
			outrw(&e1, R_NORM);
		} else
		if (t1 == T_IMM) {
			if (op == 0x41) {
				if (mchimm(&e1)) {
					outab(PAGE3);
					outab((op|0x30)&0x3F);
					outrw(&e1, R_NORM);
				} else {
					outab(0x7C);
					outrb(&e1, R_NORM);
				}
			} else {
				outab(PAGE3);
				outab((op|0x30)&0x3F);
				outrw(&e1, R_NORM);
			}
		} else
		if ((t1 & T_INDX) && (t1 != (T_INDX|T_IND8))) {
			outab(PAGE3);
			outab(op|(t1 & 0x30));
			outrw(&e1, R_NORM);
		} else {
			dot.s_addr += 4;
			aerr();
		}
		break;

	case S_DOPD:
		t1 = addr(&e1);
		if (t1 == T_EXT) {
			outab(PAGE3);
			outab(op|0x70);
			outrw(&e1, R_NORM);
		} else
		if ((t1 == T_IMM) && (op != 0x8A)) {
			if (op == 0x81) {
				if (mchimm(&e1)) {
					outab(PAGE3);
					outab(op|0x30);
					outrw(&e1, R_NORM);
				} else {
					outab(0xFC);
					outrb(&e1, R_NORM);
				}
			} else {
				outab(PAGE3);
				outab(op|0x30);
				outrw(&e1, R_NORM);
			}
		} else
		if (t1 & T_INDX) {
			if (mchindx(t1, &e1)) {
				outab(PAGE3);
				outab(op|0x40|(t1 & 0x30));
				outrw(&e1, R_NORM);
			} else {
				outab(op|(t1 & 0x30));
				mchubyt(&e1);
				outrb(&e1, R_USGN);
			}
		} else
		if (t1 & T_E_I) {
			outab(PAGE2);
			outab(op|(t1 & 0x30));
		} else {
			dot.s_addr += 4;
			aerr();
		}
		break;

	case S_DOP:
		t1 = addr(&e1);
		if (t1 == T_EXT) {
			outab(PAGE1);
			outab(op|0x30);
			outrw(&e1, R_NORM);
		} else
		if ((t1 == T_IMM) && (op != 0x4A) && (op != 0xCA)) {
			outab(op|0x30);
			outrb(&e1, R_NORM);
		} else
		if (t1 & T_INDX) {
			if (mchindx(t1, &e1)) {
				outab(PAGE1);
				outab(op|(t1 & 0x30));
				outrw(&e1, R_NORM);
			} else {
				outab(op|(t1 & 0x30));
				mchubyt(&e1);
				outrb(&e1, R_USGN);
			}
		} else
		if (t1 & T_E_I) {
			outab(PAGE2);
			outab(op|(t1 & 0x30));
		} else {
			dot.s_addr += 4;
			aerr();
		}
		break;

	case S_INH37:
		cpg += PAGE3-PAGE2;

	case S_INH27:
		cpg += PAGE2;
		outab(cpg);
		outab(op);
		break;

	case S_LBRA:
		cpg += PAGE3-PAGE2;

	case S_LBSR:
		cpg += PAGE2;
		expr(&e1, 0);
		outab(cpg);
		outab(op);
		if (mchpcr(&e1)) {
			/*
			 * pc     = address following instruction - 4
			 *        = (dot.s_addr + 2) - 4
			 *
			 * offset = e1.e_addr - (pc + 6)
			 *        = e1.e_addr - (((dot.s_addr + 2) - 4) + 6)
			 *        = e1.e_addr - dot.s_addr - 2 + 4 - 6
			 *        = e1.e_addr - dot.s_addr - 4
			 */
			vn = e1.e_addr - dot.s_addr - 4;
			outaw(vn);
		} else {
			/*
			 * R_PCR is calculated relative to the
			 * PC value after the R_PCR word. This
			 * accounts for 4 of the 6 byte offset
			 * required.  Thus a 2 byte adjustment
			 * is required.
			 */
			e1.e_addr -= 2;
			outrw(&e1, R_PCR);
		}
		if (e1.e_mode != S_USER)
			aerr();
		break;

	case S_BRA:
	case S_BSR:
		expr(&e1, 0);
		outab(op);
		if (mchpcr(&e1)) {
			/*
			 * pc     = address following instruction - 2
			 *        = (dot.s_addr + 1) - 2
			 *
			 * offset = e1.e_addr - (pc + 6)
			 *        = e1.e_addr - (((dot.s_addr + 1) - 2) + 6)
			 *        = e1.e_addr - dot.s_addr - 1 + 2 - 6
			 *        = e1.e_addr - dot.s_addr - 5
			 */
			vn = e1.e_addr - dot.s_addr - 5;
			if ((vn < -128) || (vn > 127))
				rerr();
			outab(vn);
		} else {
			/*
			 * R_PCR is calculated relative to the
			 * PC value after the R_PCR byte. This
			 * accounts for 2 of the 6 byte offset
			 * required.  Thus a 4 byte adjustment
			 * is required.
			 */
			e1.e_addr -= 4;
			outrb(&e1, R_PCR);
		}
		if (e1.e_mode != S_USER)
			rerr();
		break;

	default:
		err('o');
	}
	if (pc & 0x0001) {
		err('b');
		dot.s_addr += 1;
	}
}

/*
 * Check if argument is a constant
 */
int
mchcon(e1)
struct expr *e1;
{
	if (e1->e_base.e_ap == NULL && e1->e_flag == 0) {
		return(1);
	}
	return(0);
}

/*
 * Addressing error if argument is a constant
 * and unsigned byte is greater than 0x00FF
 */
VOID
mchubyt(e1)
struct expr *e1;
{
	if (mchcon(e1) && (e1->e_addr & 0xFF00)) {
		aerr();
	}
}

/*
 * Check index mode
 */
int
mchindx(t1,e1)
int t1;
struct expr *e1;
{
	int flag;

	if (t1 & T_IND8) {
		flag = 0;
	} else
	if (t1 & T_IND16) {
		flag = 1;
	} else
	if (pass == 0) {
		flag = 1;
	} else
	if (pass == 1) {
		if (e1->e_addr >= dot.s_addr)
			e1->e_addr -= fuzz;
		flag = 0;
		if (e1->e_addr>255 || e1->e_flag || e1->e_base.e_ap)
			flag = 1;
		if (setbit(flag))
			flag = 1;
	} else {
		if (getbit()) {
			flag = 1;
		} else {
			flag = 0;
		}
	}
	return(flag);
}

/*
 * Check immediate mode
 */
int
mchimm(e1)
struct expr *e1;
{
	int flag, vn;

	if (pass == 0) {
		flag = 1;
	} else
	if (pass == 1) {
		if (e1->e_addr >= dot.s_addr)
			e1->e_addr -= fuzz;
		vn = e1->e_addr;
		flag = 0;
		if ((vn<-128) || (vn>127) || e1->e_flag || e1->e_base.e_ap)
			flag = 1;
		if (setbit(flag)) {
			flag = 1;
		}
	} else {
		if (getbit()) {
			flag = 1;
		} else {
			flag = 0;
		}
	}
	return(flag);
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
