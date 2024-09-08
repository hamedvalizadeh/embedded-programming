#include <FS.h>  //this needs to be first, or it all crashes and burns...
#include <DHT.h>
#include "HVACData.h"


// DHT22 Constants
#define DHT22PIN 12  //pin gpio 12 in sensor = D6
#define DHT22TYPE DHT22
#define DATA_GATHER_INTERVAL_IN_MILISECOND 60000

DHT dht22(DHT22PIN, DHT22TYPE);
unsigned long lastTime = 0;
unsigned long timerDelay = 0;
unsigned int dht22ReadCount = 0;


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
    return;
  }

  String tValue = String(data[0].GetData()) + " C";
  String hValue = String(data[1].GetData()) + "%";

  lastTime = millis();

  dht22ReadCount += 1;
}

void setup() {

  Serial.begin(115200);

  dht22.begin();

  delay(2000);
}

void loop() {
  mqtt_Publish_TempratureAndHumidity();
}
