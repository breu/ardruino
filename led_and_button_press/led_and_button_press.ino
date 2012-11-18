// Recrsive lights
// Joe Breu (breu@breu.org)
//
// LEDs attached to pins 2-9
// Button on pin 13

int ledPins[] = {2,3,4,5,6,7,8,9};
const int buttonPin = 13;
int buttonState = 0;

void setup()
{
  pinMode(buttonPin, INPUT);      
  for(int i = 0; i < 8; i++){
      pinMode(ledPins[i],OUTPUT);
  }
}

void loop()
{
  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
    light_it_up(3,4,50);
  }
}


void light_it_up(int left, int right, int delayTime)
{
  if (left < 0 || right > 8)             // check if we are out of bounds
    return;
  digitalWrite(ledPins[left],HIGH);      // turn on the left light
  digitalWrite(ledPins[right],HIGH);     // turn on the right light 
  delay(delayTime);
  light_it_up(left-1,right+1,delayTime); // go recursive
  digitalWrite(ledPins[left],LOW);       // turn off the left light
  digitalWrite(ledPins[right],LOW);      // turn off the right light
  delay(delayTime);
}
