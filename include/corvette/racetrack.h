
#define RACETRACK_LANES 2
#define LANE_LEFT 0
#define LANE_RIGHT 1

// EM = Encoder mask
#define RT_EM_SEEN_LEFT            (1 << 0)
#define RT_EM_PREVIOUS_STATE_LEFT  (1 << 1)
#define RT_EM_STALLED_LEFT         (1 << 2)
#define RT_EM_RESERVED_LEFT        (1 << 3)
#define RT_EM_SEEN_RIGHT           (1 << 4)
#define RT_EM_PREVIOUS_STATE_RIGHT (1 << 5)
#define RT_EM_STALLED_RIGHT        (1 << 6)
#define RT_EM_RESERVED_RIGHT       (1 << 7)

//
// State
//

/** The logical states of the racetrack driver's state machine */
enum mech_racetrack_state {
	/** The initial state */
	RACETRACK_INITIALIZE = 0,

	/** Calibration running */
	RACETRACK_CALIBRATE,

	/** Idle state, cars could be anywhere */
	RACETRACK_FLOAT,

	/** Cars at start of track, ready for race */
	RACETRACK_READY,

	/** Test mode */
	RACETRACK_CAR_TEST,

	/** Cars returning to start of track */
	RACETRACK_CAR_RETURN,

	/** Racing, cars moving to desired positions */
	RACETRACK_RACE
};

enum mech_racetrack_lane_state {
	/** Stop */
	LANE_STOP = 0,
	/** Seek desired position */
	LANE_SEEK,
	/** Return to start, stop when stalled or start switch activated */
	LANE_RETURN,
	/** Move forward and stop when stalled */
	LANE_CALIBRATE
};

typedef struct {
	// position are specified as a percentage, 0% being start 100% being the end.
	// percentages are used because they are easily used when drawing progress
	// meters, raising events, etc.
	U8 desired_car_position; // TODO implement
	U8 car_position;         // TODO implement

	// track lengths are specified in encoder ticks, only valid after calibration.
	U16 track_length;

	// counts the ON state changes of the encoder
	U16 encoder_count;

	// speed of the car, 1 = fastest (always on)
	U8 speed;
	U8 speed_ticks_remaining; // gets initialized to 'speed' (above)

	// id of the solenoid that enables the motor
	U8 solenoid;
	// id of lane start switch
	U8 start_switch;

	enum mech_racetrack_lane_state state;
} racetrack_lane_t;

//
// Calibration
//

// TODO add more calibration result codes

//CC = calibration code
enum mech_racetrack_calibration_codes {
	CC_NOT_CALIBRATED = 0,
	CC_CHECK_RACETRACK,
	CC_CHECK_LEFT_TRACK,
	CC_SUCCESS
};
