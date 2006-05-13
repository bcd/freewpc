
typedef struct _deff_info { int a; char b; } deff_info;

#include "deff.h"

int main ()
{
	int x = DEFF_INDEX(sound_deff);
	int y = DEFF_INDEX(attract_deff);
	int z = DEFF_INDEX(volume_deff);
	printf ("y is %d, z is %d\n", y, z);
}
