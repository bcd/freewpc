
include platform/generic.md

#switches-numbered: integer
#lamps-numbered: integer
#drives-numbered: integer
#gi-numbered: integer

[switches]
D1: Start Button, start-button, debounce(TIME_100MS)
D2: Left Flipper, cabinet
D3: Right Flipper, cabinet
D4: Trough, noscore
D5: Target, ingame

[lamps]
11: Start Button, start-button
12: Shoot Again, shoot-again, ball-save
13: Target
14: GI

[drives]
H1: Ball Release
H2: Left Flipper
H3: Right Flipper

[lamplists]
Ball Save: Shoot Again

[containers]
Trough: Ball Release, trough, init_max_count(1), Trough
