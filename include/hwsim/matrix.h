
#ifndef _HWSIM_MATRIX_H
#define _HWSIM_MATRIX_H

/*****
 *****	Generic I/O matrix
 *****/

struct io_matrix
{
	unsigned char *rowptr;
	unsigned char rowlatch;
	unsigned char rowdata[0];
};

struct io_switch_matrix
{
	struct io_matrix header;
	unsigned char data[SWITCH_BITS_SIZE+1];
};

struct io_lamp_matrix
{
	struct io_matrix header;
	unsigned char data[NUM_LAMP_COLS];
};


#endif /* _HWSIM_MATRIX_H */
