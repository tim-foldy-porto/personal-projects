//################BLE UART FRIEND################
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_UART.h"

#define BUFSIZE                        128   // Size of the read buffer for incoming data
#define VERBOSE_MODE                   false  // If set to 'true' enables debug output
#define BLUEFRUIT_UART_MODE_PIN        -1    // Set to -1 if unused

Adafruit_BluefruitLE_UART ble(Serial1, BLUEFRUIT_UART_MODE_PIN);
//################BLE UART FRIEND################



//################STEPPER SET UP#################
#define STEP_Y 22   // white
#define DIR_Y 23    // black
#define ENA_Y 24    // grey

#define STEP_R 25   // white
#define DIR_R 27    // black
#define ENA_R 26    // grey

#define STEP_G 28   // white
#define DIR_G 29    // black
#define ENA_G 30    // grey

#define STEP_B 31   // white
#define DIR_B 33    // black
#define ENA_B 32    // grey

#define STEP_W 15   // green
#define DIR_W 16    // yellow
#define ENA_W 14    // blue

/*    CURRENT CONFIGURATION
 *    ---------------------
 *    Red     Top
 *    Yellow  Base
 *    Green   Clamp
 *    Blue    Wrist
 *    White   Middle
 */

//################STEPPER SET UP#################

#define bluePin 10
#define greenPin 11
#define yellowPin 12
#define redPin 13

int tempPeriod = 17000;
int tempStepSize = 300;
bool tempDirection = false;

bool runG = false;
bool runR = false;
long lastTime;

int middlePosition = 0;   //all the way in
int basePosition = 0;     //all the way back (extended)
int clampPosition = 0;    //all the way clamped
int wristPosition = 0;    //centered
int topPosition = 0;      //all the way in

int middleSetPoint = 0;
int baseSetPoint = 0;
int clampSetPoint = 0;
int wristSetPoint = 0;
int topSetPoint = 0;


void setup() {
  
  Serial.begin(9600);
  pinInit();        //initialize all stepper control and LED pins as OUTPUT
  bleSetup();
  Serial.println("Ready to begin!");

  digitalWrite(greenPin, HIGH);
  digitalWrite(redPin, HIGH);

  
  
}

void loop() {

  if (Serial.available() > 0) {
    char readByte = Serial.read();

    if (readByte == 'a')
      enableStepper('A', true);

    if (readByte == 'b') 
      enableStepper('A', false);


    if (readByte == 'c') {
      runStepper('R', 100, true, 17000);
    }

    if (readByte == 'd') {
      runStepper('R', 100, false, 17000);
    }

    if (readByte == 'e') {
      tempStepSize += 10;
    }

    if (readByte == 'f') {
      tempStepSize -= 10;
    }

    if (readByte == 'g') {
      tempPeriod += 500;
    }

    if (readByte == 'h') {
      tempPeriod -= 500;
    }
    
    if (readByte == 'i') {
      tempDirection = !tempDirection;
    }

    Serial.print(tempDirection);
    Serial.print('\t');
    Serial.print(tempStepSize);
    Serial.print('\t');
    Serial.println(tempPeriod);
    
  }

  if (ble.isConnected()) 
    bleControl();

  if (wristPosition < wristSetPoint) {    //need to run clockwise
    runStepper('G', 10, false, tempPeriod);
  }

  if (wristPosition > wristSetPoint) {    //need to run CCW
    runStepper('G', 10, true, tempPeriod);
  }

}







