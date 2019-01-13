 #include <stdbool.h>

// Cannon reset : places the cannon back home and empty
// This is automatically called at the beginning and end of a ball
void cannon_reset(void);

// Cannon fire : Shoot the ball (if possible) or do nothing
void cannon_fire(void);

// Cannon is loaded : Check if the cannon contains a ball
bool cannon_is_loaded(void);

// Cannon start sequence : Start the sequence of swinging the cannon out with a ball
void cannon_start_sequence(void);