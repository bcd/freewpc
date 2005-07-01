
#include <freewpc.h>

#define MAX_SWITCHES_PER_DEVICE 6

/** The device operations structure.  Each device can define callbacks
 * for various types of events/operations that are device-specific.
 * You can leave a field as NULL and it will not be called.
 * These are game-specific functions, as all of the basic stuff is done
 * by the common code here. */
typedef struct device_ops
{
	/* Called when the machine is powered up */
	void (*power_up) (void);

	/* Called whenever a game is started */
	void (*game_start) (void);

	/* Called whenever a ball enters the device */
	void (*enter) (void);

	/* Called whenever the game tries to kick a ball from the device */
	void (*kick_attempt) (void);

	/* Called when a kick is successful */
	void (*kick_success) (void);

	/* Called when a kick is not successful */
	void (*kick_failure) (void);

	/* Called when the device becomes full */
	void (*full) (void);

	/* Called when the device becomes empty */
	void (*empty) (void);
} device_ops_t;


/** The device structure is a read-only descriptor that
 * contains various device properties. */
typedef struct device
{
	device_ops_t ops;
	solnum_t sol;
	switchnum_t sw[MAX_SWITCHES_PER_DEVICE+1];
} device_t;


/*
 * Device states.  Each device is governed by a state
 * machine; these values dictate the various states that
 * a device can be in.
 */
#define DEV_STATE_IDLE	0


/** The device info structure.  This is a read-write
 * structure that maintains the current state of a device.
 * Included is the state machine state, as well as other
 * properties like how many balls are currently in the
 * device. */
typedef struct device_info
{
	uint8_t size;
	uint8_t actual_count;
	uint8_t desired_count;
	uint8_t state;
} device_info_t;

typedef uint8_t devicenum_t;

#define DEV_TROUGH 0
#define MAX_DEVICES 1

#define MAX_BALLS 6

static const device_t device_table[] = {
	[DEV_TROUGH] = {
		{ 
			NULL, 
		},
		SOL_BALL_SERVE,
		{ 
			SW_TROUGH1, SW_TROUGH2, SW_TROUGH3, 
			SW_TROUGH4, SW_TROUGH5, SW_TROUGH6 
		},
	},
};

static device_info_t device_info[MAX_DEVICES];

static uint8_t max_balls;
static uint8_t counted_balls;
static uint8_t missing_balls;
static uint8_t live_balls;

/* Return the number of balls currently present in the device */
int device_recount (devicenum_t devno)
{
	uint8_t count = 0;
	const device_t *dev = &device_table[devno];
	device_info_t *dev_info = &device_info[devno];

	dev_info->actual_count = count;

	return (count);
}

static int device_empty_p (devicenum_t devno)
{
	return (device_info[devno].actual_count == 0);
}


static int device_full_p (devicenum_t devno)
{
	return (device_info[devno].actual_count == device_info[devno].size);
}



/* Request that a device eject 1 ball */
void device_request_kick (devicenum_t devno)
{
}

/* Called from a switch handler to do the common processing */
void device_sw_handler (devicenum_t devno)
{
}

void device_init (void)
{
}


