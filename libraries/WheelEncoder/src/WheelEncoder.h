#ifndef WheelEncoder_h
#define WheelEncoder_h

class WheelEncoder
{
  public:
    WheelEncoder(int pin);
	void count();
	void reset();
	int get();
	void direction(bool d);
  private:
    int  encoderValue;
    bool direction;
};

#endif
