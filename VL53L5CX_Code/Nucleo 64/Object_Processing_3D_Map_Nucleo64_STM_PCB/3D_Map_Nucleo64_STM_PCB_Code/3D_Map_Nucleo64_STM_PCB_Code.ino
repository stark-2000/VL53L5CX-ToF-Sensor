//Object Detection using VL53L5CX STM PCB interfaced with Nucleo 64 Board - Measures distance of object in each 64 zones (8*8 Resolution) & Output RAW values
//Upload this code for 3D Map visualization using "Processing"

/*
Connection Method:
Connect SDA & SCL of Nucleo Board to SDA & SCL of Sensor
Connect 5V of Nucleo Board to AVDD of Sensor
Connect 3.3V of Nucleo Board to IOVDD of Sensor
Connect GND of Nucleo Board to GND of Sensor
Connect A5 of Nucleo Board to PWREN of Sensor
*/

#include <Wire.h>
#include <SparkFun_VL53L5CX_Library.h> //http://librarymanager/All#SparkFun_VL53L5CX

#define PWREN_PIN A5 //LDO enable pin

SparkFun_VL53L5CX myImager;
VL53L5CX_ResultsData measurementData; // Result data class structure, 1356 byes of RAM

int imageResolution = 0; // Used to pretty print output
int imageWidth = 0;      // Used to pretty print output

void setup()
{
  if (PWREN_PIN >= 0) //Check whether pwr enable pin is present & enable if present
  {
    pinMode(PWREN_PIN, OUTPUT);
    digitalWrite(PWREN_PIN, HIGH);
    delay(10);
  }
  
  Serial2.begin(115200);
  delay(1000);
  Serial2.println("SparkFun VL53L5CX Imager Example");

  Wire.begin(); // This resets I2C bus to 100kHz
  // Wire.setClock(400000); //Sensor has max I2C freq of 400kHz
  Wire.setClock(1000000); // Run sensor out of spec

  myImager.setWireMaxPacketSize(128); // Increase default from 32 bytes to 128 - not supported on all platforms

  Serial2.println("Initializing sensor board. This can take up to 10s. Please wait.");
  if (myImager.begin() == false)
  {
    Serial2.println(F("Sensor not found - check your wiring. Freezing"));
    while (1)
      ;
  }

  myImager.setResolution(8 * 8); // Enable all 64 pads

  imageResolution = myImager.getResolution(); // Query sensor for current resolution - either 4x4 or 8x8
  imageWidth = sqrt(imageResolution);         // Calculate printing width

  // Using 4x4, min frequency is 1Hz and max is 60Hz
  // Using 8x8, min frequency is 1Hz and max is 15Hz
  myImager.setRangingFrequency(15);
  myImager.startRanging();
}

void loop()
{
  // Poll sensor for new data
  if (myImager.isDataReady() == true)
  {
    if (myImager.getRangingData(&measurementData)) // Read distance data into array
    {
      // The ST library returns the data transposed from zone mapping shown in datasheet
      // Pretty-print data with increasing y, decreasing x to reflect reality
      for (int y = 0; y <= imageWidth * (imageWidth - 1); y += imageWidth)
      {
        for (int x = imageWidth - 1; x >= 0; x--)
        {
          Serial2.print(measurementData.distance_mm[x + y]);
          Serial2.print(",");
        }
      }
      Serial2.println();
    }
  }
  delay(5); // Small delay between polling
}
