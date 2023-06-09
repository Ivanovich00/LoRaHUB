#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>  

#define LORA_SCK     5    
#define LORA_MISO    19   
#define LORA_MOSI    27 
#define LORA_SS      18  
#define LORA_RST     14   
#define LORA_DI0     26  
#define LORA_BAND    915E6

#include <SPI.h>
#include <MFRC522.h>

#define RFID_SDA 5 
#define RFID_SCK 18 
#define RFID_MOSI 23
#define RFID_MISO 19
#define RFID_RST 22

MFRC522 mfrc522(RFID_SDA, RFID_RST);  // Create MFRC522 instance

int current_spi = -1; // -1 - NOT STARTED   0 - RFID   1 - LORA

void setup() {
  Serial.begin(115200);   // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
}

void loop() {
  bool card_present = RFID_check();
  if (card_present) LORA_send();
}

void spi_select(int which) {
     if (which == current_spi) return;
     SPI.end();
     
     switch(which) {
        case 0:
          SPI.begin(RFID_SCK, RFID_MISO, RFID_MOSI);
          mfrc522.PCD_Init();   
        break;
        case 1:
          SPI.begin(LORA_SCK,LORA_MISO,LORA_MOSI,LORA_SS);
          LoRa.setPins(LORA_SS,LORA_RST,LORA_DI0);
        break;
     }

     current_spi = which;
}

int RFID_check() {
  spi_select(0);

  if (mfrc522.PICC_IsNewCardPresent()){  
    //Seleccionamos una tarjeta
    if (mfrc522.PICC_ReadCardSerial()) 
    {
      // Enviamos serialemente su UID
      Serial.print("Card UID:");
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);   
      } 
      Serial.println();
      // Terminamos la lectura de la tarjeta  actual
      mfrc522.PICC_HaltA();         
    }
	}

  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    delay(100);
    return false;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return false;
  }
  
  return true;
}

void LORA_send() {
  spi_select(1);
  
  Serial.println("LoRa Sender Test"); 
  
  if (!LoRa.begin(LORA_BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  Serial.println("init ok");
 
  delay(1500);

  // send packet
  LoRa.beginPacket();
  LoRa.print("hello ");

  LoRa.endPacket();

  Serial.println("sent one");
  delay(2000);                       // wait for a second
 }