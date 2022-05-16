#include <SPI.h>
#include <MFRC522.h>
//#include <Thread.h>
#include <ezButton.h>
#include "pitches.h"


#define SS_PIN 10
#define RST_PIN 5
#define LED_PIN_0 0
#define LED_PIN_1 1
#define LED_PIN_2 2
#define LED_PIN_3 3
#define LED_PIN_4 4
#define LED_PIN_5 6
#define LED_PIN_6 7
#define LED_PIN_7 8
#define BUZZER_PIN 9
#define LED_NUMBER 8
#define UID_NUMBER 9
#define UID_LENGTH 4

const int morseLetterInterval = 4000; // number of millisecs between letters
const int morseINLetterInterval = 2000; // number of millisecs between letters characters
const int morseDashDuration = 1000; //dash duration
const int morsePeriodDuration = 200; //period duration

MFRC522 rfid(SS_PIN, RST_PIN);
byte LEDPinArray[LED_NUMBER] = { LED_PIN_0,
                                 LED_PIN_1,
                                 LED_PIN_2,
                                 LED_PIN_3,
                                 LED_PIN_4,
                                 LED_PIN_5,
                                 LED_PIN_6,
                                 LED_PIN_7};
byte HexNumArray[UID_NUMBER][UID_LENGTH]  = {{0x3A,0x56,0x15,0x22},
                                             {0x20,0xEB,0xDB,0x30},
                                             {0x20,0x73,0xE5,0x30},
                                             {0x7,0x71,0xBF,0x49},
                                             {0x29,0x4,0xAD,0x98},
                                             {0x1A,0x92,0xE,0x22},
                                             {0x20,0xB9,0xC8,0x30},
                                             {0x97,0x48,0xDD,0x49}};
bool AlreadySeenUID[UID_NUMBER] = {};
int OnLEDs = 00;
byte hexNum[UID_LENGTH] = {};

ezButton toggleSwitch(A0);  // create ezButton object that attach to pin 7

//------------------------ Functions
void resetAlreadySeen(bool *array1){
  for (int i = 0; i < UID_NUMBER; i++){
    array1[i] = false;
  }
}

void initAllLEDs()
{
  for (int i = 0; i < LED_NUMBER; i++) {
    pinMode(LEDPinArray[i], OUTPUT);
  }
}

void powerOnAllLEDs(bool powerOn)
{
  for (int i = 0; i < LED_NUMBER; i++) {
    if (powerOn) {
      digitalWrite(LEDPinArray[i], HIGH);
    }
    else {
      digitalWrite(LEDPinArray[i], LOW);
    }
  }
}

void blinkAllLEDsON(int times){
  for(int i = 0; i<times; i++){
    powerOnAllLEDs(true);
    delay(200);
    powerOnAllLEDs(false);
    delay(200);
  }
  powerOnAllLEDs(true);
}

void blinkAllLEDsOFF(int times){
  for(int i = 0; i<times; i++){
    powerOnAllLEDs(true);
    delay(200);
    powerOnAllLEDs(false);
    delay(200);
  }
}

void powerOnPlusOneLED(int *OnLEDs){
  Serial.print("Power On +1 LED: ");
  Serial.println(*OnLEDs<LED_NUMBER);
  if(*OnLEDs<LED_NUMBER){
    digitalWrite(LEDPinArray[*OnLEDs], HIGH);
    *OnLEDs = *OnLEDs + 1 ;
  }
}

void powerOnMinusOneLED(int *OnLEDs){
  if(*OnLEDs<LED_NUMBER){
    digitalWrite(LEDPinArray[*OnLEDs], LOW);
    *OnLEDs = *OnLEDs - 1 ;
  }
}

bool testIfUIDEqual(byte *UID1, byte *UID2)
{
  for (int i = 0; i < UID_LENGTH; i++) {
    if (UID1[i] != UID2[i]){
      return false;
    }
  }
  return true;
}

void printUID(byte UID[UID_LENGTH]){
  Serial.print("UID:");
  for (int i = 0; i < UID_LENGTH; i++) {
    Serial.print("0x");
    Serial.print(UID[i], HEX);
    Serial.print(",");
  }
  Serial.println("");
}

