void LightSensor(int pin, int speed)
{
  int light = analogRead(pin);
  Serial.println(light);
  delay(speed);
}
void setup() 
{
  Serial.begin(9600);
}
void loop() 
{
  LightSensor(A5, 100);
}
