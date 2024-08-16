#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Define the screen width and height
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// Define the I2C address for the display
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C

// Create an instance of the SSD1306 display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define the countdown time in seconds
int countdownTime = 120;

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
const char* webAddress = "http://setup.local";

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize the display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  // Clear the display buffer
  display.clearDisplay();
  display.display();
}

void loop() {
  // Clear the display buffer
  display.clearDisplay();

  // Set text size and color
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  if (countdownTime > 0) {
    // Print the current instruction
    display.setCursor(0, 0);
    display.println(instructions[currentInstructionIndex]);

    // Print the web address
    display.setCursor(0, SCREEN_HEIGHT - 8); // Adjust y-position for text size 1
    display.print(webAddress);

    // Calculate the position for the countdown timer
    display.setCursor(SCREEN_WIDTH - 30, SCREEN_HEIGHT - 8); // Adjust y-position for text size 1

    // Print the countdown timer
    display.print(countdownTime);
    display.print(" s");

    // Decrement the countdown time
    countdownTime--;

    // Change the instruction every 2 seconds
    if (countdownTime % 2 == 0) {
      currentInstructionIndex = (currentInstructionIndex + 1) % numInstructions;
    }
  } else {
    // Display "Failed to connect"
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("Failed to connect");
  }

  // Update the display buffer
  display.display();

  // Delay for one second
  delay(1000);
}
