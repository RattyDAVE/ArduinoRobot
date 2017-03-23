# SunFounder Line Follower Module.

## Methods
### void test();
Test of Line Followe module. This prints out the readings.

### int * rawarray();
Populates an array with the reading from the module and returns the pointer address.

### int byteprocessed(int);
Returns a byte with 8bits. Each with a bit set for line detection.
    
### void calibrate();
Calibrate the device. (reads the sensors 100 times and gets high and low.)

### void calibrate_show();
Show the calibration high and low.

### void calibrate_reset();
Clear the calibration data.

## Wiki page from Sunfounder
http://wiki.sunfounder.cc/index.php?title=Line_Follower_Module
