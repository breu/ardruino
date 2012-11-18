// Recrsive lights
// Joe Breu (breu@breu.org)
//
// LEDs attached to pins 3-10
// Button on pin 13

int ledPins[] = {3,4,5,6,7,8,9,10};
const int buttonPin = 2;
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
    light_it_up_center(3,4,75);
    delay(100);
    light_one_up(0,1,75);
    delay(100);
    light_one_up(7,-1,75);
    delay(100);
    light_it_up_outside(0,7,75);
    delay(100);

  }
}


void light_it_up_center(int left, int right, int delayTime)
{
  if (left < 0 || right > 8)             // check if we are out of bounds
    return;
  digitalWrite(ledPins[left],HIGH);      // turn on the left light
  digitalWrite(ledPins[right],HIGH);     // turn on the right light 
  delay(delayTime);
  light_it_up_center(left-1,right+1,delayTime); // go recursive
  digitalWrite(ledPins[left],LOW);       // turn off the left light
  digitalWrite(ledPins[right],LOW);      // turn off the right light
  delay(delayTime);
}

void light_it_up_outside(int left, int right, int delayTime)
{
  if (right < left)
    return;
  digitalWrite(ledPins[left],HIGH);      // turn on the left light
  digitalWrite(ledPins[right],HIGH);     // turn on the right light 
  delay(delayTime);
  light_it_up_outside(left+1,right-1,delayTime); // go recursive
  digitalWrite(ledPins[left],LOW);       // turn off the left light
  digitalWrite(ledPins[right],LOW);      // turn off the right light
  delay(delayTime);
}

void light_one_up(int index, int delta, int delayTime)
{
  if ( index < 0 || index > 8)          // check if we are out of bounds
    return;
  digitalWrite(ledPins[index],HIGH);
  delay(delayTime);
  light_one_up(index+delta,delta,delayTime);
  digitalWrite(ledPins[index],LOW);
  delay(delayTime);
}
