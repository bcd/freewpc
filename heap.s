
#include "wpc.h"

.area ram

heap_ptr::		.blkw 1


.area sysrom

proc(heap_init)
	uses(x)
	ldx	#HEAP_BASE
	stx	heap_ptr
endp

proc(heap_alloc)	; D = number of bytes to allocate, returns X = base
	ldx	heap_ptr
	pshs	x
	leax	d,x
	stx	heap_ptr
	puls	x
endp

