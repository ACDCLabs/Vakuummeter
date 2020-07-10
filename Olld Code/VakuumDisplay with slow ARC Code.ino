
#include "lcdgfx.h"
#include "VAkuumDisplay.h"
#include <SoftwareSerial.h>

DisplaySSD1327_128x128_SPI display(3, { -1, 4, 5, 0, -1, -1}); // Use this line for Atmega328p (3=RST, 4=CE, 5=D/C)

/*
   Heart image below is defined directly in flash memory.
   This reduces SRAM consumption.
   The image is defined from bottom to top (bits), from left to
   right (bytes).
*/


SoftwareSerial mySerial(7, 8); // RX , TX
//String command;
unsigned long int previousMillis = 0;
int counter = 0;
// unsigned long currentMillis =0;
int interval = 1000; // millies


void setup()
{

  Serial.begin(115200);
  display.begin();
  display.setFixedFont(ssd1306xled_font8x16);
  display.clear( );
  display.setColor(GRAY_COLOR4(255));

  mySerial.begin(19200);

  Serial.println("Setup complete");
}

void loop()
{

  char pressureValueString[24];
  char pressureValueExponent[5];
  int pressure = 2;
  String message = "I2_8.21E-06";
  float pressureValueMantisse = message.substring(message.indexOf("_") + 1, message.length() - 3).toFloat();
  message.substring(message.length() - 3, message.length()).toCharArray(pressureValueExponent, 5, 0);

  //sprintf(pressureString,"mbar");
  sprintf(pressureValueString, "%3d E%s mbar ", counter, pressureValueExponent);
  //display.setFixedFont(ssd1306xled_font8x16);
  // display.setFixedFont(free_calibri11x12);
  //



  //Serial.println(message.substring(message.indexOf("_") + 1, message.length()));
  //Serial.print("Value:");
  //Serial.print(value);
  //Serial.println();

/*
  float start_angle = 0.0, end_angle = 6.3;
  float x = 50, y = 60;
  float r = 30;
  int arcWidth = 5;
  for (float radius = r ; radius < r + arcWidth; radius ++) {
    for (float i = start_angle; i < end_angle; i = i + 0.03)
    {
      display.putPixel(round(x + cos(i) * radius), round(y + sin(i) * radius)); // center point is (50,100);
    }
    }
*/ 
display.setColor(255);
// fillArc2( 50, 60, 0 ,3, 10,20,3,1);
// display.drawHLine(30,30,60);  


int w = 20;
  int rx = 70;
  int ry = 60;

  for (int s =0; s< 60; s+=2){
   fillArc2(60, 60, 90, s, rx, ry, w, 1);
  }


  

  // delay(1000);
  if ((millis() - previousMillis) > interval) {
    //Serial.print(" millies: ");
    //Serial.print(millis());
    //Serial.print(" prev millis");
    //Serial.print(previousMillis);
    //Serial.println();
    previousMillis = millis();
    if (counter < 10) counter++ ;
    else counter--;
    //display.clear();
    //display.printFixed(10,  8, pressureValueString, STYLE_NORMAL);



  }





  /*
      if (Serial.available()) {
        command = Serial.readString();
            mySerial.println(command);
      }


      // delay(10);
      while (mySerial.available()) {
        Serial.write(mySerial.read());
      }
      // delay(500);
  */
}

// #########################################################################
// Draw a circular or elliptical arc with a defined thickness
// #########################################################################

// x,y == coords of centre of arc
// start_angle = 0 - 359
// seg_count = number of 3 degree segments to draw (120 => 360 degree arc)
// rx = x axis radius
// yx = y axis radius
// w  = width (thickness) of arc in pixels
// colour = 16 bit colour value
// Note if rx and ry are the same then an arc of a circle is drawn

int fillArc2(int x, int y, int start_angle, int seg_count, int rx, int ry, int w, unsigned int colour)
{

  byte seg = 3; // Segments are 3 degrees wide = 120 segments for 360 degrees
  byte inc = 3; // Draw segments every 3 degrees, increase to 6 for segmented ring

    // Calculate first pair of coordinates for segment start
    float sx = cos((start_angle - 90) * DEG2RAD);
    float sy = sin((start_angle - 90) * DEG2RAD);
    uint16_t x0 = sx * (rx - w) + x;
    uint16_t y0 = sy * (ry - w) + y;
    uint16_t x1 = sx * rx + x;
    uint16_t y1 = sy * ry + y;

  // Draw colour blocks every inc degrees
  for (int i = start_angle; i < start_angle + seg * seg_count; i += inc) {

    // Calculate pair of coordinates for segment end
    float sx2 = cos((i + seg - 90) * DEG2RAD);
    float sy2 = sin((i + seg - 90) * DEG2RAD);
    int x2 = sx2 * (rx - w) + x;
    int y2 = sy2 * (ry - w) + y;
    int x3 = sx2 * rx + x;
    int y3 = sy2 * ry + y;

    fillTriangle(x0, y0, x1, y1, x2, y2, colour);
    fillTriangle(x1, y1, x2, y2, x3, y3, colour);

    // Copy segment end to sgement start for next segment
    x0 = x2;
    y0 = y2;
    x1 = x3;
    y1 = y3;
  }
}



