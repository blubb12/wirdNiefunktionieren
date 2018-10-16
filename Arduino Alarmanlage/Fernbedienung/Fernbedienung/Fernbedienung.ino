#include <IRremote.h>
//Zahl = Hexcode/Dezimal
//1= FFA25D/16753245    2= FF629D/16736925 3= FFE21D/16769565
//4= FF22DD/16720605    5= FF02FD/16712445 6= FFC23D/16761405
//7= FFE01F/16769055    8= FFA857/16754775 9= FF906F/16748655
                      //0= FF9867/16750695


const int buzzer = 7;
const int RECV_PIN = 6;
IRrecv irrecv(RECV_PIN);
decode_results results;

long Pin[4] = {16753245,16736925,16769565,16720605};      // Set the required PIN code.
int currentPosition = 0;     // Keeps track of the position of the code entered.
long currentKey[4];
int counter = 0;

void invalidCode()
{
 Serial.println("buuh");

}


void validCode()
{
//mach was cooles
Serial.println("juhu");
}

void setup(){
  Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);
  pinMode(buzzer, OUTPUT);
}

void loop(){
  
  if (irrecv.decode(&results)){
    if(results.value != 4294967295){
        //Serial.println(results.value, HEX);
        Serial.println(results.value);
        currentKey[counter++] = results.value;
    }
    irrecv.resume();
  }
  
  if(counter == 4){
    for(int i = 0; i < 4; i++){
      if(currentKey[i] != Pin[i]){
        invalidCode();
         tone(buzzer, 1000); 
         delay(1000);       
         noTone(buzzer);
        
      }
    }
    counter = 0;
  }
}

/*
  if (int(key) != 4294967295) {     
   char key = results.value;      // Get the key press from the REMOTE.
  Serial.println(key);
                    // Check to see if there is a value present.
     
      Serial.println(Pin[currentPosition]);
      if (int(key) == Pin[currentPosition]){        // If the value is correct, increase he code position by one.
          ++currentPosition;
          
          if (currentPosition == 4)                // If all four parts of the code are correct, unlock the door.
          {
            validCode();
            currentPosition = 0;
          }
        } 
       
      else {
          invalidCode();                           // If any parts of the code are wrong, do not open the door.
          currentPosition = 0;
        }
   }*/
