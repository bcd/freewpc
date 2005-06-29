
;;;/* const_far is a macro which defines a far constant 
;;; * pointer.  It requires two arguments, the offset and
;;; * the page number.
;;; */
;;;define(const_far, [
;;;	.dw	$1
;;;	.db	$2
;;;])
;;;

	/* Input: X = address of buffer, A = bit position */
   /* Output: X = address of byte, B = bitmask */
macro(bitshift)
	stx	WPC_SHIFTADDR
	sta	WPC_SHIFTBIT
	ldx	WPC_SHIFTADDR
	ldb	WPC_SHIFTBIT
endmacro


