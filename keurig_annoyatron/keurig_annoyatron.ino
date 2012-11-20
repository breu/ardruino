const int lightPin=0;
int blinkThreshold=220;
int blinkLengthThreshold=60;
int blinkLengthInterval=500;
const unsigned long timeToWait=15000;
volatile int klaxonCounter=0;

// Clock stuff
volatile unsigned long previousMillis=0;
volatile unsigned long klaxonStartTimer=0;
volatile unsigned long playingKlaxonStart=0;
volatile unsigned long sleepTimer=0;                        // sleep timer
const unsigned long playingKlaxonDuration=30*1000; // play the alarm for 30 seconds

void setup() {
  Serial.begin(115200);
  Serial.println("startup");
  pinMode(13,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(2,INPUT);
}

void playTheAlarm() {
  Serial.println("SOUND THE ALARM!");
  digitalWrite(13,HIGH);
  delay(1000);
  digitalWrite(13,LOW);
}

int getLight(){
  return analogRead(lightPin);
}

void learn() {
/*
Things we need to try and learn
1) the threshold of the led (average, median, and low)
2) the amount of time the led is lit
3) other things that I can't think of now.
*/
}

void loop(){
  unsigned long currentMillis=millis();
  int lightLevel=getLight();

  if (digitalRead(2) == LOW) {
    Serial.print("Got a button press");
    sleepTimer=millis()+timeToWait;
    klaxonCounter=0;
  }
    
  if (currentMillis < sleepTimer && sleepTimer) {
    digitalWrite(12,HIGH);
    digitalWrite(11,LOW);
    delay(1000);
    digitalWrite(12,LOW);
  } else if (currentMillis > sleepTimer && sleepTimer) {
    sleepTimer=0;
  } else if (lightLevel<blinkThreshold) {
    digitalWrite(11,LOW);
    // we detected a light level less than our defined threshold.
    if (previousMillis==0) {
      // this is the start of the blink.  record the time.
      previousMillis=millis();
    }
    Serial.print("DEBUG: lightLevel=");
    Serial.println(lightLevel);
  } else {
    digitalWrite(11,HIGH);
    // the light source is now outside of our threshold.
    if (previousMillis) {
      // we saw a start of the blink.  record the time duration of the blink.
      int delta=currentMillis-previousMillis;
      int testDelta=0;
      
      Serial.print("delay interval=");
      Serial.println(delta);
      previousMillis=0;
      
      testDelta=delta-blinkLengthThreshold;

      Serial.print("testDelta=");
      Serial.println(testDelta);
        
      if (testDelta<=100 && testDelta >=-100) {
        // valid blink...
        if (!klaxonCounter) {
          // this is the first blink we have seen
          Serial.println("!!! Setting the klaxonCounter");
          klaxonStartTimer=currentMillis;
          klaxonCounter++;
        } else {
          Serial.print("!!! klaxonCounter=");
          Serial.println(klaxonCounter);
          if (klaxonCounter==6) {
            if (currentMillis-playingKlaxonStart>=playingKlaxonDuration) {
              Serial.println("Done sounding the ALARM!");
              klaxonCounter=0;
              klaxonStartTimer=0;
              sleepTimer=millis()+timeToWait;
              playingKlaxonStart=0;
            } else {
              playTheAlarm();
              if (!playingKlaxonStart) {
                playingKlaxonStart=currentMillis;
              }
            }
          } else if ((currentMillis-klaxonStartTimer)>=(blinkLengthInterval*6)) {
            // invalid or super long blink... (with some leeway...)
            Serial.println("** invalid led interval");
            unsigned long debug1=currentMillis-klaxonStartTimer;
            unsigned long debug2=blinkLengthInterval*6;
            klaxonCounter=0;
            klaxonStartTimer=0;
          } else {
            klaxonCounter++;
          }
        }
      }        
    }
  }
}