void bleControl() {
  digitalWrite(bluePin, HIGH);
  enableStepper('A', true);

  char inString[9];
  int i = 0;
  byte command = 0;
  uint8_t stepperID;
  uint8_t setPoint;
  int debounceTime = 50;
  long lastTime;
  bool skip = false;

  while (ble.isConnected()) {

    ble.println("AT+BLEUARTRX");
    ble.readline();

    if (strcmp(ble.buffer, "OK") != 0 & (millis() - lastTime) > debounceTime) {
        skip = true;
      }
  
    if (strcmp(ble.buffer, "OK") != 0 & !skip) {
      
      ((String)ble.buffer).toCharArray(inString, 9);
      
      for (int n = 0; n < 8; n++) {
        command |= ((String)inString[n]).toInt() << 7-n;
      }
  
      stepperID = (command & B11100000) >> 5;
      tempDirection = (command & B00010000) >> 4;
      setPoint = (command & B00001111);
      Serial.println(stepperID);
      Serial.println(tempDirection);
      Serial.println(setPoint);
      command = 0;

      if (stepperID == 1 & tempDirection == 1 & setPoint != 0)
        clampSetPoint = constrain(map(setPoint, 0, 15, 0, 3000), 0, 3000);
      if (stepperID == 1 & tempDirection == 0 & setPoint != 0)
        clampSetPoint = -constrain(map(setPoint, 0, 15, 0, 3000), 0, 3000);
      if (stepperID == 1 & setPoint == 0)
        clampSetPoint = 0;

      if (stepperID == 2 & tempDirection == 1 & setPoint != 0)
        wristSetPoint = constrain(map(setPoint, 0, 15, 0, 5100), 0, 5100);
      if (stepperID == 2 & tempDirection == 0 & setPoint != 0)
        wristSetPoint = -constrain(map(setPoint, 0, 15, 0, 5100), 0, 5100);
      if (stepperID == 2 & setPoint == 0)
        wristSetPoint = 0;

      if (stepperID == 3 & tempDirection == 1 & setPoint != 0)
        topSetPoint = constrain(map(setPoint, 0, 15, 0, 3000), 0, 3000);
      if (stepperID == 3 & tempDirection == 0 & setPoint != 0)
        topSetPoint = -constrain(map(setPoint, 0, 15, 0, 3000), 0, 3000);
      if (stepperID == 3 & setPoint == 0)
        topSetPoint = 0;

      if (stepperID == 4 & tempDirection == 1 & setPoint != 0)
        middleSetPoint = constrain(map(setPoint, 0, 15, 0, 3000), 0, 3000);
      if (stepperID == 4 & tempDirection == 0 & setPoint != 0)
        middleSetPoint = -constrain(map(setPoint, 0, 15, 0, 3000), 0, 3000);
      if (stepperID == 4 & setPoint == 0)
        middleSetPoint = 0;

      if (stepperID == 5 & setPoint != 0)
        baseSetPoint = constrain(map(setPoint, 0, 15, 0, 1000), 0, 3000);
      if (stepperID == 5 & setPoint == 0)
        baseSetPoint = 0;

      if (stepperID == 7 & setPoint == 9) { //volume up
        topSetPoint = 1600;
        middleSetPoint = -2000;
        clampSetPoint = -500;
      }

      if (stepperID == 7 & setPoint == 7) { //water
        runStepper('B', 2550, false, 17000);
        delay(2000);
        runStepper('B', 2550, true, 17000);
      }

      if (stepperID == 7 & setPoint == 2)   //clamp
        clampSetPoint = 0;

      if (stepperID == 7 & setPoint == 3)   //release
        clampSetPoint = 1500;

      if (stepperID == 7 & setPoint == 8) { //click
        runStepper('W', 50, true, 17000);
        delay(50);
        runStepper('W', 50, false, 17000);
      }

      if (stepperID == 7 & setPoint == 10) { //click
        runStepper('W', 50, true, 17000);
        delay(50);
        runStepper('W', 50, false, 17000);
      }
      
        
    }

    while (clampPosition < clampSetPoint) {    //need to unclamp
        runStepper('G', 1, false, 17000);
      } 
    while (clampPosition > clampSetPoint) {    //need to clamp
        runStepper('G', 1, true, 17000);
      }

    while (wristPosition < wristSetPoint) {    //need to run clockwise
        runStepper('B', 1, false, 17000);
      }  
    while (wristPosition > wristSetPoint) {    //need to run clockwise
        runStepper('B', 1, true, 17000);
      }

    while (topPosition < topSetPoint) {    //need to move out
        runStepper('R', 1, true, 17000);
      } 
    while (topPosition > topSetPoint) {    //need to move in
        runStepper('R', 1, false, 17000);
      }

    while (middlePosition < middleSetPoint) {    //need to move down
        runStepper('W', 1, false, 17000);
      } 
    while (middlePosition > middleSetPoint) {    //need to move up
        runStepper('W', 1, true, 17000);
      }

    while (basePosition < baseSetPoint) {    //need to move down
        runStepper('Y', 1, false, 17000);
      } 
    while (basePosition > baseSetPoint) {    //need to move up
        runStepper('Y', 1, true, 17000);
      }

    lastTime = millis();
    skip = false;
    
  }
  digitalWrite(bluePin, LOW);
  enableStepper('A', false);
}








void enableStepper(char stepperID, bool enabled) {
  
  switch (stepperID) {
    case 'R':
      digitalWrite(ENA_R, !enabled);
      break;
      
    case 'Y':
      digitalWrite(ENA_Y, !enabled);
      break;

    case 'G':
      digitalWrite(ENA_G, !enabled);
      break;

    case 'B':
      digitalWrite(ENA_B, !enabled);
      break;

    case 'W':
      digitalWrite(ENA_W, !enabled);
      break;
      
    case 'A':     //'A' for all
      digitalWrite(ENA_R, !enabled);
      digitalWrite(ENA_Y, !enabled);
      digitalWrite(ENA_G, !enabled);
      digitalWrite(ENA_B, !enabled);
      digitalWrite(ENA_W, !enabled);
      digitalWrite(yellowPin, enabled);
      digitalWrite(redPin, !enabled);
      break;
  }
}








