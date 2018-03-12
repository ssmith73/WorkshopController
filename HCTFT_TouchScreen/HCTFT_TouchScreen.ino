/* FILE:    HCTFT_Touch_Example_1
   DATE:    10/06/14
   VERSION: 0.2
   AUTHOR:  Andrew Davies
   
   V0.2 (17_06_2015): Updated for our latest TFT displays (HCARDU0097 & HCARDU0096)

This is an example of how to use the HCTFT_Touch library. The library provides the 
ability to read the current status of the touch screen. It does not provide any 
functions for controlling the TFT screen itself but is intended for providing touch 
screen support to any existing sketches or libraries. However the library will work 
standalone as this example sketch will demonstrate.

The example will read the current state of the touch screen and output it to the serial
port as raw X and Y coordinates, and as the current position in pixels. No information 
will be displayed on the TFT screen itself whilst this example is running.

To use this sketch you will need to uncomment the appropriate initialiser below for 
your particular display.

You may copy, alter and reuse this code in any way you like, but please leave
reference to HobbyComponents.com in your comments if you redistribute this code.
This software may not be used directly for the purpose of selling products that
directly compete with Hobby Components Ltd's own range of products.

THIS SOFTWARE IS PROVIDED "AS IS". HOBBY COMPONENTS MAKES NO WARRANTIES, WHETHER
EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ACCURACY OR LACK OF NEGLIGENCE.
HOBBY COMPONENTS SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR ANY DAMAGES,
INCLUDING, BUT NOT LIMITED TO, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY
REASON WHATSOEVER.
*/

//EasyTranfer Struct data across UART
//#include <EasyTransfer.h>
//create object
//EasyTransfer ET; 
//create two objects
//EasyTransfer ETout; 
//EasyTransfer ETin; //Just read for now

//struct RECEIVE_DATA_STRUCTURE{
//  //put your variable definitions here for the data you want to receive
//  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
//  float temperature;
//};
//struct SEND_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to receive
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  //int testNumber;
  
//};
//give a name to the group of data
//RECEIVE_DATA_STRUCTURE rxdata;
//SEND_DATA_STRUCTURE txdata;

#include <HCTFT_Touch.h>
#include "SPI.h"
#include "HCTFT.h"
//#include <OneWire.h>  //For temperature sensor

/*
DS3231: Real-Time Clock. Simple example
Read more: www.jarzebski.pl/arduino/komponenty/zegar-czasu-rzeczywistego-rtc-ds3231.html
GIT: https://github.com/jarzebski/Arduino-DS3231
Web: http://www.jarzebski.pl
(c) 2014 by Korneliusz Jarzebski
*/

#include <Wire.h>
#include <DS3231.h>
#define LCDLARGE_52PT

DS3231 clock;
RTCDateTime dt;
//int DS18S20_Pin = 2; //DS18S20 Signal pin on digital 8
//OneWire ds(DS18S20_Pin);  
float dataReceived;
int upTime;
#define LM35ANAPIN 0
int lm35ReadbackValue;
double tempC;

/* I2C Comms with other Arduino START */
#include <EasyTransferI2C.h>

//create object
EasyTransferI2C ET; 

struct RECEIVE_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to receive
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  
  double tempC;
};

//give a name to the group of data
RECEIVE_DATA_STRUCTURE mydata;

//define slave i2c address
#define I2C_SLAVE_ADDRESS 9
/* I2C Comms with other Arduino END */
/* Uncomment one of the lines below to to create an instance of the TFT_Touch 
   library to match your screen */
//HCTFT_Touch Touch(ELECFREAKS_2_4_WITH_TFT01_UNO); // Elecfreaks 2.4" TFT (HCARDU0083)
HCTFT_Touch Touch(ELECFREAKS_3_2_WITH_TFT01_MEGA); // Elecfreaks 3.2" TFT (HCARDU0097)
//HCTFT_Touch Touch(MCUFRIEND_3_5_MEGA); // Mcufriend 3.5" TFT (HCARDU0096)

HCTFT HCTFT(ELECFREAKS_3_2_9341_WITH_TFT01_MEGA); // Elecfreaks 3.2" TFT (HCARDU0097)

void setup() 
{
  Serial.begin(9600);
  analogReference(INTERNAL1V1); //For LM35 Temperature Sensor  
  HCTFT.Init(); //Initialise display
   /* Select one of the libraries fonts and seed the Arduino random function*/
//  HCTFT.SetFont(LargeProp_25pt);
//  randomSeed(analogRead(0));
//  int X1 = random(HCTFT.Max_X());
//  int X2 = random(HCTFT.Max_X());
//  int Y1 = random(HCTFT.Max_Y());
//  int Y2 = random(HCTFT.Max_Y());
//  byte Red = random(255);
//  byte Green = random(255);
//  byte Blue = random(255);
//  byte Scale = random(2);
//  /* Get the foreground colour */
//  HCTFT.SetFG(Red,Green,Blue);
//  HCTFT.SetBG(BLACK);
//  HCTFT.SetFont(LCDLarge_24pt);
    //HCTFT.Clear();
  // dt = clock.getDateTime();
    // Restore time on RTC if lost
  // Set sketch compiling time
    clock.begin();
  //clock.setDateTime(__DATE__, __TIME__);
  dt = clock.getDateTime();
   // HCTFT.SetBG(BLACK);

  Wire.begin(I2C_SLAVE_ADDRESS);
  //start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc. 
  ET.begin(details(mydata), &Wire);
  //define handler function on receiving data
  Wire.onReceive(receive);
   
  
}

