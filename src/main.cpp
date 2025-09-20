#include <Arduino.h>
#include <PCF8575.h>
#include <i2cdetect.h>
#include <Wire.h>

// put function declarations here:
int i = 0;
void read_all_pin_values();

//PCF8575 PCF(0x20);
PCF8575 pcf8575(0x20);



void setup() {
  Serial.begin(115200);
  
  // setup pinmods
  // Port 1
  pcf8575.pinMode(P0, INPUT);
  pcf8575.pinMode(P1, INPUT);
  pcf8575.pinMode(P2, INPUT);
  pcf8575.pinMode(P3, OUTPUT);
  // Port 2
  pcf8575.pinMode(P4, INPUT);
  pcf8575.pinMode(P5, INPUT);
  pcf8575.pinMode(P6, INPUT);
  pcf8575.pinMode(P7, OUTPUT);
  // Port 3
  pcf8575.pinMode(P8, INPUT);
  pcf8575.pinMode(P9, INPUT);
  pcf8575.pinMode(P10, INPUT);
  pcf8575.pinMode(P11, OUTPUT);
  // Port 4
  pcf8575.pinMode(P12, INPUT);
  pcf8575.pinMode(P13, INPUT);
  pcf8575.pinMode(P14, INPUT);
  pcf8575.pinMode(P15, OUTPUT);

  pcf8575.begin();

  //Serial.swap();
  
  //Wire.begin();
  // Serial.println("i2cdetect example\n");
  // Serial.print("Scanning address range 0x03-0x77\n\n");


  // PCF.begin();

  // Serial.println(__FILE__);
  // Serial.print("PCF8575_LIB_VERSION:\t");
  // Serial.println(PCF8575_LIB_VERSION);

  // if (!PCF.begin())
  // {
  //   Serial.println("could not initialize...");
  // }
  // if (!PCF.isConnected())
  // {
  //   Serial.println("=> not connected");
  // }
  // else
  // {
  //   Serial.println("=> connected!!");
  //   PCF.selectNone();
  //   //PCF.select(7);
  // }


  
}

char c;
int selector = 0;

void loop() {

  if (Serial.available() > 0){
    c = Serial.read();

    if (c == '1'){

      pcf8575.digitalWrite(P3,HIGH);
      delay(200);
      pcf8575.digitalWrite(P3,LOW);
      delay(200);
      
    }
    if (c == '2'){

      pcf8575.digitalWrite(P7,HIGH);
      delay(200);
      pcf8575.digitalWrite(P7,LOW);
      delay(200);
    }
    if (c == '3'){

      pcf8575.digitalWrite(P11,HIGH);
      delay(200);
      pcf8575.digitalWrite(P11,LOW);
      delay(200);

    }
    if (c == '4'){
      pcf8575.digitalWrite(P15,HIGH);
      delay(200);
      pcf8575.digitalWrite(P15,LOW);
      delay(200);
    }
    
    if (c == '9'){
      // PCF.toggleMask(0x0070);
      read_all_pin_values();
    }



    //Serial.print("Char = ");
    //Serial.println(c, DEC);
  }


  //delay(1000);
}

void read_all_pin_values(){
  PCF8575::DigitalInput di = pcf8575.digitalReadAll();
  Serial.print(di.p15);
  Serial.print(" - ");
  Serial.print(di.p14);
  Serial.print(" - ");
  Serial.print(di.p13);
  Serial.print(" - ");
  Serial.print(di.p12);
  Serial.print(" - ");
  Serial.print(di.p11);
  Serial.print(" - ");
  Serial.print(di.p10);
  Serial.print(" - ");
  Serial.print(di.p9);
  Serial.print(" - ");
  Serial.print(di.p8);
  Serial.print(" - ");
  Serial.print(di.p7);
  Serial.print(" - ");
  Serial.print(di.p6);
  Serial.print(" - ");
  Serial.print(di.p5);
  Serial.print(" - ");
  Serial.print(di.p4);
  Serial.print(" - ");
  Serial.print(di.p3);
  Serial.print(" - ");
  Serial.print(di.p2);
  Serial.print(" - ");
  Serial.print(di.p1);
  Serial.print(" - ");
  Serial.println(di.p0);
}


