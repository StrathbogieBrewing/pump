// ATMEL ATMEGA8 & 168 / ARDUINO
//
//                  +-\/-+
//            PC6  1|    |28  PC5 (AI 5)
//      (D 0) PD0  2|    |27  PC4 (AI 4)
//      (D 1) PD1  3|    |26  PC3 (AI 3)
//      (D 2) PD2  4|    |25  PC2 (AI 2)
// PWM+ (D 3) PD3  5|    |24  PC1 (AI 1)
//      (D 4) PD4  6|    |23  PC0 (AI 0)
//            VCC  7|    |22  GND
//            GND  8|    |21  AREF
//            PB6  9|    |20  AVCC
//            PB7 10|    |19  PB5 (D 13)
// PWM+ (D 5) PD5 11|    |18  PB4 (D 12)
// PWM+ (D 6) PD6 12|    |17  PB3 (D 11) PWM
//      (D 7) PD7 13|    |16  PB2 (D 10) PWM
//      (D 8) PB0 14|    |15  PB1 (D 9) PWM
//                  +----+

#include <Wire.h>

// #include "TinBus.h"

#define PIN_VBAT (A0)
#define PIN_IBAT (A1)

#define PIN_PUMP (8)
#define PIN_VALVE_PATIO (9)
#define PIN_VALVE_HOTHOUSE (4)
#define PIN_VALVE_CAGE (5)

#define PRES_I2C_ADDR ((uint8_t)72)

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(PIN_PUMP, OUTPUT);
  pinMode(PIN_VALVE_PATIO, OUTPUT);
  pinMode(PIN_VALVE_HOTHOUSE, OUTPUT);
  pinMode(PIN_VALVE_CAGE, OUTPUT);

  Wire.begin();

  // tinbusBegin();
}

void loop() {
  static unsigned long minutes = 0;
  static int16_t head_cm = 0;

  static uint32_t vbat_filter = 0;

  // delay(1000);
  // int16_t rxData = tinbusRead();
  // if(rxData >= 0){
  //   Serial.println(rxData, HEX);
  // }

  delay(1000);
  // uint16_t vbat_mv = ((uint16_t)analogRead(PIN_VBAT) * 111) >> 2;

  uint32_t vbat_raw = analogRead(PIN_VBAT);
  if (vbat_filter == 0) {
    vbat_filter = vbat_raw << 8L;
  }

  vbat_filter -= vbat_filter >> 8L;
  vbat_filter += vbat_raw;

  uint32_t vbat_mv = ((vbat_filter >> 8L) * 7104L) >> 8;

  Serial.print("vbat ");
  Serial.println(vbat_mv);

  if (vbat_mv < 13400) {
    minutes = 0; // reset timer
    digitalWrite(PIN_PUMP, LOW);
  }

  delay(1000);
  unsigned long newMinutes = (millis() >> 16); // dt = 65.536 secs
  Serial.print("time ");
  Serial.println(newMinutes);

  // pump control
  if (newMinutes != minutes) {
    minutes = newMinutes;

    uint8_t time_slot = minutes % 120;

    if ((minutes >= 120) && (minutes < 480)) {
      if ((head_cm < 550) && (time_slot == 0)) {
        digitalWrite(PIN_PUMP, HIGH);
      }
    } else {
      digitalWrite(PIN_PUMP, LOW);
    }

    // if(((minutes & 0xC0) == 0x80) && (lowBatteryTimer == 0)){
    //   digitalWrite(PIN_PUMP, HIGH);
    // } else {
    //   digitalWrite(PIN_PUMP, LOW);
    // }
    // }

    // valve sequencing, 2 minutes per 2 hours per valve
    if ((time_slot == 2) || (time_slot == 3)) {
      digitalWrite(PIN_VALVE_PATIO, HIGH);
    } else {
      digitalWrite(PIN_VALVE_PATIO, LOW);
    }
    if ((time_slot == 4) || (time_slot == 5)) {
      digitalWrite(PIN_VALVE_HOTHOUSE, HIGH);
    } else {
      digitalWrite(PIN_VALVE_HOTHOUSE, LOW);
    }
    if ((time_slot >= 6) || (time_slot < 12)) {
      digitalWrite(PIN_VALVE_CAGE, HIGH);
    } else {
      digitalWrite(PIN_VALVE_CAGE, LOW);
    }
  }

    delay(1000);
    Wire.beginTransmission(PRES_I2C_ADDR);
    Wire.endTransmission();
    uint8_t config = 0x8F; // config for gain of 8,
    Wire.beginTransmission(PRES_I2C_ADDR);
    Wire.write(config);
    Wire.endTransmission();
    Wire.requestFrom(PRES_I2C_ADDR, (uint8_t)2);
    int16_t pressure = Wire.read() << 8;
    pressure |= Wire.read();
    pressure *= 19;
    pressure /= 4;
    head_cm = pressure / 100;
    Serial.print("head ");
    Serial.println(head_cm);
  }

  // delay(500);
  // digitalWrite(PIN_PUMP, HIGH);
  // digitalWrite(PIN_VALVE_PATIO, HIGH);
  // digitalWrite(PIN_VALVE_HOTHOUSE, HIGH);
  // digitalWrite(PIN_VALVE_CAGE, HIGH);
