#include <FS.h>  //this needs to be first, or it all crashes and burns...
#include <Wire.h>
#include <DHT.h>
#include "HVACData.h"
#include <SPI.h>
#include <SD.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include "Adafruit_SHT31.h"

File myFile;

const char *ssid     = "@PCW@";
const char *password = "!!P@rda%1388%";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// --------------------------------------- DHT22 ---------------------------------------
#define DHT22PIN 4  //pin gpio 4 in sensor = D2
#define DHT22TYPE DHT22
#define DATA_GATHER_INTERVAL_IN_MILISECOND 60000
DHT dht22(DHT22PIN, DHT22TYPE);
unsigned long lastTime = 0;
unsigned long timerDelay = 0;

bool get_dht22_data(HVACData data[2]) {
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


void write_to_file(String content) {
  myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile) {
    myFile.println(content);
    myFile.close();
  }
}

void setup() {
  Serial.begin(115200);
  
  dht22.begin();

  if (!SD.begin(15)) {
    Serial.println("initialization failed!");
    return;
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  timeClient.begin();
  timeClient.setTimeOffset(0);
}

void loop() {

  if ((timerDelay != 0) && (millis() - lastTime) <= timerDelay) {
    return;
  }

  timerDelay = DATA_GATHER_INTERVAL_IN_MILISECOND;

  HVACData data[2];
  bool readCorrectValue = get_dht22_data(data);

  if (readCorrectValue != true) {
    return;
  }

  String tValue = String(data[0].GetData());
  String hValue = String(data[1].GetData());

  lastTime = millis();

  //get_data_sht3x();

  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime);
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  int currentYear = ptm->tm_year+1900;
  String currentDateTime = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay) + " " + timeClient.getFormattedTime() + "," + tValue + "," + hValue;
  
  Serial.println(currentDateTime);
  write_to_file(currentDateTime);
}
