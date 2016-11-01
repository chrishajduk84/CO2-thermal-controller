#include <SparkFunMAX31855k.h>
#include <SPI.h>

/*
#define SCK 1
 #define MISO 1
 #define nSS 1
 */

/* Circuit:                                                                    *
 * MAX31855K breakout attached to the following pins                           *
 *  SS:   pin 10                                                               *
 *  MOSI: pin 11 (NC)                                                          *
 *  MISO: pin 12                                                               *
 *  SCK:  pin 13                                                               *
 *  VCC:  pin 14                                                               *
 *  GND:  pin 15   
 */

 // Define SPI Arduino pin numbers (Arduino Pro Mini)
const uint8_t CHIP_SELECT_PIN = 53; // Using standard CS line (SS)
// SCK & MISO are defined by Arduiino
const uint8_t VCC = 48; // Powering board straight from Arduino Pro Mini
const uint8_t GND = 49;
const uint8_t RELAY = 46;

//Instantiate the class
SparkFunMAX31855k probe(CHIP_SELECT_PIN, VCC, GND);

//Control Variables
float setpointTemp = 0;
float currentTemp = 0;
bool bbState = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(RELAY, OUTPUT);
  
  //Ask user for input temperature
  Serial.println("To begin heating the CO2 tank please enter the starting temperature (C): ");
  int response = 0;
  while (response == 0){
    response = Serial.readString().toInt();
  }
  setpointTemp = response;

  //Wait for sensor to stabilize
  delay(750);

  //Read Cold Junction Temperature
  float temperature = probe.readCJT();
  if (!isnan(temperature)) {
    Serial.print("CJT is (˚C): ");
    Serial.println(temperature);
  }
}


void loop() {
  //Check Control Inputs
  int response = Serial.readString().toInt();
  if (response) setpointTemp = response;
  currentTemp = probe.readTempC();

  //Output information
  char buff1[4];
  char buff2[4];
  dtostrf(setpointTemp,3,1,buff1);
  dtostrf(currentTemp,3,1,buff2);
  String textTemp1 = String(buff1);
  String textTemp2 = String(buff2);
  Serial.println("Setpoint Temperature: " + textTemp1 + "C, Current Temperature: "  + textTemp2 + "C" );

  
  delay(750);
}

//Tolerance is how much the controller is allowed to over shoot when heating/cooling
bool bbController(float setpoint, float current, float tolerance){
  if (current < setpoint - tolerance){
    bbState = true;
  }
  else if (current > setpoint + tolerance){
    bbState = false;
  }
  return bbState;
}

