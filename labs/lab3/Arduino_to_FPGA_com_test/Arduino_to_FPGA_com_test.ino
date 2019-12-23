int output  = 13;
int x_pos_1 = 12;
int x_pos_2 = 10;
int y_pos_1 = 11;
int y_pos_2 =  9;

int x = 0;
int y = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode( output, OUTPUT );
  pinMode( LED_BUILTIN, OUTPUT );
  pinMode( x_pos_1, OUTPUT );
  pinMode( y_pos_1, OUTPUT );
  pinMode( x_pos_2, OUTPUT );
  pinMode( y_pos_2, OUTPUT );
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite( output, HIGH );
  digitalWrite( LED_BUILTIN, HIGH );

  // x coordinates
  digitalWrite( x_pos_1, HIGH );
  digitalWrite( x_pos_2, HIGH );

  // y coordinates
  digitalWrite( y_pos_1, HIGH );
  digitalWrite( y_pos_2, LOW );
}
