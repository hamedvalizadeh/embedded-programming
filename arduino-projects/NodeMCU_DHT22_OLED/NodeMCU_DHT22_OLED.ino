#include <FS.h>  //this needs to be first, or it all crashes and burns...
#include <Wire.h>
#include <DHT.h>
#include "HVACData.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Constants
#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 64     // OLED display height, in pixels
#define OLED_RESET -1        // Reset pin
#define SCREEN_ADDRESS 0x3C  // Declaration for SSD1306 display connected using I2C

// DHT22 Constants
#define DHT22PIN 12  //pin gpio 12 in sensor = D6
#define DHT22TYPE DHT22
#define DATA_GATHER_INTERVAL_IN_MILISECOND 60000

// OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// DHT22
DHT dht22(DHT22PIN, DHT22TYPE);
unsigned long lastTime = 0;
unsigned long timerDelay = 0;
unsigned int dht22ReadCount = 0;

void setup_oled() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
}

void print_on_oled(int16_t x, int16_t y, const char c[]) {
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(x, y);
  display.println(c);
  display.display();
}

void print_on_oled_values(const char t[], const char h[], const char c[]) {
  print_on_oled(70, 20, "    ");
  print_on_oled(70, 30, "   ");
  print_on_oled(70, 40, "   ");
  print_on_oled(70, 20, t);
  print_on_oled(70, 30, h);
  print_on_oled(70, 40, c);
}

bool getSensorsData(HVACData data[2]) {
  delay(1000);

  bool readCorrectValue = false;

  float fH22 = 0;
  float fT22 = 0;

  int h22 = dht22.readHumidity() * 10;
  int t22 = dht22.readTemperature() * 10;
  fH22 = float(h22) / 10;
  fT22 = float(t22) / 10;

  String tValue = String(fT22) + " C";
  String hValue = String(fH22) + "%";

  if ((fT22 <= 80 && fT22 >= -40) && (fH22 <= 100 && fH22 >= 0)) {
    readCorrectValue = true;
  }

  data[0].SetData(fT22, "DHT22", "T");
  data[1].SetData(fH22, "DHT22", "H");

  return readCorrectValue;
}

void mqtt_Publish_TempratureAndHumidity() {

  if ((timerDelay != 0) && (millis() - lastTime) <= timerDelay) {
    return;
  }

  timerDelay = DATA_GATHER_INTERVAL_IN_MILISECOND;

  HVACData data[2];
  bool readCorrectValue = getSensorsData(data);

  if (readCorrectValue != true) {
    print_on_oled_values("NCV ", "NCV", "NCV");
    return;
  }

  String tValue = String(data[0].GetData()) + " C";
  String hValue = String(data[1].GetData()) + "%";

  lastTime = millis();

  dht22ReadCount += 1;
  print_on_oled_values(tValue.c_str(), hValue.c_str(), String(dht22ReadCount).c_str());
}

void setup() {

  Serial.begin(115200);

  setup_oled();

  display.clearDisplay();
  print_on_oled(50, 0, "Hello");
  print_on_oled(0, 20, "Temprature: ");
  print_on_oled(0, 30, "Humidity:   ");
  print_on_oled(0, 40, "Count:      ");

  dht22.begin();

  delay(2000);
}

void loop() {
  mqtt_Publish_TempratureAndHumidity();
}
