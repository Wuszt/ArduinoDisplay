#include <Arduino.h>

void setup() 
{
  Serial.begin(115200);
  while (!Serial) {}
  Serial.println("Hello");
}

void loop() {
  // put your main code here, to run repeatedly:

}
