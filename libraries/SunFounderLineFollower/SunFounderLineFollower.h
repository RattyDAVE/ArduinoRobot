#ifndef SunFounderLineFollower_h
#define SunFounderLineFollower_h

#ifndef SENSOR_SENSITIVITY
#define SENSOR_SENSITIVITY 127
#endif

class SunFounderLineFollower
{
  public:
    SunFounderLineFollower();
    int readsensor();
    int * getreadings();
  private:
    int _linSensor;
    unsigned char _data[16];
};

#endif
