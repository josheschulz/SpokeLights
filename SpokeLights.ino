#include <Logging.h>
#include <FrequencyTimer2.h>

const int FIRST_PIN = 2;
const int SECOND_PIN = 3;
const int THIRD_PIN = 4;
const int FOURTH_PIN = 5;
const long BRIGHT_THRESHHOLD = 800;

const int SMOOTHING_LENGTH = 5;
long revolutionTimes[SMOOTHING_LENGTH] = {};
int lastRevValueIndex = SMOOTHING_LENGTH - 1; //Set this up this way so the first loop through
                                              //   doesn't require anything special.

volatile int status[4] = {1, 0, 0, 0};

long lastTrigger = 0;
long millisPerRevolution = 0;

volatile float per = 0.0;
volatile long elapsed = 0;

void MoveLights(){
   //Write the status of the lights
   //At this point we know how many milliseconds it takes us to get around
   // The loop (calculated when the photo cell sees a change) and 
   // how long it's been since we saw one.  
   if(lastTrigger != 0 && millisPerRevolution != 0){ // Don't screw around if we don't know what we're doing
      long curMillis = millis(); //This isn't actually going to change since this timer was invoked,
                                 // don't care, it'll do what we want.

      elapsed = curMillis - lastTrigger;
      
      per = elapsed / millisPerRevolution; //Can probably brute force this as int if speed is a problem

      if(per < .25){
         status[0] = 1;
         status[1] = 0;
         status[2] = 0;
         status[3] = 0;
      } else if( per < .5){
         status[0] = 0;
         status[1] = 1;
         status[2] = 0;
         status[3] = 0;
      } else if( per < .75){
         status[0] = 0;
         status[1] = 0;
         status[2] = 1;

         status[3] = 0;
      } else {
         status[0] = 0;
         status[1] = 0;
         status[2] = 0;
         status[3] = 1;
      }
   } 
   digitalWrite(FIRST_PIN, status[0]);
   digitalWrite(SECOND_PIN, status[1]);
   digitalWrite(THIRD_PIN, status[2]);
   digitalWrite(FOURTH_PIN, status[3]);

}
void setup(){
   Log.Init(LOG_LEVEL_DEBUG, 9600);
   for(int x=0;x<SMOOTHING_LENGTH;x++){
      revolutionTimes[x] = 0;
   }
   pinMode(FREQUENCYTIMER2_PIN, OUTPUT);

   pinMode(FIRST_PIN, OUTPUT);
   pinMode(SECOND_PIN, OUTPUT);
   pinMode(THIRD_PIN, OUTPUT);
   pinMode(FOURTH_PIN, OUTPUT);
 
   FrequencyTimer2::setPeriod(5000); //5 milliseconds
   FrequencyTimer2::setOnOverflow(MoveLights);
   FrequencyTimer2::enable();
}

void adjustAverageMillisPer(long newReading){
   //revolutionTimes has the last SMOOTHING_LENGTH revolutions in seconds.   
   lastRevValueIndex ++;
   if(lastRevValueIndex >= SMOOTHING_LENGTH){
      lastRevValueIndex = 0;
   }
   revolutionTimes[lastRevValueIndex] = newReading;

   int goodReadings = 0;
   long totalMS = 0;
   for(int x=0;x<SMOOTHING_LENGTH;x++){
      if(revolutionTimes[x] != 0){
         totalMS += revolutionTimes[x];
         goodReadings++;
      }
   }

   millisPerRevolution = totalMS / goodReadings;
   char buffer[25];
   
   Log.Info("millisPerRevolution: %l, lastTrigger: %l, elapsed: %l, percent through cycle: %b"CR, millisPerRevolution, lastTrigger, elapsed, per);
}



void loop(){
   //Need to read the photo cell and come up with an RPM measurement.  
   long pinState = analogRead(A0);
   if(pinState > BRIGHT_THRESHHOLD){
      Log.Info("Found a ping with %l"CR, pinState);
      //This is one RPM.  Based on this we know that the FIRST_PIN should be high now.
      //Going to reset them all to where they should be.  This is ugly, but it will
      // help me see if I'm going down the wrong path here.
      long curTime = millis();
      long revTime = curTime - lastTrigger; 
      lastTrigger = curTime; 
      //millisPerRevolution calculations
      //Need to actually do a running average here.
      adjustAverageMillisPer( revTime );
   }

   //This is really slow, but I'm going to be leaving the thing open for so long that I don't
   // want to make it slower and double count;
   delay(500);
}

