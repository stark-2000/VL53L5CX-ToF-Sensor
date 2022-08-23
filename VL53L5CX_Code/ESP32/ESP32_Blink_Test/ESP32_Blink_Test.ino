//Built-in LED is connected to pin 2 in ESP32

// Pin 13 has an LED connected on most Arduino boards.
// Pin 11 has the LED on Teensy 2.0
// Pin 6  has the LED on Teensy++ 2.0
// Pin 13 has the LED on Teensy 3.0

int led = 2;

void setup() 
{
  pinMode(led, OUTPUT);
}

void loop() 
{
  digitalWrite(led, HIGH); 
  delay(1000);               
  digitalWrite(led, LOW);    
  delay(1000);               
}
