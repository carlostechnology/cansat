/***************************************************
  Proyecto: CANSAT - MPU6050 (DMP + Serial Plotter)
  Ejemplo 7C: yaw, pitch y roll con nombres en el gráfico
  Versión: v1.0 - 15/11/2025
  Autor: Equipo CANSAT
  Centro: IES Gonzalo Torrente Ballester (Salamanca)

  Descripción:
  Variante del Ejemplo 7 en la que usamos el DMP del
  MPU6050 para obtener yaw, pitch y roll, y los enviamos
  al Serial Plotter con nombres de variable, para que
  aparezcan etiquetados como "yaw", "pitch" y "roll"
  en el gráfico.

  Formato de salida (una muestra por línea):
    yaw: <valor>   pitch: <valor>   roll: <valor>

  Licencia: MIT
***************************************************/

/*
  ESP32 + MPU6050 (GY-521, I2C) – DMP + Serial Plotter (con nombres)

  Conexiones:
    VCC   -> 3V3
    GND   -> GND
    SDA   -> GPIO21
    SCL   -> GPIO22
    ADO   -> GND   (dirección I2C 0x68)
    INT   -> Sin conectar
    XCL   -> No conectar
    XDA   -> No conectar

  Librerías necesarias:
    - I2Cdev
    - MPU6050 (MotionApps20)
*/

#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

// Objeto global del MPU6050
MPU6050 mpu(0x68);

// Variables del DMP / FIFO
bool    dmpReady      = false;
uint8_t devStatus;
uint16_t packetSize;
uint16_t fifoCount;
uint8_t fifoBuffer[64];

// Orientación
Quaternion q;
VectorFloat gravity;
float ypr[3];

void setup() {
  Serial.begin(115200);
  delay(1000);

  // I2C de la ESP32
  Wire.begin(21, 22);
  Wire.setClock(400000);

  mpu.initialize();
  if (!mpu.testConnection()) {
    // Si no hay sensor, nos quedamos aquí
    while (1) delay(1000);
  }

  // Inicializar DMP
  devStatus = mpu.dmpInitialize();

  // Offsets obtenidos con el sketch de calibración
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
    while (1) delay(1000);  // error al iniciar DMP
  }
}

void loop() {
  if (!dmpReady) return;

  fifoCount = mpu.getFIFOCount();

  // Si FIFO llena, reseteamos y seguimos sin imprimir
  if (fifoCount == 1024) {
    mpu.resetFIFO();
    return;
  }

  // Esperar a tener un paquete completo
  while (fifoCount < packetSize) {
    fifoCount = mpu.getFIFOCount();
  }

  // Leer un paquete del DMP
  mpu.getFIFOBytes(fifoBuffer, packetSize);
  fifoCount -= packetSize;

  // Obtener yaw, pitch, roll
  mpu.dmpGetQuaternion(&q, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &q);
  mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

  float yawDeg   = ypr[0] * 180.0 / PI;
  float pitchDeg = ypr[1] * 180.0 / PI;
  float rollDeg  = ypr[2] * 180.0 / PI;

  // Formato para Serial Plotter con nombres
  Serial.print("yaw: ");
  Serial.print(yawDeg);
  Serial.print("\t pitch: ");
  Serial.print(pitchDeg);
  Serial.print("\t roll: ");
  Serial.println(rollDeg);

  delay(5);
}
