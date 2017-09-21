/*
 Five Planters w/ LCD
 Adapted from Arduino library code by:
 Daniel Woodson
 2017.04.16
 
 This sketch measures the moisture from 5 different sensors, compares that value to a set point and waters the
 plants by turning on a pump and opening one of five valves

 The circuit: Follow "hellow world" setup from arduino for the LCD
 For the moisture sensors use A0-4.  for the DIO, use whatever you have free but avoid the Tx/Rx pins


 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */
#include <LiquidCrystal.h>// include the Liquid Crystal library code

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // initialize the library with the numbers of the interface pins

int valve[5] = {2,10,6,7,8}; //arduino pin values for valves
int moistEnable = 19;
int moist[5] = {A0,A1,A2,A3,A4}; //arduino pin values for moisture sensors
unsigned long tslw[5] = {}; //time since last water in hours
unsigned long tslwReset[5] = {0,0,0,0,0}; //reset for time since last water

unsigned long sensorFunc = 5000; //time in milliseconds to run each function
unsigned long lcdFunc = 1000; //time in milliseconds to run each function
unsigned long waterFunc = 300000; //time in milliseconds to run each function

unsigned long sensorFuncReset; //time in milliseconds since last time the function was run
unsigned long lcdFuncReset; //time in milliseconds since last time the function was run
unsigned long waterFuncReset; //time in milliseconds since last time the function was run
int waterAmount[5] = {10000,10000,0,10000,10000}; //set point for how much to water

int pump = 9; //Pump IO #

float sensor[5] = {}; //maths holder, may not need
float output[5] = {}; //maths holder, may not need
float setpoint[5] = {50,50,0,50,50}; //set point for when to water

void setup() {
  sensorFuncReset = sensorFunc;//force each function to run at least once during first loop
  lcdFuncReset = lcdFunc;
  waterFuncReset = waterFunc;
  for (int v = 0; v < 5; v++){ //setup each of the 5 valves
    pinMode(valve[v], OUTPUT);
    digitalWrite(valve[v], HIGH);
    }
  pinMode(pump, OUTPUT);
  digitalWrite(pump, HIGH); //setup pump pin output

  pinMode(moistEnable, OUTPUT);
  
  digitalWrite(moistEnable, LOW); //setup moisture sensor enable (keeps sensors off unless in use)
  
  Serial.begin(9600);
  lcd.begin(20, 4); // set up the LCD's number of columns and rows:
  lcd.setCursor(0,0);
  lcd.print(" 1   2   3   4   5  ");
  lcd.setCursor(0,1); 
  lcd.print("%");
  lcd.setCursor(0,2);
  lcd.print("A");
  lcd.setCursor(0,3);
  lcd.print("T");
}
void updateSensors() {
  digitalWrite(moistEnable, HIGH);
  delay(1000);
  for (int x = 0; x < 5; x++) {
    sensor[x] = analogRead(x);
    output[x] = (1-(sensor[x] / 1023)) * 100; 
    /*
    Serial.print("Sensor ");
    Serial.print(x+1);
    Serial.print(" is ");
    Serial.println(output[x]);
    */
    delay(500);
  }
  digitalWrite(moistEnable, LOW);
  sensorFuncReset = millis();
}
void updateLCD() {

  for (int x = 0; x < 5; x++){
    for (int y = 1; y < 4; y++){
      lcd.setCursor((x*4)+1,y);
      lcd.print("    ");//Clear old data
      }
    lcd.setCursor((x*4)+1,1);
    lcd.print(output[x],0);//Write new moisture data to LCD

    lcd.setCursor((x*4)+1,2);
    lcd.print(setpoint[x],0);//Write new set point data to LCD

    lcd.setCursor((x*4)+1,3);
    tslw[x] = (millis() - tslwReset[x])/60000;
    lcd.print(tslw[x]);//Write new time since last water (tslw) data to LCD
    }
  lcdFuncReset = millis();
}
void waterThePlants(){
  for (int x = 0; x < 5; x++){
    if (output[x] < setpoint[x]){
      digitalWrite(pump, LOW); //turn on the pump
      delay(1000); //wait a bit so that there's water in the line
      digitalWrite(valve[x], LOW); //open the x'th valve to water the plants
      delay(waterAmount[x]); //leave the valve open the specified amount
      digitalWrite(pump,HIGH); //turn off the pump
      digitalWrite(valve[x], HIGH); //close the x'th valve
      delay(5000);
      tslwReset[x] = millis(); //set the time since last water reset value
      }
    }
  waterFuncReset = millis(); //set the waterFuncReset valve
}
void loop() {
 
  if(sensorFunc < millis() - sensorFuncReset){
    updateSensors();
    }
  if(lcdFunc < millis() - lcdFuncReset){
    updateLCD();
    }
  if(waterFunc < millis() - waterFuncReset){
    delay(5000);
    waterThePlants();
    }
}

