/*
 * Code for X, Y Axes to a Musical Scale
 * Must be used with a board with native USB capabilities
 * like those with ATMega32U4 or ATSAM chips (ARM) 
 * such as Zero, DUE, 101, Micro, Leonardo
 */
//
#include <MIDIUSB.h>
#include <frequencyToNote.h>
#include <MIDIUSB_Defs.h>
#include <pitchToFrequency.h>
#include <pitchToNote.h>
#include "midiUSBwrite.h"

#include <Adafruit_NeoPixel.h>
#include "WS2812_Definitions.h"

#define PIN 6
#define LED_COUNT 60

#define DEBUG_PRINT true

//MIDI_CREATE_DEFAULT_INSTANCE();


const float VCC = 4.98; // Measured voltage of Ardunio 5V line
const float R_DIV = 3230.0; // Measured resistance of 3.3k resistor
const byte FSR_PINS[] = {A0,A1,A2,A3}; // Pin connected to FSR/resistor divider
const float MAX_FORCE = 384.9;

int MIDI_NOTES[] = {60,64,67,72}; // Pin connected to FSR/resistor divider
int notesAreOn[] = {false,false,false,false,false};

byte rai; //Rainbow number
Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);
static const unsigned ledPin = 13;      // LED pin on Arduino Uno
int LED_ORDER[] = {1,0,3,2};


int lastTime; //prev. loop iteration's time
int lastNotePlay;

/*
 *   Serial.println("Forces:\t" + String(force_arr[3]) + "\t\t" + String(force_arr[0]) );
 *   Serial.println("\t" + String(force_arr[2]) + "\t\t" + String(force_arr[1]) );
 */

void setup() {
  // put your setup code here, to run once:
  ////MIDI.begin(4);
  Serial.begin(115200);
  rai=100;
  //pinMode( sensorPin, INPUT_PULLUP);
  for(int i=0; i<4; i++) {
    pinMode(FSR_PINS[i], INPUT);
  }
  
  pinMode(2,OUTPUT);
  digitalWrite(2,HIGH);
  
  leds.begin();  // Call this to start up the LED strip.
  clearLEDs();   // This function, defined below, turns all LEDs off...
  leds.show();   // ...but the LEDs don't actually update until you call this.


  lastTime = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  float fsrR_arr[4];
  float force_arr[4];
  int fsrADC_arr[4] = {0,0,0,0};
  for(int i=0; i<4; i++) {
    int FSR_PIN = FSR_PINS[i];
    int fsrADC = analogRead(FSR_PIN);
    // If the FSR has no pressure, the resistance will be
    // near infinite. So the voltage should be near 0.
    if (fsrADC >= 10) // If the analog reading is non-zero
    {
      // Use ADC reading to calculate voltage:
      float fsrV = fsrADC * VCC / 1023.0;
      // Use voltage and static resistor value to 
      // calculate FSR resistance:
      float fsrR = R_DIV * (VCC / fsrV - 1.0);
      //Serial.println("FSR #" + String(FSR_PIN) + "Resistance: " + String(fsrR) + " ohms");
      // Guesstimate force based on slopes in figure 3 of
      // FSR datasheet:
      float force;
      float fsrG = 1.0 / fsrR; // Calculate conductance
      // Break parabolic curve down into two linear slopes:
      if (fsrR <= 600) {
        force = (fsrG - 0.00075) / 0.00000032639;
      } else {
        force =  fsrG / 0.000000642857;
      }
  
      fsrR_arr[i] = fsrR;
      force_arr[i] = force;
      fsrADC_arr[i] = fsrADC;

    }
    else
    {
      fsrR_arr[i] = 0;
      force_arr[i] = 0;
      fsrR_arr[i] = 0;
      // No pressure detected
    }
    
  }   //End FOR for finding sensor values

  
  float X_axis = force_arr[0]+force_arr[1]-force_arr[3]-force_arr[2];
  X_axis/=MAX_FORCE*2;
  float Y_axis = force_arr[0]+force_arr[3]-force_arr[1]-force_arr[2];
  Y_axis/=MAX_FORCE*2;
  float XY_avg = force_arr[3]+force_arr[2]+force_arr[0]+force_arr[1];
  XY_avg/=MAX_FORCE*4;
  
  int notePlay = 24+int(X_axis*12)+int((Y_axis+1)*4)*12;

  /*
  if(XY_avg>0.05){
    if(notesAreOn[4]){
      afterTouch(0,notePlay,int(XY_avg*120));
    } else {
      noteOn(0,notePlay,127);
    }
    notesAreOn[4] = true;
  } else { 
    noteOff(0,notePlay,0);
    notesAreOn[4] = false;
  }
  
  if(lastNotePlay!=notePlay){
    noteOff(0,lastNotePlay,0);
  }
  lastNotePlay = notePlay;
  */
  
  Serial.println("X:"+String(X_axis)+"\tY:"+String(Y_axis)+"\tAvg:"+String(XY_avg));
  
  
  //4-tone system
  Serial.print("Notes:");
  
  for(int i=0; i<4; i++) {
    if(fsrADC_arr[i]>100){
      int vel = fsrADC_arr[i]*0.125;
      if(notesAreOn[i]){
        afterTouch(0,MIDI_NOTES[i],vel);
      } else {
        noteOn(0,MIDI_NOTES[i],127);
      }
      notesAreOn[i] = true;
    } else { 
      noteOff(0,MIDI_NOTES[i],0);
      notesAreOn[i] = false;
    }
    Serial.print("\t" + String(notesAreOn[i]) + ",");
    
  }
  

  //LEDS:
  for(int i=0; i<4; i++) {
    for (int ld=LED_ORDER[i]*15; ld<LED_ORDER[i]*15+LED_COUNT/4; ld++)
    {
      // There are 192 total colors we can get out of the rainbowOrder function.
      // It'll return a color between red->orange->green->...->violet for 0-191.
      int brt = fsrADC_arr[i]*0.25;
      if(notesAreOn[i]){
        leds.setPixelColor(ld, rainbowOrder(brt%192));
      } else {
        leds.setPixelColor(ld, rainbowOrder(10));
      }
    }
    
    
  } //end FOR sensors
  leds.show();

  
  Serial.println();
  
  if(DEBUG_PRINT){
    Serial.println("Forces:\t" + String(fsrADC_arr[3]) + "\t\t" + String(fsrADC_arr[0]) );
    Serial.println("\t" + String(fsrADC_arr[2]) + "\t\t" + String(fsrADC_arr[1]) );
    Serial.println("\n\n");
  }
  
  /*
  //lEDloop
  if(millis() - lastTime > 200) {
    rainbow(rai);
    //Serial.println(rai);
    rai = (rai+1)%192;
  }
  */
  
  delay(20);  // Delay between readings

  

}





