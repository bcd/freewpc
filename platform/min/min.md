
include platform/generic.md

#switches-numbered: integer
#lamps-numbered: integer
#drives-numbered: integer
#gi-numbered: integer

define MACHINE_CUSTOM_AMODE

# The system needs a minimum of 4 switches to compile: a
# start button, two flipper buttons, and a trough switch.
# A 5th scoring target is needed in order to cycle through a
# game - otherwise valid playfield can never be set.

[switches]
D1: Start Button, start-button, debounce(TIME_100MS)
D2: Left Button, cabinet
D3: Right Button, cabinet
D4: Trough, noscore
D5: Target, ingame

[lamps]
11: Start Button, start
12: Shoot Again, shoot-again, ball-save
13: Target
14: GI

[drives]
H1: Ball Release
H2: Left Flipper
H3: Right Flipper

# The ball save requires a lamplist in order to denote ball
# save active as a lamp effect.

[lamplists]
Ball Save: Shoot Again
Playfield: Shoot Again, Target

# There must be a trough device

[containers]
Trough: Ball Release, trough, init_max_count(1), Trough

[leffs]
Amode: runner, PRI_AMODE, c_decl(amode_leff)

