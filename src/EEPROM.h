#ifndef __EEPROM__
#define __EEPROM__

  #define Version_EEPROM 0

  #include <Arduino.h>
  #include <Wire.h>

  //#define Debug_EEPROM                                          // Zeig alle möchglichen meldungen an

  #ifndef EEPROM_Write_Cycle_Time                                 // Zeit zum beschreiben des moidules (Standard 5ms)
    #define EEPROM_Write_Cycle_Time  5
  #endif
  
  #ifndef EEPROM_Module_Adress
    #define EEPROM_Module_Adress 0x50                             // Wire adresse für Speicher Module
  #endif

  #define EEPROM_OUT_OF_SPACE  0                           
  #define EEPROM_OK            1

//--------------------------------------------------------------------------------------------------------------------------------
// class EEPROM
//--------------------------------------------------------------------------------------------------------------------------------

  class eeprom {
    private:
      template <typename T>
      bool Start(uint16_t& Address, T &Data);        			 // Kommunikation Start
	 uint16_t Module_Space;
	 byte     Number_of_Modul;
	 uint16_t Module_Pages;
      
    public:
	 eeprom(uint16_t Module_Space, uint16_t Module_Pages, byte Number_of_Modul=0);
      bool init();                                                // 0=ok, 1=Fehler
      template <typename T>
       T    get(uint16_t Address, T &Data);      				 // Daten Lesen
      template <typename T>
       T    read (uint16_t Address, T &Data); 
       byte read (uint16_t Address);
      template <typename T>
       bool put(uint16_t Address, T &Data);      				 // Daten Schreiben
      template <typename T>
       bool write(uint16_t Address, T &Data);
    };

//--------------------------------------------------------------------------------------------------------------------------------
// Konstruktor
//--------------------------------------------------------------------------------------------------------------------------------

	eeprom::eeprom(uint16_t Module_Space, uint16_t Module_Pages, byte Number_of_Modul) : Module_Space(Module_Space), Module_Pages(Module_Pages),Number_of_Modul(Number_of_Modul){};
	
//--------------------------------------------------------------------------------------------------------------------------------
// init
//--------------------------------------------------------------------------------------------------------------------------------

  bool eeprom::init() {                               // 0=ok, 0>Fehler mit einem der Speicher module (return Moduel nr)
    // Debug Ausgabe
      #ifdef Debug_EEPROM
          Serial.println("--EEPROM-Init--");
          Serial.print  (" -Modul : ");Serial.println(Number_of_Modul);
      #endif
    Wire.begin();
    byte GET_1;
    byte GET_2;
    uint16_t Address=10;
    GET_1=10;
    GET_2=11;
    get(Address,GET_1);               // Auslesen 1
    get(Address,GET_2);               // Auslesen 2
    if(GET_1 != GET_2){               // Auslesen 1 & 2 müßßen nach dem auslesen gleich sein
      // Debug Ausgabe
        #ifdef Debug_EEPROM
          Serial.println(" -Status: Fail");
          Serial.println(" -Fail  : \"GET_1 != GET_2\"");
          Serial.print  (" -Get_1 : ");Serial.println(GET_1);
          Serial.print  (" -Get_2 : ");Serial.println(GET_2);
        #endif
      return true;
    }
    // Debug Ausgabe
      #ifdef Debug_EEPROM
        Serial.println(" -Status: ok");
      #endif
    return false;
  }

//--------------------------------------------------------------------------------------------------------------------------------
// get
//--------------------------------------------------------------------------------------------------------------------------------

    template <typename T>
    T eeprom::get(uint16_t Address, T &Data) {
      // Kommunikation Starten
        #ifdef Debug_EEPROM
          Serial.println("--EEPROM-Get--");
        #endif
        if(!Start(Address,Data)) return 0;
      // Übertragung Ende
        Wire.endTransmission();
      // Antwort
        uint8_t* pointer = reinterpret_cast<uint8_t*>(&Data);
        Wire.requestFrom(Number_of_Modul+EEPROM_Module_Adress, sizeof(Data));
        delay(EEPROM_Write_Cycle_Time);
        for (int i = 0; i < sizeof(Data); i++) {
          if (Wire.available()){
            *pointer++ = Wire.read();
          }
        }
      // Warten bis alles asugelesen ist
        while (Wire.available()) {Wire.read();};
      // Debug Ausgabe
        #ifdef Debug_EEPROM
          Serial.print  (" -(Data): ");Serial.println(Data);
        #endif
        return Data;
    }

