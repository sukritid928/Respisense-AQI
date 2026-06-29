#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// -------------------- Customizable Settings --------------------

unsigned long sensorUpdateInterval = 1500;    // Sensor update every 1.5 sec
unsigned long screenSwitchInterval = 2000;    // OLED screen change


// RGB LED Pins (Must be PWM pins marked with ~)
const int redPin = 9;
const int greenPin = 10;
const int bluePin = 11;

// AQI Thresholds
int GOOD_LIMIT = 50;
int MODERATE_LIMIT = 100;

// Simulation ranges
int AQI_MIN = 20;
int AQI_MAX = 180;

int BPM_MIN = 65;
int BPM_MAX = 110;

float TEMP_MIN = 22.0;
float TEMP_MAX = 38.0;

float HUM_MIN = 35;
float HUM_MAX = 80;

// ---------------------------------------------------------------

unsigned long lastSensorUpdate = 0;
unsigned long lastScreenSwitch = 0;

bool screen = false;

// Simulated sensor values
int AQI = 0;
int BPM = 0;
float temperature = 0;
float humidity = 0;

String airStatus;
String asthmaPrediction;
String triggerMessage;
int asthmaRisk;

//--------------------------------------------------------

void setup() {

  Serial.begin(9600);

  pinMode(redPin, OUTPUT);
pinMode(greenPin, OUTPUT);
pinMode(bluePin, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (1);
  }

  randomSeed(analogRead(A0));

  display.clearDisplay();
  display.display();
}

//--------------------------------------------------------

void loop() {
  unsigned long currentMillis = millis();

  // Update simulated sensors
  if (currentMillis - lastSensorUpdate >= sensorUpdateInterval) {
    lastSensorUpdate = currentMillis;
    simulateSensors();
    calculateAirQuality();
    calculateAsthmaRisk();
    detectTriggers();

    if (AQI > 100) {          // High AQI -> RED
      setColor(255, 0, 0);
    } 
    else if (AQI > 50) {     // Moderate AQI -> YELLOW (Red + Green)
      setColor(0, 255, 0);
    } 
    else {                        // Low AQI -> GREEN
      setColor(0, 0, 255);
    }
  }

  // Alternate display AND LED color
  if (currentMillis - lastScreenSwitch >= screenSwitchInterval) {
    lastScreenSwitch = currentMillis;
    screen = !screen;
  }

  // Handle what displays on the screen and what color the LED shines
  if (screen) {
    displayScreen1();
    
  } else {
    displayScreen2();
    
  }
}

// Helper function to handle the RGB LED values easily
void setColor(int redValue, int greenValue, int blueValue) {
  // NOTE: If you have a Common Anode LED, change these pins to: (255 - value)
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}

//--------------------------------------------------------

void simulateSensors() {

  AQI = random(AQI_MIN, AQI_MAX);

  BPM = random(BPM_MIN, BPM_MAX);

  temperature = random(TEMP_MIN * 10, TEMP_MAX * 10) / 10.0;

  humidity = random(HUM_MIN * 10, HUM_MAX * 10) / 10.0;

}

//--------------------------------------------------------

void calculateAirQuality() {

  if (AQI <= GOOD_LIMIT)
    airStatus = "GOOD";

  else if (AQI <= MODERATE_LIMIT)
    airStatus = "MODERATE";

  else
    airStatus = "RISKY";

}

//--------------------------------------------------------

void calculateAsthmaRisk() {

  asthmaRisk = 0;

  // AQI contribution
  if (AQI <= 50)
    asthmaRisk += 10;
  else if (AQI <= 100)
    asthmaRisk += 40;
  else
    asthmaRisk += 70;

  // Temperature contribution
  if (temperature > 34 || temperature < 18)
    asthmaRisk += 15;

  // Humidity contribution
  if (humidity > 70 || humidity < 40)
    asthmaRisk += 15;

  // Heart rate contribution
  if (BPM > 100)
    asthmaRisk += 10;

  if (asthmaRisk > 100)
    asthmaRisk = 100;

  if (asthmaRisk < 30)
    asthmaPrediction = "LOW";

  else if (asthmaRisk < 60)
    asthmaPrediction = "MEDIUM";

  else
    asthmaPrediction = "HIGH";

}

//--------------------------------------------------------

void detectTriggers() {

  triggerMessage = "";

  if (AQI > 100)
    triggerMessage += "Poor Air ";

  if (temperature > 34)
    triggerMessage += "High Temp ";

  if (temperature < 18)
    triggerMessage += "Low Temp ";

  if (humidity > 70)
    triggerMessage += "High Hum ";

  if (humidity < 40)
    triggerMessage += "Low Hum ";

  if (triggerMessage == "")
    triggerMessage = "None";

}

//--------------------------------------------------------

void displayScreen1() {

  display.clearDisplay();

  display.setTextSize(1);

  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0,0);
  display.print("AQI : ");
  display.println(AQI);

  display.print("Temp: ");
  display.print(temperature);
  display.println(" C");

  display.print("Hum : ");
  display.print(humidity);
  display.println("%");

  display.print("BPM : ");
  display.println(BPM);

  display.print("Air : ");
  display.println(airStatus);

  display.display();

}

//--------------------------------------------------------

void displayScreen2() {

  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(0,0);

  display.println("ASTHMA ANALYSIS");

  display.println();

  display.print("Risk Score:");

  display.print(asthmaRisk);

  display.println("%");

  display.print("Prediction:");

  display.println(asthmaPrediction);

  display.println();

  display.print("Trigger:");

  display.println(triggerMessage);

  display.display();

}