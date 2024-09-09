#include <SPI.h>
#include <SD.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

File myFile;

// Replace with your network credentials
const char *ssid     = "@PCW@";
const char *password = "!!P@rda%1388%";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);

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
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime);
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  int currentYear = ptm->tm_year+1900;
  String currentDateTime = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay) + " " + timeClient.getFormattedTime();
  
  Serial.println(currentDateTime);

  myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile) {
    myFile.println(currentDateTime);
    myFile.close();
  } else {
    Serial.println("error opening test.txt");
  }

  delay(2000);
}
