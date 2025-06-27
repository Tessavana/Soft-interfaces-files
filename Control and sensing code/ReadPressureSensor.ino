void setup() {
  Serial.begin(9600);
}

void loop() {
  for (int i = 0; i < 16; i++) {
    // read A0-A15
    int val = analogRead(i);
    Serial.print(val);

    if (i < 15) Serial.print(',');
  }
  Serial.println();     // line terminator
  delay(50);           
}
