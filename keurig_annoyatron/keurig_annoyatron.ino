/*
 * The passively aggressive Keurig annoy-a-tron
 *
 * Joseph W. Breu (breu@breu.org)
 *
 * this is going to be placed near by Keurig so it makes an annoying noise
 * when someone empties the water and doesn't refill it.
 *
 */

/*
 * Buttons
 */
const int analog_photo_pin=A0;
const int klaxon_led=12;
const int klaxon_speaker=7;
const int sleep_led=10;
const int blink_detect_led=8;
const int sleep_button=2;

/*
 * Constants
 */
const int max_alarm_plays=10;
const int music_length=45;
const char music_notes[]="Cg";
const int music_beats[]={3,2};
const int music_tempo=200;

/*
 * Variables
 */
unsigned long sleep_counter=0;
int TC=0;
int last;
int last_state=0;
int counter=0;
int alarm_plays=0;

/*
 * Standard Setup Function
 */
void setup() {
  Serial.begin(115200);
  Serial.print("starting up...");
  
  /*
   * Set up the outputs
   */
  pinMode(klaxon_led,OUTPUT);
  pinMode(sleep_led,OUTPUT);
  pinMode(blink_detect_led,OUTPUT);
  pinMode(klaxon_speaker,OUTPUT);
  
  digitalWrite(klaxon_led,HIGH); delay(250); digitalWrite(klaxon_led,LOW);
  digitalWrite(sleep_led,HIGH); delay(250); digitalWrite(sleep_led,LOW);
  digitalWrite(blink_detect_led,HIGH); delay(250); digitalWrite(blink_detect_led,LOW);
  
  /*
   * Set up the inputs
   */
  pinMode(sleep_button,INPUT);
  
  Serial.println("ready to go");
}

/*
 * Sleep for an amount of time
 */
void sleeper(unsigned long delayTime=5000) {
  Serial.print("sleeping ");
  Serial.print(delayTime);
  Serial.println(" milliseconds");
  //if (!sleep_counter) sleep_counter=5000;
  digitalWrite(sleep_led,HIGH);
  delay(delayTime);
  digitalWrite(sleep_led,LOW);
  Serial.println("awake from my deep slumber");
}

/*
 * music functions
 */
void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) { 
    digitalWrite(klaxon_speaker, HIGH);
    delayMicroseconds(tone);
    digitalWrite(klaxon_speaker, LOW);
    delayMicroseconds(tone);
  }
}

/*
 * more music functions
 */
void playNote(char note, int duration) {
  char names[] = { 'C', 'g' };
  int tones[] = { 950, 1350 };

  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}

/*
 * Play the alarm with a built in sleep timer
 *
 * If the sleep_counter is set then light the sleep LED and turn off the blink
 * LED for the duration of the sleep.  If sleep_counter is not active then sound
 * the alarm through the piezo.  Since the pizeo is not set up on the board yet
 * we'll just light an LED to fake it.
 */
void play_alarm() {
  if (sleep_counter) {
    // If we get here and we are supposed have been sleeping then take
    // care of that first.
    sleep_counter=0;
    sleeper(sleep_counter);
  } else {
    if (alarm_plays < max_alarm_plays) {
      Serial.println("sounding the alarm!");
      digitalWrite(klaxon_led,HIGH);
      for (int i = 0; i < music_length; i++) {
        if (music_notes[i] == ' ') {
          delay(music_beats[i] * music_tempo); // rest
        } else {
          playNote(music_notes[i], music_beats[i] * music_tempo);
        }
      }
      delay(1000);
      digitalWrite(klaxon_led,LOW);
      alarm_plays++;
    } else {
      alarm_plays=0;
      sleeper(10000);
    }
  }
}

/*
 * The loop
 */
void loop() {
  int current=analogRead(analog_photo_pin);
  
  if (digitalRead(sleep_button)==LOW) {
    // button pressed
    Serial.println("button pressed");
    sleeper(5000);
  }
  
  // increment the Timer Counter (TC).  This is used to detect errant blinks. 
  if (TC) TC++;
  
  // If we last got a reading from the photo resistor it will be stored in last.
  // In that case we can try and figure out the percentage of the delta.
  if (last) {
    if (current!=last) {
      float delta;
      delta=(float)(current-last)/current;
      if (delta < -0.15) {
        /*
         * The light is on.  Set last_state=TRUE and increment the Timer Counter
         */
        last_state=1;
        TC++;
      } else if (delta > 0.15) {
        /*
         * The light is off...  And that makes a blink.
         *
         * Now we can wait until we collect 5 blinks and then we can play an alarm
         */
        if (last_state) {
          if (counter==5) {
            TC=0;
            play_alarm();
            counter=0;
          } else if (last_state) {
            counter++;
          }
          last_state=0;
          digitalWrite(blink_detect_led,HIGH);
        }
      }
    }
  }
  
  /*
   * Set last to current so we can use it next time through the loop
   */
  last=current;
  
  /*
   * Detect a blink sequence that we never finished.  If that happens
   * then reset the state to something that is good and go on.
   */
  if (TC>800) {
    Serial.println("discarding stray state transition");
    last_state=0;
    last=0;
    TC=0;
    counter=0;
  }

  delay(75);
  digitalWrite(blink_detect_led,LOW);

}

