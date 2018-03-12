
// UTFT_Demo_480x320 (C)2013 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
//
// This program is a demo of how to use most of the functions
// of the library with a supported display modules.
//
// This demo was made for modules with a screen resolution 
// of 480x320 pixels.
//
// This program requires the UTFT library.
//

/* YourDuinoStarter Example: Simple nRF24L01 Receive
  - WHAT IT DOES: Receives simple fixed data with nRF24L01 radio
  - SEE the comments after "//" on each line below
   Start with radios about 4 feet apart.
  - SEE the comments after "//" on each line below
  - CONNECTIONS: nRF24L01 Modules See:
  http://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo
  Uses the RF24 Library by TMRH2o here:
  https://github.com/TMRh20/RF24
   1 - GND
   2 - VCC 3.3V !!! NOT 5V
   3 - CE to Arduino pin 7
   4 - CSN to Arduino pin 8
   5 - SCK to Arduino pin 13
   6 - MOSI to Arduino pin 11
   7 - MISO to Arduino pin 12
   8 - UNUSED

   V1.02 02/06/2016
   Questions: terry@yourduino.com */


#if defined(_GFXFONT_H_)
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#define ADJ_BASELINE 11
#else
#define ADJ_BASELINE 11
#endif

/*
available fonts
| | | | | |-FreeMono12pt7b.h
| | | | | |-FreeMono18pt7b.h
| | | | | |-FreeMono24pt7b.h
| | | | | |-FreeMono9pt7b.h
| | | | | |-FreeMonoBold12pt7b.h
| | | | | |-FreeMonoBold18pt7b.h
| | | | | |-FreeMonoBold24pt7b.h
| | | | | |-FreeMonoBold9pt7b.h
| | | | | |-FreeMonoBoldOblique12pt7b.h
| | | | | |-FreeMonoBoldOblique18pt7b.h
| | | | | |-FreeMonoBoldOblique24pt7b.h
| | | | | |-FreeMonoBoldOblique9pt7b.h
| | | | | |-FreeMonoOblique12pt7b.h
| | | | | |-FreeMonoOblique18pt7b.h
| | | | | |-FreeMonoOblique24pt7b.h
| | | | | |-FreeMonoOblique9pt7b.h
| | | | | |-FreeSans12pt7b.h
| | | | | |-FreeSans18pt7b.h
| | | | | |-FreeSans24pt7b.h
| | | | | |-FreeSans9pt7b.h
| | | | | |-FreeSansBold12pt7b.h
| | | | | |-FreeSansBold18pt7b.h
| | | | | |-FreeSansBold24pt7b.h
| | | | | |-FreeSansBold9pt7b.h
| | | | | |-FreeSansBoldOblique12pt7b.h
| | | | | |-FreeSansBoldOblique18pt7b.h
| | | | | |-FreeSansBoldOblique24pt7b.h
| | | | | |-FreeSansBoldOblique9pt7b.h
| | | | | |-FreeSansOblique12pt7b.h
| | | | | |-FreeSansOblique18pt7b.h
| | | | | |-FreeSansOblique24pt7b.h
| | | | | |-FreeSansOblique9pt7b.h
| | | | | |-FreeSerif12pt7b.h
| | | | | |-FreeSerif18pt7b.h
| | | | | |-FreeSerif24pt7b.h
| | | | | |-FreeSerif9pt7b.h
| | | | | |-FreeSerifBold12pt7b.h
| | | | | |-FreeSerifBold18pt7b.h
| | | | | |-FreeSerifBold24pt7b.h
| | | | | |-FreeSerifBold9pt7b.h
| | | | | |-FreeSerifBoldItalic12pt7b.h
| | | | | |-FreeSerifBoldItalic18pt7b.h
| | | | | |-FreeSerifBoldItalic24pt7b.h
| | | | | |-FreeSerifBoldItalic9pt7b.h
| | | | | |-FreeSerifItalic12pt7b.h
| | | | | |-FreeSerifItalic18pt7b.h
| | | | | |-FreeSerifItalic24pt7b.h
| | | | | |-FreeSerifItalic9pt7b.h
| | | | | |-Org_01.h
| | | | | |-Picopixel.h
| | | | | |-Tiny3x3a2pt7b
| | | | | `-TomThumb.h
*/

#include <avr/wdt.h>
#include <SPI.h>          // f.k. for Arduino-1.5.2
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include "RF24.h"  //For wireless

#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/PicoPixel.h>
#include <Fonts/FreeSansOblique9pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/Org_01.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeMono18pt7b.h>
#include <Fonts/FreeMono12pt7b.h>


#include <Wire.h>
//#include <EasyTransferI2C.h>

// (Create an instance of a radio, specifying the CE and CS pins. )
RF24 myRadio(42, 53); // "myRadio" is the identifier you will use in following methods
byte addresses[][6] = { "1Node"}; // Create address for 1 pipe.

