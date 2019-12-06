
int valid  = 7;
//int bot_valid = 0;
int x_pos_1 = 2;
int x_pos_2 = 3;
int x_pos_3 = 4;
int y_pos_1 = 5;
int y_pos_2 = 6;
int y_pos_3 = 8;

void drawBot(int N, int S, int E, int W) 
{
  digitalWrite( A0, N);
  digitalWrite( A1, E);
  digitalWrite( A2, S);
  digitalWrite( A3, W);
}

void drawSquare(int x1,int x2,int x3, int x4, int y1,int y2,int y3, int y4, int N, int S, int E, int W) {
  // x coordinates
  digitalWrite( x_pos_1, x1 );
  digitalWrite( x_pos_2, x2 );
  digitalWrite( x_pos_3, x3);
  digitalWrite( A4, x4);

  // y coordinates
  digitalWrite( y_pos_1, y1 );
  digitalWrite( y_pos_2, y2 );
  digitalWrite( y_pos_3, y3 );
  digitalWrite( A5, y4 );

  // WALLS
  digitalWrite( A0, N);
  digitalWrite( A1, E);
  digitalWrite( A2, S);
  digitalWrite( A3, W);
}

void setup() {
  // put your setup code here, to run once:
  pinMode( valid, OUTPUT );
//  pinMode( bot_valid, OUTPUT );
  pinMode( LED_BUILTIN, OUTPUT );
  pinMode( x_pos_1, OUTPUT );
  pinMode( y_pos_1, OUTPUT );
  pinMode( x_pos_2, OUTPUT );
  pinMode( y_pos_2, OUTPUT );
  pinMode( x_pos_3, OUTPUT );
  pinMode( y_pos_3, OUTPUT );
  pinMode( A4, OUTPUT ); //x4
  pinMode( A5, OUTPUT ); //y4
  pinMode( A0, OUTPUT ); //NORTH
  pinMode( A1, OUTPUT ); //EAST
  pinMode( A2, OUTPUT ); //SOUTH
  pinMode( A3, OUTPUT ); //WEST
}
//aaasNESWxxxxyyyy 16bit of data from radio?

void loop() {
  // put your main code here, to run repeatedly:
  delay(10000);
  digitalWrite( valid, LOW );
//  digitalWrite( bot_valid, LOW);
  
  drawSquare(LOW,LOW,LOW,HIGH,LOW,LOW,LOW,HIGH,HIGH,LOW,LOW,HIGH);
  digitalWrite( valid, HIGH);
  delay(1000);
  
  digitalWrite( valid, LOW );
//  digitalWrite( bot_valid, HIGH);
//  drawBot(LOW,LOW,HIGH,LOW);
  digitalWrite( valid, HIGH);
  delay(2000);
  digitalWrite( valid, LOW);
//  digitalWrite( bot_valid, LOW);
  drawSquare(LOW,LOW,HIGH,LOW,LOW,LOW,LOW,HIGH,HIGH,HIGH,LOW,LOW);
  digitalWrite( valid, HIGH);
  delay(2000);
  digitalWrite( valid, LOW);
  drawSquare(LOW,LOW,HIGH,HIGH,LOW,LOW,LOW,HIGH,HIGH,LOW,HIGH,LOW);
  digitalWrite( valid, HIGH);
  delay(1000);
  digitalWrite( valid, LOW );
//  drawBot(LOW,HIGH,LOW,LOW);
//  digitalWrite( bot_valid, HIGH);
  digitalWrite( valid, HIGH);
  delay(2000);
  digitalWrite( valid, LOW);
//  digitalWrite( bot_valid, LOW);
  drawSquare(LOW,LOW,HIGH,HIGH,LOW,LOW,HIGH,LOW,LOW,LOW,HIGH,HIGH);
  digitalWrite( valid, HIGH);
  delay(2000);
  digitalWrite( valid, LOW);
  drawSquare(LOW,LOW,HIGH,HIGH,LOW,LOW,HIGH,HIGH,LOW,HIGH,HIGH,LOW);
  digitalWrite( valid, HIGH);
  delay(2000);
  digitalWrite( valid, LOW );
  drawSquare(LOW,LOW,HIGH,LOW,LOW,LOW,HIGH,HIGH,HIGH,HIGH,LOW,LOW);
  digitalWrite( valid, HIGH);
  delay(2000);
  digitalWrite( valid, LOW );
  drawSquare(LOW,LOW,LOW,HIGH,LOW,LOW,HIGH,HIGH,LOW,HIGH,LOW,HIGH);
  digitalWrite( valid, HIGH);
  delay(2000);
  digitalWrite( valid, LOW );
  drawSquare(LOW,LOW,LOW,HIGH,LOW,LOW,HIGH,LOW,LOW,LOW,HIGH,HIGH);
  digitalWrite( valid, HIGH);
  delay(2000);
  digitalWrite( valid, LOW );
  while(1)
  {
  }
}
