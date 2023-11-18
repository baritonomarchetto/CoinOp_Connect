/* CoinOp Connect - PC to NON-JAMMA arcade cabinet interface board

Arduino Pro Micro
Compatible with Atari/NAMCO Pole Position fingerboard.

This sketch reads inputs from the control panel and emulates a Direct Input Joystick and a mouse.

by Barito, 2023 (last update oct 2023)
*/

#include <Joystick.h>
#include <Mouse.h> 

#define SWITCHES 7

//Set these at your actual hardware phisical limits (0-255)
#define ANALOG1_MIN 40
#define ANALOG1_MAX 215

const int delayTime = 20;
const int noLoss = 8;

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_GAMEPAD, SWITCHES, 0, //joy type, button count, hatswitch count
  false, true, false, // X, Y, Z axis
  false, false, false, // X, Y, Z rotation
  false, false, //rudder, throttle
  false, false, false); //accelerator, brake, steering

struct digitalInput {const byte pin; boolean state; unsigned long dbTime; const byte btn;} 
digitalInput[SWITCHES] = {
  {9, HIGH, 0, 0},
  {16, HIGH, 0, 1},
  {7, HIGH, 0, 2},
  {6, HIGH, 0, 3},
  {14, HIGH, 0, 4},
  {8, HIGH, 0, 5},
  {10, HIGH, 0, 6},
};

const int optA1Pin = 2; //interrupt pin
const int optB1Pin = 3; //interrupt pin

boolean optA1_state;
boolean optB1_state;

const byte analog1Pin = A2;

int a1Value;

void setup(){
  for (int j = 0; j < SWITCHES; j++){
    pinMode(digitalInput[j].pin, INPUT_PULLUP);
    digitalInput[j].state = digitalRead(digitalInput[j].pin);
    digitalInput[j].dbTime = millis();
  }
  pinMode(optA1Pin, INPUT_PULLUP);
  pinMode(optB1Pin, INPUT_PULLUP);
  pinMode(analog1Pin, INPUT);
  optA1_state = digitalRead(optA1Pin);
  optB1_state = digitalRead(optB1Pin);
  attachInterrupt(digitalPinToInterrupt(optA1Pin), Opt1Wheel, RISING); //1X COUNTING, single interrupt
  Joystick.begin();
  Joystick.setYAxisRange(0, 255);
}

void loop(){
generalInputs();
analogInputs();
}

void generalInputs(){
  //general input handling
  for (int j = 1; j < SWITCHES; j++){
    if (millis()-digitalInput[j].dbTime > delayTime && digitalRead(digitalInput[j].pin) !=  digitalInput[j].state){
      digitalInput[j].state = !digitalInput[j].state;
      digitalInput[j].dbTime = millis();
      Joystick.setButton(digitalInput[j].btn, !digitalInput[j].state);
      delay(noLoss);
    }
  }  
}

void Opt1Wheel(){ //1X COUNTING, single interrupt
  //check channel B to see which way encoder is turning
  if(digitalRead(optB1Pin) == LOW){
    Mouse.move(2, 0, 0); //CCW
  }
  else{
    Mouse.move(-2, 0, 0); //CW
  }
}

void analogInputs(){
  a1Value = analogRead(analog1Pin)>>2;//0-255
  a1Value = map(a1Value, ANALOG1_MIN, ANALOG1_MAX, 0, 255);
  Joystick.setYAxis(a1Value);
}
