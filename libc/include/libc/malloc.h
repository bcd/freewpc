#ifndef _LIBC_MALLOC_H
#define _LIBC_MALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BLOCK {
   size_t size;
   struct BLOCK *prev;
   struct BLOCK *next;
} BLOCK;

#define BLOCK_FROM_DATA(d)  (struct BLOCK *)((char *)d - sizeof(BLOCK))
#define DATA_FROM_BLOCK(bp)  ((char *)bp + sizeof(BLOCK))
#define NEXT_CONTIGUOUS_BLOCK(bp)  (struct BLOCK *)(DATA_FROM_BLOCK(bp) + bp->size)


extern char _end;  /* from crtn.s */

#ifdef __cplusplus
}
#endif

#endif /* !defined _LIBC_MALLOC_H */
