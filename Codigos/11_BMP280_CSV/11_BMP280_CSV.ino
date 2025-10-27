/***************************************************
  Proyecto: CANSAT - BMP280 CSV para Excel
  Versión: v1.0 - 27/10/2025
  Autor: Equipo CANSAT
  Centro: IES Gonzalo Torrente Ballester (Salamanca)

  Descripción:
  Lectura del sensor BMP280 por I2C y salida por el
  Monitor Serie en formato CSV compatible con Excel:
  tiempo (s), temperatura (°C), presión (hPa) y altitud (m).
  Ideal para registrar datos y analizarlos en gráficos.

  Licencia: MIT
***************************************************/

/*
  ESP32 + BMP280 (I2C) — CSV para Excel
  Genera:
    sep=,
    time_s,temp_C,press_hPa,alt_m
    0.00,23.16,931.80,701.1
    ...

  Conexiones (I2C):
    VCC  -> 3V3
    GND  -> GND
    SDA  -> GPIO21
    SCL  -> GPIO22
    CSB  -> 3V3
    SDO  -> GND  (direccion 0x76; a 3V3 -> 0x77)

  Librerías:
    - Adafruit BMP280 Library
    - Adafruit Unified Sensor
*/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define I2C_SDA 21
#define I2C_SCL 22

const float SEA_LEVEL_HPA = 1013.25; // Sustituye por QNH del día
const uint32_t SAMPLE_MS = 1000;     // 1 dato/seg

Adafruit_BMP280 bmp;
uint32_t t0;

void setup() {
  Serial.begin(115200);
  delay(300);

  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(400000);

  if      (bmp.begin(0x76)) Serial.println(F("BMP280 en 0x76"));
  else if (bmp.begin(0x77)) Serial.println(F("BMP280 en 0x77"));
  else { Serial.println(F("No BMP280")); while(1){} }

  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);

  // Cabecera para Excel (reconoce la coma como separador)
  Serial.println(F("sep=,"));
  Serial.println(F("time_s,temp_C,press_hPa,alt_m"));

  t0 = millis();
}

void loop() {
  static uint32_t last = 0;
  if (millis() - last < SAMPLE_MS) return;
  last = millis();

  float tempC    = bmp.readTemperature();
  float pres_hPa = bmp.readPressure() / 100.0;
  float alt_m    = bmp.readAltitude(SEA_LEVEL_HPA);
  float time_s   = (millis() - t0) / 1000.0;

  if (isnan(tempC) || pres_hPa <= 0) {
    // Evita imprimir basura si la lectura no es válida
    return;
  }

  Serial.print(time_s, 2);  Serial.print(',');
  Serial.print(tempC, 2);   Serial.print(',');
  Serial.print(pres_hPa, 2);Serial.print(',');
  Serial.println(alt_m, 1);
}
