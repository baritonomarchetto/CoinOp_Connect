/*
CoinOp Connect - PC to NON-JAMMA interface board for genuine, dedicated arcade cabinets.

Arduino NANO

This sketch reads outputs signals from Howard Casto's Mamehooker, monitors sync frequency 
and enables video amplifier.

Here is an example of mamehooker .ini code:(messages sent to Arduino serial port COM4)

[General] 
MameStart=cmo 4 baud=9600_parity=N_data=8_stop=1 
MameStop=cmc 4 
StateChange= 
OnRotate= 
OnPause= 
[KeyStates] 
RefreshTime= 
[Output] 
lamp0=
lamp1=cmw 4 1., cmw 4 %s%, cmw 4 x
lamp2=cmw 4 2., cmw 4 %s%, cmw 4 x
lamp3=cmw 4 3., cmw 4 %s%, cmw 4 x

by Barito, 2023 (last update oct 2023)
*/

//Define which video signal your monitor support (options: CGA, EGA or VGA)
#define CGA

#define OUTPUTS 8

int rID;
int value;

int periodSum;
int periodIst;
int sCounter;
int fq;
int fCounter;
unsigned long fStart;
unsigned long fEnd;
const int samples = 200;
boolean enableState;
boolean prevEnState;

//digital out 1 is connected to Arduino pin A2. Digital out 2 is connected to Arduino pin A0. Digital out 3 ... you say!
const int out_pin[] = {A2, A0, 5, 6, 9, 10, 11, 12};

const byte hSyncPin = 3;//interrupt pin
const byte vSyncPin = 2;//interrupt pin (unused)
const byte LED = 13;
const byte vAmpDisable = A1;
const byte VFEpin = A5;
boolean VFEstate;
boolean VFE; //video force enable flag
unsigned long VFEdbTime;

void setup() {
  for (int j = 0; j < OUTPUTS; j++){
    pinMode(out_pin[j], OUTPUT);
  }
  pinMode(vAmpDisable, OUTPUT); //VIDEO AMP DISABLE
  pinMode(LED, OUTPUT);//BUILT-IN LED
  pinMode(hSyncPin, INPUT_PULLUP); //H-SYNC MEASURE
  pinMode(vSyncPin, INPUT_PULLUP); //V-SYNC MEASURE (unused)
  pinMode(VFEpin, INPUT_PULLUP); //VFE toggle switch
  #ifdef CGA
    fq = 50; //15KHZ - default, 63 us (CGA)
  #endif
  #ifdef EGA
    fq = 30; //25KHZ - 40 us (EGA)
  #endif
  #ifdef VGA
    fq = 10; //31KHZ - 32 us (VGA)
  #endif
  digitalWrite(vAmpDisable, HIGH);
  digitalWrite(LED, LOW);
  VFEstate = digitalRead(VFEpin);
  VFE = 0;//video output not forced to enable
  attachInterrupt(digitalPinToInterrupt(hSyncPin), FreqCalc, FALLING);
  Serial.begin(9600);
}

void loop(){
  DigitalOut();
  VFEset();
  if(VFE == 1){
    digitalWrite(vAmpDisable, LOW); //ENABLE
    digitalWrite(LED, HIGH);
  }
}

void VFEset(){
  if (millis()-VFEdbTime > 400 /*long enought delay time*/ && digitalRead(VFEpin) !=  VFEstate){
    VFEstate = !VFEstate;
    VFEdbTime = millis();
    if(VFEstate == LOW){//toggle Video Force Enable flag
      VFE = !VFE;
    }
  }
}

void DigitalOut(){
  while (Serial.available()>0){
    rID = Serial.parseInt();    //first byte (output ID number)
    value = Serial.parseInt();  //second byte (output state)
    if (Serial.read() == 'x'){  //third byte (end of message)
      if(rID >0 && rID <=OUTPUTS){
        digitalWrite(out_pin[rID-1], value);
      }
    }
  }
}

void FreqCalc(){
  if(VFE == 0){
    fCounter++;
    if(fCounter > samples){
      fEnd = micros();
      if((fEnd - fStart)/samples > fq){
        enableState = 1;
      }
      else {
        enableState = 0;
      }
      if (enableState != prevEnState){//take action on state change only
        prevEnState = enableState;
        digitalWrite(vAmpDisable, !enableState);
        digitalWrite(LED, enableState);
      }
      //Serial.println((fEnd - fStart)/samples);
      fCounter = 0;//reset counter
      fStart = micros();//reset time
    }
  }
}
  
