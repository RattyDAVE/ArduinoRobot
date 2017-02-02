#include <SunFounderLineFollower.h>

SunFounderLineFollower sflf;
int *rawdata;
int byteprocessed;
int sensitivity;

void setup(){
  Serial.begin(230400);
}

void loop(){

  // davetest method
  sflf.davetest();
  
  
  //rawarry method 
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



   
  delay(1000);

  

}
