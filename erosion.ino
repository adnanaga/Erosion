#include <Wire.h>
#include <APDS9930.h>

// Global Variables
APDS9930 apds = APDS9930();
float ambient_light = 0; // can also be an unsigned long
uint16_t ch0 = 0;
uint16_t ch1 = 1;

// variables:
int sensorValue = 0;         // the sensor value
int sensorMin = 1023;        // minimum sensor value
int sensorMax = 0;           // maximum sensor value

void setup() {
  //analogReference(EXTERNAL);

  // Initialize Serial port
  Serial.begin(9600);
  Serial.println();
  Serial.println(F("--------------------------------"));
  Serial.println(F("APDS-9930 - Ambient light sensor"));
  Serial.println(F("--------------------------------"));

  pinMode(7, OUTPUT);

  // Initialize APDS-9930 (configure I2C and initial values)
  if ( apds.init() ) {
    Serial.println(F("APDS-9930 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9930 init!"));
  }

  // Start running the APDS-9930 light sensor (no interrupts)
  if ( apds.enableLightSensor(false) ) {
    Serial.println(F("Light sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during light sensor init!"));
  }

  // calibrate during the first five seconds
  while (millis() < 5000) {

    // Read the light levels (ambient, red, green, blue)
    if (  !apds.readAmbientLightLux(ambient_light) ||
          !apds.readCh0Light(ch0) ||
          !apds.readCh1Light(ch1) ) {
      Serial.println(F("Error reading light values"));
    } else {
      Serial.println(F("Ambient: "));

      sensorValue = ambient_light;

      // record the maximum sensor value
      if (sensorValue > sensorMax) {
        sensorMax = sensorValue;
      }

      // record the minimum sensor value
      if (sensorValue < sensorMin) {
        sensorMin = sensorValue;
      }
    }
  }

  // Wait for initialization and calibration to finish
  delay(500);
}

void loop() {

  // Read the light levels (ambient, red, green, blue)
  if (  !apds.readAmbientLightLux(ambient_light) ||
        !apds.readCh0Light(ch0) ||
        !apds.readCh1Light(ch1) ) {
    Serial.println(F("Error reading light values"));
  } else {
    Serial.print(F("Ambient: "));

    Serial.println(ambient_light);

    sensorValue = ambient_light;

    // in case the sensor value is outside the range seen during calibration
    sensorValue = constrain(sensorValue, sensorMin, sensorMax);

    // apply the calibration to the sensor reading
    sensorValue = map(sensorValue, sensorMin, sensorMax, 0, 255);

    Serial.println(sensorValue);

    int drip = sensorValue < 50;
    digitalWrite(7, drip);
  }
}