#ifndef SunFounderLineFollower_h
#define SunFounderLineFollower_h

#ifndef SENSOR_SENSITIVITY
#define SENSOR_SENSITIVITY 1024
#endif

class SunFounderLineFollower
{
  public:
    SunFounderLineFollower();
    void davetest();
    int * rawarray();
    int byteprocessed(int);
  private:
};

#endif
