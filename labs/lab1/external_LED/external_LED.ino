// A simple Arduino program to make an external LED blink

int EXTERNAL_LED = 13;
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(EXTERNAL_LED, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(EXTERNAL_LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(EXTERNAL_LED, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
