
/*
 * Nuri Erginer
 * <http://makerstorage.com/>
 * Wifi PID Controller Ver 1
 * PID Controller for CNC 2 Printer 3D Conversion
 */







PID PIDExtruder(&Current_Extruder_Temp, &OutputValueExtruder, &Set_Extruder_Temp,KP_Extruder,KI_Extruder,KD_Extruder, DIRECT);
PID PIDBed(&Current_Bed_Temp, &OutputValueBed, &Set_Bed_Temp,KP_Bed,KI_Bed,KD_Bed, DIRECT);


int WindowSizeExtruder = 5000;
unsigned long windowStartTimeExtruder;

int WindowSizeBed = 5000;
unsigned long windowStartTimeBed;

float read_thermocouple(){
 thermocouple.readInternal();

   double c = thermocouple.readCelsius();
   
   if (isnan(c)) {
    // Serial.println("Something wrong with thermocouple!");
   } else {
    return c;
   }

}




int read_termistor(int THERMISTOR)
{
   int rawtemp = analogRead(THERMISTOR);
   int current_celsius = 0;

   byte i;
   for (i=1; i<NUMTEMPS; i++)
   {
      if (temptable[i][0] > rawtemp)
      {
         int realtemp  = temptable[i-1][1] + (rawtemp - temptable[i-1][0]) * (temptable[i][1] - temptable[i-1][1]) / (temptable[i][0] - temptable[i-1][0]);

         if (realtemp > 255)
            realtemp = 255; 

         current_celsius = realtemp;

         break;
      }
   }

   // Overflow: We just clamp to 0 degrees celsius
   if (i == NUMTEMPS)
   current_celsius = 0;

   return current_celsius;
}



void read_Temperatures(){

  switch (ExtruderSensor) {
        case 0:
         Current_Extruder_Temp = read_termistor(0);//A0 pin
        break;
        case 1:
         Current_Extruder_Temp = read_termistor(1);//A1 pin
        break;
        case 2:
         Current_Extruder_Temp = read_thermocouple();
        break;
  }
  switch (BedSensor) {
        case 0:
         Current_Bed_Temp = read_termistor(0);//A0 Pin
        break;
        case 1:
         Current_Bed_Temp = read_termistor(1);//A1 pin
        break;
        case 2:
         Current_Bed_Temp = read_thermocouple();
        break;
  }

}



void Manage_Extruder_Temperature (){
  read_Temperatures();
  PIDExtruder.Compute();
  analogWrite(ExtruderOutput,OutputValueExtruder); 
  if (Current_Extruder_Temp > Set_Extruder_Temp) {
    analogWrite(ExtruderOutput,0);
  }
 
}

void Manage_Bed_Temperature (){
  read_Temperatures();
  PIDBed.Compute();
  analogWrite(BedOutput,OutputValueBed);
  if (Current_Bed_Temp > Set_Bed_Temp) {
    analogWrite(BedOutput,0);
  }

  
}



void initPID(){
PIDExtruder.SetMode(AUTOMATIC);
PIDBed.SetMode(AUTOMATIC);
}

void Manage_Temperatures (){
Manage_Extruder_Temperature ();
Manage_Bed_Temperature ();
}




