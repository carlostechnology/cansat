/***************************************************
  Proyecto: CANSAT - MPU6050 (DMP) + microSD
  Módulo Caja Negra: Orientación 3D (yaw, pitch, roll)
  Versión: v1.0 - 15/11/2025
  Autor: Equipo CANSAT
  Centro: IES Gonzalo Torrente Ballester (Salamanca)

  Descripción:
  Registro completo de orientación 3D utilizando el
  DMP (Digital Motion Processor) del MPU6050. El DMP
  fusiona acelerómetro y giroscopio dentro del propio
  sensor y entrega yaw, pitch y roll filtrados, estables
  y listos para usar.

  Los datos se almacenan en una tarjeta microSD en
  formato CSV compatible con Excel y herramientas de
  análisis científico (Python, MATLAB, etc.).
  Cada vuelo genera automáticamente un archivo del tipo:

      /LOG_0001.csv
      /LOG_0002.csv
      ...

  Este módulo funciona como una "caja negra" del CANSAT:
  registra orientación en grados con marca de tiempo,
  permitiendo analizar el lanzamiento, la caída, las
  rotaciones del paracaídas y la estabilidad en vuelo.

  Los valores de offset del MPU6050 han sido obtenidos
  previamente mediante el sketch de calibración específico.
  Si se usa otro sensor, es necesario recalibrarlo.

  Formato de salida (CSV):
      time_s;yaw_deg;pitch_deg;roll_deg

  Licencia: MIT
***************************************************/

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "esp_system.h"

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

// ─ I2C (MPU6050) ─
#define I2C_SDA 21
#define I2C_SCL 22

// ─ SPI (microSD) ─
#define SD_CS   27
#define SD_SCK  18
#define SD_MISO 19
#define SD_MOSI 23

// ─ Configuración ─
const uint32_t SAMPLE_MS   = 1000;  // intervalo entre muestras (ms)
const uint32_t SAMPLES_MAX = 0;     // 0 = ilimitado
const uint8_t  FLUSH_EVERY = 10;    // flush cada 10 líneas

MPU6050 mpu(0x68);
File   logFile;
char   logName[20] = {0};

bool okMPU = false;
bool okDMP = false;
bool okSD  = false;
bool okLOG = false;

// ─ DMP / MPU6050 ─
bool    dmpReady      = false;
uint8_t devStatus;
uint16_t packetSize;
uint16_t fifoCount;
uint8_t fifoBuffer[64];

Quaternion q;
VectorFloat gravity;
float ypr[3];  // yaw, pitch, roll en radianes

// ─────────────────────────────────────────────
// Utilidades
// ─────────────────────────────────────────────

void printNumberWithComma(Print& out, float value, int decimals) {
  char buf[24];
  dtostrf(value, 0, decimals, buf);
  for (char* p = buf; *p; ++p) out.print(*p == '.' ? ',' : *p);
}

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
  Serial.println(F("# ❌ SD.begin() falló. Revisa VCC=5V, GND, FAT32 y pines."));
  return false;
}

// Lee el DMP tantas veces como haga falta para vaciar FIFO
// Si consigue una muestra válida, actualiza yaw/pitch/roll en grados
// y devuelve true; si no, devuelve false.
bool readDMP(float& yawDeg, float& pitchDeg, float& rollDeg) {
  if (!dmpReady) return false;

  fifoCount = mpu.getFIFOCount();
  if (fifoCount == 0) return false;

  if (fifoCount == 1024) {
    // Overflow: limpiamos y nos vamos
    mpu.resetFIFO();
    Serial.println(F("# Aviso: FIFO overflow, reseteando."));
    return false;
  }

  if (fifoCount < packetSize) return false;

  // Leemos hasta quedarnos con el ÚLTIMO paquete de la FIFO
  while (fifoCount >= packetSize) {
    mpu.getFIFOBytes(fifoBuffer, packetSize);
    fifoCount -= packetSize;
  }

  mpu.dmpGetQuaternion(&q, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &q);
  mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

  yawDeg   = ypr[0] * 180.0 / PI;
  pitchDeg = ypr[1] * 180.0 / PI;
  rollDeg  = ypr[2] * 180.0 / PI;

  return true;
}

