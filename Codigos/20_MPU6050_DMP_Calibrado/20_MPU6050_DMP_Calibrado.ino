/***************************************************
  Proyecto: CANSAT - MPU6050 (DMP - Digital Motion Processor)
  Ejemplo 7: Orientación 3D con DMP (yaw, pitch, roll)
  Versión: v1.1 - 15/11/2025
  Autor: Equipo CANSAT
  Centro: IES Gonzalo Torrente Ballester (Salamanca)

  Descripción:
  Uso del DMP interno del MPU6050 para obtener la
  orientación (yaw, pitch, roll) ya filtrada.

  El DMP fusiona acelerómetro y giroscopio dentro del
  propio sensor y entrega orientación muy estable.
  El ESP32 solo lee los datos de la FIFO y los muestra
  en grados por el Monitor Serie.

  Los offsets utilizados han sido obtenidos con el
  sketch de calibración específico para este MPU6050.
  Si usas otro módulo, debes recalibrar y sustituir
  los valores.

  Licencia: MIT
***************************************************/

/*
  ESP32 + MPU6050 (GY-521, I2C) – Orientación con DMP

  Conexiones:
    VCC   -> 3V3
    GND   -> GND
    SDA   -> GPIO21
    SCL   -> GPIO22
    ADO   -> GND   (dirección I2C 0x68)
    INT   -> Sin conectar (en este ejemplo usamos sondeo)
    XCL   -> No conectar
    XDA   -> No conectar

  Librerías necesarias:
    - I2Cdev
    - MPU6050 (con MotionApps: MPU6050_6Axis_MotionApps20.h)
    Descargar desde:
    https://github.com/jrowberg/i2cdevlib/tree/master/Arduino
*/

#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

// Objeto global del MPU6050 con DMP
MPU6050 mpu(0x68);

// --- Variables DMP / FIFO ---
bool    dmpReady      = false;  // true cuando el DMP está listo
uint8_t devStatus;              // resultado de dmpInitialize()
uint16_t packetSize;            // tamaño de paquete DMP
uint16_t fifoCount;             // nº de bytes en FIFO
uint8_t fifoBuffer[64];         // buffer FIFO

// --- Orientación ---
Quaternion q;                   // cuaternión
VectorFloat gravity;            // vector gravedad
float ypr[3];                   // yaw, pitch, roll (radianes)

// =====================================================
// SETUP
// =====================================================
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println(F("=== CANSAT - Ejemplo 7: MPU6050 DMP (ESP32) ==="));

  // I2C del ESP32 (SDA = 21, SCL = 22)
  Wire.begin(21, 22);
  Wire.setClock(400000);  // I2C rápido

  Serial.println(F("Inicializando MPU6050..."));
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println(F("ERROR: No se detecta el MPU6050."));
    Serial.println(F("Comprueba VCC=3V3, GND, SDA=21, SCL=22, ADO=GND."));
    while (1) delay(1000);
  }
  Serial.println(F("OK: MPU6050 conectado."));

  // --- Inicializar DMP ---
  Serial.println(F("Cargando y configurando DMP..."));
  devStatus = mpu.dmpInitialize();

  // -------------------------------------------------
  // Offsets de calibración para ESTE MPU6050
  // (obtenidos con el sketch de calibración)
  // -------------------------------------------------
  mpu.setXAccelOffset(-1062);
  mpu.setYAccelOffset( 2636);
  mpu.setZAccelOffset( 1330);
  mpu.setXGyroOffset(   104);
  mpu.setYGyroOffset(   -14);
  mpu.setZGyroOffset(    -8);

  if (devStatus == 0) {
    // Activar DMP
    mpu.setDMPEnabled(true);
    dmpReady = true;

    // Tamaño de paquete que genera el DMP
    packetSize = mpu.dmpGetFIFOPacketSize();
    Serial.print(F("DMP iniciado correctamente. packetSize = "));
    Serial.println(packetSize);
    Serial.println(F("Mostrando yaw, pitch, roll (grados)...\n"));
  } else {
    Serial.print(F("ERROR al iniciar DMP. Código devStatus = "));
    Serial.println(devStatus);
    Serial.println(F("No se puede continuar sin DMP."));
    while (1) delay(1000);
  }
}

// =====================================================
// LOOP – lectura del DMP con control de FIFO
// =====================================================
void loop() {
  if (!dmpReady) return;  // seguridad

  // Bytes presentes en la FIFO
  fifoCount = mpu.getFIFOCount();

  // Si la FIFO está llena (overflow), la reseteamos y avisamos
  if (fifoCount == 1024) {
    mpu.resetFIFO();
    Serial.println(F("Aviso: FIFO overflow, reseteando."));
    return;
  }

  // Esperar a tener al menos un paquete completo
  while (fifoCount < packetSize) {
    fifoCount = mpu.getFIFOCount();
  }

  // Leer exactamente un paquete de la FIFO
  mpu.getFIFOBytes(fifoBuffer, packetSize);
  fifoCount -= packetSize;

  // Obtener cuaternión y vector gravedad
  mpu.dmpGetQuaternion(&q, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &q);

  // Calcular yaw, pitch y roll (en radianes)
  mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

  // Pasar a grados
  float yawDeg   = ypr[0] * 180.0 / PI;
  float pitchDeg = ypr[1] * 180.0 / PI;
  float rollDeg  = ypr[2] * 180.0 / PI;

  // Mostrar por el Monitor Serie
  Serial.print("yaw = ");
  Serial.print(yawDeg, 1);
  Serial.print(" º   pitch = ");
  Serial.print(pitchDeg, 1);
  Serial.print(" º   roll = ");
  Serial.print(rollDeg, 1);
  Serial.println(" º");

  // Pequeña pausa (~200 Hz de lectura aproximada)
  delay(5);
}
