#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);
int i = 0;

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
  Serial.begin(74880);
  Serial.print("Hello World\n");
  
}

void loop() {
  // put your main code here, to run repeatedly:
  
  delay(1000);
  Serial.print("Hello Friend:");
  Serial.print(i++);
  Serial.print("\n");


}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}