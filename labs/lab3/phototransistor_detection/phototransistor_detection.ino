
#include <Servo.h>

Servo left;
Servo right;

// servo pins
int right_pin = 6;
int left_pin = 5;

// phototransistor pins
int photoPin = A1;

void halt() {
  left.write(90);
  right.write(90);
}

void moveForward() {
  left.write(180);
  right.write(0);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(photoPin, INPUT);
  Serial.begin(9600);
  right.attach(right_pin);
  left.attach(left_pin);
}

void loop() {
  int photo_input = analogRead(photoPin);
  Serial.println(photo_input);
  delay(100);

  if (photo_input >= 60) {
    moveForward();
    Serial.println("DETECT");
  } else {
    halt();
  }
  
}
