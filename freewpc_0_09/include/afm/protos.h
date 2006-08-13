/* afm/config.c */
void afm_init(void);
void unused_init(void);
void unused_start_game(void);
void unused_end_game(void);
void unused_start_ball(void);
void afm_start_game(void);
void afm_end_game(void);
void afm_start_ball(void);
void afm_ball_in_play(void);
bool afm_end_ball(void);
void afm_add_player(void);
void afm_any_pf_switch(void);
void afm_bonus(void);
void afm_tilt(void);
void afm_tilt_warning(void);
void afm_start_without_credits(void);
void afm_coin_added(void);
/* afm/lefthole.c */
void lefthole_kick_sound(void);
void lefthole_enter(device_t *dev);
void lefthole_kick_attempt(device_t *dev);
void lefthole_init(void);
/* afm/righthole.c */
void sw_righthole_handler(void);
void righthole_kick_sound(void);
void righthole_enter(device_t *dev);
void righthole_kick_attempt(device_t *dev);
void righthole_init(void);
