
#ifndef _COIN_H
#define _COIN_H

void credits_draw (void);
void credit_added_deff (void);
void add_credit (void);
bool has_credits_p (void);
void remove_credit (void);
void coin_deff (void) __taskentry__;
void sw_left_coin (void) __taskentry__;
void sw_center_coin (void) __taskentry__;
void sw_right_coin (void) __taskentry__;
void sw_fourth_coin (void) __taskentry__;
void coin_init (void);

#endif /* _COIN_H */
