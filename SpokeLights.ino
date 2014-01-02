#include <Logging.h>
#include <FrequencyTimer2.h>

const int FIRST_PIN = 2;
const int SECOND_PIN = 3;
const int THIRD_PIN = 4;
const int FOURTH_PIN = 5;
const long BRIGHT_THRESHHOLD = 800;

volatile int status[4] = {1, 0, 0, 0};

long lastTrigger = 0;
long millisPerRevolution = 0;

void setup(){
   Log.Init(LOG_LEVEL_DEBUG, 9600);
   pinMode(FREQUENCYTIMER2_PIN, OUTPUT);

   pinMode(FIRST_PIN, OUTPUT);
   pinMode(SECOND_PIN, OUTPUT);
   pinMode(THIRD_PIN, OUTPUT);
   pinMode(FOURTH_PIN, OUTPUT);
 
   FrequencyTimer2::setPeriod(5000); //5 milliseconds
   FrequencyTimer2::setOnOverflow(MoveLights);
   FrequencyTimer2::enable();
}

void MoveLights(){
   //Write the status of the lights
   //At this point we know how many milliseconds it takes us to get around
   // The loop (calculated when the photo cell sees a change) and 
   // how long it's been since we saw one.  
   if(lastTrigger != 0 && millisPerRevolution != 0){ // Don't screw around if we don't know what we're doing
      long curMillis = millis(); //This isn't actually going to change since this timer was invoked,
                                 // don't care, it'll do what we want.

      long elapsed = curMillis - lastTrigger;
      
      float per = elapsed / millisPerRevolution; //Can probably brute force this as int if speed is a problem
      
      if(per < .25){
         first_status = 1;
         second_status = 0;
         third_status 
      } else if( per < .5){
         
   } 
   digitalWrite(FIRST_PIN, first_status);
   digitalWrite(SECOND_PIN, second_status);
   digitalWrite(THIRD_PIN, third_status);
   digitalWrite(FOURTH_PIN, fourth_status);

}

void loop(){
   //Need to read the photo cell and come up with an RPM measurement.  
   long pinState = analogRead(A0);
   if(pinState > BRIGHT_THRESHHOLD){
      Log.Info("Found a ping with %l"CR, pinState);
      //This is one RPM.  Based on this we know that the FIRST_PIN should be high now.
      //Going to reset them all to where they should be.  This is ugly, but it will
      // help me see if I'm going down the wrong path here.
      first_status = 1;
      second_status = 0;
      third_status = 0;
      fourth_status = 0;

      lastTrigger = millis();
   }

   //This is really slow, but I'm going to be leaving the thing open for so long that I don't
   // want to make it slower and double count;
   delay(500);
}

