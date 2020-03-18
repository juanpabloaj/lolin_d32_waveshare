// mapping for ESP32 LOLIN D32
// BUSY -> 4
// RST -> 16
// DC -> 17
// CS -> SS(5)
// CLK -> SCK(18)
// DIN -> MOSI(23)
// GND -> GND, 3.3V -> 3.3V

#include <WiFi.h>
#include "time.h"

#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>

#include "configuration.h"

GxEPD2_BW<GxEPD2_154, GxEPD2_154::HEIGHT> display(GxEPD2_154(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4));

RTC_DATA_ATTR int wakeUpCounter = 0;

void setup() {
  unsigned long startTime = millis();

  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
  delay(100);
  display.init(115200, false);

  if (wakeUpCounter ==  0) {
    initAndGetTime();
  }

  helloWorld();
  delay(1000);

  long sleepTimer = getSleepTimer();
  messageBeforeSleep(startTime, sleepTimer);
  display.hibernate();
  beginSleep(sleepTimer);
}

void loop() {}

void helloWorld() {
  display.setPartialWindow(0, 0, display.width(), display.height());
  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);

  time_t now;
  struct tm timeinfo;

  time(&now);
  localtime_r(&now, &timeinfo);
  timeinfo.tm_sec += GmtOffsetSec + DaylightOffsetSec;
  mktime(&timeinfo);

  display.firstPage();
  do {
    display.fillRect(0, 0, display.width(), display.height()/2, GxEPD_WHITE);
    display.setCursor(80, 18);
    display.print(getVoltageAndPercentage());
    display.setCursor(0, 50);
    display.println("wake up count " + String(wakeUpCounter++));

    display.println(&timeinfo, "%a %d %b %H:%M");
  } while (display.nextPage());

}

void messageBeforeSleep(unsigned long startTime, long sleepTimer) {
  String awake = "awake for " + String((millis() - startTime) / 1000., 3) + " secs";
  String entering = awake + ", deep-sleep for " + String(sleepTimer) + " secs";

  display.setPartialWindow(0, display.height() / 2, display.width(), display.height());

  display.firstPage();
  do {
    display.fillRect(0, display.height() / 2, display.width(), display.height(), GxEPD_WHITE);
    display.setCursor(0, 140);
    display.println(entering);
  } while (display.nextPage());
}

long getSleepTimer() {
  long sleepDurationMinutes = 1;
  int currentHour = 0, currentMin = 0, currentSec = 0;
  return (sleepDurationMinutes * 60 - ((currentMin % sleepDurationMinutes) * 60 + currentSec)) + 5;
}

void beginSleep(long sleepTimer) {
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);

  esp_sleep_enable_timer_wakeup(sleepTimer * 1000000LL);

  esp_deep_sleep_start();
}

String getVoltageAndPercentage() {
  float voltage = analogRead(35) / 4096.0 * 7.23;
  uint8_t percentage = percentageFromVoltage(voltage);

  return String(voltage)+"V " + String(percentage)+"%";
}

uint8_t percentageFromVoltage(float voltage) {
  // LOLIN D32 (no voltage divider need already fitted to board.
  // or NODEMCU ESP32 with 100K+100K voltage divider
  uint8_t percentage;
  percentage = 2808.3808 * pow(voltage, 4) - 43560.9157 * pow(voltage, 3) + 252848.5888 * pow(voltage, 2) - 650767.4615 * voltage + 626532.5703;
  if (voltage > 4.19) percentage = 100;
  else if (voltage <= 3.50) percentage = 0;

  return percentage;
}

void initAndGetTime() {

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  struct tm timeinfo;

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  if (!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
  }
  Serial.println(&timeinfo, "%a %d %b %H:%M");

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}