/*
 * 
 * LED CODE
 * 
 */

void rainbow(byte startPosition) 
{
  // Need to scale our rainbow. We want a variety of colors, even if there
  // are just 10 or so pixels.
  int rainbowScale = 192 / 180;
  
  // Next we setup each pixel with the right color
  for (int i=0; i<LED_COUNT; i++)
  {
    // There are 192 total colors we can get out of the rainbowOrder function.
    // It'll return a color between red->orange->green->...->violet for 0-191.
    leds.setPixelColor(i, rainbowOrder((rainbowScale * i + startPosition) % 192));
  }
  // Finally, actually turn the LEDs on:
  leds.show();
}

uint32_t rainbowOrder(byte position) 
{
  // 6 total zones of color change:
  if (position < 31)  // Red -> Yellow (Red = FF, blue = 0, green goes 00-FF)
  {
    return leds.Color(0xFF, position * 8, 0);
  }
  else if (position < 63)  // Yellow -> Green (Green = FF, blue = 0, red goes FF->00)
  {
    position -= 31;
    return leds.Color(0xFF - position * 8, 0xFF, 0);
  }
  else if (position < 95)  // Green->Aqua (Green = FF, red = 0, blue goes 00->FF)
  {
    position -= 63;
    return leds.Color(0, 0xFF, position * 8);
  }
  else if (position < 127)  // Aqua->Blue (Blue = FF, red = 0, green goes FF->00)
  {
    position -= 95;
    return leds.Color(0, 0xFF - position * 8, 0xFF);
  }
  else if (position < 159)  // Blue->Fuchsia (Blue = FF, green = 0, red goes 00->FF)
  {
    position -= 127;
    return leds.Color(position * 8, 0, 0xFF);
  }
  else  //160 <position< 191   Fuchsia->Red (Red = FF, green = 0, blue goes FF->00)
  {
    position -= 159;
    return leds.Color(0xFF, 0x00, 0xFF - position * 8);
  }
}

void clearLEDs()
{
  for (int i=0; i<LED_COUNT; i++)
  {
    leds.setPixelColor(i, 0);
  }
}
