

/*
 * Generic table lookup routines.
 *
 * A table descriptor consists of a table size, an entry size, and a
 * pointer to the storage area.  Descriptors always live in ROM,
 * whereas the storage could be RAM or ROM.
 *
 * All tables also have a "key" field, which is used for lookups.
 * The size of the key could be 1 or 2 bytes.
 *
 */

/* The structure of the table descriptor */
#define TABLE_COUNT_OFF		0
#define TABLE_SIZE_OFF		1
#define TABLE_DATA_OFF		2
#define TABLE_KEYOFF_OFF	4	
#define TABLE_KEYLEN_OFF	5
#define TABLE_SORTOFF_OFF	6


.area fastram

table_lookup_key:		.blkw 1


.area sysrom

proc(table_init)
	uses(d,x,y)
	requires(x)
	jsr	table_get_size
	ldx	TABLE_DATA_OFF,x
	tfr	d,y
	jsr	bzerol
endp


proc(table_get_size)
	requires(x)
	returns(d)
	lda	TABLE_COUNT_OFF,x
	ldb	TABLE_SIZE_OFF,x
	mul
endp


	; Input: X = table descriptor, A = key byte
	; Output: Y = pointer to table entry that matches and true, or false
	;   if not found
proc(table_find_key_byte)
	requires(x,a)
	uses(d,u)
	returns(y)

	sta	table_lookup_key

	jsr	table_get_size					/* D = size of table in bytes */
	ldy	TABLE_DATA_OFF,x				/* Y = base of table */
	leau	d,u								/* U = address of end of table */
	ldb	TABLE_KEYOFF_OFF,x			/* B = index of key field */
	leay	b,y
	leau	b,u

	ldb	TABLE_SIZE_OFF,x
	loop
		lda	,y
		cmpa	table_lookup_key
		ifeq
			ldb	TABLE_KEYOFF_OFF,x
			negb
			leay	b,y
			true
			return
		endif
		leay	b,y
	while(nz)
	false
endp


proc(table_find_largest_sort_byte)
	requires(x,a)
	uses(d,u)
	local(byte, largest_key)
	local(word, largest_entry)
	returns(y)

	clr	largest_key
	ldy	#0
	sty	largest_entry
	
	jsr	table_get_size					/* D = size of table in bytes */
	ldy	TABLE_DATA_OFF,x				/* Y = base of table */
	leau	d,u								/* U = address of end of table */
	ldb	TABLE_SORTOFF_OFF,x			/* B = index of key field */
	leay	b,y
	leau	b,u

	ldb	TABLE_SIZE_OFF,x
	loop
		lda	,y
		cmpa	largest_key
		ifhi
			sta	largest_key
			sty	largest_entry
		endif
		leay	b,y
	/* TODO : not doing y != u comparison here */
	while(nz)
	ldy	largest_entry
	cmpy	#0
	ifne
		ldb	TABLE_SORTOFF_OFF,x
		negb
		leay	b,y
	endif
endp


	; Input: X = table descriptor, U = key word
	; Output: Y = pointer to table entry that matches and true, or false
	;   if not found
proc(table_find_key_word)
	requires(x,a)
	uses(d,u)
	returns(y)

	stu	table_lookup_key

	jsr	table_get_size					/* D = size of table in bytes */
	ldy	TABLE_DATA_OFF,x				/* Y = base of table */
	leau	d,y								/* U = address of end of table */
	ldb	TABLE_KEYOFF_OFF,x			/* B = index of key field */
	leay	b,y
	leau	b,u

	ldb	TABLE_SIZE_OFF,x
	loop
		ldd	,y
		cmpd	table_lookup_key
		ifeq
			ldb	TABLE_KEYOFF_OFF,x
			negb
			leay	b,y
			true
			return
		endif
		ldb	TABLE_SIZE_OFF,x
		leay	b,y
	while(nz)
	false
endp


