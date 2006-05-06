/* m05adr.c */

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

int
addr(esp)
register struct expr *esp;
{
	register int c;
	register struct area *espa;
	register addr_t espv;
	char *tcp;

	if ((c = getnb()) == '#') {
		expr(esp, 0);
		esp->e_mode = S_IMMED;
	} else if (c == ',') {
		if ((c = admode(ax)) != S_X)
			aerr();
		esp->e_mode = S_IX;
	} else if (c == '*') {
		expr(esp, 0);
		esp->e_mode = S_DIR;
		if (esp->e_addr & ~0xFF)
			err('d');
		if (more()) {
			comma();
			tcp = ip;
			if (admode(ax) == S_X) {
				esp->e_mode = S_I8X;
			} else {
				ip = --tcp;
			}
		}
	} else {
		unget(c);
		if ((esp->e_mode = admode(ax)) != 0) {
			;
		} else {
			expr(esp, 0);
			espa = esp->e_base.e_ap;
			espv = esp->e_addr;
			if (more()) {
				comma();
				if (admode(ax) != S_X)
					aerr();
				if (esp->e_flag == 0 && espa == NULL && (espv & ~0xFF) == 0) {
					esp->e_mode = S_I8X;
				} else {
					esp->e_mode = S_INDX;
				}
			} else {
				esp->e_mode = S_EXT;
			}
		}
	}
	return (esp->e_mode);
}
	
/*
 * Enter admode() to search a specific addressing mode table
 * for a match. Return the addressing value on a match or
 * zero for no match.
 */
int
admode(sp)
register struct adsym *sp;
{
	register char *ptr;
	register int i;
	register char *ips;

	ips = ip;
	unget(getnb());

	i = 0;
	while ( *(ptr = &sp[i].a_str[0]) ) {
		if (srch(ptr)) {
			return(sp[i].a_val);
		}
		i++;
	}
	ip = ips;
	return(0);
}

/*
 *      srch --- does string match ?
 */
int
srch(str)
register char *str;
{
	register char *ptr;
	ptr = ip;

	while (*ptr && *str) {
		if(ccase[*ptr & 0x007F] != ccase[*str & 0x007F])
			break;
		ptr++;
		str++;
	}
	if (ccase[*ptr & 0x007F] == ccase[*str & 0x007F]) {
		ip = ptr;
		return(1);
	}

	if (!*str)
		if (any(*ptr," \t\n,];")) {
			ip = ptr;
			return(1);
		}
	return(0);
}

/*
 *      any --- does str contain c?
 */
int
any(c,str)
int c;
char *str;
{
	while (*str)
		if(*str++ == c)
			return(1);
	return(0);
}

struct adsym	ax[] = {	/* a or x registers */
    {	"a",	S_A	},
    {	"x",	S_X	},
    {	"",	0x00	}
};