//--------------------------------------------------------------------------------------------------------------------------------
// read
//--------------------------------------------------------------------------------------------------------------------------------

    template <typename T>
    T eeprom::read (uint16_t Address, T &Data){
      #ifdef Debug_EEPROM
        Serial.println("--EEPROM-read--");
      #endif
      return get(Address,Data);
    };

    byte eeprom::read (uint16_t Address){
      #ifdef Debug_EEPROM
        Serial.println("--EEPROM-read-Byte--");
      #endif
      byte TempByte;
      return get(Address,TempByte);
    };

//--------------------------------------------------------------------------------------------------------------------------------
// put
//--------------------------------------------------------------------------------------------------------------------------------

    template <typename T>
    bool eeprom::put(uint16_t Address, T &Data) {
      // Kommunikation Starten
        #ifdef Debug_EEPROM
          Serial.println("--EEPROM-Put--");
        #endif
        if(!Start(Address,Data)) return EEPROM_OUT_OF_SPACE;
      // Daten Senden

        /* Hinweis:
            Wen ein Seitenende (Seiten größe: Module_Pages) ereicht wird,
            kann man nicht einfach weiter schreiben, sondern muss auf die nächste seite wächseln.
        */

        int SizeOnPage= Module_Pages-(Address%Module_Pages);
        byte temp;
        uint8_t* pointer = reinterpret_cast<uint8_t*>(&Data);
        for(int i = 0; i < sizeof(Data); i++){
          if (SizeOnPage<=i){                         // Prüfen ob Seiten ende ereicht, nächste seite weiter schreiben...
            #ifdef Debug_EEPROM
              Serial.println(" -Seiten ende (Nächste Seite)");
            #endif
            Wire.endTransmission();                   // Übertragung Ende
            delay(EEPROM_Write_Cycle_Time);           // Module benötigt 5ms zum Speichern
            Wire.flush();                             // Warten bis alles asugelesen ist
            Address+=i;                             // Neue Adresse
            Start(Address,temp);                      // Starte nächste Seite
            Address-=i;                             // Rückgänig machen
            SizeOnPage+=Module_Pages-i;               // Zwichenspeichern, wie viele element von der letzten seite bereits genutz wurden
          }
          Wire.write(*pointer++);
        }
      // Übertragung Ende
        Wire.endTransmission();
      // Module benötigt 5ms zum Speichern
        delay(EEPROM_Write_Cycle_Time);
      // Warten bis alles asugelesen ist
        while (Wire.available()) {Wire.read();};
        return EEPROM_OK;
    }

//--------------------------------------------------------------------------------------------------------------------------------
// write
//--------------------------------------------------------------------------------------------------------------------------------

    template <typename T>
    bool eeprom::write(uint16_t Address, T &Data){
      #ifdef Debug_EEPROM
        Serial.println("--EEPROM-write--");
      #endif

      // Aktuellen wert auslesen
        T GetData;
        get (Address,GetData);

     // Wert schreiben
       if (GetData != Data){
         return put(Address,Data);
       } else {
         #ifdef Debug_EEPROM
           Serial.println(" -Daten Sind identisch (kein Schreiben)");
         #endif
         return EEPROM_OK;
       }
    };

//--------------------------------------------------------------------------------------------------------------------------------
// Start
//--------------------------------------------------------------------------------------------------------------------------------

    template <typename T>
    bool eeprom::Start(uint16_t& Address, T &Data){
      // Prüfen ob Adresse Exisitiert
        if (Address+sizeof(Data) > Module_Space) {
          #ifdef Debug_EEPROM
            Serial.println("--Adress-Fail--");
            Serial.print  (" -Adress: ");Serial.println(Address);
            Serial.print  (" -sizeof: ");Serial.println(sizeof(Data));
            Serial.print  (" -Modul : ");Serial.println(Number_of_Modul );
          #endif
          return EEPROM_OUT_OF_SPACE;
        }
      // Wenn noch alt daten vorhanden, diese löschen
        Wire.flush();
      // Komunikation Starten
        Wire.beginTransmission(EEPROM_Module_Adress+Number_of_Modul);// Start communication
        Wire.write((uint16_t)(Address & 0xFF00) >> 8);        // Sende Address + MSB
        Wire.write((uint16_t)(Address & 0x00FF));             // Sende Address + LSB
      // Debug
        #ifdef Debug_EEPROM
          Serial.println("--Adress--");
          Serial.print  (" -Adress: ");Serial.println(Address);
          Serial.print  (" -sizeof: ");Serial.println(sizeof(Data));
          Serial.print  (" -Modul : ");Serial.println(Number_of_Modul );
          Serial.print  (" -Data->: ");Serial.println(Data);
        #endif
      // Fertig
        return EEPROM_OK;
    }
    
#endif
