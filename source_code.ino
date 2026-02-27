#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "RTClib.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
RTC_DS1307 rtc;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  // We run a quick check at startup to make sure the hardware is actually talking to us
  Serial.println(F("Starting I2C Scanner..."));
  byte error, address;
  int nDevices = 0;

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print(F("I2C device found at address 0x"));
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      nDevices++;
    }
  }
  Serial.println(F("Scan complete. Initializing modules..."));

  // Start up the screen. If it's not found, the program just stops here.
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    for(;;); 
  }

  // Start the clock module
  if (!rtc.begin()) {
    while (1);
  }

  // If the battery was removed or it's the first time powered on, 
  // we sync the clock to the exact time you clicked 'Upload' on your PC.
  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  display.clearDisplay();
  Serial.println(F("Simultaneous Data Processing: ACTIVE"));
}

void loop() {
  // Pull the latest time data from the RTC chip
  DateTime now = rtc.now();

  // We format the raw numbers into a nice "00:00:00" string for the console and screen
  char timeStr[20];
  sprintf(timeStr, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());

  // Show the processed data in the Serial Monitor
  Serial.print(F("Current Time: "));
  Serial.println(timeStr);

  // Update the OLED display
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Tiny text for the date at the top
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(now.year()); display.print('/');
  display.print(now.month()); display.print('/');
  display.print(now.day());

  // Bigger text for the actual clock so it's readable
  display.setTextSize(2);
  display.setCursor(15, 30);
  display.print(timeStr);

  // Push the virtual "drawing" to the physical screen
  display.display();

  // Wait 1 second before doing it all over again
  delay(1000);
}