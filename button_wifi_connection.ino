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

#define Setup_btn 4 // Button to trigger WiFi setup mode

int timeout = 120; // seconds to run for

// Instructions array
const char* instructions[] = {
  "1) Open WiFi Settings",
  "2) Connect to WiFi-Manager",
  "3) Go to the web address",
  "4) Enter WiFi Credentials"
};
const int numInstructions = sizeof(instructions) / sizeof(instructions[0]);
int currentInstructionIndex = 0;

// Web address
const char* webAddress = "192.168.4.1";

void setup() {
  Serial.begin(115200); // Set the baud rate to 115200
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);  // Don't proceed, loop forever
  }
  
  // Initial display setup
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  centerText_singleline("Hello!!", 2);
  display.display();
  delay(2000);

  // Set up WiFi mode
  WiFi.mode(WIFI_STA); // Set WiFi mode to station only
  pinMode(Setup_btn, INPUT_PULLUP); // Set the setup button as input with pull-up
}

void loop() {
  start_setup(); // Continuously check if the setup button is pressed
}

void start_setup() {
  if (digitalRead(Setup_btn) == LOW) { // Check if the setup button is pressed
    WiFiManager wm;
    wm.setConfigPortalTimeout(timeout);

    while (timeout > 0) {
      // Clear the display buffer
      display.clearDisplay();

      // Print the current instruction
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println(instructions[currentInstructionIndex]);

      // Print the web address
      display.setCursor(0, SCREEN_HEIGHT - 8); // Adjust y-position for text size 1
      display.print(webAddress);

      // Print the countdown timer
      display.setCursor(SCREEN_WIDTH - 30, SCREEN_HEIGHT - 8);
      display.print(timeout);
      display.print(" s");

      // Update the display buffer
      display.display();

      delay(1000); // Delay for one second

      // Decrement the countdown time
      timeout--;

      // Change the instruction every 2 seconds
      if (timeout % 2 == 0) {
        currentInstructionIndex = (currentInstructionIndex + 1) % numInstructions;
      }

      // Attempt to start the configuration portal
      if (wm.startConfigPortal("WiFi-Manager")) {
        // Successfully connected to WiFi
        display.clearDisplay();
        display.setTextSize(1);
        centerText_singleline("Connected successfully to:", 1);
        display.setCursor(0, 16);
        display.print(wm.getWiFiSSID());
        display.display();
        delay(3000);
        return; // Exit the function after successful connection
      }

      // If timeout reaches 0, restart the ESP
      if (timeout == 0) {
        centerText_singleline("System timed out. Restarting...", 1);
        display.display();
        delay(3000);
        ESP.restart(); // Restart the ESP32
      }
    }
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
  display.setCursor(x, y + 5);
  display.println(text);
  display.display();
}
