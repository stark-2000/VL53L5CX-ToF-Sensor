//Object Detection using VL53L5CX STM PCB interfaced with Nucleo 64 Board - Measures distance of object in each 64 zones (8*8 Resolution)

/*
Connection Method:
Connect SDA & SCL of Nucleo Board to SDA & SCL of Sensor
Connect 5V of Nucleo Board to AVDD of Sensor
Connect 3.3V of Nucleo Board to IOVDD of Sensor
Connect GND of Nucleo Board to GND of Sensor
Connect A5 of Nucleo Board to PWREN of Sensor
*/

#include <Wire.h>
#include <SparkFun_VL53L5CX_Library.h>

#define PWREN_PIN A5 //LDO enable pin

SparkFun_VL53L5CX myImager; //Sparkfun class object - used for calling all library functions
VL53L5CX_ResultsData measurementData; //Result data class structure, 1356 byes of RAM

int imageResolution = 0; //Image resolution storing variable
int imageWidth = 0;      //Image width storing variable


void setup()
{
  if (PWREN_PIN >= 0) //Check whether pwr enable pin is present & enable if present
  {
    pinMode(PWREN_PIN, OUTPUT);
    digitalWrite(PWREN_PIN, HIGH);
    delay(10);
  }
  
  Serial2.begin(115200); //Default serial communication through USB is Serial 2
  delay(1000);
  Serial2.println("VL53L5CX Sensor Multi-Object Zone-Distance Data: ");

  Wire.begin(); //Initialize I2C bus 
  //Wire.setClock(400000); 
  //Sensor has max I2C freq of 400kHz - Req for applications with low response time
  Wire.setClock(1000000); //Running sensor at 1MHz

  myImager.setWireMaxPacketSize(128); //Increase default from 32 bytes to 128 - not supported on all platforms (works for Nucleo)

  Serial2.println("Initializing sensor board. This can take up to 10s. Please wait.");
  //Check sensor initialisation status
  if (myImager.begin() == false)
  {
    Serial2.println(F("Sensor not found - check your wiring or power supply"));
    while (1);
  }
  else if (myImager.begin() == true)
  {
    Serial2.println("Sensor Found - Initialisation Successful");
  }

  myImager.setResolution(8 * 8); //Enable all 64 pads - if needed to reduce FoV, can try 4*4 for 16 pads

  imageResolution = myImager.getResolution(); //Query sensor for current resolution - either 4x4 or 8x8
  imageWidth = sqrt(imageResolution);         //Calculate printing width - basic formula

  //Using 4x4, min frequency is 1Hz and max is 60Hz
  //Using 8x8, min frequency is 1Hz and max is 15Hz
  myImager.setRangingFrequency(15);
  myImager.startRanging(); //Fxn to start sensor ranging ie get distance values within FoV
}


void loop()
{
  // Poll sensor for new data
  if (myImager.isDataReady() == true)
  {
    if (myImager.getRangingData(&measurementData)) //Read distance data into array
    {
      //Print data with increasing y, decreasing x to reflect reality
      //imagewidth is sqrt(resolution) = sqrt(64) = 8
      /*For Y-Axis:
        8 * (8-1) = 8 * 7 = 56
        56 is the max value of y axis in 8 * 8  resolution - refer pg 18 of "VL53L5CX Sensor Chip Datasheet"
      */

      /*For X-Axis:
        8-1 = 7
        7 is the max value of x axis in 8 * 8  resolution in 1st row - refer pg 18 of "VL53L5CX Sensor Chip Datasheet"
      */
      
      for (int y = 0; y <= imageWidth * (imageWidth - 1); y += imageWidth) 
      {
        for (int x = imageWidth - 1; x >= 0; x--) 
        //X value is just for count purpose (there are 8 values or columns in 8*8 zoning)
        //Running this for loop 8 times enable getting values from 8 different Zone ID
        {
          Serial2.print("Zone ID: ");
          Serial2.print(x+y); //Adding X + Y gives the Zone ID
          Serial2.print(" , Distance of Object from Sensor: ");
          Serial2.println(measurementData.distance_mm[x + y]); //Calculating distance of a Zone ID
        }
        Serial2.println(); //Line spacing between each row of data
        delay(2000);
      }
      Serial2.println("End of full scan for objects in all 64 zones");
      delay(2000);
      Serial2.println("Starting again from 0th Zone");
    }
  }
  delay(5); // Small delay between polling
}
