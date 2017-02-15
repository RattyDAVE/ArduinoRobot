// TIME ANIMATION FUNCTIONS

// basic_time()
// put the time in 4x7 font in the middle of the screen
// a dot moves around the time, one place/second
// can set time in the mode, and change 12/24 hour mode
void basic_time(){
  
  byte pend_x=0;
  byte pend_y=3;
  
  boolean power_up=true;

  ht1632_clear();

  do{ // exits based on button press (checked later in loop)

    time_now = RTC.now();

    /* UPDATE DISPLAY */
    if( ( time_now.second() != time_prev.second() ) || power_up ) {
      
      if( midnight_random() && !power_up )  return;

      // update pendulum cursor
      plot(pend_x, pend_y, 0);

      if(pend_y==0+2) { // top row
        if(pend_x<X_MAX)  pend_x++;
        else              pend_y++;
      }
      else if(pend_x==X_MAX) { // right column
        if(pend_y<Y_MAX-3)  pend_y++;
        else                pend_x--;
      }
      else if (pend_y==Y_MAX-3) { // bottom row
        if(pend_x>0)  pend_x--;
        else          pend_y--;
      }
      else if (pend_x==0) { // left column
        if(pend_y>0+2)   pend_y--;
        else             pend_x++;
      }

      plot(pend_x, pend_y, 1);

      // update time display if a minute has passed
      if( ( time_now.minute() != time_prev.minute() ) || power_up ){
        
        puttime_4x7(2,4,13,4,time_now.hour(),time_now.minute());
                                 
        if(DEBUG){
          Serial.print(time_now.hour(), DEC);
          Serial.print(':');
          Serial.print(time_now.minute(), DEC);
          Serial.print(':');
          Serial.print(time_now.second(), DEC);
          Serial.println();
        }
      }

      // change AM/PM indicator if needed
      if( ( time_now.hour() != time_prev.hour() ) || power_up ){
        if(twelve_hour){
          if(time_now.hour() < 12){
            plot(0,14,1);
            plot(23,14,0);
          }
          else{
            plot(0,14,0);
            plot(23,14,1);
          }
        }
        else{
          plot(0,14,0);
          plot(23,14,0);
        }
      }

      // update the previous time for comparison
      time_prev = time_now;
      power_up=false;

    } // end if now.sec != prev.sec

    /* CHECK BUTTONS (return to main loop if necessary) */

    if( change_animation() )  return;
    
    while( PAUSE && b5.isPressed( )); // pause mode for photos
    
    // set hours
    if(b1.isPressed()){
      if(b3.uniquePress()){
        if(time_now.hour()==0)  RTC.adjust( DateTime(time_now.year(), time_now.month(), time_now.day(), 23, time_now.minute(), 0 ) );
        else                    RTC.adjust( DateTime(time_now.year(), time_now.month(), time_now.day(), time_now.hour()-1, time_now.minute(), 0 ) );
        time_now=RTC.now();
        puttime_4x7(2,4,13,4,time_now.hour(),time_now.minute());
        time_prev = time_now;
      }
      if(b4.uniquePress()){
        if(time_now.hour()==23)  RTC.adjust( DateTime(time_now.year(), time_now.month(), time_now.day(), 0, time_now.minute(), 0 ) );
        else                     RTC.adjust( DateTime(time_now.year(), time_now.month(), time_now.day(), time_now.hour()+1, time_now.minute(), 0 ) );
        time_now=RTC.now();
        puttime_4x7(2,4,13,4,time_now.hour(),time_now.minute());
        time_prev = time_now;
      }
      if(twelve_hour){
        if(time_now.hour()<12){
          plot(0,14,1);
          plot(23,14,0);
        }
        else{
          plot(0,14,0);
          plot(23,14,1);
        }
      }
    }

    // set minutes
    if(b2.isPressed()){
      if(b3.uniquePress()){
        if(time_now.minute()==0)  RTC.adjust( DateTime(time_now.year(), time_now.month(), time_now.day(), time_now.hour(), 59, time_now.second() ) );
        else                      RTC.adjust( DateTime(time_now.year(), time_now.month(), time_now.day(), time_now.hour(), time_now.minute()-1, time_now.second() ) );
        time_now=RTC.now();
        puttime_4x7(2,4,13,4,time_now.hour(),time_now.minute());
        time_prev = time_now;
      }
      if(b4.uniquePress()){
        if(time_now.minute()==59)  RTC.adjust( DateTime(time_now.year(), time_now.month(), time_now.day(), time_now.hour(), 0, time_now.second() ) );
        else                       RTC.adjust( DateTime(time_now.year(), time_now.month(), time_now.day(), time_now.hour(), time_now.minute()+1, time_now.second() ) );
        time_now=RTC.now();
        puttime_4x7(2,4,13,4,time_now.hour(),time_now.minute());
        time_prev = time_now;
      }
    }
    
    // change 12/24 hour mode
    if( b1.isPressed() && b2.isPressed() ){
      if( b1.uniquePress() || b2.uniquePress() ){
        b1_time=millis()+1000; // set time to 1 second after initial press
      }
      else if(b1_time < millis()){ // toggle 12hour flag if buttons held down for more than a second, then update display
        if(twelve_hour){
          twelve_hour=false;
          plot(0,14,0);
          plot(23,14,0);
        } 
        else{
          twelve_hour=true;
          if(time_now.hour()<12){
            plot(0,14,1);
            plot(23,14,0);
          }
          else{
            plot(0,14,0);
            plot(23,14,1);
          }
        }
        b1_time=millis()+1000;
        time_now=RTC.now();
        puttime_4x7(2,4,13,4,time_now.hour(),time_now.minute());
        time_prev = time_now;
      }
    }
    
  }
  while(1);

} // end basic_time()