/* Main program */
void loop()
{
    //check and see if a data packet has come in. 
  if(ET.receiveData()){
    //this is how you access the variables. [name of the group].[variable name]
    //since we have data, we will blink it out. 
    
    Serial.print("Received float: ");Serial.println(mydata.tempC);
  }
  ///  Serial.print("System up since: "); Serial.println(clock.dateFormat("d F Y H:i:s", dt));
   //HCTFT.Erase(200,80,280,118);
  
   //HCTFT.SetBG(BLACK);
  
  // HCTFT.Print("Hello Sir!");
   DisplayInstructions();
  int X_RawData;
  int Y_RawData;
  int X_Coord;
  int Y_Coord;

  
  /* Check if the touch screen is currently pressed */

   if(Touch.Pressed())
    {
      /* Read the raw data for the X and Y axis */
      Y_RawData = Touch.ReadAxis(YAXIS);
      X_RawData = Touch.ReadAxis(XAXIS);
    
      /* Read the current X and Y axis as pixel co-ordinates */
      Y_Coord = Touch.GetCoord(YAXIS);
      X_Coord = Touch.GetCoord(XAXIS);
    
      HCTFT.Erase(200,80,280,118);
    
      /* Display the raw coordinates */
      HCTFT.SetFG(SILVER);
      HCTFT.SetFont(Terminal_8pt);
      HCTFT.Scale(2);
      HCTFT.Cursor(5, 80);
      HCTFT.Print("RAW X axis: ");
      HCTFT.Print(X_RawData);
      HCTFT.Cursor(5, 100);
      HCTFT.Print("RAW Y axis: ");
      HCTFT.Print(Y_RawData);
      
      /* Display the pixel coordinates */
      HCTFT.SetFG(RED);
      HCTFT.SetFont(LCDLarge_24pt);   
      HCTFT.Erase(60,170,140,220);
      HCTFT.Cursor(60, 170);
      HCTFT.Print(X_Coord);
      HCTFT.Erase(190,170,270,220);
      HCTFT.Cursor(190, 170);
      HCTFT.Print(Y_Coord);
    }
  /// Serial.println("Outside Touch.Pressed()");
}
/* Display some information on the screen */
void DisplayInstructions(void)
{
  //lm35ReadbackValue = analogRead(LM35ANAPIN);
  //tempC = lm35ReadbackValue / 9.31;
 // Serial.println(txdata.temperature);
  //txdata.temperature = 10.02;
  //txdata.testNumber = 53;
  // send that data out
   //ETout.sendData();
  //float temperature = getTemp(); //will take about 750ms to run
  //Serial.print(temperature);
  delay(10);
  HCTFT.SetFG(BLACK);
  HCTFT.SetBG(BLACK);
  HCTFT.Rect(0,15,319,38,SOLID);
  
  
  HCTFT.SetFG(TEAL);
  HCTFT.SetFont(Terminal_8pt);
  HCTFT.Scale(2);
  HCTFT.Cursor(5, 20);
  HCTFT.Print("TEMPERATURE: ");
  HCTFT.SetFG(GREEN);
  HCTFT.Cursor(215, 20);
  //HCTFT.Print(tempC,5,3);
  
  HCTFT.SetFG(BLACK);
  HCTFT.Scale(1);
  HCTFT.Cursor(1, 45);
  HCTFT.Print("Touch the screen to get the current");
  HCTFT.Cursor(10, 56);
  HCTFT.Print("coordinate as raw and pixel values");
  HCTFT.Cursor(10, 67);
  /*
   void HCTFT::Erase(unsigned int X1, unsigned int Y1, unsigned int X2, unsigned int Y2) 
   Clears a rectangular area of the screen using the background colour where:
   X1 is the starting X axis coordinate of the first corner.
   Y1 is the starting Y axis coordinate of the first corner.
   X2 is the X axis coordinate of the opposite corner.
   Y2 is the Y axis coordinate of the opposite corner.
   */
  
  
  //HCTFT.SetFont(MedProp_11pt);
  //HCTFT.SetFont(Terminal_8pt);
  HCTFT.SetFont(LCDLarge_52pt);
    //HCTFT.SetFont(LCDLarge_24pt);
  //HCTFT.Erase(0,67,319,76);

  //HCTFT.Erase(200,80,280,118);
  HCTFT.Erase(0,67,(int)HCTFT.Max_X(),190);
   //HCTFT.Rect(0,67,HCTFT.Max_X(),190,SOLID);
  HCTFT.Scale(2);
  HCTFT.SetFG(YELLOW);
  HCTFT.Cursor(2, 67);
//  HCTFT.Rect(0,67,319,76,OUTLINE);
  HCTFT.Print(clock.dateFormat("H:i:s", clock.getDateTime()));
  HCTFT.SetFont(Terminal_8pt);
  //HCTFT.SetBG(WHITE);
  HCTFT.SetFG(GREEN);
  HCTFT.Scale(2);
  HCTFT.Cursor(5, 205);
  HCTFT.Print("X/Y AXIS IN PIXELS");
}
void receive(int numBytes) {}
/*
float getTemp() {
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if (!ds.search(addr)) {
    //no more sensors on chain, reset search
    Serial.println("Failed to address device!");
    ds.reset_search();
    return -1000;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    return -1000;
  }

  if (addr[0] != 0x10 && addr[0] != 0x28) {
    Serial.print("Device is not recognized");
    return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1); // start conversion, with parasite power on at the end

  delay(750); // Wait for temperature conversion to complete

  byte present = ds.reset();
  ds.select(addr);
  ds.write(0xBE); // Read Scratchpad


  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }

  ds.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;

  return TemperatureSum;

}*/
