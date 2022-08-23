//Person Entry/Exit detection using VL53L5CX custom PCB interfaced with ESP32 Board - 8*8 zone

/*
Logic:
Case 1: Distance of objects in 1st to 3rd column from the left side of 8*8 matrix is fetched and checked for presence of object within a certain distance
Case 2: Distance of objects in 1st to 3rd column from the right side of 8*8 matrix is fetched and checked for presence of object within a certain distance
If case 1 is first true and then case 2, it is person entry and vice - versa for person exit
*/

/*
Connection Method:
Connect SDA & SCL of ESP Board to SDA & SCL of Sensor
Connect Vin of ESP Board to AVDD of Sensor
Connect 3.3V of ESP Board to IOVDD of Sensor
Connect GND of ESP Board to GND of Sensor
Connect D5 of ESP Board to PWREN of Sensor
*/

#include <Wire.h>
#include <SparkFun_VL53L5CX_Library.h>

SparkFun_VL53L5CX myImager; //Sparkfun class object - used for calling all library functions
VL53L5CX_ResultsData measurementData; //Result data class structure, 1356 byes of RAM

int imageResolution = 0; //Image resolution storing variable
int imageWidth = 0;      //Image width storing variable


void setup()
{
  Serial.begin(115200); //Default serial communication through USB is Serial 2
  Serial.println("VL53L5CX Sensor Multi-Object Zone-Distance Data: ");

  Wire.begin(); //Initialize I2C bus 
  //Wire.setClock(400000);
  Wire.setClock(1000000); //Running sensor at 1MHz, deafult is 100KHz (if not specified)

  //myImager.setWireMaxPacketSize(128); //Increase default from 32 bytes to 128 - not supported on all platforms (works for Nucleo 64)
  //If not declared some pre defined default packet side is used

  Serial.println("Initializing sensor board. This can take up to 10s. Please wait.");
  //Check sensor initialisation status
  if (myImager.begin() == false)
  {
    Serial.println(F("Sensor not found - check your wiring or power supply"));
    while (1);
  }
  else if (myImager.begin() == true)
  {
    Serial.println("Sensor Found - Initialisation Successful");
  }

  myImager.setResolution(8 * 8);
  myImager.setRangingFrequency(15);
  myImager.startRanging(); 
}


void loop()
{
  int i;
  int flag = 0, count = 0, timer = 0;
  int flag1 = 0, count1 = 0, timer1 = 0;

  if (myImager.isDataReady() == true)
  {
    if (myImager.getRangingData(&measurementData)) //Read distance data into array
    {
        i = 0;
        while (i <= 56)
        {
          if(measurementData.distance_mm[i] < 800)
          {
            count ++;
          }
          i = i+8;
        }

        i = 7;
        while (i <= 63)
        {
          if(measurementData.distance_mm[i] < 800)
          {
            count1 ++;
          }
          i = i+8;
        }

        if (count > 3)
        {
          i = 1;
          while (i <= 57)
          {
            if (measurementData.distance_mm[i] < 800)
            {
              flag ++;
            }
            i = i+8;
          }
        }

        else if (count1 > 3)
        {
          i = 6;
          while (i <= 62)
          {
            if (measurementData.distance_mm[i] < 800)
            {
              flag1 ++;
            }
            i = i+8;
          }
        }

        if (flag > 3)
        {
          i = 2;
          while (i <= 58)
          {
            if (measurementData.distance_mm[i] < 800)
            {
              timer ++;
            }
            i = i+8;
          }
        }

        else if (flag1 > 3)
        {
          i = 5;
          while (i <= 61)
          {
            if (measurementData.distance_mm[i] < 800)
            {
              timer1 ++;
            }
            i = i+8;
          }
        }

        if (timer > 3 && timer1 < 3)
        {
          Serial.println("Person Exit");
          delay(500);
        }

        else if (timer1 > 3 && timer < 3)
        {
          Serial.println("Person Entry");
          delay(500);
        }

//        int prev_timer = timer;
//        int prev_timer1 = timer1;
//        
//        if (prev_timer > 5 && timer < 5)
//        {
//          if(prev_timer1 < 5 && timer1 > 5)
//          {
//            Serial.println("Person Exit");
//            delay(500);
//          }
//        }
//        
//        else if (prev_timer1 > 5 && timer1 < 5)
//        {
//          if(prev_timer < 5 && timer > 5)
//          {
//            Serial.println("Person Entry");
//            delay(500);
//          }
//        }
    }
  }
}
