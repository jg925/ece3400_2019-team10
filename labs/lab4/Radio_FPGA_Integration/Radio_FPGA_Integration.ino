#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

// 000rvwwwxxxxyyyy

/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Example for Getting Started with nRF24L01+ radios.
 *
 * This is an example of how to use the RF24 class.  Write this sketch to two
 * different nodes.  Put one of the nodes into 'transmit' mode by connecting
 * with the serial monitor and sending a 'T'.  The ping node sends the current
 * time to the pong node, which responds by sending the value back.  The ping
 * node can then see how long the whole cycle took.
 */

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(9,10);

//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0x0000000016LL, 0x0000000017LL };

//
// Role management
//
// Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.
//

// The various roles supported by this sketch
typedef enum { role_ping_out = 1, role_pong_back } role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};

// The role of the current running sketch
role_e role = role_pong_back;

int output  = 7;
int x_pos_1 = 2;
int x_pos_2 = 3;
int x_pos_3 = 4;
//int x_pos_4 =  ;
int y_pos_1 = 5;
int y_pos_2 = 6;
int y_pos_3 = 8;
//int y_pos_4 =  ;

void drawSquare(int x1,int x2,int x3, int x4, int y1,int y2,int y3, int y4) {
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

}

void setup() {
  // put your setup code here, to run once:
  //Radio setup
  //
  // Print preamble
  //

  Serial.begin(57600);
  printf_begin();

  //
  // Setup and configure rf radio
  //

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  radio.setAutoAck(true);
  // set the channel
  radio.setChannel(0x50);
  // set the power
  // RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
  radio.setPALevel(RF24_PA_HIGH);
  //RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.setDataRate(RF24_250KBPS);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  radio.setPayloadSize(2);

  //
  // Open pipes to other nodes for communication
  //

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)
  
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);
  

  //
  // Start listening
  //

  radio.startListening();

  //
  // Dump the configuration of the rf unit for debugging
  //

  radio.printDetails();
  
  //other
  pinMode( output, OUTPUT ); //valid output
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
//aaaaNESWxxxxyyyy 16bit of data from radio?

uint16_t radio(void) {
// if there is data ready
  if ( radio.available() ){
    
    // Dump the payloads until we've gotten everything
    uint16_t info;
    bool done = false;
    while (!done)
    {
      // Fetch the payload, and see if this was the last one.
      done = radio.read( &info, sizeof(uint16_t) );

      // Spew it
      uint16_t xcord = (info & B11110000) >> 4 ;
      uint16_t ycord = (info & B00001111)      ;
      uint16_t walls = (info >> 8)  & B00000111;
      uint16_t visit = (info >> 11) & B00010000;
      uint16_t robot = (info >> 12) & B00001000;
      
      printf("Got payload... ");
      printf("message: %x", info);
      printf("\n");
      printf("x-coord: %d", xcord );
      printf(" y-coord: %d", ycord );
      printf(" walls: %d",   walls );
      printf(" visited: %d", visit );
      printf(" robot: %d",   robot );
      printf("\n");        
    }

    // First, stop listening so we can talk
    radio.stopListening();

    // Send the final one back.
    radio.write( &info, sizeof(uint16_t) );
    printf("Sent response.\n\r");

    // Now, resume listening so we catch the next packets.
    radio.startListening();
  }
}

void loop(void) {
  

  
  // put your main code here, to run repeatedly:
  digitalWrite( output, LOW );
//  delay(1000);
//  delay(10000);
//  delay(10000);
//  drawSquare(HIGH,LOW,HIGH,LOW,HIGH,LOW);
//  delay(1000);
//  digitalWrite( output, HIGH);
//  digitalWrite( output, LOW );
//  digitalWrite( output, HIGH);
//  drawSquare(LOW,LOW,HIGH,LOW,LOW,HIGH);
//  digitalWrite( output, HIGH);
//  delay(10000);
//  digitalWrite( output, LOW);
//  drawSquare(LOW,HIGH,LOW,LOW,LOW,HIGH);
//  digitalWrite( output, HIGH);
//  delay(5000);
//  digitalWrite( output, LOW);
//  drawSquare(LOW,HIGH,HIGH,LOW,LOW,HIGH);
//  digitalWrite( output, HIGH);
//  delay(5000);
//  digitalWrite( output, LOW);
//  drawSquare(LOW,HIGH,HIGH,LOW,HIGH,LOW);
//  digitalWrite( output, HIGH);
//  delay(5000);
//  digitalWrite( output, LOW);
//  drawSquare(LOW,HIGH,HIGH,LOW,HIGH,HIGH);
//  digitalWrite( output, HIGH);
//  delay(5000);
//  digitalWrite( output, LOW );
//  drawSquare(LOW,HIGH,LOW,LOW,HIGH,HIGH);
//  digitalWrite( output, HIGH);
//  delay(5000);
//  digitalWrite( output, LOW );
//  drawSquare(LOW,LOW,HIGH,LOW,HIGH,HIGH);
//  digitalWrite( output, HIGH);
//  delay(5000);
//  digitalWrite( output, LOW );
//  drawSquare(LOW,LOW,HIGH,LOW,HIGH,LOW);
//  digitalWrite( output, HIGH);
//  delay(5000);
//  digitalWrite( output, LOW );
  while(1)
  {
    radio;
  }
}
