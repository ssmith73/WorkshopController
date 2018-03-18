
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
   3 - CE to Arduino pin 7 (42 on mega - though it doens't matter - any digital pin)
   4 - CSN to Arduino pin 8 (53 on mega - or any digital pin)
   5 - SCK to Arduino pin 13 (52 MEGA)
   6 - MOSI to Arduino pin 11 (51 MEGA)
   7 - MISO to Arduino pin 12 (50 MEGA)
   8 - UNUSED (IRQ)

   V1.02 02/06/2016
   Questions: terry@yourduino.com */


#if defined(_GFXFONT_H_)
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#define ADJ_BASELINE 11
#else
#define ADJ_BASELINE 11
#endif


#include <avr/wdt.h>
#include <SPI.h>          // f.k. for Arduino-1.5.2
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include "RF24.h"  //For wireless

#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
//#include <Fonts/PicoPixel.h>
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
byte addresses[][6] = { "1Node","2Node"}; // Create address for 1 pipe.



struct payload_t {
  int	channelNumber;
  float tempC;
  float pipeTempC;
  float ambTempTh;
  float pipeTempTh;
  bool boilerOn;
};

struct ElapsedTime
{
	int elapsedMinutes;
	int elapsedSeconds;
	int elapsedHours;
};

ElapsedTime convertToTimeOn(int timeInSeconds);

double startTime;
struct ElapsedTime et;

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
int duration = 0;

void setup()
{
    Serial.begin(9600);
    uint32_t when = millis();
    Serial.println("Serial took " + String((millis() - when)) + "ms to start");

    myRadio.begin();         // Start up the physical nRF24L01 Radio
    myRadio.setChannel(108); // Above most Wifi Channels
    // Set the PA Level low to prevent power supply related issues since this is a
    myRadio.setPALevel(RF24_PA_MIN);
    //myRadio.setPALevel(RF24_PA_MAX);  // Uncomment for more power
    myRadio.setDataRate(RF24_250KBPS); // Fast enough.. Better range

    myRadio.openReadingPipe(1, addresses[1]); // Use the first entry in array 'addresses' (Only 1 right now)
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
    if (ID == 0xD3D3)
        ID = 0x9481; // write-only shield
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
    WDTCSR = B01101001;

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
    count = count < 25 ? count += 1 : count;
    payload_t payload;

    while (myRadio.available()) // While there is data ready
    {
        myRadio.read(&payload, sizeof(payload)); // Get the data payload (You must have defined that already!)
        Serial.println("data captured");
        Serial.println("chan Number: " + String(payload.channelNumber));
        Serial.println("tempC: "+  String(payload.tempC));
        Serial.println("ambTempTh: "+  String(payload.ambTempTh));
        Serial.println("pipeTempTh: "+  String(payload.pipeTempTh));
        Serial.println("boilerOn: "+  String(payload.boilerOn));
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

    // Set date at top of screen
    dt = clock.getDateTime();
    tft.setFont(&FreeSans12pt7b);
    tft.setTextColor(VGA_SILVER);
    printmsg(18, clock.dateFormat("d F Y ", dt));

    tft.setTextSize(1);
    if (dt.hour == 0 && dt.minute == 0 && dt.second < 5)
        tft.fillRect(0, 0, 180, 25, RED);

    // Draw the box that clears the active time
    // and print just the hour and mintue
    tft.setFont(&FreeSans24pt7b);
    tft.setTextSize(2);
    tft.setCursor(200, 70);
    tft.setTextColor(VGA_YELLOW, VGA_BLACK);
    dt = clock.getDateTime();
    tft.println(clock.dateFormat("H:i", dt));
    if (dt.second < 2)
        tft.fillRect(200, 0, 240, 80, BLACK);

    tft.setTextColor(VGA_RED);
    tft.setCursor(0, ht - 10);
    tft.setFont(&FreeSans12pt7b);
    tft.setTextSize(1);
    tft.print("System up since: ");
    tft.println(clock.dateFormat("d F Y H:i", upTime));

    tft.drawRect(0, ht - 230, 265, 100, VGA_AQUA); //x/y/wid/height/color

    tft.setTextColor(YELLOW);
    tft.setTextSize(1);
    tft.setFont(&FreeMono18pt7b);
    tft.setCursor(5, ht - 200);
    tft.fillRect(105, ht - 225, 145, 30, BLACK); //x/y/wid/height/color
    averagedTempC = approxRollingAverage(averagedTempC, payload.tempC, 5);
    //tft.println("Pipe: " + String(payload.channelNumber) + "'C");
    tft.println("AMB: " + String(averagedTempC) + "'C");
    Serial.println("Pipe Temp: " + String(payload.channelNumber) + " °C");

    tft.fillRect(105, ht - 175, 145, 30, BLACK); //x/y/wid/height/color
    tft.setCursor(5, ht - 150);
    tft.println("Pipe:" + String(payload.pipeTempC) + "'C");
    Serial.print("Amb Temp: ");
    Serial.println(payload.tempC);

    //Min/Max values

    if (count > 20 && averagedTempC < minAmbTemperature)
        minAmbTemperature = averagedTempC;
    if (averagedTempC > maxAmbTemperature)
        maxAmbTemperature = averagedTempC;

    int row1 = 120;
    int row2 = 160;
    int col = 300;
    int margin = 5;

    //Draw a rectangle around the min/max boxes
    tft.drawRect(col - 2 * margin, 95, 165, 80, VGA_AQUA); //x/y/wid/height/color

    tft.setFont(&FreeMono12pt7b);
    tft.setTextColor(GREEN);
    tft.fillRect(col + 50, row1 - 22, 100, 30, BLACK); //x/y/wid/height/color
    tft.setCursor(col, row1);
    tft.println("Max " + String(maxAmbTemperature));
    Serial.print("Max ");
    Serial.println(maxAmbTemperature);

    tft.setTextColor(BLUE);
    tft.fillRect(col + 50, row2 - 22, 100, 30, BLACK); //x/y/wid/height/color
    tft.setCursor(col, row2);
    tft.println("Min " + String(minAmbTemperature));
    Serial.print("Min");
    Serial.println(minAmbTemperature);

    tft.setTextColor(VGA_TRANSPARENT);
    
    tft.setCursor(col, row2 + 50);
    tft.println("PipeTh " + String(payload.pipeTempTh));
    tft.setCursor(col, row2 + 90);
    tft.println("AmbTh " + String(payload.ambTempTh));


    //Boiler on flag
    
    tft.setFont(&FreeMonoBold18pt7b);
    tft.setTextColor(VGA_AQUA);
    
    if(payload.boilerOn) {
        
        Serial.println("Boiler On");
        if(startTime == 0)
            startTime = millis();

        // if (dt.second == 0 || dt.second == 1) { //print once a minute
		// 	//myTime = CalculateDuration(numSeconds);
        //     tft.fillRect(200, 220, 50, 50, GREEN);
        // }
        // else
        //     tft.fillRect(200, 220, 50, 50, VGA_GRAY);
    }
    else if(startTime != 0){
        Serial.println("Boiler OFF");
        duration += ((millis() - startTime)/1000);
        startTime = 0;
        ElapsedTime dur = convertToTimeOn(duration);
        
        tft.setCursor(0, 250);
        tft.println("Boiler on for: ");
        tft.fillRect(115, 260, 170, 28, BLACK); //x/y/wid/height/color
        tft.println("H:M:S " + String(dur.elapsedHours) +
         ":" + String(dur.elapsedMinutes) + ":" +
               String(dur.elapsedSeconds));
    }

    delay(100);
    wdt_reset();
}

double approxRollingAverage (double avg, double new_sample , int N) {

    avg -= avg / N;
    avg += new_sample / N;
    return avg;
}


ElapsedTime convertToTimeOn(int timeInSeconds) {
	struct ElapsedTime et;
    et.elapsedSeconds = timeInSeconds%60;
    et.elapsedMinutes = timeInSeconds/60;
    et.elapsedHours = et.elapsedMinutes/60;
    return et;
}

struct ElapsedTime myTime;
ElapsedTime CalculateDuration(int numElapsedSeconds) {
	struct ElapsedTime et;

	et.elapsedSeconds = numElapsedSeconds % 60;
	et.elapsedMinutes = numElapsedSeconds > 60 ? (numElapsedSeconds / 60 - et.elapsedHours*60) : numElapsedSeconds/60;
	et.elapsedHours = et.elapsedMinutes / 60;
	return et;
}

