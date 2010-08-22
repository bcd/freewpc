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
