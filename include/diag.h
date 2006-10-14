
#ifndef __DIAG_H
#define __DIAG_H

#define MAX_DIAG_ERRORS 4

typedef enum {
	DIAG_CPU_ERROR,
	DIAG_RAM_ERROR,
	DIAG_ROM_ERROR,
} diag_error_code_t;


__common__ int diag_get_error_count (void);
__common__ void diag_test_cpu (void);
__common__ void diag_test_ram (void);
__common__ void diag_test_rom (void);
__common__ void diag_test_wpc (void);
__common__ void diag_run (void);
__common__ void diag_run_at_reset (void);

#endif /* __DIAG_H */
