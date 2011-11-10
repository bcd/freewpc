
#include <SDL/SDL.h>
#include <freewpc.h>
#include <simulation.h>

SDL_Surface *screen;
Uint32 color_pixels[4];
Uint32 on_color;
Uint32 off_color;
Uint32 black;

int sdl_sol_history[NUM_POWER_DRIVES] = {};

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 400

#define BOX_SIZE 12
#define BOX_BORDER 4
#define BOX_DIM (BOX_SIZE + BOX_BORDER)

#define DMD_XPOS 0
#define DMD_YPOS 0

#define LAMP_XPOS 0
#define LAMP_YPOS 120

#define SW_XPOS 180
#define SW_YPOS 120

#define SOL_XPOS 360
#define SOL_YPOS 120

static void box (unsigned int x, unsigned int y, unsigned int width, unsigned int height, Uint32 color)
{
	Uint32 *bufp;
	int i;

	if (!screen)
		return;
	while (height > 0)
	{
		bufp = (Uint32 *)screen->pixels + y * screen->pitch/4 + x;
		for (i=0; i < width; i++)
			*bufp++ = color;
		height--;
		y++;
	}
}

void ui_print_command (const char *cmdline)
{
}

void ui_console_render_string (const char *buffer)
{
}

void ui_write_debug (enum sim_log_class c, const char *buffer)
{
	printf ("%s\n", buffer);
}

void ui_write_solenoid (int solno, int on_flag)
{
	int x = solno / 8;
	int y = solno % 8;
	if (on_flag)
		box (SOL_XPOS + x * BOX_DIM, SOL_YPOS + y * BOX_DIM, BOX_SIZE, BOX_SIZE, on_flag ? on_color : off_color);
	//sdl_sol_history[solno] = on_flag ? 0 : 254;
}

void ui_write_lamp (int lampno, int on_flag)
{
	int x = lampno / 8;
	int y = lampno % 8;
	box (LAMP_XPOS + x * BOX_DIM, LAMP_YPOS + y * BOX_DIM, BOX_SIZE, BOX_SIZE, on_flag ? on_color : off_color);
}

void ui_write_triac (int triacno, int on_flag)
{
	box (LAMP_XPOS + triacno * BOX_DIM, LAMP_YPOS + 12 * BOX_DIM, BOX_SIZE, BOX_SIZE, on_flag ? on_color : off_color);
}

void ui_write_switch (int switchno, int on_flag)
{
	int x = switchno / 8;
	int y = switchno % 8;
	box (SW_XPOS + x * BOX_DIM, SW_YPOS + y * BOX_DIM, BOX_SIZE, BOX_SIZE, on_flag ? on_color : off_color);
}

void ui_write_sound_command (unsigned int x)
{
}

void ui_write_sound_reset (void)
{
}

void ui_write_task (int taskno, int gid)
{
}

#if (MACHINE_DMD == 1)
void ui_refresh_asciidmd (unsigned char *data)
{
	int x, y;
	for (y = 0; y < PINIO_DMD_HEIGHT; y++)
	{
		for (x = 0; x < PINIO_DMD_WIDTH; x++)
		{
			Uint32 color = color_pixels[(*data <= 3) ? *data : 0];
			box (DMD_XPOS + x * 3, DMD_YPOS + y * 3, 2, 2, color);
			data++;
		}
	}
}
#else
void ui_refresh_display (unsigned int x, unsigned int y, char c)
{
}
#endif

void ui_update_ball_tracker (unsigned int ballno, const char *location)
{
}

U8 ui_poll_events (void)
{
	SDL_Event event;
	static U8 key = '\0';

	if (SDL_PollEvent (&event))
	{
		if (event.type == SDL_KEYDOWN && key == '\0')
			return (key = event.key.keysym.unicode & 0x7F);
		else if (event.type == SDL_KEYUP)
			key = '\0';
	}
	return '\0';
}

void ui_init (void)
{
	printf ("Initializing SDL UI\n");

	if (SDL_Init (SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
		fprintf (stderr, "error: unable to initialize SDL\n");
		exit (1);
	}
	atexit (SDL_Quit);

	SDL_WM_SetCaption ("FreeWPC Simulation - " MACHINE_NAME, NULL);

	screen = SDL_SetVideoMode (WINDOW_WIDTH, WINDOW_HEIGHT, 32, SDL_SWSURFACE);
	if (screen == NULL) {
		fprintf (stderr, "error: unable to set video mode\n");
		exit (1);
	}

	SDL_EnableUNICODE (1);

	black = color_pixels[0] = SDL_MapRGB (screen->format, 0, 0, 0);
	off_color = SDL_MapRGB (screen->format, 64, 64, 64);
	color_pixels[1] = SDL_MapRGB (screen->format, 128, 128, 128);
	color_pixels[2] = SDL_MapRGB (screen->format, 192, 192, 192);
	on_color = color_pixels[3] = SDL_MapRGB (screen->format, 255, 255, 255);
}

void ui_refresh_all (void)
{
	int sol;

	if (!screen)
		return;
#if 0
	for (sol=0; sol < NUM_POWER_DRIVES; sol++)
	{
		if (sdl_sol_history[sol] > 64)
		{
			int x = sol / 8;
			int y = sol % 8;
			int intensity = --sdl_sol_history[sol];
			box (SOL_XPOS + x * BOX_DIM, SOL_YPOS + y * BOX_DIM, BOX_SIZE, BOX_SIZE,
				SDL_MapRGB (screen->format, intensity, intensity, intensity));
		}
	}
#endif
	SDL_UpdateRect (screen, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void ui_exit (void)
{
}