struct payload_t {
  int channelNumber;
  float tempC;
};

//create object
//EasyTransferI2C ET; 

/*struct RECEIVE_DATA_STRUCTURE{
    //put your variable definitions here for the data you want to receive
    //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
    bool      BoilerOn;
    bool      PumpOn;
    bool      FanOn;
    float     AmbTempTh;
    float     PipeTempTh;
    double    ambTemp;
    double    pipeTemp;
};

float pipeTemp;
*/
//give a name to the group of data
//RECEIVE_DATA_STRUCTURE mydata;

//define slave i2c address
//#define I2C_SLAVE_ADDRESS 9

//void receive(int numBytes){}

//#include <UTFTGLUE.h>
//UTFTGLUE myGLCD(0x9488,A2,A1,A3,A4,A0);
MCUFRIEND_kbv tft;

// Declare which fonts we will be using
//#if !defined(SmallFont)
//extern uint8_t SmallFont[];    //.kbv GLUE defines as GFXFont ref
//#endif

#if !defined(SevenSegNumFont)
#endif
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

//*********************************
// COLORS
//*********************************
// VGA color palette
#define VGA_BLACK		0x0000
#define VGA_WHITE		0xFFFF
#define VGA_RED			0xF800
#define VGA_GREEN		0x0400
#define VGA_BLUE		0x001F
#define VGA_SILVER		0xC618
#define VGA_GRAY		0x8410
#define VGA_MAROON		0x8000
#define VGA_YELLOW		0xFFE0
#define VGA_OLIVE		0x8400
#define VGA_LIME		0x07E0
#define VGA_AQUA		0x07FF
#define VGA_TEAL		0x0410
#define VGA_NAVY		0x0010
#define VGA_FUCHSIA		0xF81F
#define VGA_PURPLE		0x8010
#define VGA_TRANSPARENT	0xFFFFFFFF

#define RGB(r, g, b) (((r&0xF8)<<8)|((g&0xFC)<<3)|(b>>3))

#define GREY      RGB(127, 127, 127)
#define DARKGREY  RGB(64, 64, 64)
#define TURQUOISE RGB(0, 128, 128)
#define PINK      RGB(255, 128, 192)
#define OLIVE     RGB(128, 128, 0)
#define PURPLE    RGB(128, 0, 128)
#define AZURE     RGB(0, 128, 255)
#define ORANGE    RGB(255,128,64)
/*
DS3231: Real-Time Clock. Simple example
Read more: www.jarzebski.pl/arduino/komponenty/zegar-czasu-rzeczywistego-rtc-ds3231.html
GIT: https://github.com/jarzebski/Arduino-DS3231
Web: http://www.jarzebski.pl
(c) 2014 by Korneliusz Jarzebski
*/

#include <DS3231.h>

DS3231 clock;
RTCDateTime dt;
RTCDateTime upTime; 

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define LM35ANAPIN 0
int lm35ReadbackValue;
float minAmbTemperature = 0;
float maxAmbTemperature = 0;
double averagedTempC;

void setup()
{
    Serial.begin(9600);
    uint32_t when = millis();
    Serial.println("Serial took " + String((millis() - when)) + "ms to start");

    myRadio.begin();  // Start up the physical nRF24L01 Radio
    myRadio.setChannel(108);  // Above most Wifi Channels
    // Set the PA Level low to prevent power supply related issues since this is a
    myRadio.setPALevel(RF24_PA_MIN);
    //myRadio.setPALevel(RF24_PA_MAX);  // Uncomment for more power
    myRadio.setDataRate(RF24_250KBPS); // Fast enough.. Better range

    myRadio.openReadingPipe(1, addresses[0]); // Use the first entry in array 'addresses' (Only 1 right now)
    myRadio.startListening();

	clock.begin();
    
	// Restore time on RTC if lost
	// Set sketch compiling time
	  //clock.setDateTime(__DATE__, __TIME__);
    upTime = clock.getDateTime();
    analogReference(INTERNAL1V1); //For LM35 Temperature Sensor  
    randomSeed(analogRead(0));

    //Wire.begin(I2C_SLAVE_ADDRESS);
    //start the i2c library, pass in the data details 
    //and the name of the serial port. 
    //Can be Serial, Serial1, Serial2, etc. 
    //ET.begin(details(mydata), &Wire);
    //define handler function on receiving data
    //Wire.onReceive(receive);

    uint16_t ID = tft.readID(); //
    Serial.print("ID = 0x");
    Serial.println(ID, HEX);
    if (ID == 0xD3D3) ID = 0x9481; // write-only shield
    tft.begin(ID);
    tft.invertDisplay(true);
    tft.setFont(&FreeSans24pt7b);


    //Enable watchdog timer

	//disable all interrupts
	cli();
	/* Clear MCU Status Register. Not really needed here as we don't need to know why the MCU got reset. page 44 of datasheet */
	MCUSR = 0;


	wdt_reset();

	WDTCSR |= B00011000;
	WDTCSR =  B01101001;

	// Enable all interrupts.
	sei();
    tft.fillScreen(BLACK);
    minAmbTemperature = 25;
}

