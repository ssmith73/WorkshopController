/*
  This is the touch-controller. The task for this are.
   - Set the 2 thresholds, ambient and pipeTemperature
   - Turn on the fan only.
   - Turn on LED lighting?
   Capture touch information for add-subract values.
   Pass values via i2c interface, direct to controller
   arduino.
   
   
*/
//#define DEBUG

#include <Adafruit_GFX.h>    // Core graphics library
#include <MCUFRIEND_kbv.h>
#include <avr/wdt.h>
#include <TouchScreen.h>
#include <Wire.h>
#include <EasyTransferI2C.h>

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
// Touch screen stuff

#define MINPRESSURE 20
#define MAXPRESSURE 1000
MCUFRIEND_kbv tft;

//For I2C data packet transfer to 2nd controller
EasyTransferI2C ET; 
#define I2C_SLAVE_ADDRESS 9

struct SEND_DATA_STRUCTURE {
  // put your variable definitions here for the 
  // data you want to send
  // Put exact equivilent stuct in Rx
  int ambTempTh;
  int pipeTempTh;
  bool turnOnFan;
};

SEND_DATA_STRUCTURE i2cData;

void SendPacket(int amb, int pipe, bool fanOn, bool fanOff) {
  // Send Fan-control signal
  // Send ambient and pipe threshold values

  #ifdef DEBUG
  Serial.println("Sending I2C data packet" +
    String(i2cData.ambTempTh) + ":" +
    String(i2cData.pipeTempTh) + ":" + 
    String(i2cData.turnOnFan));
  #endif

    i2cData.ambTempTh = amb;
    i2cData.pipeTempTh = pipe;
    i2cData.turnOnFan = fanOn;
    i2cData.turnOnFan = fanOff;

    tft.setTextSize(3);
    tft.fillRoundRect(295,120,100,60,8,GREEN);
    tft.setCursor(310, 138);
    tft.setTextColor(VGA_BLACK, GREEN);
    tft.println("SENT");

    //Just to be sure send the data 5 times
     for(int i=0;i<5;i++)
       ET.sendData(I2C_SLAVE_ADDRESS);
     delay(3000);

    tft.setTextSize(3);
    tft.fillRoundRect(295,120,100,60,8,VGA_AQUA);
    tft.setCursor(315, 138);
    tft.setTextColor(VGA_BLACK, VGA_AQUA);
    tft.println("SET");
}

typedef struct TouchRegions {
  int x1;
  int x2;
  int y1;
  int y2;
} touchRegion;


uint8_t YP = A1;  // must be an analog pin, use "An" notation!
uint8_t XM = A2;  // must be an analog pin, use "An" notation!
uint8_t YM = 7;   // can be a digital pin
uint8_t XP = 6;   // can be a digital pin
uint8_t SwapXY = 0;

uint16_t TS_LEFT = 880;
uint16_t TS_RT  = 170;
uint16_t TS_TOP = 950;
uint16_t TS_BOT = 180;
int16_t PENRADIUS = 3;

char *name = "Unknown controller";
uint16_t identifier, oldcolor, currentcolor;
#define SWAP(a, b) {uint16_t tmp = a; a = b; b = tmp;}

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 260 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 260);
//TSPoint tp;


void show_Serial(void)
{
    Serial.print(F("Found "));
    Serial.print(name);
    Serial.println(F(" LCD driver"));
    Serial.print(F("ID=0x"));
    Serial.println(identifier, HEX);
    Serial.println("Screen is " + String(tft.width()) + "x" + String(tft.height()));
    Serial.println("Calibration is: ");
    Serial.println("LEFT = " + String(TS_LEFT) + " RT  = " + String(TS_RT));
    Serial.println("TOP  = " + String(TS_TOP)  + " BOT = " + String(TS_BOT));
    Serial.print("Wiring is: ");
    Serial.println(SwapXY ? "SWAPXY" : "PORTRAIT");
    Serial.println("YP=" + String(YP)  + " XM=" + String(XM));
    Serial.println("YM=" + String(YM)  + " XP=" + String(XP));

    int Orientation=2;
    int tmp;
    switch (Orientation) {      // adjust for different aspects
        case 0:   break;        //no change,  calibrated for PORTRAIT
        case 1:   tmp = TS_LEFT, TS_LEFT = TS_BOT, TS_BOT = TS_RT, TS_RT = TS_TOP, TS_TOP = tmp;  break;
        case 2:   SWAP(TS_LEFT, TS_RT);  SWAP(TS_TOP, TS_BOT); break;
        case 3:   tmp = TS_LEFT, TS_LEFT = TS_TOP, TS_TOP = TS_RT, TS_RT = TS_BOT, TS_BOT = tmp;  break;
    }
}