// big_time()
// put the time in 5x12 font in the middle of the screen
// can set time in the mode, and change 12/24 hour mode
void big_time(){

  int shift_del=66; // delay for digit update shifting
  boolean power_up=true;

  ht1632_clear();

  do{ // exits based on button press (checked later in loop)

    time_now = RTC.now();

    /* UPDATE DISPLAY */
    if( ( time_now.minute() != time_prev.minute() ) || power_up ) {
      
      if( midnight_random() && !power_up )  return;

      // update time display at power up
      if( power_up ){
        puttime_5x12(0,2,13,2,time_now.hour(),time_now.minute());
        //time_prev=time_now;
        //power_up=false;
      }
      
      // animated time update 
      if( ( time_now.minute() != time_prev.minute() ) && !power_up ){
        
        int hour_now=time_now.hour();
        int hour_prev=time_prev.hour();
        if(twelve_hour){
          if(hour_now>12)       hour_now-=12;
          else if(hour_now==0)  hour_now=12;
          if(hour_prev>12)       hour_prev-=12;
          else if(hour_prev==0)  hour_prev=12;
        }

        // all characters need to scroll
        if(hour_now/10 != hour_prev/10){ 
          if(DEBUG) Serial.println("CASE 1");
          for(int y=0; y<=Y_MAX+1; y++){
            // new characters coming in
            if(twelve_hour && hour_now==1){ // going from 12 to 1
              putchar_5x12(3,  y-Y_MAX+1, hour_now%10);
            }
            else{ // two digits coming in
              putchar_5x12(0,  y-Y_MAX+1, hour_now/10);
              putchar_5x12(6,  y-Y_MAX+1, hour_now%10);
            }
            putchar_5x12(13, y-Y_MAX+1, time_now.minute()/10);
            putchar_5x12(19, y-Y_MAX+1, time_now.minute()%10);
            // old characters going out
            if(twelve_hour && hour_now==10){ // going from 9 to 10
              putchar_5x12(3,  y+2, hour_prev%10);
            }
            else{ // two digits going out
              putchar_5x12(0,  y+2, hour_prev/10);
              putchar_5x12(6,  y+2, hour_prev%10);
            }
            putchar_5x12(13, y+2, time_prev.minute()/10);
            putchar_5x12(19, y+2, time_prev.minute()%10);
            // spaces above characters
            for(int x=0; x<=X_MAX; x++){ // space above characters
              plot(x,y-Y_MAX-0,0);
              plot(x,y-Y_MAX-1,0);
              plot(x,y,0);
              plot(x,y+1,0);
            }
            // fix the am/pm dot
            if(twelve_hour){
              if(time_now.hour() < 12){
                plot(0,15,1);
                plot(23,15,0);
              }
              else{
                plot(0,15,0);
                plot(23,15,1);
              }
            }            
            delay(shift_del);
          }
        }
        
        // hours ones and minutes need to scroll
        else if(hour_now%10 != hour_prev%10){ 
          if(DEBUG) Serial.println("CASE 2");
          for(int y=0; y<=Y_MAX+1; y++){
            // new characters coming in
            if(twelve_hour && hour_now<10){ // single digit hour
              putchar_5x12(3,  y-Y_MAX+1, hour_now%10);
            }
            else{ // hour ones digit
              putchar_5x12(6,  y-Y_MAX+1, hour_now%10);
            }
            putchar_5x12(13, y-Y_MAX+1, time_now.minute()/10);
            putchar_5x12(19, y-Y_MAX+1, time_now.minute()%10);
            // old characters going out
            if(twelve_hour && hour_now<10){
              putchar_5x12(3,  y+2, hour_prev%10);
            }
            else{
              putchar_5x12(6,  y+2, hour_prev%10);
            }
            putchar_5x12(13, y+2, time_prev.minute()/10);
            putchar_5x12(19, y+2, time_prev.minute()%10);
            // spaces above characters
            int x=6; // for one digit of hour
            if(twelve_hour && hour_now<10) x=3; // for single hour digit
            for(x; x<=X_MAX; x++){ // space above characters
              plot(x,y-Y_MAX-0,0);
              plot(x,y-Y_MAX-1,0);
              plot(x,y,0);
              plot(x,y+1,0);
            }
            // fix the am/pm dot
            if(twelve_hour){
              if(time_now.hour() < 12){
                plot(0,15,1);
                plot(23,15,0);
              }
              else{
                plot(0,15,0);
                plot(23,15,1);
              }
            }
            delay(shift_del);
          }
        }

        // both minute characters need to scroll
        else if(time_now.minute()/10 != time_prev.minute()/10){ 
          for(int y=0; y<=Y_MAX+1; y++){
            // new characters coming in
            putchar_5x12(13, y-Y_MAX+1, time_now.minute()/10);
            putchar_5x12(19, y-Y_MAX+1, time_now.minute()%10);
            // old characters going out
            putchar_5x12(13, y+2, time_prev.minute()/10);
            putchar_5x12(19, y+2, time_prev.minute()%10);
            // spaces above characters
            for(int x=13; x<=X_MAX; x++){ // space above characters
              plot(x,y-Y_MAX-0,0);
              plot(x,y-Y_MAX-1,0);
              plot(x,y,0);
              plot(x,y+1,0);
            }
            // fix the am/pm dot
            if(twelve_hour){
              if(time_now.hour() < 12){
                plot(0,15,1);
                plot(23,15,0);
              }
              else{
                plot(0,15,0);
                plot(23,15,1);
              }
            }
            delay(shift_del);
          }
        }
      
        // only minute ones needs update 
        else{
          for(int y=0; y<=Y_MAX+1; y++){
            // new character coming in
            putchar_5x12(19, y-Y_MAX+1, time_now.minute()%10);
            // old character going out 
            putchar_5x12(19, y+2, time_prev.minute()%10); 
            // space above characters
            for(int x=19; x<=X_MAX; x++){ 
              plot(x,y-Y_MAX-0,0);
              plot(x,y-Y_MAX-1,0);
              plot(x,y,0);
              plot(x,y+1,0);
            }
            // fix the am/pm dot
            if(twelve_hour){
              if(time_now.hour() < 12){
                plot(0,15,1);
                plot(23,15,0);
              }
              else{
                plot(0,15,0);
                plot(23,15,1);
              }
            }
            delay(shift_del);
          }
        }
                                 
        if(DEBUG){
          Serial.print(time_now.hour(), DEC);
          Serial.print(':');
          Serial.print(time_now.minute(), DEC);
          Serial.print(':');
          Serial.print(time_now.second(), DEC);
          Serial.println();
        }
      }

      // change AM/PM indicator if needed
      if( ( time_now.hour() != time_prev.hour() ) || power_up ){
        if(twelve_hour){
          if(time_now.hour() < 12){
            plot(0,15,1);
            plot(23,15,0);
          }
          else{
            plot(0,15,0);
            plot(23,15,1);
          }
        }
        else{
          plot(0,15,0);
          plot(23,15,0);
        }
      }

      // update the previous time for comparison
      time_prev = time_now;
      power_up=false;

    } // end if now.min != prev.min

    /* CHECK BUTTONS (return to main loop if necessary) */

    if( change_animation() )  return;
    
    while( PAUSE && b5.isPressed( )); // pause mode for photos
    
    // set hours
    if(b1.isPressed()){
      if(b3.uniquePress()){
        if(time_now.hour()==0)  RTC.adjust( DateTime(time_now.year(), time_now.month(), time_now.day(), 23, time_now.minute(), 0 ) );
        else                    RTC.adjust( DateTime(time_now.year(), time_now.month(), time_now.day(), time_now.hour()-1, time_now.minute(), 0 ) );
        time_now=RTC.now();
        puttime_5x12(0,2,13,2,time_now.hour(),time_now.minute());
        time_prev = time_now;
      }
      if(b4.uniquePress()){
        if(time_now.hour()==23)  RTC.adjust( DateTime(time_now.year(), time_now.month(), time_now.day(), 0, time_now.minute(), 0 ) );
        else                     RTC.adjust( DateTime(time_now.year(), time_now.month(), time_now.day(), time_now.hour()+1, time_now.minute(), 0 ) );
        time_now=RTC.now();
        puttime_5x12(0,2,13,2,time_now.hour(),time_now.minute());
        time_prev = time_now;
      }
      if(twelve_hour){
        if(time_now.hour()<12){
          plot(0,15,1);
          plot(23,15,0);
        }
        else{
          plot(0,15,0);
          plot(23,15,1);
        }
      }
    }

    // set minutes
    if(b2.isPressed()){
      if(b3.uniquePress()){
        if(time_now.minute()==0)  RTC.adjust( DateTime(time_now.year(), time_now.month(), time_now.day(), time_now.hour(), 59, time_now.second() ) );
        else                      RTC.adjust( DateTime(time_now.year(), time_now.month(), time_now.day(), time_now.hour(), time_now.minute()-1, time_now.second() ) );
        time_now=RTC.now();
        puttime_5x12(0,2,13,2,time_now.hour(),time_now.minute());
        time_prev = time_now;
      }
      if(b4.uniquePress()){
        if(time_now.minute()==59)  RTC.adjust( DateTime(time_now.year(), time_now.month(), time_now.day(), time_now.hour(), 0, time_now.second() ) );
        else                       RTC.adjust( DateTime(time_now.year(), time_now.month(), time_now.day(), time_now.hour(), time_now.minute()+1, time_now.second() ) );
        time_now=RTC.now();
        puttime_5x12(0,2,13,2,time_now.hour(),time_now.minute());
        time_prev = time_now;
      }
    }
    
    // change 12/24 hour mode
    if( b1.isPressed() && b2.isPressed() ){
      if( b1.uniquePress() || b2.uniquePress() ){
        b1_time=millis()+1000; // set time to 1 second after initial press
      }
      else if(b1_time < millis()){ // toggle 12hour flag if buttons held down for more than a second, then update display
        if(twelve_hour){
          twelve_hour=false;
          plot(0,15,0);
          plot(23,15,0);
        } 
        else{
          twelve_hour=true;
          if(time_now.hour()<12){
            plot(0,15,1);
            plot(23,15,0);
          }
          else{
            plot(0,15,0);
            plot(23,15,1);
          }
        }
        b1_time=millis()+1000;
        time_now=RTC.now();
        puttime_5x12(0,2,13,2,time_now.hour(),time_now.minute());
        time_prev = time_now;
      }
    }
    
  }
  while(1);

} // end big_time()