void runStepper(char stepperID, int steps, bool directions, int period) {
  /*  stepperID: character corresponding to 1 of the 5 motor drivers: Y, R, W, G, B
      steps: number of steps for the motor to move
      directions: boolean corresponding to forward or backwards: true, false
      period: the period of the stepper pulse in microseconds */
      
  
  
  switch (stepperID) {
    case 'Y':
      digitalWrite(DIR_Y, directions);
      for (int i = 0; i < steps; i++) {
        digitalWrite(STEP_Y, HIGH);
        delayMicroseconds(period);
        digitalWrite(STEP_Y, LOW);
        delayMicroseconds(period);
        if (directions) basePosition -= 1;   //up means we're decreasing (towards zero)
        if (!directions) basePosition += 1;  //down means we're increasing
      }
      break;
      
    case 'R':
      digitalWrite(DIR_R, directions);
      for (int i = 0; i < steps; i++) {
        digitalWrite(STEP_R, HIGH);
        delayMicroseconds(period);
        digitalWrite(STEP_R, LOW);
        delayMicroseconds(period);
        if (!directions) topPosition -= 1;   //CCW means we're decreasing
        if (directions) topPosition += 1;  //CW means we're increasing
      }
      break;

    case 'G':
      digitalWrite(DIR_G, directions);
      for (int i = 0; i < steps; i++) {
        digitalWrite(STEP_G, HIGH);
        delayMicroseconds(period);
        digitalWrite(STEP_G, LOW);
        delayMicroseconds(period);
        if (directions) clampPosition -= 1;   //clamping means we're decreasing
        if (!directions) clampPosition += 1;  //unclamping means we're increasing
      }
      break;

    case 'B':
      digitalWrite(DIR_B, directions);
      for (int i = 0; i < steps; i++) {
        digitalWrite(STEP_B, HIGH);
        delayMicroseconds(period);
        digitalWrite(STEP_B, LOW);
        delayMicroseconds(period);
        if (directions) wristPosition -= 1;   //CCW means we're decreasing
        if (!directions) wristPosition += 1;  //CW means we're increasing
      }
      break;

    case 'W':
      digitalWrite(DIR_W, directions);
      for (int i = 0; i < steps; i++) {
        digitalWrite(STEP_W, HIGH);
        delayMicroseconds(period);
        digitalWrite(STEP_W, LOW);
        delayMicroseconds(period);
        if (directions) middlePosition -= 1;   //up means we're decreasing (towards zero)
        if (!directions) middlePosition += 1;  //down means we're increasing
      }
      break;
  }
}








void pinInit() {
  pinMode(bluePin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  
  pinMode(STEP_Y, OUTPUT);
  pinMode(DIR_Y, OUTPUT);
  pinMode(ENA_Y, OUTPUT);
  digitalWrite(DIR_Y, HIGH);
  digitalWrite(ENA_Y, HIGH);

  pinMode(STEP_R, OUTPUT);
  pinMode(DIR_R, OUTPUT);
  pinMode(ENA_R, OUTPUT);
  digitalWrite(DIR_R, HIGH);
  digitalWrite(ENA_R, HIGH);

  pinMode(STEP_G, OUTPUT);
  pinMode(DIR_G, OUTPUT);
  pinMode(ENA_G, OUTPUT);
  digitalWrite(DIR_G, HIGH);
  digitalWrite(ENA_G, HIGH);

  pinMode(STEP_B, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  pinMode(ENA_B, OUTPUT);
  digitalWrite(DIR_B, HIGH);
  digitalWrite(ENA_B, HIGH);

  pinMode(STEP_W, OUTPUT);
  pinMode(DIR_W, OUTPUT);
  pinMode(ENA_W, OUTPUT);
  digitalWrite(DIR_W, HIGH);
  digitalWrite(ENA_W, HIGH);
}








void bleSetup() {

  Serial.print(F("Initialising the Bluefruit LE module: "));
  
  if ( !ble.begin(VERBOSE_MODE) ) {
    Serial.println(F("Couldn't find Bluefruit"));
    while (1);
  }
  
  Serial.println(F("OK!"));

  Serial.print(F("Performing a factory reset: "));
  if ( ! ble.factoryReset() ){
    Serial.println(F("Couldn't factory reset"));
  }

  Serial.println(F("OK!"));

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  ble.verbose(false);  // debug info is a little annoying after this point!
  
}
