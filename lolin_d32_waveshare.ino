// mapping for ESP32 LOLIN D32
// BUSY -> 4
// RST -> 16
// DC -> 17
// CS -> SS(5)
// CLK -> SCK(18)
// DIN -> MOSI(23)
// GND -> GND, 3.3V -> 3.3V

#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>

GxEPD2_BW<GxEPD2_154, GxEPD2_154::HEIGHT> display(GxEPD2_154(/*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4));


void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
  delay(100);
  display.init(115200);
}

void loop() {
  helloWorld();
  delay(1000);

  delay(4000);
}

const char HelloWorld[] = "random n ";

void helloWorld() {
  display.setPartialWindow(0, 0, display.width(), display.height());
  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  
  int16_t tbx, tby;
  uint16_t tbw, tbh;

  display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);

  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;

  String randomString = String(random(1, 60));

  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(72, 18);
    display.print(getVoltageAndPercentage());
    display.setCursor(x, y);
    display.print(HelloWorld);
    display.setCursor(x + tbw + 10, y);
    display.print(randomString);
  } while (display.nextPage());

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
