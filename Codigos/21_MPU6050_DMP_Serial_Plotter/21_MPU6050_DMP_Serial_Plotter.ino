/***************************************************
  Proyecto: CANSAT - MPU6050 (DMP - Serial Plotter)
  Ejemplo 7B: Orientación 3D con DMP (gráfico)
  Versión: v1.0 - 15/11/2025
***************************************************/

#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

MPU6050 mpu(0x68);

// DMP / FIFO
bool    dmpReady      = false;
uint8_t devStatus;
uint16_t packetSize;
uint16_t fifoCount;
uint8_t fifoBuffer[64];

Quaternion q;
VectorFloat gravity;
float ypr[3];

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(21, 22);
  Wire.setClock(400000);

  mpu.initialize();
  if (!mpu.testConnection()) {
    while (1) delay(1000); // error grave
  }

  devStatus = mpu.dmpInitialize();

  // Offsets de TU MPU6050 (calibración previa)
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
  } else {
    while (1) delay(1000); // no hay DMP
  }

  // Cabecera para entender las columnas (Serial Plotter la ignora,
  // pero viene bien si copias los datos a un CSV)
  Serial.println("time_ms,yaw_deg,pitch_deg,roll_deg");
}

void loop() {
  if (!dmpReady) return;

  fifoCount = mpu.getFIFOCount();

  // Si FIFO llena, la reseteamos en silencio
  if (fifoCount == 1024) {
    mpu.resetFIFO();
    return;
  }

  // Espera hasta tener un paquete completo
  while (fifoCount < packetSize) {
    fifoCount = mpu.getFIFOCount();
  }

  // Leer un paquete
  mpu.getFIFOBytes(fifoBuffer, packetSize);
  fifoCount -= packetSize;

  // Convertir a yaw/pitch/roll
  mpu.dmpGetQuaternion(&q, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &q);
  mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

  float yawDeg   = ypr[0] * 180.0 / PI;
  float pitchDeg = ypr[1] * 180.0 / PI;
  float rollDeg  = ypr[2] * 180.0 / PI;

  unsigned long t = millis();

  // Formato para Serial Plotter: 4 columnas separadas por comas
  Serial.print(t);
  Serial.print(",");
  Serial.print(yawDeg);
  Serial.print(",");
  Serial.print(pitchDeg);
  Serial.print(",");
  Serial.println(rollDeg);

  delay(5);
}