// basic_date_time
// show the full date and time in 3x5 font
// can change date in this mode
void basic_date_time(){
  
  boolean power_up=true;
  
  byte pend_x;
  byte pend_y;
  byte hour;

  // display init
  ht1632_clear();
  
  // plot the slashes and colons
  plot(7,3,1);
  plot(7,4,1);
  plot(7,5,1);
  plot(7,10,1);
  plot(7,12,1);
  plot(16,3,1);
  plot(16,4,1);
  plot(16,5,1);
  plot(16,10,1);
  plot(16,12,1);
  
  /* TIME LOOP */
  do{

    time_now=RTC.now();
    
    /* UPDATE DISPLAY */

    // check if a second has passed
    if( ( time_now.second() != time_prev.second() ) || power_up ) {
      
      if( !power_up && midnight_random() )  return;
      power_up=false;
      
      // date
      putchar_3x5(0,2,(time_now.year()%100)/10);
      putchar_3x5(3,2,time_now.year()%10);
      putchar_3x5(9,2,time_now.month()/10);
      putchar_3x5(12,2,time_now.month()%10);
      putchar_3x5(18,2,time_now.day()/10);
      putchar_3x5(21,2,time_now.day()%10);
      
      // time
      if(twelve_hour){
        hour=time_now.hour();
        if(hour>12)  hour-=12;
        if(hour==0)  hour=12;
        if(hour<10)    putchar_3x5(0,9,' ');
        else           putchar_3x5(0,9,hour/10);
        putchar_3x5(3,9,hour%10);
      }
      else{
        putchar_3x5(0,9,time_now.hour()/10);
        putchar_3x5(3,9,time_now.hour()%10);
      }
      putchar_3x5(9,9,time_now.minute()/10);
      putchar_3x5(12,9,time_now.minute()%10);
      putchar_3x5(18,9,time_now.second()/10);
      putchar_3x5(21,9,time_now.second()%10);
      
      time_prev = time_now;
                  
    } // end if prev_sec != now_sec
    
    /* CHECK BUTTONS (return from to main loop if necessary) */

    if( change_animation() )  return;
    
    while( PAUSE && b5.isPressed( )); // pause mode for photos

    // set year
    if(b1.isPressed() && b2.isPressed()){
      if(b3.uniquePress()){
        if(time_now.year()==2000)  RTC.adjust( DateTime(2099, time_now.month(), time_now.day(), time_now.hour(), time_now.minute(), time_now.second() ) );
        else                       RTC.adjust( DateTime(time_now.year()-1, time_now.month(), time_now.day(), time_now.hour(), time_now.minute(), time_now.second() ) );
        time_now=RTC.now();
        putchar_3x5(0,2,(time_now.year()%100)/10);
        putchar_3x5(3,2,time_now.year()%10);
        time_prev = time_now;
      }
      if(b4.uniquePress()){
        if(time_now.month()==2099)  RTC.adjust( DateTime(2000, time_now.month(), time_now.day(), time_now.hour(), time_now.minute(), time_now.second() ) );
        else                        RTC.adjust( DateTime(time_now.year()+1, time_now.month(), time_now.day(), time_now.hour(), time_now.minute(), time_now.second() ) );
        time_now=RTC.now();
        putchar_3x5(0,2,(time_now.year()%100)/10);
        putchar_3x5(3,2,time_now.year()%10);
        time_prev = time_now;
      }
    }
    
    // set month
    if(b1.isPressed()){
      if(b3.uniquePress()){
        if(time_now.month()==1)  RTC.adjust( DateTime(time_now.year(), 12, time_now.day(), time_now.hour(), time_now.minute(), time_now.second() ) );
        else                     RTC.adjust( DateTime(time_now.year(), time_now.month()-1, time_now.day(), time_now.hour(), time_now.minute(), time_now.second() ) );
        time_now=RTC.now();
        putchar_3x5(9,2,time_now.month()/10);
        putchar_3x5(12,2,time_now.month()%10);
        time_prev = time_now;
      }
      if(b4.uniquePress()){
        if(time_now.month()==12)  RTC.adjust( DateTime(time_now.year(), 1, time_now.day(), time_now.hour(), time_now.minute(), time_now.second() ) );
        else                      RTC.adjust( DateTime(time_now.year(), time_now.month()+1, time_now.day(), time_now.hour(), time_now.minute(), time_now.second() ) );
        time_now=RTC.now();
        putchar_3x5(9,2,time_now.month()/10);
        putchar_3x5(12,2,time_now.month()%10);
        time_prev = time_now;
      }
    }

    // set day
    if(b2.isPressed()){
      if(b3.uniquePress()){
        if(time_now.day()==1)  RTC.adjust( DateTime(time_now.year(), time_now.month(), 31, time_now.hour(), time_now.minute(), time_now.second() ) );
        else                   RTC.adjust( DateTime(time_now.year(), time_now.month(), time_now.day()-1, time_now.hour(), time_now.minute(), time_now.second() ) );
        time_now=RTC.now();
        putchar_3x5(18,2,time_now.day()/10);
        putchar_3x5(21,2,time_now.day()%10);
        time_prev = time_now;
      }
      if(b4.uniquePress()){
        if(time_now.day()==31)  RTC.adjust( DateTime(time_now.year(), time_now.month(), 1, time_now.hour(), time_now.minute(), time_now.second() ) );
        else                    RTC.adjust( DateTime(time_now.year(), time_now.month(), time_now.day()+1, time_now.hour(), time_now.minute(), time_now.second() ) );
        time_now=RTC.now();
        putchar_3x5(18,2,time_now.day()/10);
        putchar_3x5(21,2,time_now.day()%10);
        time_prev = time_now;
      }
    }
       
  }while(1); // end time loop
  
} // end fuction basic_date_time

// column_time()
// fill the screen up, one column/hour for 24 hours
// one dot = 4 minutes
// a single dot moves along the bottom (16th row) one spot / second
void column_time(){

  byte pend_x;
  byte pend_y;
  
  boolean power_up=true;
  
  ht1632_clear();
  
  // init the display
  time_now=RTC.now();
  time_prev=time_now;

  // hours
  for(int h=0; h<time_now.hour(); h++){
    for(int m=0; m<Y_MAX; m++) {
      plot(h,m,1);
     }
   }

  // minutes
  for(int m=0; m<time_now.minute()/4; m++){
    plot(time_now.hour(), Y_MAX-1-m, 1);
  }  

  // seconds
  pend_x=X_MAX;
  pend_y=Y_MAX;
  plot(pend_x, pend_y, 1);
  
  // time loop 
  do{
    
    time_now=RTC.now();
    
    /* UPDATE DISPLAY */

    // check if a second has passed
    if(time_now.second() != time_prev.second()) {

      // update pendulum cursor
      plot(pend_x, pend_y, 0);

      if(pend_x==X_MAX)  pend_x=0;
      else               pend_x++;

      // update the minutes display
      if(time_now.minute() != time_prev.minute()) {
        
        if( midnight_random() && !power_up )  return; 
        power_up=false;

        for(int m=0; m<time_now.minute()/4; m++){
          plot(time_now.hour(), Y_MAX-1-m, 1);
        }    

        // update the last minute light for the prev hour column
        if(time_now.hour() != time_prev.hour()){
          plot(time_prev.hour(), 0, 1);
        }

        // clear if midnight
        if(time_now.minute()==0 && time_now.hour()==0) {
          if( midnight_random() )  return;
          ht1632_clear();
        }
      }

      plot(pend_x, pend_y, 1);

      time_prev = time_now;

    } // end if time_now != time_prev
    
    /* CHECK BUTTONS */
    if( change_animation() )  return;
    
    while( PAUSE && b5.isPressed( )); // pause mode for photos
    
  } while(1); // end time loop
  
} // end function column_time

// float_time()
// shows the hours on the left side of the screen, the minutes on the right side
// each second the position of the hour and minute digit alternately move randomly up/down/left/right
void float_time(){

  int x_min;
  int y_min;
  int x_hour;
  int y_hour;
  
  boolean power_up=true;

  //init time
  ht1632_clear();

  x_hour=random(0,6);
  y_hour=random(0,12);
  x_min=random(12,18);
  y_min=random(0,12);

  time_now = RTC.now();
  time_prev = time_now;

  puttime_3x5(x_hour, y_hour, x_min, y_min, time_now.hour(), time_now.minute());

  // time loop
  do{

    time_now = RTC.now();

    // move one of the numebers each second, time updates automatically
    if( time_now.second()!=time_prev.second() ){

      if( midnight_random() && !power_up )  return;
      power_up=false;
      
      if(time_now.second()%2){ // move the hour every other second
        switch(random(0,4)){
        case 0: // left
          x_hour--;
          break;
        case 1: // right
          x_hour++;
          break;
        case 2: // up
          y_hour--;
          break;
        case 3: // down
          y_hour++;
          break;
        }
        // bound the hours
        if(x_hour<0)     x_hour=1;
        if(x_hour>5)     x_hour=4;
        if(y_hour<0)     y_hour=1;
        if(y_hour>11)    y_hour=10;        
      }
      else{ // move the minutes every other second
        switch(random(0,4)){
        case 0: // left
          x_min--;
          break;
        case 1: // right
          x_min++;
          break;
        case 2: // up
          y_min--;
          break;
        case 3: // down
          y_min++;
          break;
        }
        // bound the minutes
        if(x_min<12)     x_min=13;
        if(x_min>17)     x_min=16;
        if(y_min<0)      y_min=1;
        if(y_min>11)     y_min=10;
      }

      // place the time in the new place (automatically updates via call to time at top of loop)
      ht1632_clear();
      puttime_3x5(x_hour, y_hour, x_min, y_min, time_now.hour(), time_now.minute());
      
      time_prev=time_now;

    } // end if this sec != prev sec

    /* check buttons, return if necessary */
    if( change_animation() )  return;
    
    while( PAUSE && b5.isPressed( )); // pause mode for photos

  }
  while(1); // end time loop

} // end function float_time()

