//
// State
//

/** The logical states of the zr1 engines driver state machine */
enum mech_zr1_state {
	/** The initial state */
	ZR1_INITIALIZE = 0,
	/** The engine should not be moving at all, solenoids powered off (attract mode) */
	ZR1_IDLE,
	/** The engine should be calibrated */
	ZR1_CALIBRATE,
	/** The engine should be at it's center position, solenoids powered on (game in progress) */
	ZR1_CENTER,
	/** The engine should be moving full left/full right with 1/2 second delays between movements to free stuck balls */
	ZR1_BALL_SEARCH,
	/** The engine should be shaking */
	ZR1_SHAKE
};

// CC = calibration code
enum mech_zr1_calibration_codes {
	CC_NOT_CALIBRATED = 0,
	CC_CHECK_F111,
	CC_CHECK_FULL_LEFT_OPTO,
	CC_CHECK_FULL_RIGHT_OPTO,
	CC_CHECK_ENGINE,
	CC_SUCCESS,
};

#define ZR1_SHAKE_SPEED_MIN 1
#define ZR1_SHAKE_SPEED_MAX 10
#define ZR1_SHAKE_SPEED_FASTEST ZR1_SHAKE_SPEED_MIN
#define ZR1_SHAKE_SPEED_SLOWEST ZR1_SHAKE_SPEED_MAX

#define ZR1_SHAKE_RANGE_MIN 1
#define ZR1_SHAKE_RANGE_MAX 5
#define ZR1_SHAKE_RANGE_LARGEST_ARC ZR1_SHAKE_RANGE_MAX
#define ZR1_SHAKE_RANGE_SMALLEST_ARC ZR1_SHAKE_RANGE_MIN
