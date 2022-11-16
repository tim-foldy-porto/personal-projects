void setup() {
  Serial.begin (115200);
  pinMode (2, OUTPUT);
  pinMode (15, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    int readByte = Serial.read();

    if (readByte == 97) {
      Serial.println ("outputs high");
      digitalWrite (15, HIGH);
      digitalWrite (2, HIGH);
    }
    if (readByte == 98) {
      Serial.println ("outputs low");
      digitalWrite (15, LOW);
      digitalWrite (2, LOW);
    }
  }
}
