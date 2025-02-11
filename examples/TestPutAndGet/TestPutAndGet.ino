  #include <EEPROM.h>

  #define EEPROM_Module_Space      0x7FFF                          // Exp.: 256Kbit * 1024 bit / 8 bit/byte == 0x7FFF bytes
  #define EEPROM_Number_of_Modul   0                               // EEPROM Module number
  #define EEPROM_Test_Adresse      0                               // Put / Get test adress
  #define EEPROM_Pages             64                              // Page Space

  //#define Debug_EEPROM                                           // Debuging infos

  eeprom EEPROM(EEPROM_Module_Space,EEPROM_Pages,EEPROM_Number_of_Modul);
  
void setup() {
  // Start Serial
    Serial.begin(9600);
    while(!Serial);
  
  // Init Module
    Serial.println("Init EEPROM");
    bool EEPROM_Status=EEPROM.init();
  
    if (EEPROM_Status > 0){
      Serial.println("Fail to init EEPROM");
      while(true);
    }

  // Get Value
    byte TestValue=0;
    Serial.println("--Get Value--");
    EEPROM.get(EEPROM_Test_Adresse, TestValue);
    Serial.print(" Value:");Serial.println(String(TestValue)); Serial.println();
  
  // Put Value +1
    Serial.println("--Put Value--");
    TestValue++;
    EEPROM.put(EEPROM_Test_Adresse, TestValue);
    Serial.print(" Value:");Serial.println(String(TestValue)); Serial.println();
  
  // Get Value
    Serial.println("--Get Value--");
    EEPROM.get(EEPROM_Test_Adresse, TestValue);
    Serial.print(" Value:");Serial.println(String(TestValue)); Serial.println();
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
