
#include "lcdgfx.h"
#include "VakuumDisplay.h"
//#include <SoftwareSerial.h>
#include <AltSoftSerial.h>
// #include "SchulVokalFont.h"

DisplaySSD1327_128x128_SPI display(3, { -1, 4, 5, 0, -1, -1}); // Use this line for Atmega328p (3=RST, 4=CE, 5=D/C)


// Arduino Nano

// Display wireing
// waveshare SSD 1327
// RST - D3
// DC - D5
// CS - D4
// CLK - D13
// DIN - D11

// Max2323 wireing
// R2OUT - D8 - Serial RX
// T2IN - D9 - Serial TX





/*
   Heart image below is defined directly in flash memory.
   This reduces SRAM consumption.
   The image is defined from bottom to top (bits), from left to
   right (bytes).
*/


//SoftwareSerial mySerial(8, 9); // RX , TX


AltSoftSerial mySerial;

//String command;
unsigned long int previousMillis = 0;
// int counter = 0;
// unsigned long currentMillis =0;
int interval = 500; // millies

enum OperatingMode  {
  Pirani,
  IoniVac,
  NotDetected
};

typedef struct PressureVal {
  float mantisse = 0;
  int8_t exponent = 0;
  OperatingMode opMode = NotDetected;
  uint8_t filamentNum = 0;
};


PressureVal pressureVal;

// float counter = 11;
/*
  const int canvasWidth = 128; // Width must be power of 2, i.e. 16, 32, 64, 128...
  const int canvasHeight = 128; // Height must be divided on 8, i.e. 8, 16, 24, 32...
  uint8_t canvasData[canvasWidth*(canvasHeight/8)] ;
  /* Create canvas object */
/*
  NanoCanvas1 canvas(canvasWidth, canvasHeight, canvasData);
*/
void setup()
{

  Serial.begin(115200);
  display.begin();
  display.fill(0x00);
  //display.setFixedFont(courier_new_font11x16_digits);
  display.setFixedFont(ssd1306xled_font8x16);

  mySerial.begin(19200);

  Serial.println("Setup complete");
}

void loop()
{

  String pressureValueString;
  String pressureValueExponentString;
  char pressureValueCharArray[6];
  char pressureValueExponentCharArray[6];
  // char pressureValueExponentString[8];
  // int pressure = 2;
  const int8_t textXpos = 0; //10
  const int8_t textYpos = 5;

  String message = "";
  char messageCharArray[32];

  mySerial.print("R");
  //delay(30);
  mySerial.print("V");
  //delay(30);
  mySerial.write('\r');

  delay(50);
  message = "";

  while (mySerial.available()) {
    char responseChar = mySerial.read();
    //Serial.write(responseChar);
    message += responseChar;
  }

  message.toCharArray(messageCharArray,32);
  display.printFixed(5, 95, messageCharArray);


  //Serial.println();

  // delay(100);
  //Serial.print("Message:");
  //Serial.println(message);

  int8_t startParseIndex = 0;

  if (message.substring(0, 1) == "P") {
    pressureVal.opMode = Pirani;
    startParseIndex = 1;

  } else if (message.substring(0, 1) == "I") {

    pressureVal.opMode = IoniVac;
    pressureVal.filamentNum = message.substring(1, 2).toInt();
    startParseIndex = 2;

  } else {

    pressureVal.opMode = NotDetected;

  }


  /*
    Serial.print("operatingMode: ");
    Serial.println(pressureVal.opMode);

    Serial.print("Filament: ");
    Serial.println(pressureVal.filamentNum);
    Serial.print("Mantisse: ");
    Serial.println(pressureVal.mantisse);
    Serial.print("Exponent: ");
    //Serial.println( message.substring(message.indexOf("E")+1, message.length()));
    Serial.println(pressureVal.exponent);
  */
  // delay(100);


  // display.setColor(255);
  // display.drawLine(30, 30, 20, 20 );
  // display.drawLine(30, 30, 30, 60 );
  // display.drawLine(30, 60, 60, 60 );

  //display.drawHLine(40,70,80);
  // delay(100);

  if ((millis() - previousMillis) > interval) {
    //Serial.print(" millies: ");
    //Serial.print(millis());
    //Serial.print(" prev millis");
    //Serial.print(previousMillis);
    //Serial.println();
    previousMillis = millis();
    /*
        if (counter < 20 ) counter -= 1;
        else counter = 0;

    */
    pressureValueString = "";
    pressureValueExponentString = "";


    // needed for Display Update
    float previousValueMantisse = pressureVal.mantisse;
    float previousValueExponent = pressureVal.exponent;

    pressureVal.mantisse = message.substring(startParseIndex, message.length() - 3).toFloat();
    pressureVal.exponent = message.substring(message.indexOf("E") + 1, message.length()).toInt();

    // pressureVal.mantisse = counter;


    // pressureVal.mantisse =2.09;
    // pressureVal.exponent = -3;

    pressureValueString += pressureVal.mantisse ;
    pressureValueExponentString = + pressureVal.exponent;


    pressureValueString.toCharArray(pressureValueCharArray, 6);
    pressureValueExponentString.toCharArray(pressureValueExponentCharArray, 6);
    //dtostrf(pressureValueMantisse,4,2,pressureValueString);


    //display.clear();
    display.setFixedFont(comic_sans_font24x32_123);

    uint8_t shiftXpos = 5;
    if (pressureVal.mantisse < 10) shiftXpos += 10;


    if (previousValueMantisse > 9.99 && pressureVal.mantisse < 10) {
      display.setColor(0);
      display.fillRect(0, 28, 128, 60);
    }

    display.setColor(255);
    display.printFixed(textXpos + shiftXpos ,  textYpos + 20, pressureValueCharArray, STYLE_NORMAL);
    display.setFixedFont(ssd1306xled_font8x16);

    display.printFixed(textXpos + 37,  textYpos + 73, "x 10", STYLE_NORMAL);
    //display.setFixedFont(ssd1306xled_font5x7);

    display.setColor(0);
    display.fillRect(textXpos + 70, textYpos + 72, textXpos + 80, textYpos + 82);
    display.setColor(255);
    display.setFixedFont(ssd1306xled_font6x8);
    display.printFixed(textXpos + 70, textYpos + 72, pressureValueExponentCharArray, STYLE_NORMAL);
    display.setFixedFont(ssd1306xled_font8x16);
    //display.setFixedFont(free_calibri11x12_latin);
    display.printFixed(textXpos + 42,  textYpos + 90, "mbar", STYLE_NORMAL);
    Serial.println(pressureValueCharArray);



    // drawFilledCircle (70,70,11);
    /*
      for(uint8_t i = 0; i<10; i++) {
      display.putPixel(64+i,61);
      // delay(1000);
      }
    */

  }

}

void drawFilledCircle (uint8_t originX, uint8_t originY, uint8_t radius) {

  for (int y = -radius; y <= radius; y++)
    for (int x = -radius; x <= radius; x++)
      if (x * x + y * y <= radius * radius) {
        //delay(500);
        display.putPixel(originX + x, originY + y);
        //Serial.print("X : "); Serial.print(originX+x); Serial.print("  Y: "); Serial.println(originY+y);
      }
}
