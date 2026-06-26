#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

const float BODY_WEIGHT_KG = 80.0;
const bool IS_MALE = true;
const float METABOLISM_PER_HOUR = 0.015;

const float R = IS_MALE ? 0.73 : 0.66;

const float DRINK_GRAMS_ALC = 14.0; 
const int BTN_ADD = 2;
const int BTN_SHOW = 3;
const int BTN_RESET = 4;

DateTime drinkTimes[20];
int drinkCount = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);

  Wire.setSDA(16);
  Wire.setSCL(17);
  Wire.begin();

  if (!rtc.begin()) {
    Serial.println("RTC not found!");
    while (1);
  }

  pinMode(BTN_ADD, INPUT_PULLUP);
  pinMode(BTN_SHOW, INPUT_PULLUP);
  pinMode(BTN_RESET, INPUT_PULLUP);

  Serial.println("BAC Calculator Ready");
}

float computeBAC() {
  if (drinkCount == 0) return 0.0;

  DateTime now = rtc.now();
  float totalBAC = 0.0;

  for (int i = 0; i < drinkCount; i++) {
    float hoursPassed = (now - drinkTimes[i]).totalseconds() / 3600.0;

    float bac = (DRINK_GRAMS_ALC / (BODY_WEIGHT_KG * R)) * 100.0;
    bac -= METABOLISM_PER_HOUR * hoursPassed;

    if (bac > 0) totalBAC += bac;
  }

  return totalBAC;
}

void loop() {
  if (!digitalRead(BTN_ADD)) {
    if (drinkCount < 20) {
      drinkTimes[drinkCount++] = rtc.now();
      Serial.print("Drink added. Total drinks: ");
      Serial.println(drinkCount);
      delay(300);
    }
  }

  if (!digitalRead(BTN_SHOW)) {
    float bac = computeBAC();
    Serial.print("Current BAC: ");
    Serial.println(bac, 4);
    delay(300);
  }

  if (!digitalRead(BTN_RESET)) {
    drinkCount = 0;
    Serial.println("Session reset.");
    delay(300);
  }
}
