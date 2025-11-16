/***************************************************
  Proyecto: CANSAT - MPU6050 (fusión de sensores)
  Versión: v1.0 - 15/11/2025
  Autor: Equipo CANSAT
  Centro: IES Gonzalo Torrente Ballester (Salamanca)

  Descripción:
  Cálculo de los ángulos de orientación (roll y pitch)
  utilizando un Filtro Complementario que combina:

     - Acelerómetro  → estable pero ruidoso en movimiento
     - Giroscopio    → rápido pero con deriva con el tiempo

  El filtro complementario consigue una orientación
  más suave, estable y útil en aplicaciones reales,
  como el seguimiento de actitud durante el vuelo del
  CANSAT.

  Licencia: MIT
***************************************************/

/*
  ESP32 + MPU6050 (GY-521, I2C) – Filtro complementario

  Conexiones (GY-521 → ESP32):
    VCC   -> 3V3
    GND   -> GND
    SDA   -> GPIO21
    SCL   -> GPIO22
    ADO   -> GND (dirección 0x68)
    INT   -> Sin conectar
    XCL   -> No conectar
    XDA   -> No conectar

  Notas:
    - El giroscopio mide velocidad angular. Integrando su
      valor se obtiene el ángulo… pero acumula deriva.
    - El acelerómetro da un ángulo aproximado usando la
      gravedad, pero sufre ruido cuando el sensor se mueve.
    - El filtro complementario combina ambos:
          ángulo = α*(ángulo + gyro*dt) + (1-α)*acc
    - Para CANSAT, α entre 0.95 y 0.99 funciona muy bien.

  Librerías necesarias:
    - Adafruit MPU6050
    - Adafruit Unified Sensor
*/

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

// Ángulos calculados por el filtro
float angleRoll  = 0.0;
float anglePitch = 0.0;

// Peso del filtro (0–1). Cuanto mayor, más peso tiene el giroscopio.
const float ALPHA = 0.98;

unsigned long lastMillis = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println(F("=== CANSAT - Filtro Complementario (MPU6050) ==="));

  // Iniciar I2C en ESP32
  Wire.begin(21, 22);

  if (!mpu.begin(0x68)) {
    Serial.println(F("ERROR: No se detecta el MPU6050 en 0x68."));
    Serial.println(F("Comprueba conexiones SDA=21, SCL=22, ADO=GND."));
    while (1) delay(1000);
  }

  Serial.println(F("OK: MPU6050 detectado."));
  Serial.println(F("Ejecutando filtro complementario...\n"));

  // Configurar rangos
  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);

  lastMillis = millis();
}

void loop() {
  unsigned long now = millis();
  float dt = (now - lastMillis) / 1000.0;  // tiempo en segundos
  lastMillis = now;

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // --- Lecturas del acelerómetro ---
  float ax = a.acceleration.x;
  float ay = a.acceleration.y;
  float az = a.acceleration.z;

  // Ángulos estimados SOLO con acelerómetro
  float accRoll  = atan2(ay, az) * 180.0 / PI;
  float accPitch = atan2(-ax, sqrt(ay*ay + az*az)) * 180.0 / PI;

  // --- Lecturas del giroscopio ---
  // rad/s → grados/s
  float gyroX = g.gyro.x * 180.0 / PI;
  float gyroY = g.gyro.y * 180.0 / PI;

  // Integración del giroscopio
  angleRoll  += gyroX * dt;
  anglePitch += gyroY * dt;

  // --- Filtro complementario ---
  angleRoll  = ALPHA * angleRoll  + (1.0 - ALPHA) * accRoll;
  anglePitch = ALPHA * anglePitch + (1.0 - ALPHA) * accPitch;

  // Mostrar resultado final (más suave y estable)
  Serial.print("roll = ");
  Serial.print(angleRoll, 1);
  Serial.print(" º   pitch = ");
  Serial.print(anglePitch, 1);
  Serial.println(" º");

  delay(10);  // ~100 Hz
}

