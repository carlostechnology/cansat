/***************************************************
  Proyecto: CANSAT - Test del sensor BMP280 (I2C)
  Versión: v1.0 - 27/10/2025
  Autor: Equipo CANSAT
  Centro: IES Gonzalo Torrente Ballester (Salamanca)

  Descripción:
  Detección y lectura del BMP280 por I2C. Muestra en
  el Monitor Serie temperatura (°C), presión (hPa) y
  altitud calculada según presión a nivel del mar.

  Licencia: MIT
***************************************************/

/*
  ESP32 + BMP280 (I2C) – Test de comprobación
  Conexiones:
    VCC  -> 3V3
    GND  -> GND
    SDA  -> GPIO21
    SCL  -> GPIO22
    CSB  -> 3V3   (para forzar I2C)
    SDO  -> GND   (dirección 0x76). Si lo llevas a 3V3 será 0x77.

  Librerías (Administrador de Bibliotecas):
    - Adafruit BMP280 Library
    - Adafruit Unified Sensor
*/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

// Pines I2C por defecto en ESP32
#define I2C_SDA 21
#define I2C_SCL 22

// Presión a nivel del mar (QNH) para calcular altitud.
// Ajusta con la presión local de tu zona (hPa). 1013.25 es un valor estándar.
const float SEA_LEVEL_HPA = 1013.25;

Adafruit_BMP280 bmp;  // I2C
uint8_t bmpAddr = 0;  // se rellenará con 0x76 u 0x77

void setup() {
  Serial.begin(115200);
  delay(300);

  // Inicia I2C en los pines estándar y a 400 kHz (rápido y estable)
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(400000); //I2C rápido

  Serial.println();
  Serial.println(F("== ESP32 + BMP280 (I2C) – Test =="));
  Serial.println(F("Buscando BMP280 en 0x76/0x77..."));

  // Intento en 0x76 y luego 0x77
  if (bmp.begin(0x76)) {
    bmpAddr = 0x76;
  } else if (bmp.begin(0x77)) {
    bmpAddr = 0x77;
  } else {
    Serial.println(F("❌ No se detecta BMP280. Revisa cableado y direccion (0x76/0x77)."));
    Serial.println(F("Pistas: VCC=3V3, GND comun, SDA->21, SCL->22, CSB->3V3, SDO->GND."));
    while (true) delay(1000);
  }

  Serial.print(F("✅ BMP280 detectado en direccion 0x"));
  Serial.println(bmpAddr, HEX);

  // Configuración de muestreo: modo normal + oversampling y filtro IIR
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     // modo
                  Adafruit_BMP280::SAMPLING_X2,     // temp oversampling
                  Adafruit_BMP280::SAMPLING_X16,    // presión oversampling
                  Adafruit_BMP280::FILTER_X16,      // filtro IIR
                  Adafruit_BMP280::STANDBY_MS_500); // standby entre medidas

  Serial.println(F("T(°C)\tP(hPa)\tP(Pa)\tAlt(m)"));
}

void loop() {
  // Lecturas
  float tempC   = bmp.readTemperature();      // °C
  float presPa  = bmp.readPressure();         // Pa
  float pres_hPa= presPa / 100.0;             // hPa
  float alt_m   = bmp.readAltitude(SEA_LEVEL_HPA);

  // Validación básica (por si el sensor aún no respondió)
  if (isnan(tempC) || presPa <= 0) {
    Serial.println(F("Lectura no válida. Esperando..."));
    delay(500);
    return;
  }

  // Salida formateada (útil para monitor serie y telemetría)
  Serial.print(tempC, 2);   Serial.print('\t');
  Serial.print(pres_hPa, 2);Serial.print('\t');
  Serial.print(presPa, 0);  Serial.print('\t');
  Serial.println(alt_m, 1);

  delay(1000);
}
