
#define MAX_ANIMATION_OBJECTS 8

/* Animation object flags */

#define ANO_FLASH 0x1


struct animation_object
{
	U8 x;
	U8 y;
	void (*draw) (struct animation_object *this);
	union {
		U8 u8;
		U16 u16;
		void *ptr;
	} data;
	U8 flags;
	U8 flash_period;
};


/* Animation flags */

#define AN_MONO 0x0
#define AN_DOUBLE 0x1
#define AN_CLEAN 0x2

struct animation
{
	U8 n_objects;
	U8 interframe_delay;
	struct animation_object *object[MAX_ANIMATION_OBJECTS];
	U8 position;
	U8 flags;
	U8 iteration;
};

struct animation *animation_begin (U8 flags);
void animation_set_speed (U8 interframe_delay);
struct animation_object *animation_add (
	U8 x, U8 y,
	void (*draw) (struct animation_object *));
void animation_object_flash (struct animation_object *obj, U8 period);
void animation_step (void);
void animation_run (void);
void animation_end (void);

