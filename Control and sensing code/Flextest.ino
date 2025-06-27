const int control0Pin = 2;  // Control pin 0 (Pump 1 Up)
const int control1Pin = 3;  // Control pin 1 (Pump 1 Down)
const int control2Pin = 4;  // Control pin 2 (Pump 2 Up)
const int control3Pin = 5;  // Control pin 3 (Pump 2 Down)
const int control4Pin = 6;  // Control pin 4 (Pump 3 Up)
const int control5Pin = 7;  // Control pin 5 (Pump 3 Down)
const int control6Pin = 8;  // Control pin 6 (Pump 4 Up)
const int control7Pin = 9;  // Control pin 7 (Pump 4 Down)

int mode = 1;  // ← Change this to 1, 2, or 3 for different pump modes

void setup() {
  pinMode(control0Pin, OUTPUT);
  pinMode(control1Pin, OUTPUT);
  pinMode(control2Pin, OUTPUT);
  pinMode(control3Pin, OUTPUT);
  pinMode(control4Pin, OUTPUT);
  pinMode(control5Pin, OUTPUT);
  pinMode(control6Pin, OUTPUT);
  pinMode(control7Pin, OUTPUT);

  motorStop();
  Serial.begin(9600);
}

void loop() {

      //  motorStop();

  if (mode == 1) {
    // Mode 1: All pumps forward
   motorForward(1);
  motorForward(2);
   motorForward(3);
   motorForward(4);
    printPumpStates();
    delay(8000);

    motorStop();
    printPumpStates();
    delay(10);

    // All pumps backward
    motorBackward(1);
    motorBackward(2);
   motorBackward(3);
   motorBackward(4);
    printPumpStates();
    delay(8000);

    motorStop();
    printPumpStates();
    delay(10);

  } else if (mode == 2) {
// Step 1: Move each pump DOWN (inflate)
for (int i = 1; i <= 4; i++) {
  Serial.print("Inflating pump ");
  Serial.println(i);
  motorForward(i);         // Move pump DOWN
  printPumpStates();
  delay(3000);             // Wait long enough to inflate
  motorStop();             // Stop movement
  printPumpStates();
  delay(500);              // Short pause before next pump
}

// Step 2: Move each pump UP (deflate)
for (int i = 1; i <= 4; i++) {
  Serial.print("Deflating pump ");
  Serial.println(i);
  motorBackward(i);        // Move pump UP
  printPumpStates();
  delay(3000);             // Wait long enough to deflate
  motorStop();
  printPumpStates();
  delay(500);
}

  } else if (mode == 3) {
    // Mode 3: Fast switching
    for (int i = 2; i <= 3; i++) {
      motorForward(i);
      printPumpStates();
      delay(500);
      motorStop();
      delay(100);
    }
    for (int i = 2; i <= 3; i++) {
      motorBackward(i);
      printPumpStates();
      delay(500);
      motorStop();
      delay(100);
    }
  }
}

void motorForward(int pumpNumber) {
  switch(pumpNumber) {
    case 1:
      digitalWrite(control0Pin, HIGH);
      digitalWrite(control1Pin, LOW);
      break;
    case 2:
      digitalWrite(control2Pin, HIGH);
      digitalWrite(control3Pin, LOW);
      break;
    case 3:
      digitalWrite(control4Pin, HIGH);
      digitalWrite(control5Pin, LOW);
      break;
    case 4:
      digitalWrite(control6Pin, HIGH);
      digitalWrite(control7Pin, LOW);
      break;
  }
}

void motorBackward(int pumpNumber) {
  switch(pumpNumber) {
    case 1:
      digitalWrite(control0Pin, LOW);
      digitalWrite(control1Pin, HIGH);
      break;
    case 2:
      digitalWrite(control2Pin, LOW);
      digitalWrite(control3Pin, HIGH);
      break;
    case 3:
      digitalWrite(control4Pin, LOW);
      digitalWrite(control5Pin, HIGH);
      break;
    case 4:
      digitalWrite(control6Pin, LOW);
      digitalWrite(control7Pin, HIGH);
      break;
  }
}

void motorStop() {
  digitalWrite(control0Pin, LOW);
  digitalWrite(control1Pin, LOW);
  digitalWrite(control2Pin, LOW);
  digitalWrite(control3Pin, LOW);
  digitalWrite(control4Pin, LOW);
  digitalWrite(control5Pin, LOW);
  digitalWrite(control6Pin, LOW);
  digitalWrite(control7Pin, LOW);
}

void printPumpStates() {
  Serial.print("Pump States: ");
  Serial.print("Pump 1: ");
  if (digitalRead(control0Pin)) Serial.print("omlaag ");
  else if (digitalRead(control1Pin)) Serial.print("omhoog ");
  else Serial.print("stopped ");

  Serial.print("Pump 2: ");
  if (digitalRead(control2Pin)) Serial.print("omlaag ");
  else if (digitalRead(control3Pin)) Serial.print("omhoog ");
  else Serial.print("stopped ");

  Serial.print("Pump 3: ");
  if (digitalRead(control4Pin)) Serial.print("omlaag ");
  else if (digitalRead(control5Pin)) Serial.print("omhoog ");
  else Serial.print("stopped ");

  Serial.print("Pump 4: ");
  if (digitalRead(control6Pin)) Serial.print("omlaag ");
  else if (digitalRead(control7Pin)) Serial.print("omhoog ");
  else Serial.print("stopped ");

  Serial.println();
  delay(100);
}
