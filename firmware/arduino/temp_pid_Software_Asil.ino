#include <PID_v1.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include <PID_v1.h>


#include <SPI.h>
#include "Adafruit_MAX31855.h"

#define MAXDO   4
#define MAXCS   3
#define MAXCLK  5

// initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

// Thermistor lookup table for RepRap Temperature Sensor Boards (http://make.rrrf.org/ts)
// Made with createTemperatureLookup.py (http://svn.reprap.org/trunk/reprap/firmware/Arduino/utilities/createTemperatureLookup.py)
// ./createTemperatureLookup.py --r0=100000 --t0=25 --r1=0 --r2=4700 --beta=4066 --max-adc=1023
// r0: 100000
// t0: 25
// r1: 0
// r2: 4700
// beta: 4066
// max adc: 1023
#define NUMTEMPS 20
short temptable[NUMTEMPS][2] = {
   {1, 841},
   {54, 255},
   {107, 209},
   {160, 184},
   {213, 166},
   {266, 153},
   {319, 142},
   {372, 132},
   {425, 124},
   {478, 116},
   {531, 108},
   {584, 101},
   {637, 93},
   {690, 86},
   {743, 78},
   {796, 70},
   {849, 61},
   {902, 50},
   {955, 34},
   {1008, 3}
};


/*
 * Nuri Erginer
 * <http://makerstorage.com/>
 * Wifi PID Controller Ver 1
 * PID Controller for CNC 2 Printer 3D Conversion
 */



#define DEBUG


//Mosfet Outputs
#define ExtruderOutput 9  //mosfet 1 connected to digital 9
#define BedOutput 10 //mosfet 2 connected to digital 10



//Sensors
int ExtruderSensor =-1; //0=Thermistor1  1=Thermistor2  3=Thermocouple
int BedSensor =-1;




long Status_previousMillis = 0;        // will store last time status updated
long Status_interval = 2500;           // interval at which status send (milliseconds)

long Temp_previousMillis =0;
long Temp_interval = 500;

#define TEMPERATURE_SAMPLES 10
#define TEMP_UPDATE_INTERVAL 200
unsigned long temp_prev_time;

double Current_Extruder_Temp = 0;
double Set_Extruder_Temp = 185;

double Current_Bed_Temp = 0;
double Set_Bed_Temp = 75;

String jsonIn = "";

double OutputValueExtruder;
double OutputValueBed;

double KP_Extruder = 400;
double KI_Extruder = 0.08;
double KD_Extruder = 400;

double KP_Bed = 400;
double KI_Bed = 0.08;
double KD_Bed = 400;


//sending status data to esp in json format
StaticJsonBuffer<200> jsonBuffer;
JsonObject& CurStatus = jsonBuffer.createObject();

void setup() {

  ReadFromEprom();
  Serial.begin(9600);
  
  pinMode(ExtruderOutput, OUTPUT);
  digitalWrite(ExtruderOutput, LOW);

  pinMode(BedOutput, OUTPUT);
  digitalWrite(BedOutput, LOW);

  initPID();

}



void loop() {
//ExtruderSensor =0; //0=Thermistor1  1=Thermistor2 2=Thermocouple
//BedSensor =1;
//Set_Extruder_Temp =55;  

  statusUpdate();
  tempUpdate();
  serialUpdate();



}//loop


void statusUpdate(){
  //Atmel sending current values about temperature management
  
  if (millis() - Status_previousMillis > Status_interval) 
  {
    Status_previousMillis = millis();   // PID update rate
    CurStatus["ST1"] = Set_Extruder_Temp;
    CurStatus["ST2"] = Set_Bed_Temp;
    CurStatus["CT1"] = Current_Extruder_Temp;
    CurStatus["CT2"] = Current_Bed_Temp;
    CurStatus["O1"] = OutputValueExtruder;
    CurStatus["O2"] = OutputValueBed;

    //send status data to esp
     CurStatus.printTo(Serial);
     Serial.println("");
     

  }//if millis 
}

void tempUpdate(){
   if (millis() - Temp_previousMillis > Temp_interval) 
   {
    Temp_previousMillis = millis();   // PID update rate
    Manage_Temperatures ();

   }// if milis
}

void serialUpdate(){
  /*
  ESP send serial data to us when settings web page updated
  below code handle the comming json data and write them to
  approprate vars.
 */
    while (Serial.available()) {
      // get the new byte:
     //{"pid1_sensor":"0","pid1_settemp":"55","pid1_P":"400","pid1_I":"0.08","pid1_D":"400","pid2_sensor":"2","pid2_settemp":"60","pid2_P":"400","pid2_I":"0.08","pid2_D":"400"}
     
     jsonIn = Serial.readStringUntil('\n');  //read json 

   //  Serial.println(jsonIn);
     
     StaticJsonBuffer<300> jsonBuffer;
     JsonObject& root = jsonBuffer.parseObject(jsonIn);

     if (root.success()) {
     
       ExtruderSensor = String(root["pid1_sensor"].asString()).toInt();
       BedSensor = String(root["pid2_sensor"].asString()).toInt();

//       Serial.println(ExtruderSensor);
//       Serial.println(BedSensor);
//       Serial.println("------");
       
       Set_Extruder_Temp = String(root["pid1_settemp"].asString()).toFloat();
       Set_Bed_Temp = String(root["pid2_settemp"].asString()).toFloat();

//       Serial.println(Set_Extruder_Temp);
//       Serial.println(Set_Bed_Temp);
//       Serial.println("------");

       KP_Extruder = String(root["pid1_P"].asString()).toFloat();
       KP_Bed = String(root["pid2_P"].asString()).toFloat();

//       Serial.println(KP_Extruder);
//       Serial.println(KP_Bed);
//       Serial.println("------");

       KI_Extruder = String(root["pid1_I"].asString()).toFloat();
       KI_Bed = String(root["pid2_I"].asString()).toFloat();

 //      Serial.println(KI_Extruder);
//       Serial.println(KI_Bed);
//       Serial.println("------");

       KD_Extruder = String(root["pid1_D"].asString()).toFloat();
       KD_Bed = String(root["pid2_D"].asString()).toFloat();

//       Serial.println(KD_Extruder);
//       Serial.println(KD_Bed);
//       Serial.println("------");
     
      Write2Eprom(); // the new settings must be written in eeprom
     }
    }//while
}