unsigned int testIfUIDinArray(byte BigArray[UID_NUMBER][UID_LENGTH], byte UID[UID_LENGTH])
{
  for (int i = 0; i < UID_NUMBER; i++){
    if (testIfUIDEqual(BigArray[i], UID)){
      printUID(hexNum);
      return i;
    }
  }
  return -1;
}

//---------------------- Buzzer code
void victorySound(){
  int Melody[] = {
    NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
  };

  int Notes[] = {
    4, 8, 8, 4, 4, 4, 4, 4
    };


  const int num_elements_in_arr = sizeof(Notes)/sizeof(Notes[0]);
  
  // iterate over the notes of the melody:
  for (int ThisNote = 0; ThisNote < num_elements_in_arr; ThisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 500/Notes[ThisNote];
    tone(BUZZER_PIN, Melody[ThisNote],noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(BUZZER_PIN);
  }
}


void victoryBeepSound(){
  int Melody[] = {
    NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3
  };

  int Notes[] = {
    4, 8, 8, 4, 4
    };


  const int num_elements_in_arr = sizeof(Notes)/sizeof(Notes[0]);
  
  // iterate over the notes of the melody:
  for (int ThisNote = 0; ThisNote < num_elements_in_arr; ThisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 500/Notes[ThisNote];
    tone(BUZZER_PIN, Melody[ThisNote],noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(BUZZER_PIN);
  }
}

void sadSound(){
  Serial.println("COOL! I'm running");
  int Melody[] = {
    NOTE_G4, NOTE_C4
  };

  int Notes[] = {
    2, 4
    };


  const int num_elements_in_arr = sizeof(Notes)/sizeof(Notes[0]);
  
  // iterate over the notes of the melody:
  for (int ThisNote = 0; ThisNote < num_elements_in_arr; ThisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 500/Notes[ThisNote];
    tone(BUZZER_PIN, Melody[ThisNote],noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(BUZZER_PIN);
  }
}

void periodMorse(){
  tone(BUZZER_PIN, NOTE_C6, morsePeriodDuration);
}

void dashMorse(){
  tone(BUZZER_PIN, NOTE_C3, morseDashDuration);
}

void codedMessage(){
  //AAAAAAAAAA
  periodMorse();
  delay(morseINLetterInterval);
  dashMorse();
  
  delay(morseLetterInterval);

  //OOOOOOOO
  dashMorse();
  delay(morseINLetterInterval);
  dashMorse();
  delay(morseINLetterInterval);
  dashMorse();


  delay(morseLetterInterval);
  noTone(BUZZER_PIN);
  }


//---------------------- Real code

void setup() {
  Serial.begin(9600);
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522
  toggleSwitch.setDebounceTime(50); // set debounce time to 50 milliseconds

  resetAlreadySeen(AlreadySeenUID); // AlreadySeen reset with all false
  initAllLEDs(); //Define mode for all leds

  blinkAllLEDsOFF(2);//Test LEDs
  tone(BUZZER_PIN, NOTE_G4, 200); //Test buzzer
  noTone(BUZZER_PIN);
  
  //victorySound();
  codedMessage();
}

void loop() {
  if (rfid.PICC_IsNewCardPresent() and OnLEDs!=LED_NUMBER) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

      for (int i = 0; i < rfid.uid.size; i++) {
        //Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        //Serial.print(rfid.uid.uidByte[i], HEX);
        hexNum[i] = rfid.uid.uidByte[i];
      }
      
      unsigned int k = testIfUIDinArray(HexNumArray, hexNum);
      if(k >= 0){
        Serial.print("Already Seen: ");
        Serial.println(AlreadySeenUID[k]);
        if(!AlreadySeenUID[k]){
          powerOnPlusOneLED(&OnLEDs);
          victoryBeepSound();
        }
        AlreadySeenUID[k] = true;
      }
      else{
        Serial.println("Card not found");
      }

      Serial.print("OnLEDs: ");
      Serial.println(OnLEDs);
      
      if(OnLEDs==LED_NUMBER){
        victorySound();
        blinkAllLEDsON(5);
      }
      Serial.println("---------");
      
      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }

  toggleSwitch.loop();
  int state = toggleSwitch.getState();
//  if (OnLEDs==LED_NUMBER and state != HIGH){
  if (state != HIGH){
    Serial.println("playing");
    codedMessage();
  }
}
