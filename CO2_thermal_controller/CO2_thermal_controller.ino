#include <SparkFunMAX31855k.h>
#include <SPI.h>

/*
#define SCK 1
 #define MISO 1
 #define nSS 1
 */

/* Circuit:                                                                    *
 * MAX31855J breakout attached to the following pins                           *
 *  SS:   pin 53                                                               *
 *  MOSI: pin 50 (NC)                                                          *
 *  MISO: pin 51                                                               *
 *  SCK:  pin 52                                                               *
 *  VCC:  pin 48                                                               *
 *  GND:  pin 49   
 *  
 * Relay Circuit attache to the following pins
 *  VCC: pin 46 
 *  GND: pin 47
 */

 // Define SPI Arduino pin numbers (Arduino Pro Mini)
const uint8_t CHIP_SELECT_PIN = 53; // Using standard CS line (SS)
// SCK & MISO are defined by Arduiino
const uint8_t VCC = 48; // Powering board straight from Arduino Pro Mini
const uint8_t GND = 49;
const uint8_t RELAY = 46;
const uint8_t R_GND = 47;

//Time Keeping
unsigned long timer;
unsigned long timerOffset;

//Instantiate the class
SparkFunMAX31855k probe(CHIP_SELECT_PIN, VCC, GND);

//Control Variables
float setpointTemp = 0;
float currentTemp = 0;
float tolerance = 1; //1dC
bool bbState = false;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(RELAY, OUTPUT);
  pinMode(R_GND, OUTPUT);
  digitalWrite(RELAY, LOW);
  digitalWrite(R_GND, LOW);
  
  //Ask user for input temperature
  Serial.println("To begin heating the CO2 tank please enter the starting temperature (C): ");
  int response = 0;
  while (response == 0){
    response = Serial.readString().toInt();
  }
  setpointTemp = (float)response;

  //Wait for sensor to stabilize
  delay(750);

  //Read Cold Junction Temperature
  float temperature = probe.readCJT();
  if (!isnan(temperature)) {
    Serial.print("CJT is (˚C): ");
    Serial.println(temperature);

  //Set start time of the program
  timerOffset = millis();
  }
}


void loop() {
  //Check Control Inputs
  int response = Serial.readString().toInt();
  if (response) {
    setpointTemp = (float)response;
    //Serial.println(setpointTemp);
  }
  currentTemp = probe.readTempC();

  //Output information
  char buff1[4];
  dtostrf(setpointTemp,3,1,buff1);
  String textTemp1 = String(buff1);
  char buff2[4];
  dtostrf(currentTemp,3,1,buff2);
  String textTemp2 = String(buff2);
  timer = millis() - timerOffset;
  String textTemp3 = String(timer);
  Serial.println("T: " + textTemp3 + "ms, Setpoint Temperature: " + textTemp1 + "C, Current Temperature: "  + textTemp2 + "C" );

  bool output = bbController(setpointTemp, 25, tolerance);
  digitalWrite(RELAY, output==true?HIGH:LOW);
  
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

