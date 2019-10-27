
#include <Servo.h>

Servo left;
Servo right;

// servo pins
int right_pin = 5;
int left_pin = 6;

// phototransistor pins
int photoPin = A3;

int pi_arr[10];

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
  //Serial.println(photo_input);
  //delay(10000);

  pi_arr[9] = photo_input;

  Serial.println("ARRAY START");
  for (int i= 0; i < 10; i++) {
    Serial.println(pi_arr[i]);
  }
  Serial.println("ARRAY END");
  delay(1000);
  
  for (int i= 1; i < 10; i++) {
    pi_arr[i-1] = pi_arr[i];
  }
  
  if (photo_input >= 5) {
    //halt();
    //Serial.println("DETECT");
  } else {
    //moveForward();
  }
  
}
