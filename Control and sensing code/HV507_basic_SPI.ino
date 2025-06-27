#include <SPI.h>

#define HV_LE 4   // Latch Enable pin
#define DIR   7   // Shift direction: DIOA → HVOUT64 first
#define BLK   6   // Blanking (HIGH = outputs enabled)
#define POL   5   // Polarity (HIGH = “1” → VPP)

const uint8_t patternJ1[8] = {
  0x00, // [0]
  0x00, // [1]
  0x00, // [2]
  0xFF, // [3] → HVOUT33–40 HIGH (J1-9…16)
  0xFF, // [4] → HVOUT25–32 HIGH (J1-1…8)
  0x00, // [5]
  0x00, // [6]
  0x00  // [7]
};

///— Pattern 2: J2 (pins 49–64 → HVOUT9–24) = HIGH, J1 (HVOUT25–40) = LOW
const uint8_t patternJ2[8] = {
  0x00, // [0]
  0xFF, // [1] → HVOUT17–24 HIGH (J2-9…16)
  0xFF, // [2]→ HVOUT9–16  HIGH (J2-1…8)
  0x00, // [3]
  0x00, // [4]
  0x00, // [5]
  0x00, // [6] 
  0x00  // [7]
};

void setup() {
  pinMode(HV_LE, OUTPUT);
  pinMode(DIR,   OUTPUT);
  pinMode(BLK,   OUTPUT);
  pinMode(POL,   OUTPUT);

  digitalWrite(HV_LE, LOW);
  digitalWrite(DIR,   LOW);
  digitalWrite(BLK,   HIGH);
  digitalWrite(POL,   HIGH);

  SPI.begin();
  Serial.begin(115200);
  Serial.println(F("Toggle J1 vs J2 every 2 s"));
}

void loop() {
  Serial.println(F("→ J1 HIGH, J2 LOW"));
  sendToHV507(patternJ1);
  delay(5000);

  Serial.println(F("→ J2 HIGH, J1 LOW"));
  sendToHV507(patternJ2);
  delay(5000);
}

void sendToHV507(const uint8_t* data) {
  SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));
  for (uint8_t i = 0; i < 8; i++) {
    SPI.transfer(data[i]);
  }
  SPI.endTransaction();

  // LE pulse: LOW→HIGH→LOW
  digitalWrite(HV_LE, LOW);
  delayMicroseconds(1);
  digitalWrite(HV_LE, HIGH);
  delayMicroseconds(1);
  digitalWrite(HV_LE, LOW);
}