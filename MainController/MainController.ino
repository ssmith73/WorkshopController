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

//#define RTC_CONNECTED
#define I2C_RX
#define I2C_RX_DEBUG
#define DEBUG

#define AMB_RELAY_EN 2
#define PUMP_RELAY_EN 3
#define FAN_RELAY_EN 4
//#define TEMPERATURE_MAX 7
//#define PIPE_TEMP_MIN 21
#define PIPE_ANALOG_PIN 0

   //RGB LED - 5V module
#define RED 5
#define GREEN 6
#define BLUE 9

#include <SPI.h>   // Comes with Arduino IDE
#include "RF24.h"  // Download and Install (See above)
#include <avr/wdt.h>
#include <string.h>


// (Create an instance of a radio, specifying the CE and CS pins. )
RF24 myRadio(7,8); // "myRadio" is the identifier you will use in following methods
/*-----( Declare Variables )-----*/
byte addresses[][6] = { "1Node","2Node" }; // Create address for 1 pipe.
//byte addresses[][6] = { "1Node" }; // Create address for 1 pipe.
float watchdog;

#ifdef I2C_RX
	/*
	  For the nano, connect
	  SCL: pin A5, SDA: pin A4
	  For the mega: SCL 21, SDA 20
	*/

	#include <Wire.h>
	#include <EasyTransferI2C.h>



	//Create object
	EasyTransferI2C ET;
	#define I2C_SLAVE_ADDRESS 9

	struct RECEIVE_DATA_STRUCTURE {
	// put your variable definitions here for the 
	// data you want to send
	// Put exact equivilent stuct in Rx
		int ambTempTh; 
		int pipeTempTh;
		bool turnOnFan;
		bool turnOffFan;
	};

	int i2c_ambTempTh = 7;
	int i2c_pipeTempTh = 21;
	bool i2c_turnOnFan = true;
	bool i2c_turnOffFan = true;

	RECEIVE_DATA_STRUCTURE i2cData;

#endif 

/*
DS3231: Real-Time Clock. Simple example
Read more: www.jarzebski.pl/arduino/komponenty/zegar-czasu-rzeczywistego-rtc-ds3231.html
GIT: https://github.com/jarzebski/Arduino-DS3231
Web: http://www.jarzebski.pl
(c) 2014 by Korneliusz Jarzebski
*/

#ifdef RTC_CONNECTED

#include <DS3231.h>

DS3231 clock;
RTCDateTime dt;
RTCDateTime upTimeClk;
int numSeconds;
RTCDateTime startTime;
RTCDateTime EndTime;
int upTime;
#endif
float averagedAmbTemp = 0;
float dataReceived;

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

#ifdef RTC_CONNECTED
struct ElapsedTime
{
	int elapsedMinutes;
	int elapsedSeconds;
	int elapsedHours;
};
struct ElapsedTime myTime;
ElapsedTime CalculateDuration(int numElapsedSeconds);
#endif


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
	#ifdef I2C_RX
	Wire.begin(I2C_SLAVE_ADDRESS);
	//Start the i2c library, pass in the data details
	//and name the serial(1/2/3) port
	ET.begin(details(i2cData),&Wire);
	Wire.onReceive(receive);
	#endif
	pinMode(AMB_RELAY_EN, OUTPUT);
	pinMode(PUMP_RELAY_EN, OUTPUT);
	pinMode(FAN_RELAY_EN, OUTPUT);
	pinMode(RED, OUTPUT);
	pinMode(GREEN, OUTPUT);
	pinMode(BLUE, OUTPUT);

	digitalWrite(PUMP_RELAY_EN, HIGH);
	digitalWrite(FAN_RELAY_EN, 
	   HIGH & i2c_turnOnFan | i2c_turnOffFan);
	digitalWrite(AMB_RELAY_EN, HIGH);

	//For the LM35 Temperature sensor, (used on the pipe),use more of the ADC range
	//This sensor is used as it seems simpler to stick it to the water pipe
	analogReference(INTERNAL);
	Serial.begin(115200);

    #ifdef RTC_CONNECTED
	clock.begin();
	numSeconds = 0;
	upTimeClk = clock.getDateTime();

	// Restore time on RTC if lost
	// Set sketch compiling time
	//clock.setDateTime(__DATE__, __TIME__);

	// Set from UNIX timestamp
	// clock.setDateTime(1397408400);
	#endif

	delay(1000);
	Serial.println(F("RF24/Simple Receive data Test"));
	Serial.println(F("Questions: terry@yourduino.com"));

	myRadio.begin();		 // Start up the physical nRF24L01 Radio
	myRadio.setChannel(108); // Above most Wifi Channels
	// Set the PA Level low to prevent power supply related issues since this is a
	myRadio.setPALevel(RF24_PA_MIN);
	//myRadio.setPALevel(RF24_PA_MAX);  // Uncomment for more power
	myRadio.setDataRate(RF24_250KBPS); // Fast enough.. Better range
	//Working in the workshiop version
	//myRadio.openReadingPipe(0, addresses[0]); // Use the first entry in array 'addresses' (Only 1 right now)

    
	//                     (pipe number (0..5), address)
	myRadio.openReadingPipe(1, addresses[0]); // Use address 0 for reading, address of write must match
	myRadio.openWritingPipe(addresses[1]); // use address 1 for writing
	myRadio.startListening();
	watchdog = 0;

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

} //--(end setup )--j

