// function to test the line sensor

int LINE_SENSOR_L = A5;
int LINE_SENSOR_R = A4;

void setup() {
  // put your setup code here, to run once:
  pinMode(LINE_SENSOR_L, INPUT);
  pinMode(LINE_SENSOR_R, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
//  Serial.println("left:");
//  Serial.println(analogRead(LINE_SENSOR_L));
  Serial.println("right:");
  Serial.println(analogRead(LINE_SENSOR_R));
  delay(500);
}
