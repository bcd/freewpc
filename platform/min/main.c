
#ifndef CONFIG_SIM
void native_init (void)
{
	protected_memory_load ();
}

void native_exit (void)
{
	protected_memory_save ();
}

int main (void)
{
	native_init ();
	freewpc_init ();
	return 0;
}
#endif

void puts_default (const char *s)
{
	printf ("%s\n", s);
}
