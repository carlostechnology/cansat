/***************************************************
  Proyecto: CANSAT - Calibración del MPU6050 (DMP)
  Versión: v1.0 - 15/11/2025
  Autor: Equipo CANSAT
  Centro: IES Gonzalo Torrente Ballester (Salamanca)

  Descripción:
  Este sketch calibra el MPU6050 usando la librería
  I2Cdev + MPU6050 de Jeff Rowberg. Calcula los
  offsets del acelerómetro y el giroscopio necesarios
  para que el DMP (yaw, pitch, roll) funcione bien.

  Al final muestra 6 offsets:
    ax_offset, ay_offset, az_offset, gx_offset, gy_offset, gz_offset
  que deberás copiar en tu sketch del DMP.

  Licencia: MIT
***************************************************/

/*
  ESP32 + MPU6050 (I2C) – Calibración del DMP

  Conexiones:
    VCC  -> 3V3
    GND  -> GND
    SDA  -> GPIO21
    SCL  -> GPIO22
    ADO  -> GND  (dirección I2C = 0x68)
*/

#include "I2Cdev.h"
#include "MPU6050.h"
#include <Wire.h>

// ---------- CONFIGURACIÓN DE LA CALIBRACIÓN ----------
int buffersize    = 1000;  // nº de lecturas para hacer media
int acel_deadzone = 8;     // error permitido en acelerómetro
int giro_deadzone = 1;     // error permitido en giroscopio

// Dirección habitual: 0x68 (ADO a GND)
MPU6050 accelgyro(0x68);

int16_t ax, ay, az, gx, gy, gz;
int mean_ax, mean_ay, mean_az, mean_gx, mean_gy, mean_gz;
int ax_offset, ay_offset, az_offset, gx_offset, gy_offset, gz_offset;
int state = 0;

// ---------- SETUP ----------
void setup() {
  // I2C del ESP32 en pines 21–22
  Wire.begin(21, 22);

  Serial.begin(115200);
  delay(1000);

  accelgyro.initialize();

  Serial.println("MPU6050 Calibration Sketch (ESP32)");
  Serial.println("Coloca el sensor horizontal, sin tocarlo.");
  Serial.println("Pulsa cualquier tecla para comenzar...\n");

  // Esperar a que el usuario pulse algo
  while (!Serial.available()) delay(100);
  while (Serial.available()) Serial.read();  // vaciar

  Serial.println("Comenzando calibración...");
  delay(2000);

  // Reset de offsets
  accelgyro.setXAccelOffset(0);
  accelgyro.setYAccelOffset(0);
  accelgyro.setZAccelOffset(0);
  accelgyro.setXGyroOffset(0);
  accelgyro.setYGyroOffset(0);
  accelgyro.setZGyroOffset(0);

  // Comprobar conexión
  Serial.println(accelgyro.testConnection() ?
                 "MPU6050 conectado correctamente" :
                 "ERROR: No se detecta el MPU6050");
  delay(1000);
}

// ---------- LOOP ----------
void loop() {
  if (state == 0) {
    Serial.println("\nLeyendo sensores (primera pasada)...");
    meansensors();
    state++;
    delay(500);
  }
  else if (state == 1) {
    Serial.println("Calculando offsets...");
    calibration();
    state++;
    delay(500);
  }
  else if (state == 2) {
    meansensors();
    Serial.println("\n¡CALIBRACIÓN COMPLETADA!\n");

    Serial.print("Lecturas promedio con offsets:\t");
    Serial.print(mean_ax); Serial.print("\t");
    Serial.print(mean_ay); Serial.print("\t");
    Serial.print(mean_az); Serial.print("\t");
    Serial.print(mean_gx); Serial.print("\t");
    Serial.print(mean_gy); Serial.print("\t");
    Serial.println(mean_gz);

    Serial.println("\nTus OFFSETS finales:");
    Serial.print("ax_offset = "); Serial.println(ax_offset);
    Serial.print("ay_offset = "); Serial.println(ay_offset);
    Serial.print("az_offset = "); Serial.println(az_offset);
    Serial.print("gx_offset = "); Serial.println(gx_offset);
    Serial.print("gy_offset = "); Serial.println(gy_offset);
    Serial.print("gz_offset = "); Serial.println(gz_offset);

    Serial.println("\nCopia estos valores en tu sketch DMP como:");
    Serial.println("mpu.setXAccelOffset(ax_offset);");
    Serial.println("mpu.setYAccelOffset(ay_offset);");
    Serial.println("mpu.setZAccelOffset(az_offset);");
    Serial.println("mpu.setXGyroOffset(gx_offset);");
    Serial.println("mpu.setYGyroOffset(gy_offset);");
    Serial.println("mpu.setZGyroOffset(gz_offset);");

    while (1);  // terminar
  }
}

// ---------- FUNCIONES AUXILIARES ----------

void meansensors() {
  long i = 0;
  long buff_ax = 0, buff_ay = 0, buff_az = 0;
  long buff_gx = 0, buff_gy = 0, buff_gz = 0;

  while (i < (buffersize + 101)) {
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // descartamos las primeras 100 lecturas
    if (i > 100 && i <= (buffersize + 100)) {
      buff_ax += ax;
      buff_ay += ay;
      buff_az += az;
      buff_gx += gx;
      buff_gy += gy;
      buff_gz += gz;
    }

    if (i == (buffersize + 100)) {
      mean_ax = buff_ax / buffersize;
      mean_ay = buff_ay / buffersize;
      mean_az = buff_az / buffersize;
      mean_gx = buff_gx / buffersize;
      mean_gy = buff_gy / buffersize;
      mean_gz = buff_gz / buffersize;
    }

    i++;
    delay(2);
  }
}

void calibration() {
  ax_offset = -mean_ax / 8;
  ay_offset = -mean_ay / 8;
  az_offset = (16384 - mean_az) / 8;

  gx_offset = -mean_gx / 4;
  gy_offset = -mean_gy / 4;
  gz_offset = -mean_gz / 4;

  while (1) {
    int ready = 0;

    accelgyro.setXAccelOffset(ax_offset);
    accelgyro.setYAccelOffset(ay_offset);
    accelgyro.setZAccelOffset(az_offset);
    accelgyro.setXGyroOffset(gx_offset);
    accelgyro.setYGyroOffset(gy_offset);
    accelgyro.setZGyroOffset(gz_offset);

    meansensors();
    Serial.println("Calibrando...");

    if (abs(mean_ax) <= acel_deadzone) ready++;
    else ax_offset = ax_offset - mean_ax / acel_deadzone;

    if (abs(mean_ay) <= acel_deadzone) ready++;
    else ay_offset = ay_offset - mean_ay / acel_deadzone;

    if (abs(16384 - mean_az) <= acel_deadzone) ready++;
    else az_offset = az_offset + (16384 - mean_az) / acel_deadzone;

    if (abs(mean_gx) <= giro_deadzone) ready++;
    else gx_offset = gx_offset - mean_gx / (giro_deadzone + 1);

    if (abs(mean_gy) <= giro_deadzone) ready++;
    else gy_offset = gy_offset - mean_gy / (giro_deadzone + 1);

    if (abs(mean_gz) <= giro_deadzone) ready++;
    else gz_offset = gz_offset - mean_gz / (giro_deadzone + 1);

    if (ready == 6) break;  // todo dentro de los márgenes
  }
}
