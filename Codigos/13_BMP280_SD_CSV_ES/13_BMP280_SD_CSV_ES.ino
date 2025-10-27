/***************************************************
  Proyecto: CANSAT - BMP280 + microSD CSV (España)
  Versión: v1.0 - 27/10/2025
  Autor: Equipo CANSAT
  Centro: IES Gonzalo Torrente Ballester (Salamanca)

  Descripción:
  Lectura del BMP280 y registro de datos en tarjeta microSD
  en formato CSV compatible con Excel (separador ';' y coma
  decimal). Modo vuelo: SAMPLES_MAX = 0 para almacenamiento
  continuo. Crea automáticamente LOG_XXXX.csv numerados.

  Licencia: MIT
***************************************************/

/*
  ESP32 (DIYMORE) + BMP280 (I2C) + microSD (SPI)
  CSV para Excel en España (separador ';' y coma decimal ',').
*/

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include "esp_system.h"

// ─ I2C (BMP280) ─
#define I2C_SDA 21
#define I2C_SCL 22

// ─ SPI (microSD) ─
#define SD_CS   27
#define SD_SCK  18
#define SD_MISO 19
#define SD_MOSI 23   // ⚠️ “G33” en tu placa es en realidad GPIO23

// ─ Configuración ─
const float    SEA_LEVEL_HPA = 1024.0;  // presión QNH del día
const uint32_t SAMPLE_MS     = 1000;    // intervalo entre muestras (ms)
const uint32_t SAMPLES_MAX   = 0;       // 0 = ilimitado (modo vuelo)
const uint8_t  FLUSH_EVERY   = 10;      // flush cada 10 líneas

Adafruit_BMP280 bmp;
File   logFile;
char   logName[20] = {0};

// convierte '.' a ',' para CSV español
void printNumberWithComma(Print& out, float value, int decimals) {
  char buf[24];
  dtostrf(value, 0, decimals, buf);
  for (char* p = buf; *p; ++p) out.print(*p == '.' ? ',' : *p);
}

// crea archivo nuevo: LOG_0001.csv, LOG_0002.csv, ...
bool createUniqueLog(char* outName, size_t len) {
  for (uint16_t n = 1; n < 10000; n++) {
    snprintf(outName, len, "/LOG_%04u.csv", n);
    if (!SD.exists(outName)) {
      File f = SD.open(outName, FILE_WRITE);
      if (!f) return false;
      f.close();
      return true;
    }
  }
  return false;
}

bool beginSD() {
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  Serial.println(F("# Iniciando SD..."));
  if (SD.begin(SD_CS, SPI, 10000000)) {
    Serial.println(F("# SD OK (10 MHz)."));
    return true;
  }
  Serial.println(F("# Aviso: 10 MHz falló. Reintentando a 4 MHz..."));
  if (SD.begin(SD_CS, SPI, 4000000)) {
    Serial.println(F("# SD OK (4 MHz)."));
    return true;
  }
  Serial.println(F("# ❌ SD.begin() falló. Revisa VCC=5V, GND real, FAT32 y pines."));
  return false;
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\n# ESP32 DIYMORE + BMP280 + microSD — CSV (ES)");

  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(400000);

  if      (bmp.begin(0x76)) Serial.println(F("# BMP280 en 0x76"));
  else if (bmp.begin(0x77)) Serial.println(F("# BMP280 en 0x77"));
  else { Serial.println(F("# ❌ No se detecta BMP280")); while (1) delay(1000); }

  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);

  if (!beginSD()) while (1) delay(2000);

  if (!createUniqueLog(logName, sizeof(logName))) {
    Serial.println(F("# ❌ No se pudo crear archivo nuevo."));
    while (1) delay(1000);
  }

  logFile = SD.open(logName, FILE_WRITE);
  if (!logFile) {
    Serial.println(F("# ❌ No se pudo abrir el archivo para escribir."));
    while (1) delay(1000);
  }

  Serial.print(F("# Archivo: ")); Serial.println(logName);
  const char* header = "time_s;temp_C;press_hPa;alt_m\n";
  logFile.print(header); logFile.flush();
  Serial.print(header);
}

void loop() {
  static uint32_t next_due = 0;
  static uint32_t k = 0;
  static uint8_t linesSinceFlush = 0;

  if (SAMPLES_MAX > 0 && k >= SAMPLES_MAX) {
    Serial.println(F("# FIN: limite de muestras alcanzado. Copia el CSV."));
    logFile.flush(); logFile.close();
    while (1) delay(1000);
  }

  if (next_due == 0) next_due = millis();
  if ((int32_t)(millis() - next_due) < 0) return;

  float t = bmp.readTemperature();
  float p = bmp.readPressure() / 100.0;
  float a = bmp.readAltitude(SEA_LEVEL_HPA);

  if (isnan(t) || p <= 0) { next_due += SAMPLE_MS; return; }

  float ts = k * (SAMPLE_MS / 1000.0);

  printNumberWithComma(Serial, ts, 2); Serial.print(';');
  printNumberWithComma(Serial, t,  2); Serial.print(';');
  printNumberWithComma(Serial, p,  2); Serial.print(';');
  printNumberWithComma(Serial, a,  1); Serial.println();

  if (logFile) {
    printNumberWithComma(logFile, ts, 2); logFile.print(';');
    printNumberWithComma(logFile, t,  2); logFile.print(';');
    printNumberWithComma(logFile, p,  2); logFile.print(';');
    printNumberWithComma(logFile, a,  1); logFile.print('\n');

    if (++linesSinceFlush >= FLUSH_EVERY) {
      logFile.flush();
      linesSinceFlush = 0;
    }
  }

  k++;
  next_due += SAMPLE_MS;
}
