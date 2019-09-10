// This program allows us to continuous read the voltage across a voltage divider by alternating the potentiometer

int VOLT_DIVIDE = A0;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin VOLT_DIVIDE as an output.
  pinMode(VOLT_DIVIDE, INPUT);
  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {
  Serial.println(analogRead(VOLT_DIVIDE));    // Print out the value being read from the analog pin
  delay(500);                                 // wait for half a second
}