// seconds_time()
// show the seconds passed since midnight along the top of screen
// and the seconds remaining until midnight along the bottom of the screen
void seconds_time(){
  
  boolean power_up=true;
  
  //long seconds;
  long seconds_passed;
  long seconds_left;
  
  // init time
  ht1632_clear();

  /* TIME LOOP */  
  do{
    
    time_now = RTC.now();

    // update each second
    if( ( time_now.second()!=time_prev.second() ) || power_up ){

      if( !power_up && midnight_random() )  return;      
      power_up=false;
      
      seconds_passed = time_now.hour()*long(3600) + time_now.minute()*60 + time_now.second();
      seconds_left = 86400 - seconds_passed;

      putchar_4x7(0,0, seconds_passed/10000 + 48);
      putchar_4x7(5,0, (seconds_passed%10000)/1000 + 48);
      putchar_4x7(10,0, (seconds_passed%1000)/100 + 48);    
      putchar_4x7(15,0, (seconds_passed%100)/10 + 48);
      putchar_4x7(20,0, seconds_passed%10 + 48);

      putchar_4x7(0,9, seconds_left/10000 + 48);
      putchar_4x7(5,9, (seconds_left%10000)/1000 + 48);
      putchar_4x7(10,9, (seconds_left%1000)/100 + 48);
      putchar_4x7(15,9, (seconds_left%100)/10 + 48);  
      putchar_4x7(20,9, seconds_left%10 + 48);
      
      time_prev = time_now;
  
    } // end if time_now.second != time_prev.second

    /* check buttons, return if necessary */
    if( change_animation() )  return;
    
    // change screen brightness
    if(b1.isPressed() && b3.uniquePress()){
      if(brightness<4){
        brightness++;
        ht1632_sendcmd(ht1632_cs1, HT1632_CMD_PWM+bright_commands[brightness]);
      }
    } 
    if(b1.isPressed() && b4.uniquePress()){
      if(brightness>0){
        brightness--;
        ht1632_sendcmd(ht1632_cs1, HT1632_CMD_PWM+bright_commands[brightness]);
      }
    }

    while( PAUSE && b5.isPressed( )); // pause mode for photos
    
  }while(1);
  
} // end function seconds_time()

// percent_time()
// show the time as a percentage
// based on minutes passed since midnight
void percent_time(){
  
  boolean power_up=true;
  
  int minutes;
  int percent;
  int percent_10;
  int percent_1;
  int percent_tenth;
  int percent_hundredth;
  
  /* INIT DISPLAY */
  
  ht1632_clear();
  
  // display decimal
  plot(10,9,1);
  
  // display percent sign
  plot(20,5,1);  plot(22,5,1);
  plot(22,6,1);
  plot(21,7,1);
  plot(20,8,1);
  plot(20,9,1);  plot(22,9,1);
    
  /* TIME LOOP */
  do{
    
    time_now = RTC.now();
    
    if( ( time_now.minute() != time_prev.minute() ) || power_up ){
      
      if( !power_up && midnight_random() )  return;      
      power_up=false;
      
      minutes = time_now.hour()*60 + time_now.minute();
      percent = long(10000)*minutes/1440; // multiply by 10000 to avoid use of floats
      
      if(DEBUG)  Serial.println(minutes);
      if(DEBUG)  Serial.println(percent);

      percent_10 = percent/1000;    percent = percent%1000;
      percent_1 = percent/100;      percent = percent%100;
      percent_tenth = percent/10;
      percent_hundredth = percent%10;
  
      putchar_3x5(2,5,percent_10);
      putchar_3x5(6,5,percent_1);
      putchar_3x5(12,5,percent_tenth);
      putchar_3x5(16,5,percent_hundredth);
      
      //if( midnight_random() )  return;
      
      time_prev = time_now;

    } // end if time_now.min != time_prev.min

    /* CHECK BUTTONS, return if necessary */  
    if( change_animation() )  return;
    
    while( PAUSE && b5.isPressed( )); // pause mode for photos
            
  }while(1);

} // end function percent_time()


// random_dot_time() 
// screen fills up randomly one dot per 4 minutes
void random_dot_time(){
  
  // declare local variables
  
  boolean power_up=true; // used to force time display on function startup
  boolean done;
  int x;
  int y;
  int minutes;
  int minutes_to_go=0;
  
  /* INIT DISPLAY */
  
  clear_display();
  
  /* TIME LOOP */
  do{ // exit based on button presses, checked later in loop
  
    time_now = RTC.now();
    
    // generate display based on time passed so far
    if( power_up ){

      minutes = ( time_now.hour()*60 + time_now.minute() )/4;
      if(DEBUG) Serial.println(minutes);
      
      while(minutes_to_go < minutes){
        
        done=false;
        do{
          x=random(0,X_MAX+1);
          y=random(0,Y_MAX); // don't use 16th line of display
          if(display[x][y]==0){
            display[x][y]=1;
            plot(x,y,1);
            done=true;
          }
        } while(!done);
        minutes_to_go++;
      } // end while (minutes...)
      power_up=false;
      time_prev=time_now;
    } // end if(power_up);
            
    if( time_now.minute() != time_prev.minute() ){ // condition for update based on time change
    
      if( midnight_random() )  return;  // check if need to switch animation at midnight
      
      // animation stuff here  
      
      if(!(time_now.minute()%4)){
        
        if(time_now.minute()==0 && time_now.hour()==0)  clear_display();
        
        else{

          done=false;
          do{  
            x=random(0,X_MAX+1);
            y=random(0,Y_MAX);

            if(display[x][y]==0){
              display[x][y]=1;
              plot(x,y,1);
              done=true;
            }
          }while(!done);
        } // end if 4th minute
      } // end else
  
      // end of animation stuff    
      
      time_prev = time_now;

    } // end if time_now.min != time_prev.min

    /* CHECK BUTTONS, return if necessary */  
    if( change_animation() )  return;
    
    while( PAUSE && b5.isPressed( )); // pause mode for photos
            
  }while(1);
  
} // end function random_dot_time

