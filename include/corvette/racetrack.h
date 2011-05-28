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

	/** Cars returning to start of track */
	RACETRACK_CAR_RETURN,

	/** Racing, cars moving to desired positions */
	RACETRACK_RACE
};

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
