#ifndef SunFounderLineFollower_h
#define SunFounderLineFollower_h

class SunFounderLineFollower
{
  public:
    SunFounderLineFollower();
    void test();
    int * rawarray();
    int byteprocessed(int);
    
    void calibrate();
    void calibrate_show();
    void calibrate_reset();
    
  private:
    int  cal_min[8];
    int  cal_max[8];
};

#endif
