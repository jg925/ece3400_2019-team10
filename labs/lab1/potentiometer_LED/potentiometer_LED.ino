// This program allows us to control the brightness of an LED by turning the potentiometer

int VOLT_DIVIDE = A5;
int LED = 3;
int v = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(VOLT_DIVIDE, INPUT);
  pinMode(LED, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  v = analogRead(VOLT_DIVIDE);
  analogWrite(LED, v/2);
}
