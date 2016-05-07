int led = 2;

void setup() {
  // put your setup code here, to run once:
  pinMode(led, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < 256; i++) {
    analogWrite(led, i);
    delay(50);
  }
}
