int irLedPin = 5;
int ledPin = 3;
int photo_input = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(irLedPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  photo_input = analogRead(0);
  Serial.println(analogRead(0));
  analogWrite(ledPin, photo_input/10);
  digitalWrite(irLedPin, HIGH);
  delay(100);
}
