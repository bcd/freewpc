/* model this after wcs/whatever_driver.c */

/*
 * Copyright 2017 by Noah Luskey <LuskeyNoah@gmail.com>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

 /* this file is how one should interact with the cannon. It will (hopefully)
  * prevent balls from being fired at the wrong time or wrong thing
  *
  * See the cannon state machine diagram drawing in machines/t2/notes
  * for a clearer picture of what's going on here (if you're curious)
  */


// TODO: add an api call to start the cannon sequence. It shouldn't automatically start
//       the cannon state machine sequence just when the ball enters the gun.
 #include <freewpc.h>
 #include <gun_motor.h>
 #include <t2/cannon_driver.h>

typedef enum
{
    HOME_STATIONARY=0,
    CANNON_READY,
    HOME_MOVING,
    ON_MARK,
    FIRE_READY,
    FORGOTTEN,
    FIRED,
    GET_HOME,       // like a demand "GET HOME!" (and empty)
    FINDING_HOME    // just gets the cannon home
} cannon_state_t;

cannon_state_t cannon_state;
bool fireable;

bool cannon_check_valid_state_transition(const cannon_state_t from, const cannon_state_t to);
void cannon_fire(void);

void cannon_error_handler(void)
{
    //TODO: something bad happened
    // print a debug statement or possibly try to fix it
}

void cannon_motor_on(void)
{
    gun_motor_start_forward();
}

void cannon_motor_off(void)
{
    gun_motor_stop();
}

void cannon_enable_firing(void)
{
    fireable = true;
}

void cannon_disable_firing(void)
{
    fireable = false;
}

void cannon_set_ball_count(U8 count)
{
    if (count == 1) {
        device_entry(DEVNO_GUN)->max_count = 1;
    } else if (count == 0) {
        device_request_empty(device_entry(DEVNO_GUN));
    } else {
        // TODO: invalid argument, print some debug statement
    }
}

//
// Force set the cannon_state
// This really shouldn't ever be called by the user. Just me.
//
void cannon_force_state(const cannon_state_t state)
{
    switch (state) {
    case HOME_STATIONARY:
    case CANNON_READY:
        cannon_motor_off();
        cannon_disable_firing();
        cannon_set_ball_count(1);
        break;
    case HOME_MOVING:
    case ON_MARK:
        cannon_motor_on();
        cannon_disable_firing();
        cannon_set_ball_count(1);
        break;
    case FIRE_READY:
        cannon_motor_on();
        cannon_enable_firing();
        cannon_set_ball_count(1);
        break;
    case FORGOTTEN:
        cannon_motor_on();
        cannon_enable_firing();
        cannon_fire();
        break;
    case FIRED:
        cannon_motor_on();
        cannon_disable_firing();
        cannon_set_ball_count(1);
        break;
    case GET_HOME:
        cannon_motor_off();
        cannon_disable_firing();
        cannon_set_ball_count(1);
        break;
    case FINDING_HOME:
        cannon_motor_on();
        cannon_disable_firing();
        cannon_set_ball_count(1);
        break;
    }
    cannon_state = state;
}

//
// set the state safely - will only allow valid transitions
// if the requested transitions is illegal, nothing happens
//
void cannon_set_state(const cannon_state_t state) {
    if (cannon_check_valid_state_transition(cannon_state, state)) {
        cannon_force_state(state);
    } else {
        // TODO: print a debug message
    }
}

//
// Triggers for state transitions
// Here, we have the various callset entries that will set the state of the cannon
// State may also be set manually (externally) by the API functions at the bottom of this file
//

//callset entry for home switch
CALLSET_ENTRY(cannon, SW_GUN_HOME)
{
    switch (cannon_state) {
    case FIRED:
        cannon_set_state(HOME_STATIONARY);
        break;
    case FINDING_HOME:
        cannon_set_state(GET_HOME);
        if (cannon_is_loaded()) {
            cannon_set_state(HOME_MOVING);
        } else {
            cannon_set_state(HOME_STATIONARY);
        }
        break;
    default:
        cannon_error_handler();
    }
}

