  #include <EEPROM.h>

  #define EEPROM_Module_Space      0xFFFF                          // Exp.: 256Kbit * 1024 bit / 8 bit/byte == 0x7FFF bytes
  #define EEPROM_Number_of_Modul   0                               // EEPROM Module number
  #define EEPROM_Pages             65535                           // Temp Page Space (uint16_t)

  //#define Debug_EEPROM                                           // Debuging infos

  eeprom EEPROM(EEPROM_Module_Space,EEPROM_Pages,EEPROM_Number_of_Modul);
  
void setup() {
  // Start Serial
    Serial.begin(9600);
    while(!Serial);
  
  // Init Modul
    Serial.println("Init EEPROM");
    bool EEPROM_Status=EEPROM.init();
  
    if (EEPROM_Status > 0){
      Serial.println("Fail to init EEPROM");
      while(true);
    }

  // Module Page Space
    uint16_t PutValue  = 0xAAAA;
    uint16_t GetValue  = 0;
    uint16_t PageSpace = 0;
    uint16_t Zero      = 0;
    Serial.println("--Test module Page Space--");
    for (uint16_t Adress=2; Adress <= EEPROM_Pages; Adress*=2){
	    // Test
	      Serial.print("Space: ");
	      Serial.println(Adress);
	   
      // Put Value
        EEPROM.put(Adress, Zero);
        EEPROM.put((Adress-1), PutValue);

      // Get Value
        GetValue=0;
        EEPROM.get((Adress-1), GetValue);

	    // Result
	      if (PutValue != GetValue){
          Serial.println("Complete!");
          PageSpace=Adress;
          Serial.print("Page-Space => ");
          Serial.println(PageSpace);
          break;
        }
    }
  
  // Fail
    if (PageSpace == 0){
      Serial.println("Fail, no Page Space found");
	    while(true);
    }

  // Module Pages
    uint16_t NumberOfPages=0;
    Serial.println();
    Serial.println("--Test module number of Pages--");
    for (int Page=1;Page<=EEPROM_Pages;Page*=2){
      // Test
        Serial.print("Page: ");
        Serial.println(Page);
	    
      // Put Value
        EEPROM.get((Page*PageSpace)-2, PutValue);

      // Get Value
        GetValue=0;
        EEPROM.get((Page*PageSpace)-2, GetValue);

      // Result
        if (PutValue != GetValue){
          Serial.println("Complete!");
          NumberOfPages=Page/2;
          Serial.print("Pages =>");
          Serial.println(NumberOfPages);
          break;
        }
    }
    
  // Fail
    if (NumberOfPages == 0){
      Serial.println("Fail, no number of Pages found (>65535 / uint16_t)");
	    while(true);
    }
}

void loop() {
  // put your main code here, to run repeatedly:

}
