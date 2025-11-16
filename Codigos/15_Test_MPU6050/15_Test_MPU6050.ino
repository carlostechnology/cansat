/***************************************************
  Proyecto: CANSAT - Test del sensor MPU6050 (IMU)
  Versión: v1.0 - 27/10/2025
  Autor: Equipo CANSAT
  Centro: IES Gonzalo Torrente Ballester (Salamanca)

  Descripción:
  Detección y lectura básica del MPU6050 por I2C.
  Muestra en el Monitor Serie las lecturas directas
  del acelerómetro (m/s²) y del giroscopio (rad/s),
  sin aplicar filtros ni conversión a ángulos.
  Sirve como prueba de montaje antes de usar
  algoritmos de orientación más avanzados.

  Licencia: MIT
***************************************************/

/*
  ESP32 + MPU6050 (GY-521, I2C) – Test de comprobación

  Conexiones (según módulo GY-521):

    MPU6050   ->   ESP32
    -------------------------
    VCC       ->   3V3
    GND       ->   GND
    SDA       ->   GPIO21
    SCL       ->   GPIO22
    ADO       ->   GND    (dirección I2C 0x68)
    INT       ->   Sin conectar
    XCL       ->   No conectar
    XDA       ->   No conectar

  Notas:
    - El bus I2C del ESP32 usará SDA=21 y SCL=22.
    - El sensor devuelve aceleración en m/s² y
      velocidad angular en rad/s.
    - Con la placa quieta, la norma del vector
      aceleración debe ser ≈ 9.81 m/s² (1 g).
    - Este ejemplo es el equivalente a "valores RAW"
      del artículo de orientación con IMU.
  
  Librerías necesarias (Administrador de Bibliotecas):
    - Adafruit MPU6050
    - Adafruit Unified Sensor
*/

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// Objeto global para el MPU6050
Adafruit_MPU6050 mpu;

void setup() {
  // Monitor serie para ver los datos
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println(F("=== CANSAT - Test básico MPU6050 (ESP32) ==="));

  // Inicializar bus I2C en los pines del ESP32
  Wire.begin(21, 22);

  // Intentar iniciar el MPU6050 en la dirección 0x68 (ADO a GND)
  if (!mpu.begin(0x68)) {
    Serial.println(F("ERROR: No se detecta el MPU6050 en 0x68."));
    Serial.println(F("Revisa conexiones: VCC=3V3, GND, SDA=21, SCL=22, ADO=GND."));
    while (1) {
      delay(1000);  // Bucle infinito si falla (no seguimos sin IMU)
    }
  }

  Serial.println(F("OK: MPU6050 detectado correctamente."));

  // Configurar rangos de medida (adecuados para CANSAT)
  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);       // ±4 g
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);            // ±500 º/s

  Serial.println(F("Rangos configurados: Acel ±4g, Gyro ±500 º/s."));
  Serial.println(F("Mostrando lecturas básicas cada 200 ms...\n"));
}

void loop() {
  // Estructuras para guardar las lecturas
  sensors_event_t accelEvent;
  sensors_event_t gyroEvent;
  sensors_event_t tempEvent;

  // Obtener una muestra del MPU6050
  mpu.getEvent(&accelEvent, &gyroEvent, &tempEvent);

  // Acelerómetro: valores en m/s²
  Serial.print(F("Ax = "));
  Serial.print(accelEvent.acceleration.x, 2);
  Serial.print(F("  Ay = "));
  Serial.print(accelEvent.acceleration.y, 2);
  Serial.print(F("  Az = "));
  Serial.print(accelEvent.acceleration.z, 2);

  // Giroscopio: valores en rad/s
  Serial.print(F("  |  Gx = "));
  Serial.print(gyroEvent.gyro.x, 3);
  Serial.print(F("  Gy = "));
  Serial.print(gyroEvent.gyro.y, 3);
  Serial.print(F("  Gz = "));
  Serial.print(gyroEvent.gyro.z, 3);

  // Temperatura interna del IMU (opcional, solo informativa)
  Serial.print(F("  |  T_IMU = "));
  Serial.print(tempEvent.temperature, 1);
  Serial.println(F(" °C"));

  delay(200);  // Periodo de muestreo: ~5 Hz
}
