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

V1.02 02/06/2016
Questions: terry@yourduino.com */

/*-----( Import needed libraries )-----*/
#include <SPI.h>   // Comes with Arduino IDE
#include "RF24.h"  // Download and Install (See above)
#include <OneWire.h>  //For temperature sensor
/*-----( Declare Constants and Pin Numbers )-----*/
int DS18S20_Pin = 2; //DS18S20 Signal pin on digital 2
					 /*-----( Declare objects )-----*/
					 // (Create an instance of a radio, specifying the CE and CS pins. )
RF24 myRadio(7, 8); // "myRadio" is the identifier you will use in following methods
					/*-----( Declare Variables )-----*/
byte addresses[][6] = { "1Node","2Node" }; // Create address for 2 pipes.
float dataTransmitted = 12;  // Data that will be Transmitted from the transmitter
							 //Temperature chip i/o
OneWire ds(DS18S20_Pin);  // on digital pin 2

/* What will actually be transmitted across wireless link */
//struct payload_t {
//	int channelNumber;
//	float tempC;
//};

struct payload_t {
  int channelNumber;
  float tempC;
  float pipeTempC;
  float ambTempTh;
  float pipeTempTh;
  bool boilerOn;
};


void setup()   /****** SETUP: RUNS ONCE ******/
{
	// Use the serial Monitor (Symbol on far right). Set speed to 115200 (Bottom Right)
	Serial.begin(115200);
	float temperature = getTemp(); //will take about 750ms to run
	Serial.println(temperature);
	delay(1000);
	Serial.println(F("RF24/Simple Transmit data Test"));
	Serial.println(F("Questions: terry@yourduino.com"));
	dataTransmitted = -21; // Arbitrary known data to transmit. Change it to test...
	myRadio.begin();  // Start up the physical nRF24L01 Radio
	myRadio.setChannel(108);  // Above most Wifi Channels
							  // Set the PA Level low to prevent power supply related issues since this is a
							  // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
							  // myRadio.setPALevel(RF24_PA_MIN);
	myRadio.setPALevel(RF24_PA_MAX);  // Uncomment for more power
	myRadio.setDataRate(RF24_250KBPS); // Fast enough.. Better range

	myRadio.openWritingPipe(addresses[0]); // Use the first entry in array 'addresses' (Only 1 right now)
	delay(1000);
}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{

	//myRadio.write(&dataTransmitted, sizeof(dataTransmitted)); //  Transmit the data
	payload_t payload = { 1,dataTransmitted,999, 999,999,false};

	//myRadio.write(&tempC, sizeof(tempC)); //  Transmit the data
	myRadio.write(&payload, sizeof(payload)); //  Transmit the data

	Serial.print(F("Data Transmitted = channel: "));Serial.println(payload.channelNumber);
	Serial.print(F("Data Transmitted = temperature: "));Serial.println(payload.tempC);
	Serial.print(dataTransmitted);
	Serial.println(F(" No Acknowledge expected"));
	//dataTransmitted = dataTransmitted +1.3;  // Send different data next time
	dataTransmitted = getTemp();
	delay(800);

}//--(end main loop )---

 /*-----( Declare User-written Functions )-----*/
float getTemp() {
	//returns the temperature from one DS18S20 in DEG Celsius

	byte data[12];
	byte addr[8];

	if (!ds.search(addr)) {
		//no more sensors on chain, reset search
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

}

//*********( THE END )***********