// void pong_time()
// the score of an autonomous game of pong shows the time
// left paddle scores once an hour
// right paddle scores once a minute
void pong_time()
{

  boolean next_minute;
  boolean next_hour;

  unsigned long time_ball;
  unsigned long time_paddle;

  int ball_x_pos=X_MAX/2;
  int ball_y_pos=random(0,Y_MAX+1);
  int ball_x_dir=random(0,2);
  int ball_y_dir=random(0,2);
  int ball_delay=75;

  int paddle_height=4;
  int paddle_L=random(0,Y_MAX-paddle_height+1);
  int paddle_R=random(0,Y_MAX-paddle_height+1);
  int paddle_delay=ball_delay*0.75;

  byte score_L=0;
  byte score_R=0;

  ht1632_clear();

  // initialize the ball
  plot(ball_x_pos, ball_y_pos, 1);
  time_ball=millis()+ball_delay;
  time_paddle=millis()+paddle_delay;

  // initialize the paddles
  for(int i=0; i<paddle_height; i++) {
    plot(0, paddle_L+i, 1);
    plot(X_MAX, paddle_R+i, 1);
  }

  // initialize the score/time
  time_now = RTC.now();
  time_prev = time_now;
  puttime_3x5(4,0,13,0,time_now.hour(),time_now.minute());

  //time_prev = time_now;

  do{

    /* UPDATE BALL */
    if(millis()-time_ball >= ball_delay) {

      plot(ball_x_pos, ball_y_pos, 0);

      // paddle hits
      if(ball_x_pos==X_MAX-1 && ball_y_pos>=paddle_R && ball_y_pos<=paddle_R+paddle_height) {
        ball_x_pos--;
        ball_x_dir=0;
        ball_delay--;
        paddle_delay=ball_delay*0.75;
      }

      else if(ball_x_pos==1 && ball_y_pos>=paddle_L && ball_y_pos<=paddle_L+paddle_height) {
        ball_x_pos++;
        ball_x_dir=1;
        ball_delay--;
        paddle_delay=ball_delay*0.75;
      }

      // top corner cases, paddle hit and bounce
      else if(ball_y_pos==0 && ball_x_pos==1 && paddle_L==1) {
        ball_x_pos++;
        ball_y_pos++;
        ball_x_dir=1;
        ball_y_dir=1;
        ball_delay--;
        paddle_delay=ball_delay*0.75;
      }

      else if(ball_y_pos==0 && ball_x_pos==X_MAX-1 && paddle_R==1) {
        ball_x_pos--;
        ball_y_pos++;
        ball_x_dir=0;
        ball_y_dir=1;
        ball_delay--;
        paddle_delay=ball_delay*0.75;
      }

      // bottome corner cases, paddle hit and bounce
      else if(ball_y_pos==Y_MAX && ball_x_pos==1 && paddle_L+paddle_height==Y_MAX-1) {
        ball_x_pos++;
        ball_y_pos--;
        ball_x_dir=1;
        ball_y_dir=0;
        ball_delay--;
        paddle_delay=ball_delay*0.75;
      }

      else if(ball_y_pos==Y_MAX && ball_x_pos==X_MAX-1 && paddle_R+paddle_height==Y_MAX-1) {
        ball_x_pos--;
        ball_y_pos--;
        ball_x_dir=0;
        ball_y_dir=0;
        ball_delay--;
        paddle_delay=ball_delay*0.75;
      } 

      // if no paddle hits
      else {

        // update x pos
        if(ball_x_dir)        ball_x_pos++;
        else                  ball_x_pos--;

        // update y pos
        if(ball_y_dir)        ball_y_pos++;
        else                  ball_y_pos--;

        // check y bounces
        if(ball_y_pos<0)      { 
          ball_y_pos=1;        
          ball_y_dir=1; 
        }
        if(ball_y_pos>Y_MAX)  { 
          ball_y_pos=Y_MAX-1;  
          ball_y_dir=0; 
        }

        // check for scores (time changes)
        if(ball_x_pos<0) {
          //score_R++;
          //paddle_delay-=4;
          //ball_delay-=5;
          ball_x_pos=X_MAX/2;
          ball_y_pos=random(0,Y_MAX+1);
          ball_x_dir=random(0,2);
          ball_y_dir=random(0,2);
          puttime_3x5(4,0,13,0,time_now.hour(),time_now.minute());
          //time_prev=time_now;
          ball_delay=75;
          paddle_delay=ball_delay*0.75;
          //if(DEBUG) { Serial.print(int(score_L)); Serial.print(" "); Serial.println(int(score_R)); }
          delay(750);
          if( midnight_random() )  return;
          time_prev=time_now;
        }

        if(ball_x_pos>X_MAX) {
          //score_L++;
          //ball_delay-=5;
          //paddle_delay-=4;
          ball_x_pos=X_MAX/2;
          ball_y_pos=random(0,Y_MAX+1);
          ball_x_dir=random(0,2);
          ball_y_dir=random(0,2);
          //if(DEBUG) { Serial.print(int(score_L)); Serial.print(" "); Serial.println(int(score_R)); } 
          puttime_3x5(4,0,13,0,time_now.hour(),time_now.minute());         
          //time_prev=time_now;
          ball_delay=75;
          paddle_delay=ball_delay*0.75;
          delay(750);
          if( midnight_random() )  return;
          time_prev=time_now;
        }

        /* redraw the score if the ball is overlapping */
        if( (ball_x_pos>3 || ball_x_pos<20) && ball_y_pos<6 )  puttime_3x5(4,0,13,0,time_prev.hour(),time_prev.minute());

        plot(ball_x_pos, ball_y_pos, 1);

        if(ball_delay<25) ball_delay=random(25,36);
        time_ball=millis();

      } // end else

    } // end if(time_ball)

    /* UPDATE PADDLES */
    // update according to time changes

      if(millis()-time_paddle>=paddle_delay && ball_x_pos>0 && ball_x_pos<X_MAX) {

      time_now = RTC.now();

      if( time_now.minute()!=time_prev.minute() && time_now.hour()==time_prev.hour() )  next_minute=true;
      else                                                                              next_minute=false;

      if(time_now.hour()!=time_prev.hour())  next_hour=true;
      else                                   next_hour=false;

      //if(ball_x_dir) { // heading right
      if(ball_x_pos>(X_MAX+1)/3) {
        if(ball_y_pos>paddle_R+paddle_height-1 && paddle_R+paddle_height<=Y_MAX) {
          for(int i=0; i<paddle_height; i++) { 
            plot(X_MAX, paddle_R+i, 0); 
          }
          if(next_hour){
            if(paddle_R>0)  paddle_R--; // move out of the way if it's time to score an hour
          }
          else              paddle_R++;
          for(int i=0; i<paddle_height; i++) { 
            plot(X_MAX, paddle_R+i, 1); 
          }
        }
        else if(ball_y_pos<paddle_R+1 && paddle_R>0) {
          for(int i=0; i<paddle_height; i++) { 
            plot(X_MAX, paddle_R+i, 0); 
          }
          if(next_hour){
            if(paddle_R+paddle_height<Y_MAX)  paddle_R++;  // move out of the way if it's time to score an hour
          }
          else                                paddle_R--;
          for(int i=0; i<paddle_height; i++) { 
            plot(X_MAX, paddle_R+i, 1); 
          }
        }
      }

      //else { // heading left
      if(ball_x_pos<(X_MAX+1)*2/3) { 
        if(ball_y_pos>paddle_L+paddle_height-1 && paddle_L+paddle_height<=Y_MAX) {
          for(int i=0; i<paddle_height; i++) { 
            plot(0, paddle_L+i, 0); 
          }
          if(next_minute){
            if(paddle_L>0)  paddle_L--; // move out of the way if it's time to score an hour
          }
          else              paddle_L++;
          for(int i=0; i<paddle_height; i++) { 
            plot(0, paddle_L+i, 1); 
          }
        }
        else if(ball_y_pos<paddle_L+1 && paddle_L>0) {
          for(int i=0; i<paddle_height; i++) { 
            plot(0, paddle_L+i, 0); 
          }
          if(next_minute){
            if(paddle_L+paddle_height<Y_MAX)  paddle_L++; // move out of the way if it's time to score an hour
          }
          else                                paddle_L--;
          for(int i=0; i<paddle_height; i++) { 
            plot(0, paddle_L+i, 1); 
          }
        }
      }

      time_paddle=millis();

    } // end if(time_paddle)        

    //if( midnight_random() )  return;
    
    /* check buttons, return if necessary */
    if( change_animation() )  return; 
    
    while( PAUSE && b5.isPressed( )); // pause mode for photos

  }while(1); 

} // end function pong_time

// relative_column_time()
// date and time mapped to height of columns, can only approximate actual time from display
void relative_column_time(){

  // declare + initialize local variables
  
  time_now=RTC.now();
  time_prev=time_now;
  
  boolean power_up=true; // used to force time display on function startup
  
  byte sec_col_prev=map(time_now.second(), 0,59, 15,0);
  
  /* INIT DISPLAY */
  
  ht1632_clear();
  
  /* TIME LOOP */
  do{ // exit based on button presses, checked later in loop
    
    time_now = RTC.now();
    
    if( ( time_now.second() != time_prev.second() ) || power_up ){ // condition for update based on time change, may not need power_up check if previous if(power_up) routine used
    
      if( !power_up && midnight_random() )  return;  // check if need to switch animation at midnight

      // animation stuff here  
       
      if( power_up || sec_col_prev!=map(time_now.second(), 0,59, 15,0) ){
        
        ht1632_clear();
      
        for(int x=0; x<4; x++){
          plot(x, map(time_now.year()%10, 0,9, 15,0), 1);
        }
        for(int x=4; x<8; x++){
          plot(x, map(time_now.month(), 1,12, 15,0), 1);
        }
        for(int x=8; x<12; x++){
          plot(x, map(time_now.day(), 1,31, 15,0), 1);
        }
        for(int x=12; x<16; x++){
          plot(x, map(time_now.hour(), 0,23, 15,0), 1);
        }
        for(int x=16; x<20; x++){
          plot(x, map(time_now.minute(), 0,59, 15,0), 1);
        }
        for(int x=20; x<24; x++){
          plot(x, map(time_now.second(), 0,59, 15,0), 1);
        }
        
        sec_col_prev=map(time_now.second(), 0,59, 15,0);
        power_up=false; 
      }

      // end of animation stuff    
      
      time_prev = time_now;

    } // end if time_now.min != time_prev.min

    /* CHECK BUTTONS, return if necessary */  
    if( change_animation() )  return;
    
    while( PAUSE && b5.isPressed( )); // pause mode for photos
            
  }while(1);
  
} // end function relative_column_time

