
/* YourDuinoStarter Example: Simple nRF24L01 Transmit
- WHAT IT DOES: Transmits simple fixed data with nRF24L01 radio
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

  From Top Side (xtal at bottom): Pinout
  1 2	GND	VCC
  3 4	CE	CSN
  5 6	SCK	MOSI
  7 8	MISO	IRQ

V1.02 02/06/2016
Questions: terry@yourduino.com */

/*-----( Import needed libraries )-----*/
#include <SPI.h>   // Comes with Arduino IDE
#include "RF24.h"  // Download and Install (See above)
/*-----( Declare Constants and Pin Numbers )-----*/
//None yet
/*-----( Declare objects )-----*/
// (Create an instance of a radio, specifying the CE and CS pins. )
RF24 myRadio(7, 8); // "myRadio" is the identifier you will use in following methods
					/*-----( Declare Variables )-----*/
byte addresses[][6] = { "1Node" }; // Create address for 1 pipe.
float tempC;
int reading;
int tempPin = 0;

struct payload_t {
	int channelNumber;
	float tempC;
};

void setup()   /****** SETUP: RUNS ONCE ******/
{
	//For the LM35 Temperature sensor, use more of the ADC range
	//This sensor is used as it seems simpler to stick it to the water pipe
	analogReference(INTERNAL);

	// Use the serial Monitor (Symbol on far right). Set speed to 115200 (Bottom Right)
	Serial.begin(115200);
	delay(1000);
	Serial.println(F("RF24/Simple Transmit data Test"));
	Serial.println(F("Questions: terry@yourduino.com"));
	myRadio.begin();  // Start up the physical nRF24L01 Radio
	myRadio.setChannel(108);  // Above most Wifi Channels
							  // Set the PA Level low to prevent power supply related issues since this is a
							  // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
	//myRadio.setPALevel(RF24_PA_MIN);
	myRadio.setPALevel(RF24_PA_MAX);  // Uncomment for more power
    myRadio.setDataRate(RF24_250KBPS); // Fast enough.. Better range
	myRadio.openWritingPipe(addresses[0]); // Use the first entry in array 'addresses' (Only 1 right now)
	delay(1000);
}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
	//TempSense
	reading = analogRead(tempPin);
	tempC = reading / 9.31;
	Serial.println(tempC);
	payload_t payload = { 0,tempC };
	//myRadio.write(&tempC, sizeof(tempC)); //  Transmit the data
	myRadio.write(&payload, sizeof(payload)); //  Transmit the data

	//Wireless Tx
	Serial.print(F("Data Transmitted = pipe: ")); Serial.print(payload.channelNumber);
	Serial.print(F(" Temp: "));Serial.println(payload.tempC);
	Serial.println(F(" No Acknowledge expected"));
	delay(1000);

}//--(end main loop )---
