#define BLYNK_PRINT Serial

#define BLYNK_AUTH_TOKEN "Yrc5eOcgVXVfnCslnlYYRUv_FeWltrSN"
#define BLYNK_TEMPLATE_ID "TMPL6CZ_fHxP3"
#define BLYNK_TEMPLATE_NAME "SHRC AC"

// ===== LIBRARY =====
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <DHT.h>

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Samsung.h>   // jenis ac 

// ===== WIFI =====
char ssid[] = ""; //Nama wifi
char pass[] = ""; //pasword wifi

// ===== PIN =====
#define IR_PIN 4
#define DHTPIN 15
#define DHTTYPE DHT22

#define SDA_PIN 21
#define SCL_PIN 22

// ===== OBJECT =====
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

IRSamsungAc ac(IR_PIN);

// ===== VAR =====
float roomTemp = 0;
int setPoint = 24;
bool acPower = false;

// ===== BLYNK PIN =====
#define VPIN_POWER   V0
#define VPIN_SETPOINT V1
#define VPIN_TEMP    V2

// ===== BLYNK CALLBACK =====
BLYNK_WRITE(VPIN_POWER) {
  acPower = param.asInt();
}

BLYNK_WRITE(VPIN_SETPOINT) {
  setPoint = param.asInt();
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);

  // LCD
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Connecting...");

  // DHT
  dht.begin();

  // IR
  ac.begin();

  // Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

// ===== LOOP =====
void loop() {
  Blynk.run();
  controlAC();
}

// ===== AC CONTROL =====
void controlAC() {
  static unsigned long lastRead = 0;
  if (millis() - lastRead < 2000) return;
  lastRead = millis();

  roomTemp = dht.readTemperature();
  if (isnan(roomTemp)) return;

  Blynk.virtualWrite(VPIN_TEMP, roomTemp);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Room: ");
  lcd.print(roomTemp, 1);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Set : ");
  lcd.print(setPoint);
  lcd.print(" C ");

  if (!acPower) {
    lcd.print("OFF");
    return;
  }

  if (roomTemp > setPoint + 1) {
    lcd.print("ON ");
    ac.on();
    ac.setMode(kSamsungAcCool);
    ac.setTemp(setPoint);
    ac.send();
  }
  else if (roomTemp < setPoint - 1) {
    lcd.print("OFF");
    ac.off();
    ac.send();
  }
  else {
    lcd.print("IDLE");
  }
}