// analog_dot_time()
// displays the time like an analog clock
// a circle of dots marks the 12 hours
// a dot on the outside of the circle marks the minutes (like the long hand on a clock)
// and a dot on the inside marks hours (like the short hand on a clock)
// a dot in the middle indicates PM, no dot indicates AM
// only precise to 5 minute increments
void analog_dot_time(){

  // declare + initialize local variables
  byte mins[2][12] = { { 12, 16, 18, 19, 18, 16, 12, 8,  6,  5, 6, 8 },
                        { 1,  2,  4,  8,  12, 14, 15, 14, 12, 8, 4, 2 } };
                        
  byte hours[2][12] = { { 12, 14, 16, 17, 16, 14, 12, 10, 8,  7, 8, 10 },
                       { 3,  4,  6,  8,  10, 12, 13, 12, 10, 8, 6, 4  } };
                       
  byte hour;
                        
  boolean power_up=true; // used to force time display on function startup
  
  /* INIT DISPLAY */
  
  ht1632_clear();
  
  // draw the number dots
  //plot(12,8,1);
  plot(12,2,1);
  plot(15,3,1);
  plot(17,5,1);
  plot(18,8,1);
  plot(17,11,1);
  plot(15,13,1);
  plot(12,14,1);
  plot(9,13,1);
  plot(7,11,1);
  plot(6,8,1);
  plot(7,5,1);
  plot(9,3,1);
  
  /* TIME LOOP */
  do{ // exit based on button presses, checked later in loop
    
    time_now = RTC.now();
    
    if( ( time_now.minute() != time_prev.minute() ) || power_up ){ // condition for update based on time change
    
      if( !power_up && midnight_random() )  return;  // check if need to switch animation at midnight

      // animation stuff here  
      
      if(time_now.minute()<4){
        plot( mins[X][11], mins[Y][11], 0 ); // erase the previous minute setting
        
        plot( mins[X][0],  mins[Y][0],  1 ); // plot the new minutes setting
      }
      else{      
        plot( mins[X][time_now.minute()/5-1], mins[Y][time_now.minute()/5-1], 0); // erase the previous minute setting
        plot( mins[X][time_now.minute()/5],   mins[Y][time_now.minute()/5],   1); // plot the new minutes setting
      }
      
      if( ( time_now.hour() != time_prev.hour() ) || power_up ){
        
        hour = time_now.hour();
        if( hour >= 12 ){
          hour-=12;
          plot(12,8,1); // plot the middle dot for PM
        }
        else
        {
          plot(12,8,0); // no middle dot for AM
        }
        
        if(hour==0){
          plot( hours[X][11], hours[Y][11], 0); // erase the previous hour setting
          plot( hours[X][0],  hours[Y][0],  1); // plot the previous hour setting
        }
        else{
          plot( hours[X][hour-1], hours[Y][hour-1], 0); // erase the previous hour setting
          plot( hours[X][hour],   hours[Y][hour],   1); // plot the previous hour setting
        }
      } // end if new hour
            
      // end of animation stuff    
      
      power_up=false;  // power_up is only true once
      time_prev = time_now;

    } // end if time_now.min != time_prev.min

    /* CHECK BUTTONS, return if necessary */  
    if( change_animation() )  return;
    
    while( PAUSE && b5.isPressed( )); // pause mode for photos
            
  }while(1);
  
} // end function analog_dot_time()


// game_time()
// each minute, pac man, space invaders, or tetris erase the time
// the new time fades back in
void game_time(){

  // declare + initialize local variables
  
  //boolean power_up=true; // used to force time display on function startup
  
  byte display_status[24][16];
  
  int updated=0;
  int all=(X_MAX+1)*(Y_MAX+1);
  int x;
  int y;
  
  int enemy;
  int falls;
  
  /* INIT DISPLAY */
 
  ht1632_clear();  
  time_now = RTC.now();
  time_prev = time_now;
  puttime_4x7(1,4,13,4,time_now.hour(),time_now.minute());
  plot(11,5,1); plot(11,9,1); // place the colon

  /* TIME LOOP */
  do{ // exit based on button presses, checked later in loop
    
    time_now = RTC.now();
    
    if( time_now.minute() != time_prev.minute() ){ // condition for update based on time change
    
      // animation stuff here  

      // erase the current screen w/ a space invader, tetris, or pacman (or nothing)
      
      //if(!power_up){
        
        //enemy=random(0,4);
        enemy=random(0,6);
        //enemy=2;
        
        switch(enemy){
          
          // only summon an enemy to erease half of the time
          // ie ignore 0-2
          
          //case 0:
            // do nothing
            //break;
          
          case 3:
            invader();
            break;
          
          case 4:
            falls=random(6,11);
            for(int i=0; i<falls; i++){
              tetris_fall(random(40,70));
            }
            break;
          
          case 5:
            pacman(random(45,65));
            break;
        } // end switch(enemy)

        delay(1000);
      //} // end if(!power_up)
      
      // clear the old frame
      for(x=0; x<=X_MAX; x++) {
        for(y=0; y<=Y_MAX; y++) {
          display[x][y]=0;
          display_status[x][y]=0;
         }
      }
      
      if( midnight_random() )  return;  // check if need to switch animation at midnight
      //power_up=false;  // power_up is only true once

      // create the new frame    
      time_now=RTC.now();
      buftime_4x7(1,4,13,4,time_now.hour(),time_now.minute());
      display[11][5]=1; display[11][9]=1; // place the colon
                  
      // update the screen w/ the new frame, ranomly dot by dot
      updated=0;
      while(updated < all){
      
          x=random(0,X_MAX+1);
          y=random(0,Y_MAX+1);
          
          if(!display_status[x][y]){
            if(display[x][y])  plot(x, y, 1);
            else               plot(x, y, 0);
            display_status[x][y]=1;
            updated++;
            delay(10);
          }
      }
      
      // end of animation stuff    
      
      time_prev = time_now;

    } // end if time_now.min != time_prev.min

    /* CHECK BUTTONS, return if necessary */  
    if( change_animation() )  return;
            
  }while(1);
  
} // end function game_time

