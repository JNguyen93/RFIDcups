#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN   9
#define SS_PIN    10

const char* master = "65F42D6A";

const char* cup0 = "95DD306A";
const char* cup0bu = "15F42D6A";

const char* cup1 = "951358C9";
const char* cup1bu = "B5F32D6A";
.
const char* cup2 = "65C72F6A";
const char* cup2bu = "C5322F6A";

const char* cup3 = "C5C72F6A";
const char* cup3bu = "5C72F6A";

const byte maglock = 7;
byte readCard[4];
char* approvedCup0 = cup2;
char* approvedCup1 = cup2bu;
char* myTags[100] = {};
char* preapproved[3] = {master, approvedCup0, approvedCup1};
int tagsCount = 0;
String tagID = "";
boolean successRead = false;
boolean correctTag = false;

// Create instances
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  // Initiating
  SPI.begin();        // SPI bus
  mfrc522.PCD_Init(); //  MFRC522
  Serial.begin(9600);
  Serial.println("RFID Reader");
  pinMode(maglock, OUTPUT);

  myTags[tagsCount] = strdup(master); // Sets the master tag into position 0 in the array
  Serial.print("Master Tag is ");
  Serial.println(master);
  tagsCount++;
}

void loop() {
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return;
  }
  tagID = "";
  // The MIFARE PICCs that we use have 4 byte UID
  for ( uint8_t i = 0; i < 4; i++) {  //
    readCard[i] = mfrc522.uid.uidByte[i];
    tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
  }
  tagID.toUpperCase();
  Serial.println(tagID);
  mfrc522.PICC_HaltA(); // Stop reading

  correctTag = false;
  // Checks whether the scanned tag is the master tag
  if (tagID == master) {
    Serial.print("Program mode:");
    Serial.println(" Add/Remove Tag");
    while (!successRead) {
      tagID = "";
      successRead = getID();
      if (tagID == myTags[0]) {
        Serial.println("Exiting");
        return;
      }
      else {
        if ( successRead == true) {
          for (int i = 1; i < 100; i++) {
            if (tagID == myTags[i]) {
              myTags[i] = "";
              Serial.println("Tag Removed!");
              return;
            }
          }
          myTags[tagsCount] = strdup(tagID.c_str());
          Serial.println("Tag Added!");
          tagsCount++;
          return;
        }
      }
    }
    successRead = false;
  }
  else {
    // Checks whether the scanned tag is authorized
    Serial.print("Validating tagID: ");
    Serial.println(tagID);
    for (int j = 0; j < 3; j++) {
      if (tagID == preapproved[j]) {
        Serial.println("Access Granted!");
        Serial.println("Preapproved Tag.");
        correctTag = true;
      }
    }
    if (!correctTag) {
      for (int i = 0; i < 100; i++) {
        if (tagID == myTags[i]) {
          Serial.println("Access Granted!");
          Serial.println("Added Tag.");
          correctTag = true;
        }
      }
    }
    if (!correctTag) {
      Serial.println("Access Denied.");
      Serial.println("LOW");
      digitalWrite(maglock, HIGH);
    }
    else{
      Serial.println("HIGH");
      digitalWrite(maglock, LOW);
    }
  }
}

uint8_t getID() {
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return 0;
  }
  tagID = "";
  for ( uint8_t i = 0; i < 4; i++) {  // The MIFARE PICCs that we use have 4 byte UID
    readCard[i] = mfrc522.uid.uidByte[i];
    tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
  }
  tagID.toUpperCase();
  mfrc522.PICC_HaltA(); // Stop reading
  return 1;
}
