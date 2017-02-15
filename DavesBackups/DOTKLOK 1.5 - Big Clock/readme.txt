DOTKLOK rev 1.5
Andrew O'Malley, 15 Dec. 2011
aomalley_AT_gmail_DOT_com
www.aomalley.org/dotklok

ANIMATIONS INCLUDED (described in dotklok_rev_1_5.pde, *=new or updated in this revision):

big_time
basic_date_time
game_time
pong_time
float_time
seconds_time
percent_time
column_time
random_dot_time
analog_dot_time
binary_year_time
scroll_time*

CHANGES:

- removed relative_column_time, morse_time, and accumulator_time to make room for scroll_time (and fonts.h)

TO IMPROVE:

- make generic function for characters/fonts (putchar_3x5, putchar_4x7, etc.)
- make brightness adjustment work with just B1 and B2 (currently only seems to work correctly with B1 combined with B3/B4)

KNOWN ISSUES:

- minor, occasional button bounces
- DEBUG mode now crashes in game_time (not really an issue for the casual user but could affect developers), probably a memory error due to invoking the serial library

INSTALLATION AND UPLOADING NOTES:

Tested in Arduino IDE 0022, available from www.arduino.cc

Place the "dotklok_rev_1_5" folder in the Arduino sketches folder (in Windows 7 this is "Libraries/Documents/Arduino", in MAC OS X Lion this is "Documents/Arduino").

Place the folders in the "libraries" folder ("Button", "ht1632", and "RTClib") into the Arduino user libraries folder (in Windows 7 this is "Libraries/Documents/Arduino/libraries", in MAC OS X Lion this is "Documents/Arduino/libraries").

Open the Arduino IDE.

Open "dotklok_rev_1_5.pde"

Make sure the "SURE_NEW" variable is set to 0 if your DOTKLOK has an old screen (chips and switch on front / LED side), or set to 1 if you have a new screen (chips and switch on back).

Make sure the "screen_color" variable in the ht1632 tab is set to color of your DOTKLOK (RED or GREEN) if you have an old screen (chips and switch on front / LED side); the color setting doesn't matter for new screens (chips and switch on back).

Connect DOTKLOK to your computer according to the DOTKLOK usage instructions at www.aomalley.org/dotklok

Select Tools -> Board -> Arduino Duemilanove or Nano w/ ATmega328

Select Tools -> Serial Port -> [ applicable port ]

Select File -> Upload to I/O Board


PREVIOUS REVISIONS


DOTKLOK rev 1.4
Andrew O'Malley, 15 Dec. 2011
aomalley_AT_gmail_DOT_com
www.aomalley.org/dotklok

ANIMATIONS INCLUDED (described in dotklok_rev_1_4.pde, *=new or updated in this revision):

big_time*
basic_date_time
game_time
pong_time
float_time
seconds_time
percent_time
column_time
random_dot_time
relative_column_time
morse_time
analog_dot_time
accumulator_time
binary_year_time

CHANGES:

- updated big_time to have scrolling digits when time updates
- added roll-over to animation selection (finally)

TO IMPROVE:

- make generic function for characters/fonts (putchar_3x5, putchar_4x7, etc.)
- make brightness adjustment work with just B1 and B2 (currently only seems to work correctly with B1 combined with B3/B4)

KNOWN ISSUES:

- minor, occasional button bounces


DOTKLOK rev 1.3
Andrew O'Malley, 26 April 2011
aomalley_AT_gmail_DOT_com
www.aomalley.org/dotklok

ANIMATIONS INCLUDED (described in dotklok_rev_1_3.pde, *=new to this revision):

big_time*
basic_date_time
game_time
pong_time
float_time
seconds_time
percent_time
column_time
random_dot_time
relative_column_time
morse_time
analog_dot_time
accumulator_time
binary_year_time

CHANGES:

- replaced basic_time with big_time (same functionality, just a bigger font to make it more distinct from game_time)
- added brightness control (accessed in seconds_time, hold B1 and use B3/B4 to adjust)

TO IMPROVE:

- make generic function for characters/fonts (putchar_3x5, putchar_4x7, etc.)
- make brightness adjustment work with just B1 and B2 (currently only seems to work correctly with B1 combined with B3/B4)

KNOWN ISSUES:

- minor, occassional button bounces


DOTKLOK rev 1.2
Andrew O'Malley, 16 March 2011
aomalley_AT_gmail_DOT_com
www.aomalley.org/dotklok

ANIMATIONS INCLUDED (described in dotklok_rev_1_2.pde, *=new to this revision):

basic_time
basic_date_time
game_time
pong_time
float_time
seconds_time
percent_time
column_time
random_dot_time
relative_column_time
morse_time
analog_dot_time
accumulator_time*
binary_year_time*

CHANGES:

- updated ht1632_setup and plot functions to work w/ new Sure displays w/ HT1632C drivers
- changed game_time so that an enemy only appears 50% of time, w/ fading update other 50%
- cleaned up digit calculation in seconds_time
- fixed handling of midnight_random in morse_time for proper behaviour

TO IMPROVE:

- make generic function for characters/fonts (putchar_3x5, putchar_4x7, etc.)

KNOWN ISSUES:

- minor, occassional button bounces


DOTKLOK rev 1.1
Andrew O'Malley, 13 February 2011
aomalley_AT_gmail_DOT_com
www.aomalley.org/dotklok

ANIMATIONS INCLUDED (described in dotklok_rev_1_1.pde, *=new to this revision):

basic_time
basic_date_time
game_time*
pong_time
float_time
seconds_time
percent_time
column_time
random_dot_time
relative_column_time
morse_time*
analog_dot_time*

CHANGES:

- added game_time, morse_time, and analog_dot_time animations
- updated midnight_random to eliminate false triggers w/ pong_time
- changed where pong_time checks midnight_random for smoother transition at midnight
- added software version display to screen on power up (for 1 sec)
- reorganised tabs (in Arduino IDE)

TO IMPROVE:

- make generic function for characters/fonts (putchar_3x5, putchar_4x7, etc.)

KNOWN ISSUES:

- minor, occassional button bounces


DOTKLOK rev 1.0
Andrew O'Malley, 1 January 2011
aomalley_AT_gmail_DOT_com
www.aomalley.org/dotklok

ANIMATIONS INCLUDED (described in dotklok_rev_1_0.pde):

basic_time
basic_date_time
pong_time
float_time
seconds_time
percent_time
column_time
random_dot_time
relative_column_time

KNOWN ISSUES:

- minor, occassional button bounces
- when midnight_random triggers pong_time, pong_time immediately retriggers midnight_random and picks a new animation
