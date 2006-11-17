/* wcs/config.c */
void wcs_init(void);
void unused_init(void);
void unused_start_game(void);
void unused_end_game(void);
void unused_start_ball(void);
void wcs_start_game(void);
void wcs_end_game(void);
void wcs_start_ball(void);
void wcs_ball_in_play(void);
bool wcs_end_ball(void);
void wcs_add_player(void);
void wcs_any_pf_switch(void);
void wcs_bonus(void);
void wcs_tilt(void);
void wcs_tilt_warning(void);
void wcs_start_without_credits(void);
void wcs_coin_added(void);
/* wcs/goal_popper.c */
void goal_popper_kick_sound(void);
void goal_popper_enter(device_t *dev);
void goal_popper_kick_attempt(device_t *dev);
void goal_popper_init(void);
/* wcs/tv_popper.c */
void tv_popper_kick_sound(void);
void tv_popper_enter(device_t *dev);
void tv_popper_kick_attempt(device_t *dev);
void tv_popper_init(void);
