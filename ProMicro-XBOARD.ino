/* CoinOp Connect - PC to NON-JAMMA arcade cabinet interface board

Arduino Pro Micro
Compatible with SEGA X-BOARD fingerboard and SEGA Hang-on fingerboard

This sketch reads inputs from the control panel and emulates a Direct Input Joystick.

by Barito, 2023 (last update oct 2023)
*/

#include <Joystick.h>

#define SWITCHES 12

//Set these at your actual hardware phisical limits (0-255)
#define ANALOG1_MIN 40
#define ANALOG1_MAX 215
#define ANALOG2_MIN 40
#define ANALOG2_MAX 215
#define ANALOG3_MIN 40
#define ANALOG3_MAX 215
#define ANALOG4_MIN 40
#define ANALOG4_MAX 215

const int delayTime = 20;
const int noLoss = 8;

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_MULTI_AXIS, SWITCHES, 0, //joy type, button count, hatswitch count
  true, false, false, // X, Y, Z axis
  false, false, false, // X, Y, Z rotation
  false, false, //rudder, throttle
  true, true, true); //accelerator, brake, steering

struct digitalInput {const byte pin; boolean state; unsigned long dbTime; const byte btn;} 
digitalInput[SWITCHES] = {
  {9, HIGH, 0, 0},
  {8, HIGH, 0, 1},
  {10, HIGH, 0, 2},
  {7, HIGH, 0, 3},
  {16, HIGH, 0, 4},
  {6, HIGH, 0, 5},
  {14, HIGH, 0, 6},
  {5, HIGH, 0, 7},
  {15, HIGH, 0, 8},
  {4, HIGH, 0, 9,},
  {3, HIGH, 0, 10},
  {2, HIGH, 0, 11},
};

const byte DCmotorPin = 1;//unused

const byte analog1Pin = A0;
const byte analog2Pin = A1;
const byte analog3Pin = A2;
const byte analog4Pin = A3;

int a1Value;
int a2Value;
int a3Value;
int a4Value;

void setup(){
  for (int j = 0; j < SWITCHES; j++){
    pinMode(digitalInput[j].pin, INPUT_PULLUP);
    digitalInput[j].state = digitalRead(digitalInput[j].pin);
    digitalInput[j].dbTime = millis();
  }
  pinMode(analog1Pin, INPUT);
  pinMode(analog2Pin, INPUT);
  pinMode(analog3Pin, INPUT);
  pinMode(analog4Pin, INPUT);
  Joystick.begin();
  Joystick.setXAxisRange(0, 255);
  Joystick.setAcceleratorRange(0, 255);
  Joystick.setBrakeRange(0, 255);
  Joystick.setSteeringRange(0, 255);
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

void analogInputs(){
  a1Value = analogRead(analog1Pin)>>2;//0-255
  a1Value = map(a1Value, ANALOG1_MIN, ANALOG1_MAX, 0, 255);
  Joystick.setSteering(a1Value);
  
  a2Value = analogRead(analog2Pin)>>2;//0-255
  a2Value = map(a2Value, ANALOG2_MIN, ANALOG2_MAX, 0, 255);
  Joystick.setAccelerator(a2Value);
  
  a3Value = analogRead(analog3Pin)>>2;//0-255
  a3Value = map(a3Value, ANALOG3_MIN, ANALOG3_MAX, 0, 255);
  Joystick.setBrake(a3Value);

  a4Value = analogRead(analog4Pin)>>2;//0-255
  a4Value = map(a4Value, ANALOG4_MIN, ANALOG4_MAX, 0, 255);
  Joystick.setXAxis(a4Value);
}
