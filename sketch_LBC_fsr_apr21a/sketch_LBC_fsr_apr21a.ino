/******************************************************************************
Force_Sensitive_Resistor_Example.ino
Example sketch for SparkFun's force sensitive resistors
  (https://www.sparkfun.com/products/9375)
Jim Lindblom @ SparkFun Electronics
April 28, 2016

Create a voltage divider circuit combining an FSR with a 3.3k resistor.
- The resistor should connect from A0 to GND.
- The FSR should connect from A0 to 3.3V
As the resistance of the FSR decreases (meaning an increase in pressure), the
voltage at A0 should increase.

Development environment specifics:
Arduino 1.6.7
******************************************************************************/
const int FSR_PINS[] = {A0,A1,A2,A3}; // Pin connected to FSR/resistor divider



// Measure the voltage at 5V and resistance of your 3.3k resistor, and enter
// their value's below:
const float VCC = 4.98; // Measured voltage of Ardunio 5V line
const float R_DIV = 3230.0; // Measured resistance of 3.3k resistor

void setup() 
{
  Serial.begin(57600);
  for(int i=0; i<4; i++) {
    pinMode(FSR_PINS[i], INPUT);
  }
  
}


void loop() 
{
  float fsrR_arr[4];
  float force_arr[4];
  for(int i=0; i<4; i++) {
    int FSR_PIN = FSR_PINS[i];
    int fsrADC = analogRead(FSR_PIN);
    // If the FSR has no pressure, the resistance will be
    // near infinite. So the voltage should be near 0.
    if (fsrADC != 0) // If the analog reading is non-zero
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
      if (fsrR <= 600) 
        force = (fsrG - 0.00075) / 0.00000032639;
      else
        force =  fsrG / 0.000000642857;
      //Serial.println("Force: " + String(force) + " g");
      //Serial.println();
  
      fsrR_arr[i] = fsrR;
      force_arr[i] = force;
    }
    else
    {
      fsrR_arr[i] = 0;
      force_arr[i] = 0;
      
      // No pressure detected
    }
  }
  Serial.println("Forces:\t" + String(force_arr[3]) + "\t\t" + String(force_arr[0]) );
  Serial.println("\t" + String(force_arr[2]) + "\t\t" + String(force_arr[1]) );
  Serial.println("\n\n\n");
  delay(200);
}
