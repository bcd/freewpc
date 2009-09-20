
#ifndef _PROTOS_H
#define _PROTOS_H

__machine__ void fh_clock_advance (U8);
__machine__ void rudy_look_left (void);
__machine__ void rudy_look_straight (void);
__machine__ void rudy_look_right (void);
__machine__ void rudy_blink (void);
__common__ void collect_extra_ball (void);

__machine__ bool multiball_mode_running_p (void);


#endif
