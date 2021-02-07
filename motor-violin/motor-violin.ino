/*********************************************************
This is a library for the MPR121 12-channel Capacitive touch sensor

Designed specifically to work with the MPR121 Breakout in the Adafruit shop 
  ----> https://www.adafruit.com/products/

These sensors use I2C communicate, at least 2 pins are required 
to interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.  
BSD license, all text above must be included in any redistribution
**********************************************************/
#include "violin_audio.h"

#include <Wire.h>
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

#ifndef _BV2
#define _BV2(bit) (1 << (bit)) 
#endif

// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();
Adafruit_MPR121 cap2 = Adafruit_MPR121();

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;
uint16_t lasttouched2 = 0;
uint16_t currtouched2 = 0;

double freq_A0 = 27.5;

int nut = 44;  // Starting note E4

uint32_t the_note = 0;

double get_freq(int n)
{
  double f = freq_A0 * pow( 2 , ((double)(n-1)/12.0) );
  return f;
}

double get_freq_delta(double f, double d)
{
  if(d > 1.0) d  =  2.0;
  if(d < -1.0) d = -2.0;
  double f_delta = f * pow( 2 , (d/12.0) );
  return f_delta;
}

void set_note(int n)
{
  double f = get_freq(n);
  set_note(f,f);
  return;
}

void set_note(double f, double d)
{
  wav0.frequency(f);
  wav1.frequency(f);
  wav2.frequency(f/2.0);
  wav3.frequency(d);
  Serial.print(f); Serial.print(" "); Serial.println(d);
  return;
}

void setup_wav()
{
  float freq = get_freq(49); // 49 is A 440.0
  float delta = get_freq_delta(freq, 0.5);
  wav0.begin(1.0, freq, WAVEFORM_SINE);
  wav1.begin(1.0, freq, WAVEFORM_TRIANGLE);
  wav2.begin(1.0, freq/2.0, WAVEFORM_SINE);
  wav3.begin(1.0, delta, WAVEFORM_TRIANGLE);
}

void setup_amp()
{
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH); // turn on the amplifier
  delay(10);             // allow time to wake up
  AudioMemory(64);
  dac1.analogReference(EXTERNAL); // much louder!
  delay(50);             // time for DAC voltage stable
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH); // turn on the amplifier
  delay(10);             // allow time to wake up
  amp1.gain(0.5);
  mixer1.gain(0,0.25);
  mixer1.gain(1,0.25);
  mixer1.gain(2,0.25);
  mixer1.gain(3,0.25);
}
void setup() {
  setup_amp();
  setup_wav();
  Serial.begin(9600);

  while (!Serial) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }
  
  Serial.println("Adafruit MPR121 Capacitive Touch sensor test"); 
  
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 1 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 1 found!");
    if (!cap2.begin(0x5B)) {
    Serial.println("MPR121 2 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 2 found!");
  memset(&the_note, 0, sizeof(the_note));
}


void set_the_note(int i, int val)
{
  uint32_t d = 1;
  d = d << i;
  if( val >= 1) the_note = the_note | d;
  else the_note = the_note & ~d;
  return;
}

int get_the_note(int n)
{
  uint32_t d = 1;
  uint32_t val = the_note & ( d << n );
  if(val > 0) return 1;
  return 0;
}

void print_notes()
{
  for(int i = 0; i < 24; i++)
  {
    Serial.print(get_the_note(i));
  }
  Serial.println();
}

int play_note()
{
  int note = 0;
  for(int i = 0; i < 24; i++)
  {
    if(get_the_note(i)) note = i + 1;
  }
  note = note + nut;
  set_note(note);
  return note;
}

uint32_t the_note_prev = ~0; 

void loop() {
  // Get the currently touched pads
  currtouched = cap.touched();
  currtouched2 = cap2.touched();

  int note = nut;
  int note_off = nut;

  int i_top = 12;
  bool fret_hit = false;
  for (uint8_t i=0; i<12; i++) {
    i_top--;
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      note = i_top; // changes to the lowest touched note
      //Serial.print(note); Serial.println(" touched");
      //envelope1.noteOn();
      set_the_note(i_top,1);
      fret_hit = true;
      //Serial.println(note);
      
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      note_off = i_top;
      set_the_note(i_top,0);
      //Serial.print(note_off); Serial.println(" released");
      //envelope1.noteOff();
      fret_hit = true;
    }
  }


    int i_bottom = 24;

    for (uint8_t i=0; i<12; i++) {
      i_bottom--;
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched2 & _BV2(i)) && !(lasttouched2 & _BV2(i)) ) {
      note = i_bottom;
      set_the_note(i_bottom,1);
      //Serial.print(i_bottom); Serial.println(" touched");
      //envelope1.noteOn();
      fret_hit = true;
      //Serial.println(note);
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched2 & _BV2(i)) && (lasttouched2 & _BV2(i)) ) {
      note_off = i_bottom;
      set_the_note(i_bottom,0);
      //Serial.print(i_bottom); Serial.println(" released");
      //envelope1.noteOff();
      fret_hit = true;
    }
  }

  
  if(the_note_prev != the_note) Serial.println(play_note());
  envelope1.noteOn();
  the_note_prev = the_note;

  if(fret_hit) print_notes();
  
  // reset our state
  lasttouched = currtouched;
  lasttouched2 = currtouched2;

  return;
}
