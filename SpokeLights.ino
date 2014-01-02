#include <Logging.h>

int ledState = 0;
void setup(){
   Log.Init(LOG_LEVEL_DEBUG, 9600);
   pinMode(13, OUTPUT);
}

void loop(){
   if(ledState == 0){
      digitalWrite(13, HIGH);
      ledState = 1;
   } else {
      digitalWrite(13, LOW);
      ledState = 0;
   }
   delay(1000);
}