void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
	
	SensorPositions sensorPosition = ReturnPipe;
  
	readTempC = analogRead(PIPE_ANALOG_PIN);
	pipeTemperature = readTempC / 9.31;
	#ifdef RTC_CONNECTED
	Serial.print("System up since: "); Serial.println(clock.dateFormat("d F Y H:i:s", upTimeClk));
	#endif

    #ifdef I2C_RX
	if(ET.receiveData()) {

     #ifdef I2C_RX_DEBUG
     Serial.println("****************************************************************");
     Serial.println("****************************************************************");
     Serial.println("****************************************************************");
		Serial.println("I2C data received:" +
		  String("\n\tAmb Temperature: ") + String(i2cData.ambTempTh) + 
		  String("\n\tPipe Temperature: ") + String(i2cData.pipeTempTh) +
		  String("\n\tFanOn: ") + String(i2cData.turnOnFan) +
		  String("\n\tFanOFF: ") + String(i2cData.turnOffFan));
     Serial.println("****************************************************************");
     Serial.println("****************************************************************");
     Serial.println("****************************************************************");
	 #endif
	 i2c_ambTempTh = i2cData.ambTempTh;
	 i2c_pipeTempTh = i2cData.pipeTempTh;
	 i2c_turnOnFan = !i2cData.turnOnFan; //relays are active low, so invert here.
	 i2c_turnOffFan = !i2cData.turnOffFan; //relays are active low, so invert here.
	}
	#endif


	#ifdef DEBUG 
	Serial.println(F("Data is not available ")); 
	#endif
	if (myRadio.available()) // Check for incoming data from transmitters
	{
		#ifdef DEBUG
		Serial.println(F("Data is available "));
		#endif

	    #ifdef RTC_CONNECTED
		dt = clock.getDateTime();

		if (dt.second == 0 || dt.second == 1) { //print once a minute
			myTime = CalculateDuration(numSeconds);

			Serial.print("\n\t\tHeater was on for: H:M:S ");
			Serial.print(myTime.elapsedHours);Serial.print(":");
			Serial.print(myTime.elapsedMinutes);Serial.print(":");
			Serial.print(myTime.elapsedSeconds);
			Serial.println("\n");
		}
		#endif

        #ifdef DEBUG
		Serial.println(F("\nRadio is available: "));
		#endif
		//Serial.println(clock.dateFormat("d F Y H:i:s", dt));
		while (myRadio.available())  // While there is data ready
		{
			myRadio.read(&payload, sizeof(payload)); // Get the data payload (You must have defined that already!)
			watchdog++;
			wdt_reset();
		}
		#ifdef DEBUG 
		Serial.print(watchdog); 
		#endif

		switch (payload.channelNumber)
		{
		case ReturnPipe:
			#ifdef DEBUG
			Serial.println("------------------------------------");
			Serial.println(">PipeSensor data returned");
			Serial.println(">Data Received = channelNumber: " + String(payload.channelNumber));
			Serial.println(">Data Received = amb: " + String(payload.tempC));
			Serial.println(">Data Received = amb/2: " + String(payload.tempC/2));
			Serial.println(">Data Received = ambTempTh: " + String(payload.ambTempTh));
			Serial.println(">Data Received = pipeTempTh: " + String(payload.pipeTempTh));
			Serial.println(">Data Received = boilerOn: " + String(payload.boilerOn));
			Serial.println("------------------------------------");
			#endif
			pipeTemperature = payload.tempC;
			break;
		case WorkshopAmbient:
			#ifdef DEBUG
			Serial.println(" Workshop ambient data returned");
			Serial.print("Number of measurements: Pipe:Ambient : ");
			Serial.print(numPipeMeasurements);Serial.print(":");Serial.println(++numAmbMeasurements);
			#endif
			ambientTemperature = payload.tempC;
			averagedAmbTemp = approxRollingAverage(averagedAmbTemp, payload.tempC, 10);
			#ifdef DEBUG
			Serial.println("Averaged ambient temperature is: " + String(averagedAmbTemp));
			#endif
			break;
		default:
			break;
		}
		#ifdef DEBUG
		Serial.print(" Data received = channel "); Serial.println(payload.channelNumber);
		Serial.print(" Data received = Temp:  "); Serial.println(payload.tempC);
		Serial.print("Last Recorded temperatures Ambient:Pipe ");
		Serial.print(ambientTemperature);Serial.print(":");
		Serial.println(pipeTemperature);
		#endif

		if (averagedAmbTemp > i2c_ambTempTh) {

			boilerOn = false;
			#ifdef DEBUG 
			Serial.print("Ambient temperature > "); Serial.print(i2c_ambTempTh); Serial.println(" Turn off Heat"); 
			#endif
			digitalWrite(AMB_RELAY_EN, HIGH);
			#ifdef RTC_CONNECTED
			Serial.print("Boiler turned off: "); Serial.println(clock.dateFormat("d F Y H:i:s", dt));
			#endif
			// As heater turns on, the water temperture will heat up, turn fans off 
			// if temperature of pipe is less than some (empirical) threshold
			if (pipeTemperature < i2c_pipeTempTh) {
				#ifdef DEBUG
				Serial.print(" Pipe temperature below ");Serial.print(i2c_pipeTempTh);
				Serial.println(" .Turned off Fans and Pump");
				#endif

				digitalWrite(PUMP_RELAY_EN, HIGH);
				delay(100);
				digitalWrite(FAN_RELAY_EN, HIGH & i2c_turnOnFan | i2c_turnOffFan);
				digitalWrite(RED, LOW);
				digitalWrite(GREEN, LOW);
				digitalWrite(BLUE, HIGH);
			}

			else {
				boilerOn = true;
				#ifdef DEBUG
				Serial.print(" Pipe temperature above ");Serial.print(i2c_pipeTempTh);
				Serial.println(" .Turned on Fans and Pump");
				#endif

				digitalWrite(PUMP_RELAY_EN, LOW);
				delay(100);
				digitalWrite(FAN_RELAY_EN, LOW & i2c_turnOnFan | i2c_turnOffFan);
				digitalWrite(RED, LOW);
				digitalWrite(GREEN, HIGH);
				digitalWrite(BLUE, LOW);
			}

		}
		else if (averagedAmbTemp != 0 && averagedAmbTemp < i2c_ambTempTh) {
			#ifdef RTC_CONNECTED
			numSeconds++;
			Serial.println(clock.dateFormat("d F Y H:i:s", dt));
			#endif

			#ifdef DEBUG
			Serial.print("temperature <= "); Serial.print(i2c_ambTempTh);
			Serial.println(" .Turned on Heater and pump");
			#endif
			digitalWrite(AMB_RELAY_EN, LOW);
			boilerOn = true;
			delay(100);
			digitalWrite(PUMP_RELAY_EN, LOW);
			digitalWrite(RED, LOW);
			digitalWrite(GREEN, HIGH);
			digitalWrite(BLUE, LOW);
			if (pipeTemperature > i2c_pipeTempTh)
				digitalWrite(FAN_RELAY_EN, LOW & i2c_turnOnFan | i2c_turnOffFan);
			else {
				digitalWrite(FAN_RELAY_EN, HIGH & i2c_turnOnFan | i2c_turnOffFan);
				digitalWrite(RED, LOW);
				digitalWrite(GREEN, LOW);
				digitalWrite(BLUE, HIGH);
			}
		}
	} //END Radio available

  delay(500);

  if(myRadio.available()){
    //Send data to the display

    #ifdef DEBUG
     Serial.println("*****************Sending Data to display************************");
	 #endif
     myRadio.stopListening();
     //payload = {0, payload.tempC,pipeTemperature,i2c_ambTempTh,i2c_pipeTempTh,boilerOn};
     payload = {0, payload.tempC,payload.tempC/2,i2c_ambTempTh,i2c_pipeTempTh,boilerOn};
	
     myRadio.write(&payload, sizeof(payload)); //  Transmit the data
     myRadio.startListening();
    #ifdef DEBUG
	 
	 char fanOnStr[25] = "Fan On!";
	 char fanOffStr[25] = "Fan Off!";
	 char fanAutoStr[25] = "Fan on Auto Control!";
	 char fanMsg[25];
	 if(i2c_turnOnFan == false) //remember it's inverted.
	   strcpy(fanMsg,fanOnStr);
	 if(i2c_turnOffFan == false) //remember it's inverted.
	   strcpy(fanMsg,fanOffStr);
	 else
	   strcpy(fanMsg,fanAutoStr);
	   

     Serial.println("****************************************************************");
     Serial.println("Sent: \n" + String("tempC ") + String(payload.tempC) + String("\nPipe Temp ") + 
	 				String(pipeTemperature) + String("\nAmbTh ") + String(i2c_ambTempTh) +
					  String("\nPipeTh ") + String(i2c_pipeTempTh) + String("\nfan status: ") + 
					  fanMsg );
     Serial.println("****************************************************************");
     Serial.println("****************************************************************");
	 #endif
  }
	delay(200);
}//--(end main loop )---

#ifdef RTC_CONNECTED
ElapsedTime CalculateDuration(int numElapsedSeconds) {
	struct ElapsedTime et;

	et.elapsedSeconds = numElapsedSeconds % 60;
	et.elapsedMinutes = numElapsedSeconds > 60 ? (numElapsedSeconds / 60 - et.elapsedHours * 60) : numElapsedSeconds / 60;
	et.elapsedHours = et.elapsedMinutes / 60;
	return et;
}
#endif

double approxRollingAverage (double avg, double new_sample , int N) {

    avg -= avg / N;
    avg += new_sample / N;
    return avg;
}

/* Store a running average of the last 5 results for each measurement
	tbd
*/

#ifdef I2C_RX
void receive(int numBytes) {}
#endif