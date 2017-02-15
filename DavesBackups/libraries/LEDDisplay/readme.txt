This Library encodes data to a multi digit 7 segment display by timer interrupt
so there is no need to service the display in your code. It does this in one of
a few possible modes:

 * Display integer in decimal onto the display
 * Display integer in as fixed point decimal
 * Display integer as hex onto display


Installation
--------------------------------------------------------------------------------

To install this library, just place this entire folder as a subfolder in your
Arduino/lib/targets/libraries folder.

When installed, this library should look like:

libraries/LEDDisplay                 (this library's folder)
libraries/LEDDisplay/LEDDisplay.cpp  (the library implementation file)
libraries/LEDDisplay/LEDDisplay.h    (the library description file)
libraries/LEDDisplay/keywords.txt    (the syntax coloring file)
libraries/LEDDisplay/examples        (the examples in the "open" menu)
libraries/LEDDisplay/readme.txt      (this file)

License: this library is provided under the Apache License,
