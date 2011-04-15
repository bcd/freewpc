
callset_invoke (left_orbit_shot);
callset_invoke (captive_ball_shot);
callset_invoke (left_ramp_shot);
callset_invoke (right_ramp_shot);

/* Right orbit shot not awarded on falloff from the bumpers */
callset_invoke (right_orbit_shot);

/* Detect center loops ... ignore the second switch */
callset_invoke (loop_shot);
callset_invoke (left_loop_shot);
callset_invoke (right_loop_shot);

/* Detect a direct spinner shot when it occurs before either
	of the center loop switches */
callset_invoke (spinner_shot);

/* Detect front trunk hole by trunk eddy then hole */
callset_invoke (trunk_hole_shot);

/* Detect rear trunk hole by center loop switch then hole */

/* Detect trunk hit = trunk eddy debounced when trunk is in
	closed position */
callset_invoke (trunk_wall_shot);

callset_invoke (trunk_magnet_shot);

/* Trunk hole shot disabled Basement kickout score.  Also
	whenever Basement is thought to be closed */
callset_invoke (basement_shot);