// show the time as three scrolling rows of morse code
// one each for hours, minutes, and seconds
void morse_time(){

  // declare + initialize local variables
  
  char zero[] = "1110111011101110111";
  char one[] = "10111011101110111";
  char two[] = "101011101110111";
  char three[] = "1010101110111";
  char four[] = "10101010111";
  char five[] = "101010101";
  char six[] = "11101010101";
  char seven[] = "1110111010101";
  char eight[] = "111011101110101";
  char nine[] = "11101110111011101";
  
  char* digit[]={ zero, one, two, three, four, five, six, seven, eight, nine };

  byte hours_frame[41];
  byte mins_frame[41];
  byte secs_frame[41];
  
  int hours_x=-41;
  int mins_x=-41;
  int secs_x=-41;
  
  int hours_y;
  int mins_y;
  int secs_y;
  
  byte hour_del=125;
  byte min_del=125;
  byte sec_del=125;
  
  unsigned long hour_prev=0;
  unsigned long min_prev=0;
  unsigned long sec_prev=0;
  
  byte pos1;
  byte pos2;
  
  boolean exit_hour=false;
  boolean exit_min=false;
  boolean exit_sec=false;
  
  /* INIT DISPLAY */
  
  ht1632_clear();
  
  /* TIME LOOP */
  do{ // exit based on button presses, checked later in loop
    
    // HOURS
    if(millis()-hour_prev > hour_del){
      
      // create new frame if needed
      if(hours_x<=-41){
        
        time_now=RTC.now();
        if ( midnight_random() )  exit_hour=true;
        time_prev=time_now;
        
        hours_x=24;
        pos1=0;
        pos2=0;
        for(int i=0; i<41; i++)  hours_frame[i]=0;
        hours_y=random(1,5);
        time_now=RTC.now();
        
        while( *(digit[time_now.hour()/10]+pos1) != '\0' ){
          if( *(digit[time_now.hour()/10]+pos1) == '1' )  hours_frame[pos1]=1;
          pos1++;
        }
        pos1+=3;
        while( *(digit[time_now.hour()%10]+pos2) != '\0' ){
          if( *(digit[time_now.hour()%10]+pos2) == '1' )  hours_frame[pos1]=1;
          pos1++;
          pos2++;
        }
        
        if(random(0,2))  hour_del*=1.05;
        else             hour_del*=0.95;
        hour_del=constrain(hour_del, 50, 200);

      } // end if hours_x<=-41
      
      if(!exit_hour){
        // erase the frame from the current position
        for(int i=0; i<41; i++){
          if(hours_frame[i])  plot(hours_x+i, hours_y, 0);
        }
      
        // redraw the frame one spot to the left
        hours_x--;
        for(int i=0; i<41; i++){
          if(hours_frame[i])  plot(hours_x+i, hours_y, 1);
        }
      }
      
      hour_prev=millis();
      
    } // end if hour_prev > hour_del

    // MINUTES
    if(millis()-min_prev > min_del){
      
      // create new frame if needed
      if(mins_x<=-41){
        
        if(exit_hour || exit_sec)  exit_min=true;
        
        mins_x=24;
        pos1=0;
        pos2=0;
        for(int i=0; i<41; i++)  mins_frame[i]=0;
        mins_y=random(6,10);
        time_now=RTC.now();
        
        while( *(digit[time_now.minute()/10]+pos1) != '\0' ){
          if( *(digit[time_now.minute()/10]+pos1) == '1' )  mins_frame[pos1]=1;
          pos1++;
        }
        pos1+=3;
        while( *(digit[time_now.minute()%10]+pos2) != '\0' ){
          if( *(digit[time_now.minute()%10]+pos2) == '1' )  mins_frame[pos1]=1;
          pos1++;
          pos2++;
        }
        
        if(random(0,2))  min_del*=1.05;
        else             min_del*=0.95;
        min_del=constrain(min_del, 50, 200);
                
      } // end if mins_x<=-41
      
      if(!exit_min){
        // erase the frame from the current position
        for(int i=0; i<41; i++){
          if(mins_frame[i])  plot(mins_x+i, mins_y, 0);
        }
      
        // redraw the frame one spot to the left
        mins_x--;
        for(int i=0; i<41; i++){
          if(mins_frame[i])  plot(mins_x+i, mins_y, 1);
        }
      }
      
      min_prev=millis();
      
    } // end if min_prev > min_del

    // SECONDS
    if(millis()-sec_prev > sec_del){
      
      // create new frame if needed
      if(secs_x<=-41){
        
        if(exit_hour || exit_min)  exit_sec=true;
        
        secs_x=24;
        pos1=0;
        pos2=0;
        for(int i=0; i<41; i++)  secs_frame[i]=0;
        secs_y=random(11,15);
        time_now=RTC.now();
        
        while( *(digit[time_now.second()/10]+pos1) != '\0' ){
          if( *(digit[time_now.second()/10]+pos1) == '1' )  secs_frame[pos1]=1;
          pos1++;
        }
        pos1+=3;
        while( *(digit[time_now.second()%10]+pos2) != '\0' ){
          if( *(digit[time_now.second()%10]+pos2) == '1' )  secs_frame[pos1]=1;
          pos1++;
          pos2++;
        }
        
        if(random(0,2))  sec_del*=1.05;
        else             sec_del*=0.95;
        sec_del=constrain(sec_del, 50, 200);
                
      } // end if secs_x<=-41
      
      if(!exit_sec){
        // erase the frame from the current position
        for(int i=0; i<41; i++){
          if(secs_frame[i])  plot(secs_x+i, secs_y, 0);
        }
      
        // redraw the frame one spot to the left
        secs_x--;
        for(int i=0; i<41; i++){
          if(secs_frame[i])  plot(secs_x+i, secs_y, 1);
        }
      }
      
      sec_prev=millis();
      
    } // end if sec_prev > sec_del
              
    /* CHECK BUTTONS, return if necessary */  
    if( change_animation() )  return;
    
    // return if midnight_random and all rows are off screen
    if(exit_hour && exit_min && exit_sec)  return;
    
    while( PAUSE && b5.isPressed( )); // pause mode for photos
            
  }while(1);
  
} // end function morse_time


// binary_year_time
// show the number of seconds passed in the current year as a 5x5 grid
void binary_year_time(){
  
  // declare + initialize local variables
  
  int nrml[12]={0,31,59,90,120,151,181,212,243,273,304,334};
  int leap[12]={0,31,60,91,121,152,182,213,244,274,305,335};
  
  int coords_x[5]={5,8,11,14,17};
  int coords_y[5]={1,4,7,10,13};
  
  int dot;
  //int x;
  //int y;
  
  boolean leap_year;
  
  unsigned long seconds;
  //unsigned long power;
  
  boolean power_up=true; // used to force time display on function startup
  
  /* INIT DISPLAY ETC. */
  
  ht1632_clear();
  
  /* TIME LOOP */
  do{ // exit based on button presses, checked later in loop
    
    time_now = RTC.now();
      
    if( ( time_now.second() != time_prev.second() ) || power_up ){ // condition for update based on time change, may not need power_up check if previous if(power_up) routine used
    
      if( !power_up && midnight_random() )  return;  // check if need to switch animation at midnight
      power_up=false;  // power_up is only true once

      // animation stuff here  
  
      // calculate if leap year
      if(!time_now.year()%400)       leap_year=true;
      else if(!time_now.year()%400)  leap_year=false;
      else if(!time_now.year()%4)    leap_year=true;
      else                           leap_year=false;
      
      if(leap_year)  seconds = ( (leap[time_now.month()-1] + time_now.day() - 1)*86400 + time_now.hour()*3600 + time_now.minute()*60 + time_now.second() );
      else           seconds = ( (nrml[time_now.month()-1] + time_now.day() - 1)*86400 + time_now.hour()*3600 + time_now.minute()*60 + time_now.second() );
      
      if(DEBUG)  Serial.println(seconds,BIN);
      
      for(int i=0; i<25; i++){
        
        unsigned long seconds_temp = seconds;
        int result = seconds_temp>>i & 1;

        if(result)  dot=1;
        else        dot=0;  
        
        plot(coords_x[i%5],   coords_y[i/5],   dot);
        plot(coords_x[i%5]+1, coords_y[i/5],   dot);
        plot(coords_x[i%5],   coords_y[i/5]+1, dot);
        plot(coords_x[i%5]+1, coords_y[i/5]+1, dot);
      }

      // end of animation stuff    
      
      time_prev = time_now;

    } // end if time_now.sec != time_prev.sec

    /* CHECK BUTTONS, return if necessary */  
    if( change_animation() )  return;
    
    while( PAUSE && b5.isPressed( )); // pause mode for photos
            
  }while(1);
  
} // end function binary_year_time

// accumulator_time
// time accumulates in rectangular blocks, each representing 1 hour (5 mins = 1 dot of a block)
// arranged like a square clock face
void accumulator_time(){

  // declare + initialize local variables
  
  byte origins[2][12] = { { 13,18,18,18,18,13,7,2,2,2,2,7 },
                          { 0,0,4,8,12,12,12,12,8,4,0,0 } };
  byte offsets[2][12] = { { 0,0,0,1,1,1,2,2,2,3,3,3 },
                          { 0,1,2,0,1,2,0,1,2,0,1,2 } };
                          
  byte hour;
  byte minute;
  
  boolean power_up=true; // used to force time display on function startup
  
  /* INIT DISPLAY */
  
  ht1632_clear();
  
  /* TIME LOOP */
  do{ // exit based on button presses, checked later in loop
    
    time_now = RTC.now();

    hour = time_now.hour();
    if(hour>=12)  hour-=12;

    if( power_up ){ // generate any display stuff if needed, this is optional depending on the animation
    
      // display initialisation based on existing time here
      
      minute = time_now.minute()/5;
      
      // plot completed hours
      for(int h=0; h<hour; h++){
        for(int m=0; m<12; m++){
          plot(origins[X][h]+offsets[X][m], origins[Y][h]+offsets[Y][m], 1);
        }
      }
      
      // plot minutes
      for(int m=0; m<minute; m++){
        plot(origins[X][hour]+offsets[X][m], origins[Y][hour]+offsets[Y][m], 1);
      }
   
      power_up = false;
      time_prev = time_now;
      
    } // end if(power_up)
    
    if( time_now.minute() != time_prev.minute() ){ // condition for update based on time change
    
      if( midnight_random() )  return;  // check if need to switch animation at midnight

      // animation stuff here  
      
      if( time_now.hour() != time_prev.hour() ){ // new hour
      
        minute=0;
      
        if( hour == 0){ // if midnight or 12:00, clear screen
          ht1632_clear();  
        }
        else{ // plot the last minute dot
          plot(origins[X][hour-1]+offsets[X][11], origins[Y][hour-1]+offsets[Y][11], 1);
        }
      }
      else{ // same hour, new minute
       
        if( time_now.minute()%5 == 0 ){ // next five minute increment
          plot(origins[X][hour]+offsets[X][minute], origins[Y][hour]+offsets[Y][minute], 1);
          minute++;
        }
      }
  
      // end of animation stuff    
      
      time_prev = time_now;

    } // end if time_now.min != time_prev.min

    /* CHECK BUTTONS, return if necessary */  
    if( change_animation() )  return;
            
  }while(1);
  
} // end function accumulator_time

