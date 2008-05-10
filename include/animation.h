
#define MAX_ANIMATION_OBJECTS 8

/* Animation object flags */

#define ANO_FLASH 0x1


/** Each object within an animation has an instance of
 * the object to track its state/configuration. */
struct animation_object
{
	U8 x;
	U8 y;
	void (*draw) (struct animation_object *this);

	/** Private data used by the draw function for any
	 * purpose.  There are 16-bits of data, which can be
	 * used as an integer or a pointer. */
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

/** Set when each frame is in 4 colors rather than mono */
#define AN_DOUBLE 0x1

/** Set if each frame should be cleaned before calling the
 * draw function */
#define AN_CLEAN 0x2

/** Set when the animation should stop running. */
#define AN_STOP 0x4

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


extern inline struct animation_object *animation_add_static (
	void (*draw) (struct animation_object *))
{
	return animation_add (0, 0, draw);
}

