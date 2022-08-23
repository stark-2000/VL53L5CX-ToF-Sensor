//Object Detection using VL53L5CX STM PCB interfaced with Nucleo 64 Board - Measures distance of object in each 16 zones (4*4 Resolution)
//Uses STM library for VL53L5CX instead of Sparkfun - all other codes use sparkfun

/*
Connection Method:
Connect SDA & SCL of Nucleo Board to SDA & SCL of Sensor
Connect 5V of Nucleo Board to AVDD of Sensor
Connect 3.3V of Nucleo Board to IOVDD of Sensor
Connect GND of Nucleo Board to GND of Sensor
Connect A5 of Nucleo Board to PWREN of Sensor
*/

#include <Arduino.h>
#include <Wire.h>
#include <vl53l5cx_class.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

#define DEV_I2C Wire //for i2c communication 
#define SerialPort Serial2 //Default serial communication through USB is Serial 2
#define LedPin 13 //Built-in LED
#define PWREN_PIN A5 //LDO enable pin
#define LPN_PIN A3 //Not req to be connected - just declared for fxn argument purpose
#define I2C_RST_PIN A1 //Not req to be connected - just declared for fxn argument purpose

VL53L5CX sensor_vl53l5cx_sat(&DEV_I2C, LPN_PIN, I2C_RST_PIN); //Fxn req all 3 arguements to work

void blink_led_loop(void); //Fxn triggered when sensor initialisation status fails
void blink_led_loop(void) // Blinks built-in pin 13 led forever
{
  do 
  {
    digitalWrite(LedPin, HIGH);
    delay(500);
    digitalWrite(LedPin, LOW);
  } while (1);
}


void setup()
{
  pinMode(LedPin, OUTPUT);

  if (PWREN_PIN >= 0) //Check whether pwr enable pin is present & enable if present
  {
    pinMode(PWREN_PIN, OUTPUT);
    digitalWrite(PWREN_PIN, HIGH);
    delay(10);
  }

  SerialPort.begin(115200); //Default serial communication through USB is Serial 2
  SerialPort.println("VL53L5CX Multi Object Distance Data: ");
  SerialPort.println("Initialize... Please wait, it may take few seconds...");

  DEV_I2C.begin(); //Initialize I2C bus
  sensor_vl53l5cx_sat.begin(); //Configure VL53L5CX satellite component

  //Call the initialisation fxn & status and store the return value in variable "a"
  int a = sensor_vl53l5cx_sat.init_sensor(); 
  if (a==0) //Fxn returns 0 when initialisation is successful
  {
    SerialPort.println("Sensor Found - Initialisation Successful");
  }
  else if (a==1) //Fxn returns 1 when initialisation is not successful
  {
    SerialPort.println("Sensor not found - check your wiring or power supply");
    while (1);
  }
  delay(3000); //Short delay after initialisation status check

  sensor_vl53l5cx_sat.vl53l5cx_start_ranging(); //Fxn to start sensor ranging ie get distance values within FoV
}


void loop()
{
  static uint8_t loop_count = 0;
  VL53L5CX_ResultsData Results; //Ranging output - distance storing variable
  uint8_t NewDataReady = 0;
  char report[64];
  uint8_t status;

  if (loop_count < 10) 
  {
    do 
    {
      status = sensor_vl53l5cx_sat.vl53l5cx_check_data_ready(&NewDataReady);
    } while (!NewDataReady);
    
    digitalWrite(LedPin, HIGH); //Led on

    if ((!status) && (NewDataReady != 0)) 
    {
      status = sensor_vl53l5cx_sat.vl53l5cx_get_ranging_data(&Results);

      /* As the sensor is set in 4x4 mode by default, we have a total
       * of 16 zones to print.
      */

      snprintf(report, sizeof(report), "Print data no : %3u\r\n", sensor_vl53l5cx_sat.get_stream_count());
      SerialPort.print(report);
      for (int i = 0; i < 16; i++) 
      {
        snprintf(report, sizeof(report), "Zone : %3d, Status : %3u, Distance : %4d mm\r\n",
                 i,
                 Results.target_status[VL53L5CX_NB_TARGET_PER_ZONE * i],
                 Results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE * i]);
        SerialPort.print(report);
      }
      SerialPort.println("");
      loop_count++;
    }

    digitalWrite(LedPin, LOW);
  } 
  else if (loop_count == 10) 
  {
    /* Stop measurements */
    status = sensor_vl53l5cx_sat.vl53l5cx_stop_ranging();
    if (status) 
    {
      snprintf(report, sizeof(report), "vl53l5cx_stop_ranging failed, status %u\r\n", status);
      SerialPort.print(report);
      blink_led_loop();
    }

    loop_count++;
    /* End of the demo */
    SerialPort.println("End of ULD demo");
  } 
  else 
  {
    delay(1000);
  }
}