// ─────────────────────────────────────────────
// SETUP
// ─────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\n# ESP32 + MPU6050 (DMP) + microSD — CSV (ES)");

  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(400000);

  Serial.println(F("# Inicializando MPU6050..."));
  mpu.initialize();
  if (mpu.testConnection()) {
    Serial.println(F("# MPU6050 conectado."));
    okMPU = true;
  } else {
    Serial.println(F("# ❌ No se detecta MPU6050"));
  }

  if (okMPU) {
    Serial.println(F("# Configurando DMP..."));
    devStatus = mpu.dmpInitialize();

    // Offsets de tu calibración
    mpu.setXAccelOffset(-1062);
    mpu.setYAccelOffset( 2636);
    mpu.setZAccelOffset( 1330);
    mpu.setXGyroOffset(   104);
    mpu.setYGyroOffset(   -14);
    mpu.setZGyroOffset(    -8);

    if (devStatus == 0) {
      mpu.setDMPEnabled(true);
      dmpReady   = true;
      packetSize = mpu.dmpGetFIFOPacketSize();
      Serial.println(F("# DMP iniciado correctamente."));
      okDMP = true;
    } else {
      Serial.print(F("# ❌ Error al iniciar DMP. devStatus = "));
      Serial.println(devStatus);
    }
  }

  okSD = beginSD();
  if (okSD) {
    if (createUniqueLog(logName, sizeof(logName))) {
      logFile = SD.open(logName, FILE_WRITE);
      if (logFile) {
        okLOG = true;
        Serial.print(F("# Archivo creado: "));
        Serial.println(logName);
        const char* header =
          "time_s;yaw_deg;pitch_deg;roll_deg\n";
        logFile.print(header); logFile.flush();
        Serial.print(header);
      } else {
        Serial.println(F("# ❌ No se pudo abrir el archivo para escribir."));
      }
    } else {
      Serial.println(F("# ❌ No se pudo crear archivo nuevo."));
    }
  }

  Serial.print(F("# ESTADO INICIAL -> MPU="));
  Serial.print(okMPU);
  Serial.print(F("  DMP="));
  Serial.print(okDMP);
  Serial.print(F("  SD="));
  Serial.print(okSD);
  Serial.print(F("  LOG="));
  Serial.println(okLOG);
}

// ─────────────────────────────────────────────
// LOOP
// ─────────────────────────────────────────────
void loop() {
  static uint32_t lastLog = 0;
  static uint32_t k       = 0;
  static uint8_t  linesSinceFlush = 0;

  static float lastYawDeg   = 0;
  static float lastPitchDeg = 0;
  static float lastRollDeg  = 0;

  // 1) Siempre que podamos, leemos del DMP para vaciar la FIFO
  float yawDeg, pitchDeg, rollDeg;
  if (readDMP(yawDeg, pitchDeg, rollDeg)) {
    lastYawDeg   = yawDeg;
    lastPitchDeg = pitchDeg;
    lastRollDeg  = rollDeg;
  }

  // 2) Cada SAMPLE_MS registramos la ÚLTIMA lectura disponible
  if (millis() - lastLog >= SAMPLE_MS) {
    lastLog = millis();
    float ts = ++k * (SAMPLE_MS / 1000.0);

    // Serial
    Serial.print("DATA -> ");
    printNumberWithComma(Serial, ts,           2); Serial.print(';');
    printNumberWithComma(Serial, lastYawDeg,   1); Serial.print(';');
    printNumberWithComma(Serial, lastPitchDeg, 1); Serial.print(';');
    printNumberWithComma(Serial, lastRollDeg,  1); Serial.println();

    // SD
    if (okLOG && logFile) {
      printNumberWithComma(logFile, ts,           2); logFile.print(';');
      printNumberWithComma(logFile, lastYawDeg,   1); logFile.print(';');
      printNumberWithComma(logFile, lastPitchDeg, 1); logFile.print(';');
      printNumberWithComma(logFile, lastRollDeg,  1); logFile.print('\n');

      if (++linesSinceFlush >= FLUSH_EVERY) {
        logFile.flush();
        linesSinceFlush = 0;
      }
    }
  }

  delay(5);  // pequeña pausa para no saturar la CPU
}
