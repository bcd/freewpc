

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

/* The structure of the 6-byte table descriptor */
#define TABLE_COUNT_OFF		0
#define TABLE_SIZE_OFF		1
#define TABLE_DATA_OFF		2
#define TABLE_KEYOFF_OFF	4	
#define TABLE_KEYLEN_OFF	5


proc(table_init)
	uses(y)
	requires(x)
	ldx	TABLE_DATA_OFF,x
	jsr	table_get_size
	jsr	bzerol
endp


proc(table_get_size)
	requires(x)
	uses(a,b)
	lda	TABLE_COUNT_OFF,x
	ldb	TABLE_SIZE_OFF,x
	mul
	tfr	d,y
endp


	; Input: X = table descriptor, A = key byte
	; Output: Y = pointer to table entry that matches and true, or false
	;   if not found
proc(table_find_key_byte)
	requires(x,a)
	uses(y,b)
	returns(y)

	jsr	table_get_size
	tfr	y,u

	ldy	TABLE_DATA_OFF,x				/* Y = base of table */
	//!!!!!
	//leau	y,u								/* U = address of end of table */

	loop
		ldb	TABLE_KEYOFF_OFF,x
		cmpa	b,y
		ifeq
			true
			return
		endif
		ldb	TABLE_SIZE_OFF,x
		leay	b,y
	while(nz)
endp


	; Input: X = table descriptor
	; Output: Y = pointer to table entry that matches and true, or false
	;   if not found
proc(table_find_key_word)
	requires(x)
endp

