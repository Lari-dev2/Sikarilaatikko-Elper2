#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

Servo lockServo;
#define SERVO_PIN 6
#define LOCKED_POS 0    
#define UNLOCKED_POS 90 
bool isLocked = true;    

#define GREEN_LED 7
#define RED_LED 8
#define BUZZER 5  

struct User {
    byte uid[4];
    const char* name;
};

User authorizedUsers[] = {
    {{0xB3, 0x7B, 0xCA, 0x1B}, "Oikea lätkä"},
    {{0xA1, 0xB2, 0xC3, 0xD4}, "Vara1"},
    {{0x11, 0x22, 0x33, 0x44}, "Vara2"},
    {{0x99, 0x88, 0x77, 0x66}, "Vara3"}
};

const int userCount = sizeof(authorizedUsers) / sizeof(User);

void setup() {
    Serial.begin(9600);
    SPI.begin();
    mfrc522.PCD_Init();

    lockServo.attach(SERVO_PIN);
    lockServo.write(LOCKED_POS);

    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    pinMode(BUZZER, OUTPUT);
}

void loop() {
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        const char* userName = checkUID();
        if (userName != nullptr) {
            toggleLock();
        } else {
            wrongTag();
        }
        mfrc522.PICC_HaltA();
    }
}

const char* checkUID() {
    if (mfrc522.uid.size != 4) return nullptr;
    for (int i = 0; i < userCount; i++) {
        bool match = true;
        for (byte j = 0; j < 4; j++) {
            if (mfrc522.uid.uidByte[j] != authorizedUsers[i].uid[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            return authorizedUsers[i].name;
        }
    }
    return nullptr;
}

void toggleLock() {
    if (isLocked) {
        lockServo.write(UNLOCKED_POS);
        digitalWrite(GREEN_LED, HIGH);
        isLocked = false;
    } else {
        lockServo.write(LOCKED_POS);
        digitalWrite(GREEN_LED, HIGH);
        isLocked = true;
    }
    delay(1000);
    digitalWrite(GREEN_LED, LOW);
}

void wrongTag() {
    digitalWrite(RED_LED, HIGH);
    tone(BUZZER, 500);
    delay(1000);
    noTone(BUZZER);
    digitalWrite(RED_LED, LOW);
}