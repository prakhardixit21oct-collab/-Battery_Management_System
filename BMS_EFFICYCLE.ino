#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "BluetoothSerial.h"

// Check if Bluetooth is properly configured
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run menuconfig to enable it
#endif

BluetoothSerial SerialBT;

// Pin Definitions
#define VOLTAGE_PIN 34
#define CURRENT_PIN 35
#define RELAY_PIN 23

// Calibration Factors
const float R1 = 100000.0;
const float R2 = 5600.0;
const float ACS_SENSITIVITY = 0.040; // 40mV/A for ACS758-050B
const float V_REF = 3.3;

// Safety Thresholds
const float MAX_VOLTAGE = 54.6;
const float MIN_VOLTAGE = 42.0;
const float MAX_CURRENT = 35.0;

LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long previousMillis = 0;
const long interval = 500; // Broadcast every 500ms

void setup() {
  Serial.begin(115200);
  
  // Start Bluetooth with the device name visible to your phone
  SerialBT.begin("Efficycle_BMS_Telemetry"); 
  Serial.println("Bluetooth Telemetry Ready. Pair with 'Efficycle_BMS_Telemetry'");

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  
  lcd.init();
  lcd.backlight();
}

void loop() {
  // Read Sensors
  int rawVoltage = analogRead(VOLTAGE_PIN);
  float vOut = (rawVoltage / 4095.0) * V_REF;
  float batteryVoltage = vOut * ((R1 + R2) / R2);

  int rawCurrent = analogRead(CURRENT_PIN);
  float vCurrent = (rawCurrent / 4095.0) * V_REF;
  float batteryCurrent = abs((vCurrent - (V_REF / 2.0)) / ACS_SENSITIVITY);
  
  float powerWatts = batteryVoltage * batteryCurrent;

  // Local Display Update
  lcd.setCursor(0, 0);
  lcd.print("V:" + String(batteryVoltage, 1) + "V I:" + String(batteryCurrent, 1) + "A");

  // Non-blocking Bluetooth Transmission
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Standard CSV format for easy parsing: "VOLTAGE,CURRENT,POWER"
    String telemetryData = String(batteryVoltage, 2) + "," + 
                           String(batteryCurrent, 2) + "," + 
                           String(powerWatts, 2) + "\n";
                           
    SerialBT.print(telemetryData); // Send to smartphone
  }

  // Safety Protections
  if (batteryVoltage < MIN_VOLTAGE || batteryVoltage > MAX_VOLTAGE || batteryCurrent > MAX_CURRENT) {
    digitalWrite(RELAY_PIN, LOW);
    SerialBT.println("ALERT: BMS FAULT TRIP!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BMS TRIP: FAULT!");
    while(1);
  }
}