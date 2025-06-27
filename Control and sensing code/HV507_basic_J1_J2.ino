#include <SPI.h>

#define HV_LE 4
#define DIR   7
#define BLK   6
#define POL   5

const uint8_t patternHVOUT9[8] = {
  0x00, // [0] HVOUT64–57
  0x00, // [1] HVOUT56–49
  0x00, // [2] HVOUT48–41
  0x00, // [3] HVOUT40–33
  0x01, // [4] HVOUT32–25
  0x00, // [5] HVOUT24–17
  0x00, // [6] HVOUT16–9 → bit 7 = HVOUT9 = HIGH
  0x00  // [7] HVOUT8–1
};


void setup() {
  pinMode(HV_LE, OUTPUT);
  pinMode(DIR,   OUTPUT);
  pinMode(BLK,   OUTPUT);
  pinMode(POL,   OUTPUT);

  digitalWrite(HV_LE, LOW);
  digitalWrite(DIR,   LOW);
  digitalWrite(BLK,   HIGH);  // enable outputs
  digitalWrite(POL,   HIGH);  // logic HIGH = VPP

  SPI.begin();
  Serial.begin(115200);
  Serial.println(F("→ Setting HVOUT9 HIGH"));

  sendToHV507(patternHVOUT9);
}

void loop() {
  // Do nothing
}

void sendToHV507(const uint8_t* data) {
  SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));
  for (uint8_t i = 0; i < 8; i++) {
    SPI.transfer(data[i]);
  }
  SPI.endTransaction();

  digitalWrite(HV_LE, LOW);
  delayMicroseconds(1);
  digitalWrite(HV_LE, HIGH);
  delayMicroseconds(1);
  digitalWrite(HV_LE, LOW);
}