int   ambTemp = 0;
int   ambTempTh = 7;
int   pipeTempTh = 21;
int   pipeTemp = 0;
bool  fanOn = false;
bool  fanOff = false;
bool  setPressed = false;

/*
   Setup touch regions for the 6 elements
   + Ambient, pipeTemperature
   - Ambient, pipeTemperature
   FAN ON, SET buttons
   2 hidden areas which allow for hot/cold 
   immediate change to ambient values
*/
  touchRegion plusAmb;
  touchRegion plusPipe;
  touchRegion minusAmb;
  touchRegion minusPipe;
  touchRegion fanOnButton;
  touchRegion setButton;
  touchRegion setHigh;
  touchRegion setLow;

void setup() {
  //i2c transfer setup
  Wire.begin();
  ET.begin(details(i2cData),&Wire);


  plusAmb.x1 = 270;   plusAmb.x2 =410; 
  plusAmb.y1 = 895;   plusAmb.y2 =700; 
  minusAmb.x1 = 270;  minusAmb.x2 =410; 
  minusAmb.y1 = 600;  minusAmb.y2 =475; 
  
  plusPipe.x1 = 580;  plusPipe.x2 =710; 
  plusPipe.y1 = 895;  plusPipe.y2 =700; 
  minusPipe.x1 = 570; minusPipe.x2 =700; 
  minusPipe.y1 = 600; minusPipe.y2 =475; 

  fanOnButton.x1 =  779;  fanOnButton.x2 = 966;
  fanOnButton.y1 =  860;  fanOnButton.y2 = 670;

  setButton.x1 =  779;  setButton.x2 = 966;
  setButton.y1 =  570;  setButton.y2 = 380;

  setHigh.x1 =  220;  setHigh.x2 = 450;
  setHigh.y1 =  350;  setHigh.y2 = 230;

  setLow.x1 =  540;  setLow.x2 = 760;
  setLow.y1 =  350;  setLow.y2 = 230;

  Serial.begin(115200);
  Serial.print("TFT size is ");
  Serial.print(tft.width());
  Serial.print("x");
  Serial.println(tft.height());

  uint16_t ID = tft.readID(); //
  Serial.print("ID = 0x");
  Serial.println(ID, HEX);
  ts = TouchScreen(XP, YP, XM, YM, 260);     //call the constructor AGAIN with new values.
  if (ID == 0xD3D3)
      ID = 0x9481; // write-only shield
  show_Serial();

  tft.begin(ID);
  tft.invertDisplay(true);
  tft.fillScreen(BLACK);
  tft.setRotation(3);
}