uint16_t scrollbuf[320];    // my biggest screen is 320x480
int count = 0;

void printmsg(int row, const char *msg)
{
    //tft.setTextColor(YELLOW, BLACK);
    tft.setCursor(0, row);
    tft.println(msg);
}


void loop()
{
    count = count < 25 ? count+=1 : count;
    payload_t payload;

    while (myRadio.available())  // While there is data ready
    if (myRadio.available())  // While there is data ready
    {
      myRadio.read(&payload, sizeof(payload)); // Get the data payload (You must have defined that already!)
      Serial.println("data captured");
      //watchdog++;
      wdt_reset();
    }
    Serial.println("Readback Temperature: " + String(payload.tempC));    
    uint8_t aspect;
    uint16_t pixel;
    uint16_t dx, rgb, n, wid, ht, msglin;
    tft.setRotation(1);
    wid = tft.width();
    ht = tft.height();
    msglin = (ht > 160) ? 200 : 112;
    dx = wid / 32;


    dt = clock.getDateTime();
    
    tft.setFont(&FreeSans12pt7b);
    tft.setTextColor(VGA_SILVER);
    printmsg(16, clock.dateFormat("d F Y ",dt));

    //dt = clock.getDateTime();
    tft.setFont(&FreeSans24pt7b);
    tft.setTextSize(1);
    tft.setCursor(0, 92);
    //tft.setTextColor(VGA_SILVER);
    //tft.println(" COLOR GRADES");
    tft.drawRect(0, 0, 180, 25,RED);

    // Draw the box that clears the active time
    // and print just the hour and mintue
    tft.setFont(&FreeSans24pt7b);
    tft.setTextSize(2);
    tft.setCursor(200, 70);
    tft.setTextColor(VGA_YELLOW,VGA_BLACK);
    dt = clock.getDateTime();
    tft.println(clock.dateFormat("H:i",dt));
    if(dt.second < 2)
      tft.fillRect(200, 0, 240, 80,BLACK);


    tft.setTextColor(VGA_RED);
    tft.setCursor(0, ht-10);
    tft.setFont(&FreeSans12pt7b);
    tft.setTextSize(1);
    tft.print("System up since: ");
        tft.println(clock.dateFormat("d F Y H:i",upTime));

            tft.drawRect(0, ht-230, 265, 100,VGA_AQUA); //x/y/wid/height/color

            tft.setTextColor(YELLOW);
            tft.setTextSize(1);
            tft.setFont(&FreeMono18pt7b);
            tft.setCursor(5, ht-200);
            tft.fillRect(105, ht-225, 125, 30,BLACK); //x/y/wid/height/color
            averagedTempC = approxRollingAverage(averagedTempC,payload.tempC,5);
            //tft.println("Pipe: " + String(payload.channelNumber) + "'C");
            tft.println("AVG: " + String(averagedTempC));
            Serial.println("Pipe Temp: " + String(payload.channelNumber) + " °C");

            tft.fillRect(105, ht-175, 125, 30,BLACK); //x/y/wid/height/color
            tft.setCursor(5, ht-150);
            tft.println("Amb: " + String(payload.tempC) + "'C");
            Serial.print("Amb Temp: ");Serial.println(payload.tempC);

            //Min/Max values
            
            if(count > 20 && averagedTempC < minAmbTemperature)
               minAmbTemperature = averagedTempC;
            if(averagedTempC > maxAmbTemperature)
               maxAmbTemperature = averagedTempC;

            tft.setFont(&FreeMono12pt7b);
            tft.fillRect(270, ht-120, 195, 40,BLACK); //x/y/wid/height/color
            tft.setTextColor(GREEN);
            tft.setCursor(270, ht-90);
            tft.println("Max Temp " + String(maxAmbTemperature));
            Serial.print("Max Temp ");Serial.println(maxAmbTemperature);

            tft.fillRect(270, ht-75, 195, 40,BLACK); //x/y/wid/height/color
            tft.setTextColor(BLUE);
            tft.setCursor(270, ht-45);
            tft.println("Min Temp " + String(minAmbTemperature));
            Serial.print("Min Temp");Serial.println(minAmbTemperature);
            delay(200);
            //Serial.println("Serial took " + String((millis() - when)) + "ms to start");
        //}
        delay(10);
        wdt_reset();
    //}
    //delay(1000);
}

double approxRollingAverage (double avg, double new_sample , int N) {

    avg -= avg / N;
    avg += new_sample / N;
    return avg;
}

