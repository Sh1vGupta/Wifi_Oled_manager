#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// select which pin will trigger the configuration portal when set to LOW
#define Setup_btn 4

int timeout = 120; // seconds to run for

// Instructions array
const char* instructions[] = {
  "1) Open Wifi Settings",
  "2) Connect to Wifi-Manager",
  "3) Go to the provided webadress",
  "4) Enter WiFi Credentials"
};
const int numInstructions = sizeof(instructions) / sizeof(instructions[0]);
int currentInstructionIndex = 0;

// Web address
const char* webAddress = "192.168.4.1";

void setup() {
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);                          // Don't proceed, loop forever
  }
  delay(2000);                       // Pause for 2 seconds
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);// Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  centerText_singleline("Hello!!",2);
  display.display();
  delay(2000);
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP  
  // put your setup code here, to run once:
  Serial.begin(115200);
  centerText_singleline("Starting...",1);
  pinMode(Setup_btn, INPUT_PULLUP);
}
void loadcell();



  // put your main code here, to run repeatedly:
}
void start_setup() {
  if (digitalRead(Setup_btn) == LOW) {
      WiFiManager wm;
      wm.setConfigPortalTimeout(timeout);
  // Calculate the remaining time
  int remainingTime = timeout;
  // Clear the display buffer
  display.clearDisplay();
  // Set text size and color
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  // Print the current instruction
  display.setCursor(0, 0);
  display.println(instructions[currentInstructionIndex]);
  // Print the web address
  display.setCursor(0, SCREEN_HEIGHT - 8); // Adjust y-position for text size 1
  display.print(webAddress);
  // Calculate the position for the countdown timer
  display.setCursor(SCREEN_WIDTH - 30, SCREEN_HEIGHT - 8); // Adjust y-position for text size 1
  // Print the countdown timer
  display.print(remainingTime);
  display.print(" s");
  // Update the display buffer
  display.display();
  // Delay for one second
  delay(1000);
  // Decrement the countdown time
  timeout--;
  // Change the instruction every 2 seconds
  if (timeout % 2 == 0) {
    currentInstructionIndex = (currentInstructionIndex + 1) % numInstructions;
  }
  if(timeout == 0){
    centerText_singleline("System timed Out.. Restarting",1);
    loadcell();
  }
  if (!wm.startConfigPortal("WiFi-Manager") || timeout == 0) {
      centerText_singleline("failed to connect and hit timeout",1);
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(5000);
    }
    display.println(F("Connected succesfully to"));
    centerText_singleline(wm.getWiFiSSID(),1);
    loadcell();
  }
}
void centerText_singleline(String text, int textSize) {
  display.setTextSize(textSize);
  display.setTextColor(SSD1306_WHITE);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  int x = (SCREEN_WIDTH - w) / 2;
  int y = (SCREEN_HEIGHT - h) / 2;
  display.setCursor(x, y+5);
  display.println(text);
}
void printWrapped(String text, int textSize) {
  display.setTextSize(textSize);
  display.setTextColor(SSD1306_WHITE);
  int16_t x1, y1;
  uint16_t w, h;
  int lineHeight = 8 * textSize; // Approximate height of text
  int cursorX = 0;
  int cursorY = 0;
  String line = "";
  for (int i = 0; i < text.length(); i++) {
    char c = text.charAt(i);
    if (c == ' ' || c == '\n' || i == text.length() - 1) {
      if (i == text.length() - 1) line += c; // Add last character

      display.getTextBounds(line, 0, 0, &x1, &y1, &w, &h);
      if (cursorX + w > SCREEN_WIDTH) {
        cursorX = 0;
        cursorY += lineHeight;
        if (cursorY + lineHeight > SCREEN_HEIGHT) {
          break; // Stop printing if we exceed display height
        }
      }

      display.setCursor(cursorX, cursorY);
      display.print(line);
      line = "";
      cursorX += w;

      if (c == '\n') {
        cursorX = 0;
        cursorY += lineHeight;
      }
    } else {
      line += c;
    }
  }
}
void loop() {
  // is configuration portal requested?
  start_setup();
    //reset settings - for testing
    //wm.resetSettings();
    // set configportal timeout
    
    //if you get here you have connected to the WiFi

  }