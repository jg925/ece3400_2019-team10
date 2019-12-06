// 000swwwwxxxxyyyy

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

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

// pins for base station arduino 

int valid   = 7;
int x_pos_1 = 2;
int x_pos_2 = 3;
int x_pos_3 = 4;
int y_pos_1 = 5;
int y_pos_2 = 6;
int y_pos_3 = 8;

// function that draws a square in the given position (x,y) with walls

void drawSquare(int x1,int x2,int x3, int x4, int y1,int y2,int y3, int y4, int N, int E, int S, int W) {
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

void setup(void)
{
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

  // setting up pins for communication with the FPGA

  pinMode( valid, OUTPUT );
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

void loop(void) {
  
  byte x1;
  byte x2;
  byte x3;
  byte x4;
  
  byte y1;
  byte y2;
  byte y3;
  byte y4;

  byte north_wall;
  byte east_wall;
  byte south_wall;
  byte west_wall;

  byte global_sent;
  
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
      uint16_t walls = (info >> 8)  & B00001111;
      uint16_t sent  = (info >> 12) & B00000001;
//      uint16_t robot = (info >> 12) & B00001000;

      x1 = (xcord >> 3) & B00000001;
      x2 = (xcord >> 2) & B00000001;
      x3 = (xcord >> 1) & B00000001; 
      x4 = xcord & B00000001;

      y1 = (ycord >> 3) & B00000001;
      y2 = (ycord >> 2) & B00000001;
      y3 = (ycord >> 1) & B00000001; 
      y4 = ycord & B00000001;

      north_wall = (walls >> 3) & B00000001;
      east_wall  = (walls >> 2) & B00000001;
      south_wall = (walls >> 1) & B00000001; 
      west_wall  = walls & B00000001;

      global_sent = sent;
      
      printf("Got payload... ");
      printf("message: %x", info);
      printf("\n");
      printf("\nx-coord: %d", xcord );
      printf("\ny-coord: %d", ycord );
      printf("\nwalls: %d",   walls );
      printf("\nsent: %d", sent );
//      printf(" robot: %d",   robot );
      printf("\n\n");        
    }

    // First, stop listening so we can talk
    radio.stopListening();

    // Send the final one back.
    radio.write( &info, sizeof(uint16_t) );
//    printf("Sent response.\n\r");

    // Now, resume listening so we catch the next packets.
    radio.startListening();
  }

  // setting valid bit to LOW before we send the data to draw

  digitalWrite( valid, LOW );

  // sending the data to draw the square, then setting valid bit to HIGH
  if (!global_sent) {
    drawSquare(x1, x2, x3, x4, y1, y2, y3, y4, north_wall, east_wall, south_wall, west_wall);
  }
  
  digitalWrite( valid, HIGH );
  delay(100);
  digitalWrite( valid, LOW );
  
}
// vim:cin:ai:sts=2 sw=2 ft=cpp
