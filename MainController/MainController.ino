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


#include <SPI.h>   // Comes with Arduino IDE
#include "RF24.h"  // Download and Install (See above)
#include <avr/wdt.h>
#define AMB_RELAY_EN 2
#define PUMP_RELAY_EN 3
#define FAN_RELAY_EN 4
#define TEMPERATURE_MAX 7
#define PIPE_TEMP_MIN 21
#define PIPE_ANALOG_PIN 0

   //RGB LED - 5V module
#define RED 5
#define GREEN 6
#define BLUE 9

// (Create an instance of a radio, specifying the CE and CS pins. )
RF24 myRadio(7,8); // "myRadio" is the identifier you will use in following methods
/*-----( Declare Variables )-----*/
byte addresses[][6] = { "1Node","2Node" }; // Create address for 1 pipe.
//byte addresses[][6] = { "1Node" }; // Create address for 1 pipe.
float watchdog;

/*
DS3231: Real-Time Clock. Simple example
Read more: www.jarzebski.pl/arduino/komponenty/zegar-czasu-rzeczywistego-rtc-ds3231.html
GIT: https://github.com/jarzebski/Arduino-DS3231
Web: http://www.jarzebski.pl
(c) 2014 by Korneliusz Jarzebski
*/

#include <Wire.h>
#include <DS3231.h>

DS3231 clock;
RTCDateTime dt;
RTCDateTime upTimeClk;
int numSeconds;
RTCDateTime startTime;
RTCDateTime EndTime;
float dataReceived;
float averagedAmbTemp = 0;
int upTime;
typedef enum
{
	ReturnPipe,
	WorkshopAmbient
} SensorPositions;

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
struct ElapsedTime myTime;

ElapsedTime CalculateDuration(int numElapsedSeconds);

float numPipeMeasurements = 0;
float numAmbMeasurements = 0;
float ambientTemperature = 0;
float pipeTemperature = 0;
float pipeTempC;
int   readTempC;
bool  boilerOn = false;
payload_t payload;

void setup() /****** SETUP: RUNS ONCE ******/
{
	pinMode(AMB_RELAY_EN, OUTPUT);
	pinMode(PUMP_RELAY_EN, OUTPUT);
	pinMode(FAN_RELAY_EN, OUTPUT);
	pinMode(RED, OUTPUT);
	pinMode(GREEN, OUTPUT);
	pinMode(BLUE, OUTPUT);

	digitalWrite(PUMP_RELAY_EN, HIGH);
	digitalWrite(FAN_RELAY_EN, HIGH);
	digitalWrite(AMB_RELAY_EN, HIGH);

	//For the LM35 Temperature sensor, (used on the pipe),use more of the ADC range
	//This sensor is used as it seems simpler to stick it to the water pipe
	analogReference(INTERNAL);

	Serial.begin(115200);
	clock.begin();
	delay(1000);
	Serial.println(F("RF24/Simple Receive data Test"));
	Serial.println(F("Questions: terry@yourduino.com"));

	myRadio.begin();		 // Start up the physical nRF24L01 Radio
	myRadio.setChannel(108); // Above most Wifi Channels
	// Set the PA Level low to prevent power supply related issues since this is a
	myRadio.setPALevel(RF24_PA_MIN);
	//myRadio.setPALevel(RF24_PA_MAX);  // Uncomment for more power
	myRadio.setDataRate(RF24_250KBPS); // Fast enough.. Better range

	myRadio.openReadingPipe(0, addresses[0]); // Use the first entry in array 'addresses' (Only 1 right now)
	myRadio.openWritingPipe(addresses[1]); // Use the first entry in array 'addresses' (Only 1 right now)
	myRadio.startListening();
	watchdog = 0;
	numSeconds = 0;

	// Restore time on RTC if lost
	// Set sketch compiling time
	//clock.setDateTime(__DATE__, __TIME__);

	// Set from UNIX timestamp
	// clock.setDateTime(1397408400);

	//disable all interrupts
	cli();
	/* Clear MCU Status Register. Not really needed here as we don't need to know why the MCU got reset. page 44 of datasheet */
	MCUSR = 0;

	wdt_reset();

	WDTCSR |= B00011000;
	WDTCSR = B01101001;

	// Enable all interrupts.
	sei();
	Serial.println(F("Setup Complete: "));

	upTimeClk = clock.getDateTime();

} //--(end setup )--j