// void scroll_time()
// scrolls the time in text, using scroll_text function
void scroll_time(){

  // declare + initialize local variables
    
  char *ones[] = { "oh",
                   "one",
                   "two",
                   "three",
                   "four",
                   "five",
                   "six",
                   "seven",
                   "eight",
                   "nine",
                   "ten",
                   "eleven",
                   "twelve",
                   "thirteen",
                   "fourteen",
                   "fifteen",
                   "sixteen",
                   "seventeen",
                   "eighteen",
                   "nineteen",  
                   "o'clock", // 20
                   "a quarter", // 21
                   "half", // 22
                   "past" , // 23
                   "to" }; // 24
                   
  char *tens[] = {  "",
                    "",
                    "twenty",
                    "thirty",
                    "forty",
                    "fifty",
                    "noon", // 6
                    "midnight" }; // 7
                    
  String time_string=String();
  
  boolean power_up=true; // used to force time display on function startup
  
  int this_animation=animation;
  
  int mode;
  
  /* INIT DISPLAY */
  
  ht1632_clear();
  
  /* TIME LOOP */
  do{ // exit based on button presses, checked later in loop
    
    time_now = RTC.now();
    
    if( !power_up && midnight_random() )  return;  // check if need to switch animation at midnight
    power_up=false;  // power_up is only true once
    time_prev=time_now;
    
    // animation stuff here  
    
    mode=random(1,4);
    //if(time_now.minute()>30 && mode==2){ // force to mode 1 if the mode 2 sentence would be weird
      if( !(time_now.minute()%10==0 || time_now.minute()%10==5) )  mode=1;
    //}  
      
    char string[30];
    int pos=0;
    string[pos++]='i';
    string[pos++]='t';
    string[pos++]='\'';
    string[pos++]='s';
    string[pos++]=' ';
    
    int hour=time_now.hour();
    if(hour>12)       hour-=12;
    else if(hour==0)  hour=12;
    
    if(mode==1){ // first mode: IT'S ELEVEN FIFTY SEVEN AM
      
      int p=0;
      while(ones[hour][p]!='\0'){
        string[pos++]=ones[hour][p++];
      }
      string[pos++]=' ';

      //p=0;
      int minute=time_now.minute();
      if(minute==0){
        // do nothing
        /*
        p=0;
        while(ones[20][p]!='\0'){ // get "o'clock"
          string[pos++]=ones[20][p++];
        } */
        pos--; // adjust for space before am/pm
      }
      else if(minute<10){
        p=0;
        while(ones[0][p]!='\0'){ // get "oh"
          string[pos++]=ones[0][p++];
        }
        string[pos++]=' ';
        p=0;
        while(ones[minute][p]!='\0'){ // get ones
          string[pos++]=ones[minute][p++];
        }
      }
      else if(minute<20){
        p=0;
        while(ones[minute][p]!='\0'){ // get "oh"
          string[pos++]=ones[minute][p++];
        }
      }
      else{
        p=0;
        while(tens[minute/10][p]!='\0'){ // get tens
          string[pos++]=tens[minute/10][p++];
        }
        if(minute%10>0){
          string[pos++]=' ';
          p=0;
          while(ones[minute%10][p]!='\0'){ // get ones
            string[pos++]=ones[minute%10][p++];
          }
        }
      }
      
      string[pos++]=' ';
      if(time_now.hour()<12)  string[pos++]='a';
      else                    string[pos++]='p';
      string[pos++]='m';
      string[pos++]='.';
      
      string[pos]='\0';
    } // end mode==1
    
    else if(mode>1){ // mode 2: IT'S THIRTEEN PAST TEN.
    
      int p=0;
      int minute=time_now.minute();
      
      if(minute==0){ // IT'S something O'CLOCK.
      
        if(time_now.hour()==0){  //  IT'S MIDNIGHT
          while(tens[7][p]!='\0'){
            string[pos++]=tens[7][p++]; // get "midnight"
          }
        }
        else if(time_now.hour()==12){ // IT's NOON
          while(tens[6][p]!='\0'){
            string[pos++]=tens[6][p++]; // get "noon"
          }
        }
        
        else{ // IT'S something O'CLOCK.
           
          while(ones[hour][p]!='\0'){
            string[pos++]=ones[hour][p++];
          }
          string[pos++]=' ';
          p=0;
          while(ones[20][p]!='\0'){ // get "o'clock"
            string[pos++]=ones[20][p++];
          }
        }
      }

      else if(minute<=30){ // IT'S something PAST something.
        
        if(minute==15){
          while(ones[21][p]!='\0'){ // get "quarter"
            string[pos++]=ones[21][p++];
          }
        }
        else if(minute==30){
          while(ones[22][p]!='\0'){ // get "half"
            string[pos++]=ones[22][p++];
          }
        }
        else if(minute<20){
          //p=0;
          while(ones[minute][p]!='\0'){ 
            string[pos++]=ones[minute][p++];
          }
        }
        else{
        //p=0;
          while(tens[minute/10][p]!='\0'){ // get tens
            string[pos++]=tens[minute/10][p++];
          }
          if(minute%10>0){
            string[pos++]=' ';
            p=0;
            while(ones[minute%10][p]!='\0'){ // get ones
              string[pos++]=ones[minute%10][p++];
            }
          }
        }

        string[pos++]=' ';
        
        p=0;
        while(ones[23][p]!='\0'){ // get "past"
          string[pos++]=ones[23][p++];
        }
        
        string[pos++]=' ';  
        
        p=0;
        if(time_now.hour()==0){ // after midnight
          //p=0;
          while(tens[7][p]!='\0'){
            string[pos++]=tens[7][p++];
          }
        }
        else if(time_now.hour()==12){ // after noon
          //p=0;
          while(tens[6][p]!='\0'){
            string[pos++]=tens[6][p++];
          }
        }
        else{
          while(ones[hour][p]!='\0'){
            string[pos++]=ones[hour][p++];
          }
        }
 
        //string[pos++]='.';
       
      } // end if minute<=30
      
      else{ // IT'S something TO something
        
        minute = 60-minute;

        if(minute==15){
          while(ones[21][p]!='\0'){ // get "quarter"
            string[pos++]=ones[21][p++];
          }
        }
        else if(minute<20){
          //p=0;
          while(ones[minute][p]!='\0'){ 
            string[pos++]=ones[minute][p++];
          }
        }
        else{
        //p=0;
          while(tens[minute/10][p]!='\0'){ // get tens
            string[pos++]=tens[minute/10][p++];
          }
          if(minute%10>0){
            string[pos++]=' ';
            p=0;
            while(ones[minute%10][p]!='\0'){ // get ones
              string[pos++]=ones[minute%10][p++];
            }
          }
        }

        string[pos++]=' ';
        
        p=0;
        while(ones[24][p]!='\0'){ // get "to"
          string[pos++]=ones[24][p++];
        }
        
        string[pos++]=' ';
        p=0; 
        
        if(time_now.hour()==23){ // coming up to midnight
          //p=0;
          while(tens[7][p]!='\0'){
            string[pos++]=tens[7][p++];
          }
        }
        else if(time_now.hour()==11){ // coming up to noon
          //p=0;
          while(tens[6][p]!='\0'){
            string[pos++]=tens[6][p++];
          }
        }
        else{
          int next_hour;
          if(hour==12)  next_hour=1;
          else          next_hour=hour+1;
          while(ones[next_hour][p]!='\0'){
            string[pos++]=ones[next_hour][p++];
          }
        }
          
      } // end else (min>=31);
        
      string[pos++]='.';
      string[pos]='\0';  
    } // end mode==2
      
    scroll_text(string);
    if(animation != this_animation) return; // animation was changed in scroll_text()
    
    // end of animation stuff    
      
    //time_now=RTC.now(); // reset time so there is a 1 second gap before next time scroll, allows for animation change
    //time_prev = time_now;
            
  }while(1);
  
} // end scroll_time

// template for new time functions
void time_template(){

  // declare + initialize local variables
  
  boolean power_up=true; // used to force time display on function startup
  
  /* INIT DISPLAY */
  
  ht1632_clear();
  
  /* TIME LOOP */
  do{ // exit based on button presses, checked later in loop
    
    time_now = RTC.now();
    
    if( power_up ){ // generate any display stuff if needed, this is optional depending on the animation
    
      // display initialisation based on existing time here
      
      power_up = false;
    }
    
    if( ( time_now.minute() != time_prev.minute() ) || power_up ){ // condition for update based on time change, may not need power_up check if previous if(power_up) routine used
    
      if( !power_up && midnight_random() )  return;  // check if need to switch animation at midnight
      power_up=false;  // power_up is only true once

      // animation stuff here  
  
      // end of animation stuff    
      
      time_prev = time_now;

    } // end if time_now.min != time_prev.min

    /* CHECK BUTTONS, return if necessary */  
    if( change_animation() )  return;
            
  }while(1);
  
} // end function time_template