void loop() { // put your main code here, to run repeatedly:

// Configure display
    tft.setTextSize(2);
    tft.setTextColor(VGA_YELLOW, VGA_BLACK);

  // Ambient threshold
    tft.setCursor(50, 0);
    tft.setTextSize(12);
    tft.print("+");
    tft.setCursor(50, 80);
    tft.print("-");

    tft.setTextSize(2);
    tft.setCursor(60, 180);
    tft.setTextColor(WHITE, VGA_BLACK);
    tft.println(ambTempTh + String("'C"));
    tft.setCursor(20, 200);
    tft.setTextColor(GREEN, VGA_BLACK);
    tft.println(" Set Amb");
    tft.setCursor(22, 220);
    tft.println("Threshold");

  // pipe threshold
    tft.setCursor(200, 0);
    tft.setTextSize(12);
    tft.setTextColor(VGA_YELLOW, VGA_BLACK);
    tft.print("+");
    tft.setCursor(200, 80);
    tft.print("-");

    tft.setTextSize(2);
    tft.setCursor(210, 180);
    tft.setTextColor(WHITE, VGA_BLACK);
    tft.println(pipeTempTh + String("'C"));
    tft.setTextColor(VGA_BLUE, VGA_BLACK);
    tft.setCursor(180, 200);
    tft.println(" Set Pipe");
    tft.setCursor(182, 220);
    tft.println("Threshold");
    wdt_reset();

    if(fanOn == true) {
      tft.fillRoundRect(295,20,100,60,8,GREEN);
      tft.setTextColor(RED, GREEN);
      tft.setCursor(325, 32);
      tft.println("FAN");
      tft.setCursor(330, 55);
      tft.println("ON");
    }
    else if(fanOff == true) {
      tft.fillRoundRect(295,20,100,60,8,RED);
      tft.setTextColor(BLACK, RED);
      tft.setCursor(325, 32);
      tft.println("FAN");
      tft.setCursor(330, 55);
      tft.println("OFF");
    }
    else {
      tft.fillRoundRect(295,20,100,60,8,VGA_AQUA);
      tft.setTextColor(BLACK, VGA_AQUA);
      tft.setCursor(325, 32);
      tft.println("FAN");
      tft.setCursor(320, 55);
      tft.println("AUTO");
    }

    tft.setTextSize(3);
    tft.fillRoundRect(295,120,100,60,8,VGA_AQUA);
    tft.setCursor(315, 138);
    tft.setTextColor(VGA_BLACK, VGA_AQUA);
    tft.println("SET");

    // Configure touch-controller
    TSPoint tp;
    uint16_t xpos, ypos;  //screen coordinates
    do {
      tp = ts.getPoint();
      #ifdef DEBUG
      Serial.println("Waiting to capture point");
      #endif
    }while(tp.z == 0);

    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    pinMode(XP, OUTPUT);
    pinMode(YM, OUTPUT);


    #ifdef DEBUG
    Serial.println("tp.z = " + String(tp.z));
    #endif

    if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) {
        // is controller wired for Landscape ? or are we oriented in Landscape?
        //if (SwapXY != (Orientation & 1)) SWAP(tp.x, tp.y);
        SWAP(tp.x, tp.y);
        // scale from 0->1023 to tft.width  i.e. left = 0, rt = width
        // most mcufriend have touch (with icons) that extends below the TFT
        // screens without icons need to reserve a space for "erase"
        // scale the ADC values from ts.getPoint() to screen values e.g. 0-239

        #ifdef DEBUG
        xpos = map(tp.x, TS_LEFT, TS_RT, tft.width(), 0);
        ypos = map(tp.y, TS_TOP, TS_BOT, tft.height(), 0);
        tft.setCursor(0,150);
        tft.setTextSize(1);
        Serial.print("tp.x=" + String(tp.x) + " tp.y=" + String(tp.y) + "   ");
        tft.setCursor(320,190);
        tft.println("tp.x=" + String(tp.x) +  "   ");
        tft.setCursor(320,200);
        tft.println("tp.y=" + String(tp.y) + "   ");
        #endif


        if( (tp.x > plusAmb.x1) && tp.x < plusAmb.x2) { //Amb + OR -ve touched
          if(tp.y <= plusAmb.y1  && tp.y >= plusAmb.y2) //Was it the + ?
             ambTempTh++;
          if(tp.y <= minusAmb.y1  && tp.y >= minusAmb.y2) {//Was it the - ?
             if(ambTempTh > 9) //redraw if losing a digit
                tft.fillRect(30,177,100,20,BLACK);
             ambTempTh--;
          }
        }
        if( (tp.x > plusPipe.x1) && tp.x < plusPipe.x2) { //PipeTemp + OR -ve touched
          if(tp.y <= plusPipe.y1  && tp.y >= plusPipe.y2) //Was it the + ?
             pipeTempTh++;
          if(tp.y <= minusPipe.y1  && tp.y >= minusPipe.y2) { //Was it the - ?
             pipeTempTh--;
          }
        }

        /*3 way button
          Normal operation is fanAuto - 
          Press button in Auto Mode - Fan turns on
          Press button in Fan On mode - Fan turns OFF
          Press buttono in FanOn mode - Fan returns to auto mode
        */
        if( (tp.x > fanOnButton.x1) && tp.x < fanOnButton.x2) { //In the button regions?
          if(tp.y <= fanOnButton.y1  && tp.y >= fanOnButton.y2) { //Was it the FAN ON button  ?
            if(fanOff == false && fanOn == false) {
               fanOn = true;
               fanOff = false;
            }
            else if(fanOn){
              fanOn = false;
              fanOff = true;
            }
            else if(fanOff) {
              fanOn = false;
              fanOff = false;
            }
            else {
              fanOn = false;
              fanOff = false;
            }
             SendPacket(ambTempTh,pipeTempTh,fanOn, fanOff);
           }
          if(tp.y <= setButton.y1  && tp.y >= setButton.y2) {//Was it the set button ?
             SendPacket(ambTempTh,pipeTempTh,fanOn,fanOff);
          }
        }

     //hidden high/low ambient temperature set buttons
        if( (tp.x > setHigh.x1) && tp.x < setHigh.x2)   //In the high/low regions?
          if(tp.y <= setHigh.y1  && tp.y >= setHigh.y2)   //Was it the set high temp button  ?
             ambTempTh = 15;
        if( (tp.x > setLow.x1) && tp.x < setLow.x2)   //In the button regions?
          if(tp.y <= setLow.y1  && tp.y >= setLow.y2) {    //Was it the set low temp button  ?
             if(ambTempTh > 9)
                tft.fillRect(30,177,100,20,BLACK);
             ambTempTh = 7;
    //tft.fillRoundRect(295,120,100,60,8,VGA_AQUA);

          }
        delay(100);
    }
}