void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
	
	SensorPositions sensorPosition = ReturnPipe;
  
	readTempC = analogRead(PIPE_ANALOG_PIN);
	pipeTemperature = readTempC / 9.31;
	Serial.print("System up since: "); Serial.println(clock.dateFormat("d F Y H:i:s", upTimeClk));
	if (myRadio.available()) // Check for incoming data from transmitters
	{
		Serial.println(F("Data is available "));
		dt = clock.getDateTime();

		if (dt.second == 0 || dt.second == 1) { //print once a minute
			myTime = CalculateDuration(numSeconds);

			Serial.print("\n\t\tHeater was on for: H:M:S ");
			Serial.print(myTime.elapsedHours);Serial.print(":");
			Serial.print(myTime.elapsedMinutes);Serial.print(":");
			Serial.print(myTime.elapsedSeconds);
			Serial.println("\n");
		}

		Serial.println(F("\nRadio is available: "));
		//Serial.println(clock.dateFormat("d F Y H:i:s", dt));
		while (myRadio.available())  // While there is data ready
		{
			myRadio.read(&payload, sizeof(payload)); // Get the data payload (You must have defined that already!)
			watchdog++;
			wdt_reset();
		}
		Serial.print(watchdog);

		switch (payload.channelNumber)
		{
		case ReturnPipe:
			Serial.println(" PipeSensor data returned");
			Serial.print("Number of measurements: Pipe:Ambient : ");
			Serial.print(++numPipeMeasurements);Serial.print(":");Serial.println(numAmbMeasurements);
			pipeTemperature = payload.tempC;
			break;
		case WorkshopAmbient:
			Serial.println(" Workshop ambient data returned");
			Serial.print("Number of measurements: Pipe:Ambient : ");
			Serial.print(numPipeMeasurements);Serial.print(":");Serial.println(++numAmbMeasurements);
			ambientTemperature = payload.tempC;
			averagedAmbTemp = approxRollingAverage(averagedAmbTemp, payload.tempC, 10);
			Serial.println("Averaged ambient temperature is: " + String(averagedAmbTemp));
			break;
		default:
			break;
		}
		Serial.print(" Data received = channel "); Serial.println(payload.channelNumber);
		Serial.print(" Data received = Temp:  "); Serial.println(payload.tempC);
		Serial.print("Last Recorded temperatures Ambient:Pipe ");
		Serial.print(ambientTemperature);Serial.print(":");
		Serial.println(pipeTemperature);

		if (averagedAmbTemp > TEMPERATURE_MAX) {

			boilerOn = false;
			Serial.print("Ambient temperature > "); Serial.print(TEMPERATURE_MAX); Serial.println(" Turn off Heat");
			digitalWrite(AMB_RELAY_EN, HIGH);
			Serial.print("Boiler turned off: "); Serial.println(clock.dateFormat("d F Y H:i:s", dt));
			// As heater turns on, the water temperture will heat up, turn fans off 
			// if temperature of pipe is less than some (empirical) threshold
			if (pipeTemperature < PIPE_TEMP_MIN) {
				Serial.print(" Pipe temperature below ");Serial.print(PIPE_TEMP_MIN);
				Serial.println(" .Turned off Fans and Pump");

				digitalWrite(PUMP_RELAY_EN, HIGH);
				delay(100);
				digitalWrite(FAN_RELAY_EN, HIGH);
				digitalWrite(RED, LOW);
				digitalWrite(GREEN, LOW);
				digitalWrite(BLUE, HIGH);
			}

			else {
				boilerOn = true;
				Serial.print(" Pipe temperature above ");Serial.print(PIPE_TEMP_MIN);
				Serial.println(" .Turned on Fans and Pump");

				digitalWrite(PUMP_RELAY_EN, LOW);
				delay(100);
				digitalWrite(FAN_RELAY_EN, LOW);
				digitalWrite(RED, LOW);
				digitalWrite(GREEN, HIGH);
				digitalWrite(BLUE, LOW);
			}

		}
		else if (averagedAmbTemp != 0 && averagedAmbTemp < TEMPERATURE_MAX) {
			numSeconds++;
			Serial.println(clock.dateFormat("d F Y H:i:s", dt));
			Serial.print("temperature <= "); Serial.print(TEMPERATURE_MAX);
			Serial.println(" .Turned on Heater and pump");
			digitalWrite(AMB_RELAY_EN, LOW);
			boilerOn = true;
			delay(100);
			digitalWrite(PUMP_RELAY_EN, LOW);
			digitalWrite(RED, LOW);
			digitalWrite(GREEN, HIGH);
			digitalWrite(BLUE, LOW);
			if (pipeTemperature > PIPE_TEMP_MIN)
				digitalWrite(FAN_RELAY_EN, LOW);
			else {
				digitalWrite(FAN_RELAY_EN, HIGH);
				digitalWrite(RED, LOW);
				digitalWrite(GREEN, LOW);
				digitalWrite(BLUE, HIGH);
			}
		}
	} //END Radio available

	delay(500);
  if(myRadio.available()){
    //Send data to the display
     Serial.println("*****************Sending Data to display************************");
     myRadio.stopListening();
     payload = {0, payload.tempC,pipeTemperature,TEMPERATURE_MAX,PIPE_TEMP_MIN ,boilerOn};
     myRadio.write(&payload, sizeof(payload)); //  Transmit the data
     myRadio.startListening();
     Serial.println("****************************************************************");
     Serial.println("****************************************************************");
  }
 

	delay(200);

}//--(end main loop )---

ElapsedTime CalculateDuration(int numElapsedSeconds) {
	struct ElapsedTime et;

	et.elapsedSeconds = numElapsedSeconds % 60;
	et.elapsedMinutes = numElapsedSeconds > 60 ? (numElapsedSeconds / 60 - et.elapsedHours * 60) : numElapsedSeconds / 60;
	et.elapsedHours = et.elapsedMinutes / 60;
	return et;
}
double approxRollingAverage (double avg, double new_sample , int N) {

    avg -= avg / N;
    avg += new_sample / N;
    return avg;
}


/* Store a running average of the last 5 results for each measurement
	tbd
*/
