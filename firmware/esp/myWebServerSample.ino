
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <FS.h>
#include <ArduinoJson.h> 
#include "myWebServer.h"
#include <TimeLib.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 14, 5, 4, 2); //V2
byte lcdRows            = 2;
byte lcdCols            = 16;

String jsonIn;

long LCD_previousMillis = 0;        // will store last time status updated
long LCD_interval = 2500;           // interval at which status send (milliseconds)  

long Serial_previousMillis = 0;        // will store last time status updated
long Serial_interval = 2500;           // interval at which status send (milliseconds)  

int ST1 = 1 ; //Set_Extruder_Temp
int CT1 = 2 ; //Current_Extruder_Temp
float O1 = 3 ; // OutputValueExtruder;
int ST2 = 4 ; //Set_Bed_Temp
int CT2 = 5 ; //Current_Bed_Temp
float O2 = 6 ; // OutputValueBed;

void setup() {
  // put your setup code here, to run once:
	Serial.begin(9600);
  lcd.begin(lcdCols, lcdRows);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("--MakerStorage--");

	MyWebServer.begin();	
}

void loop() {
  // put your main code here, to run repeatedly:
	MyWebServer.handle();
 
  serialUpdate();
  LCDUpdate();

}//loop


void serialUpdate(){
  /*
  Atmel send serial data to ESP in below format
  {"ST1":0.00,"ST2":0.00,"CT1":20.00,"CT2":23.00,"O1":0.00,"O2":0.00}
 */
  if (millis() - Serial_previousMillis > Serial_interval) 
  {
        Serial_previousMillis = millis();   // PID update rate
     
    while (Serial.available()) {
      jsonIn = Serial.readStringUntil('\n');  //read json 
      StaticJsonBuffer<300> jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(jsonIn);

      if (root.success()) {

       ST1 = String(root["ST1"].asString()).toInt();
       ST2 = String(root["ST2"].asString()).toInt();
       
       CT1 = String(root["CT1"].asString()).toInt();
       CT2 = String(root["CT2"].asString()).toInt();

       O1 = String(root["O1"].asString()).toFloat();
       O2 = String(root["O2"].asString()).toFloat();


      }//if
    }//while

  }//if  
}//serialUpdate

void LCDUpdate(){
  if (millis() - LCD_previousMillis > LCD_interval) 
  {
        LCD_previousMillis = millis();   // PID update rate
    
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("S1:");
        lcd.print(ST1);
        lcd.print(" C1:");
        lcd.print(CT1);
        lcd.setCursor(0, 1);
        lcd.print("S2:");
        lcd.print(ST2);
        lcd.print(" C2:");
        lcd.print(CT2);
  }//if     
}//LCDUpdate        
