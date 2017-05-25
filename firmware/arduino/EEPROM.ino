/*

Wifi PID Controller Ver 1
WebSite: makerstorage.com
Developer: Nuri Erginer
Licence: GNU General Public License  see <http://www.gnu.org/licenses/>. 

This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
GNU General Public License for more details. 

*/
#include <EEPROM.h>

template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
  const byte* p = (const byte*)(const void*)&value;
  unsigned int i;
  for (i = 0; i < sizeof(value); i++)
    EEPROM.write(ee++, *p++);
  return i;
}

template <class T> int EEPROM_readAnything(int ee, T& value)
{
  byte* p = (byte*)(void*)&value;
  unsigned int i;
  for (i = 0; i < sizeof(value); i++)
    *p++ = EEPROM.read(ee++);
  return i;
}


void Write2Eprom(){

int curPos = 0;

curPos += EEPROM_writeAnything(curPos, ExtruderSensor);  
curPos += EEPROM_writeAnything(curPos, BedSensor);  

curPos += EEPROM_writeAnything(curPos, Set_Extruder_Temp);  
curPos += EEPROM_writeAnything(curPos, Set_Bed_Temp);  

curPos += EEPROM_writeAnything(curPos, KP_Extruder);  
curPos += EEPROM_writeAnything(curPos, KP_Bed);  

curPos += EEPROM_writeAnything(curPos, KI_Extruder);  
curPos += EEPROM_writeAnything(curPos, KI_Bed); 

curPos += EEPROM_writeAnything(curPos, KD_Extruder);  
curPos += EEPROM_writeAnything(curPos, KD_Bed); 

}  




void ReadFromEprom(){

int curPos = 0;

curPos += EEPROM_readAnything(curPos, ExtruderSensor);  
curPos += EEPROM_readAnything(curPos, BedSensor);

curPos += EEPROM_readAnything(curPos, Set_Extruder_Temp);  
curPos += EEPROM_readAnything(curPos, Set_Bed_Temp);

curPos += EEPROM_readAnything(curPos, KP_Extruder);  
curPos += EEPROM_readAnything(curPos, KP_Bed);

curPos += EEPROM_readAnything(curPos, KI_Extruder);  
curPos += EEPROM_readAnything(curPos, KI_Bed);
  
curPos += EEPROM_readAnything(curPos, KD_Extruder);  
curPos += EEPROM_readAnything(curPos, KD_Bed);



}
