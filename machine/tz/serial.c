/* Routines to check serial number of table
 * with a view to disabling 'unauthorised' tables */
#include <freewpc.h>

extern __common__ void pic_render_serial_number (void);
/*
bool check_pic_serial_number (U8 pic_number_to_check)
{
	if (pic_number_to_check == pic_serial_number)
		return TRUE;
	else
		return FALSE;
}
*/
void show_serial_number_deff (void)
{
	dmd_alloc_pair_clean ();
	//sprintf("%d", pic_serial_number);
	//font_render_string_center (&font_var5, 64, 7, sprintf_buffer);
	pic_render_serial_number ();
	dmd_show2 ();
	task_sleep_sec (7);
	deff_exit ();
}
