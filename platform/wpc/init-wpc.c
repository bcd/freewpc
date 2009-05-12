
#include <freewpc.h>


/**
 * Initialize the WPC platform.
 */
void platform_init (void)
{
	/* Reset the sound board... the earlier the better */
	pinio_reset_sound ();

	/* Initializing the RAM page */
	wpc_set_ram_page (0);

#ifdef __m6809__
	/* Install the null pointer catcher, by programming
	 * some SWI instructions at zero. */
	*(U8 *)0 = 0x3F;
	*(U8 *)1 = 0x3F;
#endif /* __m6809__ */

	/* Set up protected RAM */
	wpc_set_ram_protect (RAM_UNLOCKED);
	wpc_set_ram_protect_size (PROT_BASE_0x1800);
	wpc_set_ram_protect (RAM_LOCKED);

#ifdef CONFIG_DEBUG_ADJUSTMENTS
	irq_ack_value = 0x96;
#endif

	/* Initialize the ROM page register
	 * page of ROM adjacent to the system area is mapped.
	 * This is the default location for machine-specific files. */
	wpc_set_rom_page (MACHINE_PAGE);

	/* Initialize other critical WPC output registers relating
	 * to hardware */
	writeb (WPC_SOL_FLASH2_OUTPUT, 0);
	writeb (WPC_SOL_HIGHPOWER_OUTPUT, 0);
	writeb (WPC_SOL_FLASH1_OUTPUT, 0);
	writeb (WPC_SOL_LOWPOWER_OUTPUT, 0);
	writeb (WPC_LAMP_ROW_OUTPUT, 0);
	writeb (WPC_GI_TRIAC, 0);
}

