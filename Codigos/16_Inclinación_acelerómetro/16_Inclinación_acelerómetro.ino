/***************************************************
  Proyecto: CANSAT - MPU6050 (inclinación con acelerómetro)
  Versión: v1.0 - 15/11/2025
  Autor: Equipo CANSAT
  Centro: IES Gonzalo Torrente Ballester (Salamanca)

  Descripción:
  Cálculo de los ángulos de inclinación (roll y pitch)
  usando únicamente el acelerómetro del MPU6050.
  Los valores se muestran en grados en el Monitor Serie.

  Este ejemplo permite al alumnado visualizar cómo la
  gravedad sirve como "nivel" para estimar la orientación
  de la placa cuando está en reposo o con movimientos
  lentos.

  Licencia: MIT
***************************************************/

/*
  ESP32 + MPU6050 (GY-521, I2C) – Inclinación (roll y pitch)

  Conexiones (GY-521 → ESP32):

    VCC   -> 3V3
    GND   -> GND
    SDA   -> GPIO21
    SCL   -> GPIO22
    ADO   -> GND   (dirección I2C = 0x68)
    INT   -> Sin conectar
    XCL   -> No conectar
    XDA   -> No conectar

  Notas:
    - Solo usamos el acelerómetro para estimar los
      ángulos roll (giro sobre X) y pitch (giro sobre Y).
    - Funciona bien con el CANSAT quieto o moviéndose
      lentamente. Con movimientos bruscos aparecen errores,
      que se corrigen más adelante con filtros.

  Librerías necesarias:
    - Adafruit MPU6050
    - Adafruit Unified Sensor
*/

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println(F("=== CANSAT - Inclinación con acelerómetro (MPU6050) ==="));

  // Iniciar bus I2C del ESP32
  Wire.begin(21, 22);

  // Inicializar el MPU6050 en 0x68 (ADO a GND)
  if (!mpu.begin(0x68)) {
    Serial.println(F("ERROR: No se detecta el MPU6050 en 0x68."));
    Serial.println(F("Revisa VCC=3V3, GND, SDA=21, SCL=22, ADO=GND."));
    while (1) delay(1000);
  }

  Serial.println(F("OK: MPU6050 detectado."));

  // Rango del acelerómetro (no usamos el giroscopio en este ejemplo)
  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);

  Serial.println(F("Calculando roll y pitch solo con acelerómetro...\n"));
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Lecturas del acelerómetro (m/s²)
  float ax = a.acceleration.x;
  float ay = a.acceleration.y;
  float az = a.acceleration.z;

  // Cálculo de ángulos (aproximación clásica)
  // roll  = giro sobre eje X
  // pitch = giro sobre eje Y
  float roll  = atan2(ay, az) * 180.0 / PI;
  float pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / PI;

  Serial.print("roll  = ");
  Serial.print(roll, 1);
  Serial.print(" º\tpitch = ");
  Serial.print(pitch, 1);
  Serial.println(" º");

  delay(200);  // ~5 lecturas por segundo
}
