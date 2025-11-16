/***************************************************
  Proyecto: CANSAT - MPU6050 (DMP - Digital Motion Processor)
  Versión: v1.0 - 15/11/2025
  Autor: Equipo CANSAT
  Centro: IES Gonzalo Torrente Ballester (Salamanca)

  Descripción:
  Uso del DMP (Digital Motion Processor) interno del
  MPU6050 para obtener la orientación (yaw, pitch, roll)
  de forma eficiente. El DMP realiza la fusión de datos
  de acelerómetro y giroscopio dentro del propio sensor
  y entrega cuaterniones y ángulos ya filtrados.

  Este ejemplo está inspirado en el clásico ejemplo DMP
  de Jeff Rowberg, adaptado a ESP32 y al proyecto CANSAT.

  Licencia: MIT
***************************************************/

/*
  ESP32 + MPU6050 (GY-521, I2C) – Orientación con DMP

  Conexiones (GY-521 → ESP32):

    VCC   -> 3V3
    GND   -> GND
    SDA   -> GPIO21
    SCL   -> GPIO22
    ADO   -> GND   (dirección I2C 0x68)
    INT   -> NO usado en este ejemplo (lectura por sondeo)
    XCL   -> No conectar
    XDA   -> No conectar

  Notas:
    - El DMP se encarga de la fusión de sensores.
    - El ESP32 solo lee los datos del FIFO y calcula
      yaw, pitch y roll (en grados).
    - En este ejemplo usamos lectura por sondeo (polling),
      sin interrupciones externas, para simplificar.

  Librerías necesarias (Administrador de Bibliotecas o
  instalación manual desde I2Cdevlib):
    - I2Cdev
    - MPU6050 (la de Jeff Rowberg, con soporte DMP)
      (normalmente incluye "MPU6050_6Axis_MotionApps20.h")

  IMPORTANTE:
    - Esta no es la librería "Adafruit MPU6050", sino la
      de Jeff Rowberg (I2Cdevlib).
*/

#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

// Objeto global del MPU6050 con soporte DMP
MPU6050 mpu(0x68);

// Variables DMP / FIFO
bool    dmpReady      = false;
uint8_t mpuIntStatus;
uint8_t devStatus;
uint16_t packetSize;      // tamaño de paquete DMP
uint16_t fifoCount;       // nº de bytes en FIFO
uint8_t fifoBuffer[64];   // buffer FIFO

// Estructuras para orientación
Quaternion q;             // cuaternión
VectorFloat gravity;      // vector gravedad
float ypr[3];             // yaw, pitch, roll en radianes

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println(F("=== CANSAT - MPU6050 DMP (ESP32) ==="));

  // I2C en ESP32
  Wire.begin(21, 22);
  Wire.setClock(400000);  // I2C rápido

  Serial.println(F("Inicializando MPU6050..."));
  mpu.initialize();

  // Comprobar conexión
  if (!mpu.testConnection()) {
    Serial.println(F("ERROR: No se pudo comunicar con el MPU6050."));
    Serial.println(F("Revisa VCC=3V3, GND, SDA=21, SCL=22, ADO=GND."));
    while (1) delay(1000);
  }
  Serial.println(F("OK: MPU6050 conectado."));

  // Inicializar DMP
  Serial.println(F("Cargando y configurando DMP..."));
  devStatus = mpu.dmpInitialize();

  // Offset básicos (puedes ajustarlos por calibración)
  mpu.setXAccelOffset(0);
  mpu.setYAccelOffset(0);
  mpu.setZAccelOffset(0);
  mpu.setXGyroOffset(0);
  mpu.setYGyroOffset(0);
  mpu.setZGyroOffset(0);

  if (devStatus == 0) {
    // Activar DMP
    mpu.setDMPEnabled(true);
    dmpReady = true;

    // Tamaño del paquete que genera el DMP
    packetSize = mpu.dmpGetFIFOPacketSize();
    Serial.print(F("DMP iniciado correctamente. Tamaño de paquete = "));
    Serial.println(packetSize);
    Serial.println(F("Mostrando yaw, pitch, roll (grados)...\n"));
  } else {
    Serial.print(F("ERROR al iniciar DMP. Código: "));
    Serial.println(devStatus);
    Serial.println(F("No se puede continuar sin DMP."));
    while (1) delay(1000);
  }
}

void loop() {
  if (!dmpReady) return;  // seguridad

  // Saber cuántos bytes hay en FIFO
  fifoCount = mpu.getFIFOCount();

  if (fifoCount < packetSize) {
    // Aún no hay un paquete completo, esperamos un poco
    delay(5);
    return;
  }

  // Si hay demasiados datos, limpiamos FIFO para evitar bloqueos
  if (fifoCount >= 1024) {
    mpu.resetFIFO();
    Serial.println(F("Aviso: FIFO overflow, reseteando."));
    return;
  }

  // Leer exactamente un paquete DMP completo
  mpu.getFIFOBytes(fifoBuffer, packetSize);

  // Obtener cuaternión y gravedad
  mpu.dmpGetQuaternion(&q, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &q);

  // Obtener yaw, pitch, roll (en radianes)
  mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

  // Convertir a grados
  float yawDeg   = ypr[0] * 180.0 / PI;
  float pitchDeg = ypr[1] * 180.0 / PI;
  float rollDeg  = ypr[2] * 180.0 / PI;

  Serial.print("yaw = ");
  Serial.print(yawDeg, 1);
  Serial.print(" º   pitch = ");
  Serial.print(pitchDeg, 1);
  Serial.print(" º   roll = ");
  Serial.print(rollDeg, 1);
  Serial.println(" º");

  delay(20);  // ~50 Hz
}
