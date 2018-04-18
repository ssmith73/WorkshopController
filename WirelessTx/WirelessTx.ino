
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
#include <TimeLib.h>
#include <TimeAlarms.h>
AlarmId id;
/*-----( Declare Constants and Pin Numbers )-----*/
//None yet
/*-----( Declare objects )-----*/
// (Create an instance of a radio, specifying the CE and CS pins. )
RF24 myRadio(7, 8); // "myRadio" is the identifier you will use in following methods
					/*-----( Declare Variables )-----*/
byte 	addresses[][6] = { "1Node" "2Node" }; // Create address for 1 pipe.
float 	tempC;
int 	reading;
int 	tempPin = 0;
unsigned long startTime = 0;
bool 	boilerOn;
unsigned long loopTime = 0;

struct payload_t {
  int  	channelNumber;
  float tempC;
  float pipeTempC;
  float ambTempTh;
  float pipeTempTh;
  bool 	boilerOn;
};
#define TX_CHANNEL 109


void setup()   /****** SETUP: RUNS ONCE ******/
{

	setTime(20,25,0,3,31,2018);
	Alarm.timerRepeat(20,RepeatEvery20Seconds);
	Alarm.timerRepeat(60,RepeatEvery60Seconds);
	Alarm.timerRepeat(30,RepeatEvery30Seconds);
	//For the LM35 Temperature sensor, use more of the ADC range
	//This sensor is used as it seems simpler to stick it to the water pipe
	analogReference(INTERNAL);
	// Use the serial Monitor (Symbol on far right). Set speed to 115200 (Bottom Right)
	Serial.begin(9600);
	delay(1000);
	Serial.println(F("RF24/Simple Transmit data Test"));
	Serial.println(F("Questions: terry@yourduino.com"));
	myRadio.begin();  // Start up the physical nRF24L01 Radio
	myRadio.setChannel(TX_CHANNEL);  // Above most Wifi Channels
							  // Set the PA Level low to prevent power supply related issues since this is a
							  // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
	myRadio.setPALevel(RF24_PA_MIN);
	//myRadio.setPALevel(RF24_PA_MAX);  // Uncomment for more power
    myRadio.setDataRate(RF24_250KBPS); // Fast enough.. Better range
	myRadio.openWritingPipe(addresses[0]); // Use the first entry in array 'addresses' (Only 1 right now)
	startTime = millis();
	delay(1000);
}//--(end setup )---

unsigned long prevMilliTime = 0;
unsigned long milliTime = 0;
unsigned long diff = 0;
int interval1Sec = 1000;
int ilterval5Sec = 5000;
int ambTempTh, pipeTempTh;

void digitalClockDisplay() {
	Serial.print(hour());
	Serial.print(minute());
	Serial.print(second());
	Serial.println();
}
void RepeatEvery60Seconds() {
		ambTempTh = 1;
		pipeTempTh = 5;
		boilerOn = true;
		Serial.println("60 Seconds!: "); 
}
void RepeatEvery30Seconds() {
		ambTempTh = -10;
		pipeTempTh = -15;
		boilerOn = false;
		Serial.println("30 Seconds!");
}
void RepeatEvery20Seconds() {
		ambTempTh = 10;
		pipeTempTh = 15;
		boilerOn = false;
		Serial.println("20 Seconds ");
}
void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
	digitalClockDisplay();
	Alarm.delay(1000); //wait one second b/n display
	unsigned long progTime = 0;

	//TempSense
	reading = analogRead(tempPin);
	tempC = reading / 9.31;
	Serial.println(tempC);
	payload_t payload = {0,tempC, tempC/2,ambTempTh,pipeTempTh,boilerOn};
	myRadio.write(&payload, sizeof(payload)); //  Transmit the data

	//Wireless Tx
	Serial.println();
	Serial.println("WirelessTx\t\tUsing radio channel:  " + String(TX_CHANNEL));
	Serial.println("Data Transmitted = channelNumber: " + String(payload.channelNumber));
	Serial.println("Data Transmitted = amb: " + String(payload.tempC));
	Serial.println("Data Transmitted = amb/2: " + String(payload.tempC/2));
	Serial.println("Data Transmitted = ambTempTh: " + String(payload.ambTempTh));
	Serial.println("Data Transmitted = pipeTempTh: " + String(payload.pipeTempTh));
	Serial.println("Data Transmitted = boilerOn: " + String(payload.boilerOn));
	Serial.println();
} //--(end main loop )---
