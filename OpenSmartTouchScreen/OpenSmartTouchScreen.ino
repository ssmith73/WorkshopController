/*
  This is the touch-controller. The task for this are.
   - Set the 2 thresholds, ambient and pipeTemperature
   - Turn on the fan only.
   - Turn on LED lighting?
   Capture touch information for add-subract values.
   Pass values via i2c interface, direct to controller
   arduino.
   
*/

#include <Adafruit_GFX.h>    // Core graphics library
#include <MCUFRIEND_kbv.h>
#include <avr/wdt.h>
MCUFRIEND_kbv tft;

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

int   ambTemp = 0;
int   pipeTemp = 0;
bool  fanOn = false;


void setup() {
  // put your setup code here, to run once:
  // And Yet Another tester for me.
  Serial.begin(9600);

  Serial.print("TFT size is ");
  Serial.print(tft.width());
  Serial.print("x");
  Serial.println(tft.height());

  uint16_t ID = tft.readID(); //
  Serial.print("ID = 0x");
  Serial.println(ID, HEX);
  if (ID == 0xD3D3)
      ID = 0x9481; // write-only shield
  tft.begin(ID);
  tft.invertDisplay(true);
  //tft.setFont(&FreeSans24pt7b);

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
  tft.setRotation(1);
}

void loop() { // put your main code here, to run repeatedly:

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
  tft.println("7'C");
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
  tft.println("21'C");
  tft.setTextColor(VGA_BLUE, VGA_BLACK);
  tft.setCursor(180, 200);
  tft.println(" Set pipe");
  tft.setCursor(182, 220);
  tft.println("Threshold");
  wdt_reset();

  tft.fillRoundRect(295,20,100,60,8,VGA_RED);
  tft.setCursor(305, 32);
  tft.setTextColor(YELLOW, VGA_RED);
  tft.println("FAN ON");

  tft.setTextSize(3);
  tft.fillRoundRect(295,120,100,60,8,VGA_AQUA);
  tft.setCursor(315, 138);
  tft.setTextColor(VGA_BLACK, VGA_AQUA);
  tft.println("SET");

  
  delay(2000);
}