void fillTriangle (int16_t x1,int16_t y1,int16_t x2,int16_t y2,int16_t x3,int16_t y3, uint16_t c)

// Source: https://github.com/Bodmer/TFT_HX8357_Due/blob/master/TFT_HX8357_Due.cpp
{
  int16_t t1x,t2x,y,minx,maxx,t1xp,t2xp;
  bool changed1 = false;
  bool changed2 = false;
  int16_t signx1,signx2,dx1,dy1,dx2,dy2;
  uint16_t e1,e2;
     // Sort vertices
  if (y1>y2) { swap(y1,y2); swap(x1,x2); }
  if (y1>y3) { swap(y1,y3); swap(x1,x3); }
  if (y2>y3) { swap(y2,y3); swap(x2,x3); }

  t1x=t2x=x1; y=y1;   // Starting points

  dx1 = x2 - x1; if(dx1<0) { dx1=-dx1; signx1=-1; } else signx1=1;
  dy1 = y2 - y1;
 
  dx2 = x3 - x1; if(dx2<0) { dx2=-dx2; signx2=-1; } else signx2=1;
  dy2 = y3 - y1;
  
  if (dy1 > dx1) {   // swap values
        swap(dx1,dy1);
    changed1 = true;
  }
  if (dy2 > dx2) {   // swap values
        swap(dy2,dx2);
    changed2 = true;
  }
  
  e2 = dx2>>1;
    // Flat top, just process the second half
    if(y1==y2) goto next;
    e1 = dx1>>1;
  
  for (uint16_t i = 0; i < dx1;) {
    t1xp=0; t2xp=0;
    if(t1x<t2x) { minx=t1x; maxx=t2x; }
    else    { minx=t2x; maxx=t1x; }
        // process first line until y value is about to change
    while(i<dx1) {
      i++;      
      e1 += dy1;
          while (e1 >= dx1) {
        e1 -= dx1;
               if (changed1) t1xp=signx1;//t1x += signx1;
        else          goto next1;
      }
      if (changed1) break;
      else t1x += signx1;
    }
  // Move line
  next1:
        // process second line until y value is about to change
    while (1) {
      e2 += dy2;    
      while (e2 >= dx2) {
        e2 -= dx2;
        if (changed2) t2xp=signx2;//t2x += signx2;
        else          goto next2;
      }
      if (changed2)     break;
      else              t2x += signx2;
    }
  next2:
    if(minx>t1x) minx=t1x; if(minx>t2x) minx=t2x;
    if(maxx<t1x) maxx=t1x; if(maxx<t2x) maxx=t2x;
      display.drawHLine(minx, y, maxx);    // Draw line from min to max points found on the y
    // Now increase y
    if(!changed1) t1x += signx1;
    t1x+=t1xp;
    if(!changed2) t2x += signx2;
    t2x+=t2xp;
           y += 1;
    if(y==y2) break;
    
   }
  next:
  // Second half
  dx1 = x3 - x2; if(dx1<0) { dx1=-dx1; signx1=-1; } else signx1=1;
  dy1 = y3 - y2;
  t1x=x2;
 
  if (dy1 > dx1) {   // swap values
        swap(dy1,dx1);
    changed1 = true;
  } else changed1=false;
  
  e1 = dx1>>1;
  
  for (uint16_t i = 0; i<=dx1; i++) {
    t1xp=0; t2xp=0;
    if(t1x<t2x) { minx=t1x; maxx=t2x; }
    else    { minx=t2x; maxx=t1x; }
      // process first line until y value is about to change
    while(i<dx1) {
        e1 += dy1;
          while (e1 >= dx1) {
        e1 -= dx1;
                if (changed1) { t1xp=signx1; break; }//t1x += signx1;
        else          goto next3;
      }
      if (changed1) break;
      else          t1x += signx1;
      if(i<dx1) i++;
    }
  next3:
        // process second line until y value is about to change
    while (t2x!=x3) {
      e2 += dy2;
          while (e2 >= dx2) {
        e2 -= dx2;
        if(changed2) t2xp=signx2;
        else          goto next4;
      }
      if (changed2)     break;
      else              t2x += signx2;
    }        
  next4:

    if(minx>t1x) minx=t1x; if(minx>t2x) minx=t2x;
    if(maxx<t1x) maxx=t1x; if(maxx<t2x) maxx=t2x;
      display.drawHLine(minx, y, maxx);    // Draw line from min to max points found on the y
      
      Serial.print(" minx: ");
      Serial.print(minx);
      Serial.print(" y: ");
      Serial.print(y);
      Serial.print(" maxx: ");
      Serial.print(maxx);
      Serial.println();
      display.drawHLine(minx, y, maxx); 
    // Now increase y
    if(!changed1) t1x += signx1;
    t1x+=t1xp;
    if(!changed2) t2x += signx2;
    t2x+=t2xp;
        y += 1;
    if(y>y3) return;
  }
}
