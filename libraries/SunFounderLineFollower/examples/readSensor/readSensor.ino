#include <SunFounderLineFollower.h>

SunFounderLineFollower sflf;
int *rawdata;
int byteprocessed;
int sensitivity;

void setup(){
  Serial.begin(230400);
}

void loop(){
	
  //Should be all Zeros	
  //sflf.calibrate_show();
  
  //Should have data
  //sflf.calibrate();  
  //sflf.calibrate_show();
    
  //Back to Zeros  
  //sflf.calibrate_reset();
  //sflf.calibrate_show();

  //Should have data.
  sflf.calibrate();
  sflf.calibrate_show();

  // Selftest of raw data.
  sflf.test();
  
  
  //rawarray method 
  rawdata=sflf.rawarray();

  Serial.print("Raw \t\t");
  for (int i=0; i <= 7; i++){
    Serial.print( *(rawdata +i) );
    Serial.print("\t"); 
  }
  Serial.println("");


  //byteprocessed method
  Serial.print("Byteprocessed \t");
  sensitivity=9000;
  byteprocessed=sflf.byteprocessed(sensitivity); 
  for(byte mask = 0x80; mask; mask >>= 1){
   if(mask & byteprocessed)
       Serial.print('1');
   else
       Serial.print('0');
  }
  Serial.println(""); 
  //if (byteprocessed = B00000000) NO LINE
  //else if (byteprocessed = B11111111) T STOP
  //else if (byteprocessed & B00000001) HARD RIGHT
  //else if (byteprocessed & B00000010) MED RIGHT
  //else if (byteprocessed & B00000100) SOFT RIGHT
  //else if (byteprocessed & B00001000) Centre
  //else if (byteprocessed & B00010000) Centre
  //else if (byteprocessed & B00100000) SOFT LEFT
  //else if (byteprocessed & B01000000) MED LEFT
  //else if (byteprocessed & B10000000) HARD LEFT



   
  delay(1000);

  

}
