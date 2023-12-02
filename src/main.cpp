#include <Arduino.h>
#include <PCF8575.h>
#include <i2cdetect.h>
#include <Wire.h>

// put function declarations here:
int i = 0;
void printHex(uint16_t x);

PCF8575 PCF(0x20);


void setup() {
  Serial.begin(115200);
  //Serial.swap();
  
  //Wire.begin();
  // Serial.println("i2cdetect example\n");
  // Serial.print("Scanning address range 0x03-0x77\n\n");


  PCF.begin();

  Serial.println(__FILE__);
  Serial.print("PCF8575_LIB_VERSION:\t");
  Serial.println(PCF8575_LIB_VERSION);

  if (!PCF.begin())
  {
    Serial.println("could not initialize...");
  }
  if (!PCF.isConnected())
  {
    Serial.println("=> not connected");
  }
  else
  {
    Serial.println("=> connected!!");
    PCF.selectNone();
    //PCF.select(7);
  }


  
}

char c;
int selector = 0;

void loop() {

  // i2cdetect();  // default range from 0x03 to 0x77
  // delay(2000);
  // uint16_t x = PCF.read16();
  // printHex(x);

  if (Serial.available() > 0){
    c = Serial.read();

    // if (c == '1'){
    //   PCF8575::DigitalInput di = PCF.digitalReadAll();
    //   Serial.print("READ VALUE FROM PCF P1: ");
    //   Serial.print(di.p0);
    //   Serial.print(" - ");
    //   Serial.print(di.p1);
    //   Serial.print(" - ");
    //   Serial.print(di.p2);
    //   Serial.print(" - ");
    //   Serial.print(di.p3);
    //   Serial.print(" - ");
    //   Serial.print(di.p4);
    //   Serial.print(" - ");
    //   Serial.print(di.p5);
    //   Serial.print(" - ");
    //   Serial.print(di.p6);
    //   Serial.print(" - ");
    //   Serial.println(di.p7);
    // }

    if (c == '1'){
      int16_t x = PCF.read16();
      printHex(x);
    }
    if (c == '2'){
      PCF.toggle(6);
      delay(10);
      PCF.toggle(6);
    }
    if (c == '3'){
      int16_t x = PCF.read16();
      printHex(x);
      Serial.print("Toggle pin:");
      Serial.println(7, DEC);

      PCF.select(0x0070);
      delay(50);
      PCF.selectN(0x0070);
      delay(50);
    }
    if (c == '4'){
      int16_t x = PCF.read16();
      printHex(x);
      Serial.print("Toggle pin:");
      Serial.println(selector, DEC);

      PCF.select(selector);
      delay(10);
      PCF.selectN(selector);

      selector++;
    }
    if (c == '5'){
      int16_t x = PCF.read16();
      printHex(x);
      Serial.print("Toggle pin:");
      Serial.println(7, DEC);

      PCF.selectN(0x0070);
      delay(50);
      PCF.select(0x0070);
      delay(50);
    }
    if (c == '6'){
      selector = 0;
    }
    if (c == '7'){
      PCF.select(0x0070);
    }
    if (c == '8'){
      PCF.selectN(0x0070);
    }
    if (c == '9'){
      PCF.toggleMask(0x0070);
    }



    //Serial.print("Char = ");
    //Serial.println(c, DEC);
  }


  //delay(1000);
}



void printHex(uint16_t x)
{
  if (x < 0x1000) Serial.print('0');
  if (x < 0x100)  Serial.print('0');
  if (x < 0x10)   Serial.print('0');
  Serial.println(x, HEX);
}
