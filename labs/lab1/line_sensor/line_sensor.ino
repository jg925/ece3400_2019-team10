// function to test the line sensor

int LINE_SENSOR = A4;

void setup() {
  // put your setup code here, to run once:
  pinMode(LINE_SENSOR, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(analogRead(LINE_SENSOR));
  delay(500);
}