// callset entry for mark switch
// The mark switch is triggered on low-high and high-low edges, so we need to
// determine what the exact condition is before we do anything else
void gun_mark_on(void)
{
    switch (cannon_state) {
    case HOME_MOVING:
        cannon_set_state(ON_MARK);
        break;
    default:
        cannon_error_handler();
    }
}

void gun_mark_off(void)
{
    switch (cannon_state) {
    case ON_MARK:
        cannon_set_state(FIRE_READY);
        break;
    case FIRE_READY:
        cannon_set_state(FORGOTTEN);
        break;
    default:
        cannon_error_handler();
    }
}


CALLSET_ENTRY(cannon, SW_GUN_MARK)
{
    score(SC_100K);
    if (switch_poll_logical(SW_GUN_MARK)) {
        gun_mark_on();
    } else {
        gun_mark_off();
    }
    
}

// event for when the gun contains a ball
CALLSET_ENTRY(cannon, DEV_GUN_ENTER)
{
    switch (cannon_state) {
    case HOME_STATIONARY:
        cannon_set_state(CANNON_READY);
        break;
    default:
        cannon_error_handler();
    }
}

// event for when the ball is shot from the cannon
CALLSET_ENTRY(cannon, DEV_GUN_KICK_SUCCESS)
{
    switch (cannon_state) {
        case FIRE_READY:
        case FORGOTTEN:
            cannon_set_state(FIRED);
            break;
        default:
            cannon_error_handler();
    }
}

// TODO: callset_invoke for cannon events
//          TODO: cannon loaded
//          TODO: cannon fired - when the user intentionally shoots the ball
//          TODO: cannon fireable - when the cannon has a ball and is past the mark
//          TODO: cannon forgotten - when the user forgot to pull the trigger




//
// return true if the state transition is valid
//
bool cannon_check_valid_state_transition(const cannon_state_t from, const cannon_state_t to)
{
    // not changing state is always valid
    if (from == to) {
        return true;
    }

    // jumping to the GET_HOME state is always valid
    if (to == GET_HOME) {
        return true;
    }

    // here's the real meat and potatoes. See machines/t2/notes
    switch (from) {
    case HOME_STATIONARY:
        return (to == CANNON_READY);
    case CANNON_READY:
        return (to == HOME_MOVING);
    case HOME_MOVING:
        return (to == ON_MARK);
    case ON_MARK:
        return (to == FIRE_READY);
    case FIRE_READY:
        return (to == FORGOTTEN) || (to == FIRED);
    case FORGOTTEN:
        return (to == FIRED);
    case FIRED:
        return (to == HOME_STATIONARY);
    case GET_HOME:
        return (to == FINDING_HOME) || (to == HOME_MOVING) || (to == HOME_STATIONARY);
    case FINDING_HOME:
        return (to == GET_HOME);
    default:
        return false;
        // TODO: print some debug that this should NEVER happen
    }
}


//
// Cannon API functions: Use these functions to interact with the cannon
// from outside of this file (in game code). These are the only functions that are
// defined the the cannon_driver.h file
// A typical use would be:
// TODO: write typical use
//

// Reset cannon : Get rid of any balls and place the cannon at home positions
void cannon_reset(void)
{
    fireable = false;
    cannon_set_state(GET_HOME);
}

// Cannon fire : Shoot the ball (if possible) or do nothing
void cannon_fire(void)
{
    if (fireable) {
        cannon_set_ball_count(0);
    }
}

// Cannon start sequence : Start the sequence of swinging the cannon out with a ball
void cannon_start_sequence(void)
{
    cannon_set_state(HOME_MOVING);
}

// Cannon is loaded : Check if the cannon contains a ball
bool cannon_is_loaded(void)
{
    return (device_full_p(device_entry(DEVNO_GUN)));
}


// At the start/end of the ball, make sure the cannon is empty and home
CALLSET_ENTRY(cannon, start_ball, end_ball)
{
    cannon_reset();
}

// attempt to fire the cannon when the trigger is pulled, always
// TODO: this should be left up to the individual modes, not here
CALLSET_ENTRY(cannon_test, SW_LAUNCH_BUTTON)
{
    cannon_fire();
}

CALLSET_ENTRY(cannon_test, DEV_GUN_ENTER)
{
    cannon_start_sequence();
}