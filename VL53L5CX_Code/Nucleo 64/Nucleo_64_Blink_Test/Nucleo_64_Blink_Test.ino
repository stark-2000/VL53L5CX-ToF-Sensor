//Built-in LED connected to PA5 pin

void setup() 
{
  pinMode(PA5, OUTPUT);
  Serial2.begin(9600);
}

void loop() 
{
  digitalWrite(PA5, HIGH);   
  Serial2.println("Hello World");
  Serial2.print("LED: ");
  Serial2.println(digitalRead(PA5));
  delay(5000);                       
  digitalWrite(PA5, LOW);   
  Serial2.print("LED: ");
  Serial2.println(digitalRead(PA5));
  delay(5000);                       
}
