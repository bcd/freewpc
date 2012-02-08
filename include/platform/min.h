#ifndef __PLATFORM_MIN_H
#define __PLATFORM_MIN_H

/* This is an example of a 'minimal platform' which only provides the
   basic necessities for a pinball game. */

#define PINIO_NUM_LAMPS 16
#define PINIO_NUM_SWITCHES 8
#define PINIO_NUM_SOLS 4
#define PINIO_GI_STRINGS 0x3

#define IO_SWITCH 0
#define IO_LAMP 1
#define IO_SOL 2
#define IO_DIAG 3

/* TODO : we shouldn't need to declare anything if there is no banking... */

#define PINIO_BANK_ROM 0
#define PINIO_BANK_RAM 1
extern inline void pinio_set_bank (U8 bankno, U8 val) { }
extern inline U8 pinio_get_bank (U8 bankno) { return 0; }

/* TODO : why do I have to say this? */
#define LOCAL_SIZE 64
#define AREA_SIZE(name) 0

extern inline void pinio_write_solenoid_set (U8 set, U8 val)
{
	if (set == 0)
		writeb (IO_SOL, val & 0xF);
}

extern inline IOPTR sol_get_write_reg (U8 sol)
{
	return IO_SOL;
}

extern inline void pinio_reset_sound (void)
{
}

extern inline void pinio_write_sound (U8 val)
{
}

extern inline bool pinio_sound_ready_p (void)
{
}

extern inline U8 pinio_read_sound (void)
{
}

extern inline void pinio_nvram_lock (void)
{
}

extern inline void pinio_nvram_unlock (void)
{
}

extern inline void pinio_clear_periodic (void)
{
}

extern inline void pinio_enable_flippers (void)
{
}

extern inline void pinio_disable_flippers (void)
{
}

extern inline void pinio_active_led_toggle (void)
{
}

extern inline U8 pinio_read_locale (void)
{
}

#define timestamp_update(x)

#endif /* __PLATFORM_MIN_H */
