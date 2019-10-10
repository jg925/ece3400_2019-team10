/*
  fft_adc.pde
  guest openmusiclabs.com 8.18.12
  example sketch for testing the fft library.
  it takes in data on ADC0 (Analog0) and processes them
  with the fft. the data is sent out over the serial
  port at 115.2kb.  there is a pure data patch for
  visualizing the data.
*/

#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft

#include <FFT.h> // include the library

int detect_count = 0;

int is_maximum( int five, int six, int seven, int eight, int FFT_threshold ) {
  if ( six > FFT_threshold && seven > FFT_threshold ) {
    //if ( six > five && six > eight && seven > five && seven > eight ) {
      // checking that six and seven are a local maximum
      //if ( six - seven < 10 && six - five > 10 && six - seven > 0) {
        // checking that shape of curve is correct
        return 1;
      //}
    //}
  }
  else {
    return 0;
  }
}

void setup() {
  Serial.begin(9600); // use the serial port
  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
}

void loop() {
  while (1) { // reduces jitter
    cli();  // UDRE interrupt slows this way down on arduino1.0
    for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
      while (!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fft_input[i] = k; // put real data into even bins
      fft_input[i + 1] = 0; // set odd bins to 0
    }
    fft_window(); // window the data for better frequency response
    fft_reorder(); // reorder the data before doing the fft
    fft_run(); // process the data in the fft
    fft_mag_log(); // take the output of the fft
    sei();
    int max = is_maximum( fft_log_out[5], fft_log_out[6], fft_log_out[7], fft_log_out[8], 100 );
    if ( max == 1 && detect_count >= 5)
    {
      Serial.println("950 Hz");
      detect_count = 0;
      //Serial.println(detect_count);
    }
    else if ( max == 1 ) {
      detect_count++;
    }
  }
}

