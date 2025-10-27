/***************************************************
  Proyecto: CANSAT - BMP280 CSV con coma decimal (Excel España)
  Versión: v1.0 - 27/10/2025
  Autor: Equipo CANSAT
  Centro: IES Gonzalo Torrente Ballester (Salamanca)

  Descripción:
  Lectura del BMP280 en ESP32 por I2C y salida por monitor
  serie en formato CSV con separador ';' y coma decimal,
  compatible con Excel configurado en España.
  Muestra tiempo, temperatura, presión y altitud.

  Licencia: MIT
***************************************************/

/*
  ESP32 + BMP280 (I2C) — CSV para Excel (España)
  ----------------------------------------------
  Conexiones I2C:
    VCC  -> 3V3
    GND  -> GND
    SDA  -> GPIO21
    SCL  -> GPIO22
    CSB  -> 3V3   (modo I2C)
    SDO  -> GND   (direccion 0x76; si va a 3V3 -> 0x77)

  Librerías necesarias:
    - Adafruit BMP280 Library
    - Adafruit Unified Sensor
*/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define I2C_SDA 21
#define I2C_SCL 22

const float    SEA_LEVEL_HPA = 1024; // Sustituir por el QNH del día
const uint32_t SAMPLE_MS     = 1000;    // intervalo entre muestras (ms)
const uint16_t SAMPLES_MAX   = 10;      // nº máximo de líneas a imprimir

Adafruit_BMP280 bmp;

// 👉 Función para imprimir números con coma decimal
void printNumberWithComma(float value, int decimals) {
  char buffer[20];
  dtostrf(value, 1, decimals, buffer);  // convierte número a texto con punto decimal
  for (char* p = buffer; *p; p++) {
    if (*p == '.') *p = ',';            // reemplaza punto por coma
    Serial.print(*p);
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);

  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(400000);

  if      (bmp.begin(0x76)) Serial.println(F("# BMP280 en 0x76"));
  else if (bmp.begin(0x77)) Serial.println(F("# BMP280 en 0x77"));
  else { Serial.println(F("# ❌ No se detecta BMP280")); while (1) {} }

  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);

  // 👉 Cabecera CSV
  Serial.println(F("time_s;temp_C;press_hPa;alt_m"));
}

void loop() {
  static uint32_t next_due = 0;
  static uint16_t k = 0;

  if (k >= SAMPLES_MAX) {
    Serial.println(F("# FIN: limite de muestras alcanzado. Copia el CSV."));
    while (1) delay(1000); // detener programa
  }

  if (next_due == 0) next_due = millis();                  // primera muestra ya
  if ((int32_t)(millis() - next_due) < 0) return;          // aún no toca

  float tempC    = bmp.readTemperature();
  float pres_hPa = bmp.readPressure() / 100.0;
  float alt_m    = bmp.readAltitude(SEA_LEVEL_HPA);

  if (isnan(tempC) || pres_hPa <= 0) { next_due += SAMPLE_MS; return; }

  // Tiempo exacto (0.00, 1.00, 2.00…)
  float time_s = k * (SAMPLE_MS / 1000.0);

  // 👉 Línea CSV con separador ; y coma decimal
  printNumberWithComma(time_s, 2);   Serial.print(';');
  printNumberWithComma(tempC, 2);    Serial.print(';');
  printNumberWithComma(pres_hPa, 2); Serial.print(';');
  printNumberWithComma(alt_m, 1);    Serial.println();

  k++;
  next_due += SAMPLE_MS;
}
