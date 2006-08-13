#ifndef __DEFF_INDEX_INCLUDE_GUARD
#define __DEFF_INDEX_INCLUDE_GUARD


/* Index list */
#define DEFF_INDEX_attract_deff	0
#define DEFF_INDEX_volume_deff	1
#define DEFF_INDEX_sound_deff	2

/* Externs for all objects */
extern deff_info attract_deff;
extern deff_info volume_deff;
extern deff_info sound_deff;

/* Table generation macro
 * Invoke this macro in the C file where you want the table of
 * pointers to objects to be defined.
 */

#define DEFF_INDEX_TABLE \
deff_info *deff_table[] = { \
   &attract_deff, \
   &volume_deff, \
   &sound_deff, \
}

/*
 * Table lookup macro
 * Define the macro used by callers to translate an object's name into
 * an index into the table where its pointer is stored.
 */

#define DEFF_INDEX(obj)	DEFF_INDEX_ ## obj

/* Next free index is 3*/


#endif /* __DEFF_INDEX_INCLUDE_GUARD */
