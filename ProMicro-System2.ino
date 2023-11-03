/* CoinOp Connect - PC to NON-JAMMA arcade cabinet interface board

Arduino Pro Micro
Compatible with Atari System 2 fingerboard.

This sketch reads inputs from the control panel and emulates a Direct Input Joystick and a mouse.

by Barito, 2023 (last update oct 2023)
*/

#include <Joystick.h>
#include <Mouse.h> 

#define SWITCHES 9

//Set these at your actual hardware phisical limits (0-255)
#define ANALOG1_MIN 40
#define ANALOG1_MAX 215
#define ANALOG2_MIN 40
#define ANALOG2_MAX 215
#define ANALOG3_MIN 40
#define ANALOG3_MAX 215

const int delayTime = 20;
const int noLoss = 8;

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_MULTI_AXIS, SWITCHES, 0, //joy type, button count, hatswitch count
  true, true, true, // X, Y, Z axis
  false, false, false, // X, Y, Z rotation
  false, false, //rudder, throttle
  false, false, false); //accelerator, brake, steering

struct digitalInput {const byte pin; boolean state; unsigned long dbTime; const byte btn;} 
digitalInput[SWITCHES] = {
  {9, HIGH, 0, 0},
  {8, HIGH, 0, 1},
  {A1, HIGH, 0, 2},
  {16, HIGH, 0, 3},
  {6, HIGH, 0, 4},
  {14, HIGH, 0, 5},
  {5, HIGH, 0, 6},
  {4, HIGH, 0, 7},
  {10, HIGH, 0, 8},
};

const int optA1Pin = 7;  //interrupt pin
const int optB1Pin = 15;
const int optA2Pin = 3;  //interrupt pin
const int optB2Pin = 0;
const int optA3Pin = 2;  //interrupt pin
const int optB3Pin = 1;

boolean optA1_state;
boolean optB1_state;
boolean optA2_state;
boolean optB2_state;
boolean optA3_state;
boolean optB3_state;

const byte analog1Pin = A2;
const byte analog2Pin = A0;
const byte analog3Pin = A3;

int a1Value;
int a2Value;
int a3Value;

void setup(){
  for (int j = 0; j < SWITCHES; j++){
    pinMode(digitalInput[j].pin, INPUT_PULLUP);
    digitalInput[j].state = digitalRead(digitalInput[j].pin);
    digitalInput[j].dbTime = millis();
  }
  pinMode(optA1Pin, INPUT_PULLUP);
  pinMode(optB1Pin, INPUT_PULLUP);
  pinMode(optA2Pin, INPUT_PULLUP);
  pinMode(optB2Pin, INPUT_PULLUP);
  pinMode(optA3Pin, INPUT_PULLUP);
  pinMode(optB3Pin, INPUT_PULLUP);
  pinMode(analog1Pin, INPUT);
  pinMode(analog2Pin, INPUT);
  pinMode(analog3Pin, INPUT);
  optA1_state = digitalRead(optA1Pin);
  optB1_state = digitalRead(optB1Pin);
  optA2_state = digitalRead(optA2Pin);
  optB2_state = digitalRead(optB2Pin);
  optA3_state = digitalRead(optA3Pin);
  optB3_state = digitalRead(optB3Pin);
  attachInterrupt(digitalPinToInterrupt(optA1Pin), Opt1Wheel, RISING); //1X COUNTING, single interrupt
  attachInterrupt(digitalPinToInterrupt(optA2Pin), Opt1Wheel, RISING); //1X COUNTING, single interrupt
  attachInterrupt(digitalPinToInterrupt(optA3Pin), Opt1Wheel, RISING); //1X COUNTING, single interrupt
  Joystick.begin();
  Joystick.setXAxisRange(0, 255);
  Joystick.setYAxisRange(0, 255);
  Joystick.setZAxisRange(0, 255);
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

void Opt2Wheel(){ //1X COUNTING, single interrupt
  //check channel B to see which way encoder is turning
  if(digitalRead(optB2Pin) == LOW){
    Mouse.move(2, 0, 0); //CCW
  }
  else{
    Mouse.move(-2, 0, 0); //CW
  }
}

void Opt3Wheel(){ //1X COUNTING, single interrupt
  //check channel B to see which way encoder is turning
  if(digitalRead(optB2Pin) == LOW){
    Mouse.move(2, 0, 0); //CCW
  }
  else{
    Mouse.move(-2, 0, 0); //CW
  }
}

void analogInputs(){
  a1Value = analogRead(analog1Pin)>>2;//0-255
  a1Value = map(a1Value, ANALOG1_MIN, ANALOG1_MAX, 0, 255);
  Joystick.setXAxis(a1Value);
  
  a2Value = analogRead(analog1Pin)>>2;//0-255
  a2Value = map(a2Value, ANALOG2_MIN, ANALOG2_MAX, 0, 255);
  Joystick.setYAxis(a2Value);
  
  a3Value = analogRead(analog1Pin)>>2;//0-255
  a3Value = map(a3Value, ANALOG3_MIN, ANALOG3_MAX, 0, 255);
  Joystick.setZAxis(a3Value);
}
