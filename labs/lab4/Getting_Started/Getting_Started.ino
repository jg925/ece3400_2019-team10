// 0ddrvwwwxxxxyyyy

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
role_e role = role_ping_out;


byte maze[10][10] =

{
  B110, B000, B111, B010, B110, B010, B010, B111, B000, B001,
  B110, B000, B111, B010, B110, B010, B010, B111, B000, B001,
  B110, B000, B111, B010, B110, B010, B010, B111, B000, B001,
  B110, B000, B111, B010, B110, B010, B010, B111, B000, B001,
  B110, B000, B111, B010, B110, B010, B010, B111, B000, B001,
  B110, B000, B111, B010, B110, B010, B010, B111, B000, B001,
  B110, B000, B111, B010, B110, B010, B010, B111, B000, B001,
  B110, B000, B111, B010, B110, B010, B010, B111, B000, B001,
  B110, B000, B111, B010, B110, B010, B010, B111, B000, B001,
  B110, B000, B111, B010, B110, B010, B010, B111, B000, B001,
};


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

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)

  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  
  //
  // Start listening
  //

  radio.startListening();

  //
  // Dump the configuration of the rf unit for debugging
  //

  radio.printDetails();
}

byte robot = B000;
uint16_t msg = 0000000000000000;
int x = 0;
int y = 0;

void loop(void)
{
  // simulate robot in maze
  robot = maze[x][y];

  msg = 0000000000000000;
  msg = (msg << 1) | 1;
  msg = (msg << 1) | 0;
  msg = (msg << 3) | robot;
  msg = (msg << 4) | x;
  msg = (msg << 4) | y;
  
  printf("\ncoord: ");
  printf("%d", x);
  printf(", %d \n", y);
  printf("walls: ");
  printf("%x", robot);
  printf("\nmsg: ");
  printf("%x", msg);
  printf("\n");

  // simulate robot in maze
  if (x == 9 && y == 9) {
    x = 0;
    y = 0;
  } else if (x %2 == 0) {
    if (y == 9) {
      x++;
    } else {
      y++;
    }
  } else {
    if (y == 0) {
      x++;
    } else {
      y--;
    }
  } 

  // First, stop listening so we can talk.
  radio.stopListening();

  printf("Now sending %x...",msg);
  bool ok = radio.write( &msg, sizeof(uint16_t) );

  if (ok)
    printf("ok...");
  else
    printf("failed.\n\r");

  // Now, continue listening
  radio.startListening();

  // Wait here until we get a response, or timeout (250ms)
  unsigned long started_waiting_at = millis();
  
  bool timeout = false;
  while ( ! radio.available() && ! timeout )
    if (millis() - started_waiting_at > 200 )
      timeout = true;

  // Describe the results
  if ( timeout )
  {
    printf("Failed, response timed out.\n\r");
  }
  else
  {
    // Grab the response, compare, and send to debugging spew
    byte got_msg;
    radio.read( &got_msg, sizeof(uint16_t) );

    // Spew it
    printf("Got response %x \n\r",msg);
    
  }

  // Try again 1s later
  delay(1000);
}
// vim:cin:ai:sts=2 sw=2 ft=cpp
