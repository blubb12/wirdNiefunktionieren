#include <IRremote.h>
#include <SPI.h>
#include <MFRC522.h>
 
#define RST_PIN   9     // SPI Reset Pin
#define SS_PIN    10    // SPI Slave Select Pin


//LED
int ledrotPin = 7;
int ledgruenPin = 8;

//Fernbedienung
const int RECV_PIN = 6;
IRrecv irrecv(RECV_PIN);
decode_results results;

//RFID
MFRC522 mfrc522(SS_PIN, RST_PIN); 
const byte unsereUID[4] = {134,49,120,245}; //UID der Karte in Dezimal

//der Pin zum "öffnen"
char pin[2][14]= {
 "blah",
 "hmm"
 };
//strcpy(pin[0], "blah");
//strcpy(pin[1], "hmm");


void setup(){
  //led
  pinMode(ledrotPin, OUTPUT);
  pinMode(ledgruenPin, OUTPUT);
  
  //Fernbedienung
  Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);  
  
   //RFID
  SPI.begin();         // Initialisiere SPI Kommunikation
  mfrc522.PCD_Init();  // Initialisiere MFRC522 Lesemodul

 
  
  //pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output
  
  

  
}

void loop(){
     
  //Fernbedienung
  if (irrecv.decode(&results)){
        Serial.println(results.value);
        Serial.println(results.value, HEX);
        irrecv.resume();
  }
  int j;
 // for(j = 0; j < pin.length; j++){
    
  //}
  


  //RFID
    // Diese Funktion wird in Endlosschleife ausgeführt
  // Nur wenn eine Karte gefunden wird und gelesen werden konnte, wird der Inhalt von IF ausgeführt
  // PICC = proximity integrated circuit card = kontaktlose Chipkarte
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial() ) {
    Serial.print("Gelesene UID:");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      //Abstand zwischen HEX-Zahlen und führende Null bei Byte < 16 
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
    } 

     
       //Vergleicht gelesene Karte mit eingespeicherter
      int i;
     for(i = 0; i <= mfrc522.uid.size; i++) {
        if (mfrc522.uid.uidByte[i] != unsereUID[i]){
            //led
            digitalWrite(ledrotPin, HIGH);    
           // digitalWrite(ledgruenPin, LOW); 
          }
          //so geht das nicht. größe abfragen und so
          else{
            digitalWrite(ledgruenPin, HIGH);
             //digitalWrite(ledrotPin, LOW);   
            }
       }
  }
      
    
    Serial.println(); 

    // Versetzt die gelesene Karte in einen Ruhemodus, um nach anderen Karten suchen zu können.
    mfrc522.PICC_HaltA();
    delay(1000);
  }
