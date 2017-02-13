#ifndef WheelEncoder_h
#define WheelEncoder_h

class WheelEncoder
{
  public:
    WheelEncoder( void );
	void count();
	void reset();
	int get();
	void direction(bool d);
  private:
    int encoderValue;
    boolean dir;
};

#endif
