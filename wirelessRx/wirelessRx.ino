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

   /*-----( Import needed libraries )-----*/
#include <SPI.h>   // Comes with Arduino IDE
#include "RF24.h"  // Download and Install (See above)
/*-----( Declare Constants and Pin Numbers )-----*/
#define CE_PIN 9
#define CS_PIN 10
/*-----( Declare objects )-----*/
// (Create an instance of a radio, specifying the CE and CS pins. )
RF24 myRadio(CE_PIN, CS_PIN); // "myRadio" is the identifier you will use in following methods
/*-----( Declare Variables )-----*/
byte addresses[][6] = { "1Node" ,"2Node"}; // Create address for 2 pipes.
float dataReceived;  // Data that will be received from the transmitter

struct payload_t {
  int channelNumber;
  float tempC;
  float ambTempTh;
  float pipeTempTh;
  bool boilerOn;
};

void setup()   /****** SETUP: RUNS ONCE ******/
{
	// Use the serial Monitor (Symbol on far right). Set speed to 115200 (Bottom Right)
	Serial.begin(115200);
	delay(1000);
	Serial.println(F("RF24/Simple Receive data Test"));
	Serial.println(F("Questions: terry@yourduino.com"));

	myRadio.begin();  // Start up the physical nRF24L01 Radio
	myRadio.setChannel(108);  // Above most Wifi Channels
	// Set the PA Level low to prevent power supply related issues since this is a
	// getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
	//myRadio.setPALevel(RF24_PA_MIN);
	myRadio.setPALevel(RF24_PA_MAX);  // Uncomment for more power
	myRadio.setDataRate(RF24_250KBPS); // Fast enough.. Better range

	myRadio.openReadingPipe(1, addresses[0]); // Use the first entry in array 'addresses' (Only 1 right now)
	myRadio.startListening();

}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{

	  payload_t payload;

    while (myRadio.available()) // While there is data ready
    {
        myRadio.read(&payload, sizeof(payload)); // Get the data payload (You must have defined that already!)
        Serial.println("data captured");
        Serial.println("chan Number: " + String(payload.channelNumber));
        Serial.println("tempC: " + String(payload.tempC));
        Serial.println("ambTempTh: " + String(payload.ambTempTh));
        Serial.println("pipeTempTh: " + String(payload.pipeTempTh));
        Serial.println("boilerOn: " + String(payload.boilerOn));
        delay(500);
        //wdt_reset();
    }
    Serial.println("Readback Temperature: " + String(payload.tempC));
    delay(100);
}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/

//None yet
//*********( THE END )***********

