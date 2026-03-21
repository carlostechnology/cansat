# 🛰️ CANSAT AURORA — TORREX SPACE

Proyecto educativo-tecnológico basado en el programa CanSat de la Agencia Espacial Europea (ESA), desarrollado por el equipo **TORREX Space** del IES Gonzalo Torrente Ballester (Salamanca).

Simula una misión aeroespacial real a escala, integrando diseño de sistemas, electrónica, programación, comunicaciones y análisis de datos.

---

## 🎯 MISIÓN

### 🟢 Misión primaria
- Medición de presión atmosférica
- Medición de temperatura
- Cálculo de altitud
- Transmisión de datos en tiempo real

### 🔵 Misión secundaria
- Análisis dinámico del descenso mediante IMU
- Geolocalización GPS
- Registro en microSD
- Interpretación del vuelo mediante sensores

---

## 🧠 ARQUITECTURA DEL SISTEMA

Sensores → ESP32 → Procesamiento → LoRa → Estación tierra → Análisis

📄 ➡️ [Arquitectura del sistema](docs/arquitectura.md)

---

## 📡 TELEMETRÍA

- Transmisión en tiempo real
- Registro en CSV
- Análisis posterior

📄 ➡️ [Sistema de telemetría](docs/telemetria.md)

---

## 🧰 HARDWARE

- Heltec WiFi LoRa 32 V3 (ESP32-S3)
- BMP280 (presión y temperatura)
- MPU6050 (IMU)
- GPS NEO-6M
- microSD
- LED y buzzer

📄 ➡️ [Componentes](hardware/componentes.md)

---

## 🚀 ESTADOS DEL SISTEMA

1. INIT  
2. ASCENT  
3. DESCENT  
4. LANDING  

📄 ➡️ [Misión AURORA](mission/aurora/mission.md)

---

## 📊 VALIDACIÓN DE DATOS

- Relación presión-altitud
- Comparación con modelos reales

📄 ➡️ [Validación](docs/validacion.md)

---

## ⚙️ INGENIERÍA

- Arquitectura modular
- Fusión de sensores
- Telemetría en tiempo real

📄 ➡️ [Decisiones de ingeniería](docs/ingenieria.md)

---

# 💻 CÓDIGOS DEL PROYECTO

El proyecto incluye una colección completa de códigos organizados por módulos.

### 🔹 Ejemplos principales

| Nº | Nombre | Descripción | Código |
|---:|--------|-------------|:------:|
| 01 | Test LED | Verificación básica del sistema | [📄 Ver](Codigos/01_Test_LED/01_Test_LED.ino) |
| 02 | BMP280 | Presión y temperatura | [📄 Ver](Codigos/02_BMP280/02_BMP280.ino) |
| 03 | MPU6050 | Aceleración y orientación | [📄 Ver](Codigos/03_MPU6050/03_MPU6050.ino) |
| 04 | GPS | Posicionamiento | [📄 Ver](Codigos/04_GPS/04_GPS.ino) |
| 05 | LoRa | Comunicación inalámbrica | [📄 Ver](Codigos/05_LoRa/05_LoRa.ino) |

---

### 📂 Ver todos los códigos (40+)

➡️ [Listado completo de códigos](docs/codigos.md)

---

### 🧠 Organización del software

- Sensores → `software/sensores/`
- Telemetría → `software/telemetria/`
- Comunicaciones → `software/comunicaciones/`
- Estados → `software/estados/`

---

## 🛠️ CÓMO REPRODUCIR EL PROYECTO

1. Montar hardware → `hardware/`
2. Cargar código → `software/`
3. Ejecutar sistema
4. Recibir telemetría
5. Analizar datos

---

## 📚 TUTORIALES Y GUÍAS

➡️ https://tecnologiaingenieria.wordpress.com/tag/CANSAT/

---

## 👨‍🏫 CONTEXTO EDUCATIVO

Proyecto basado en:

- Aprendizaje basado en proyectos
- Simulación de ingeniería real
- Trabajo interdisciplinar

---

## 🤝 CONTRIBUCIONES

Puedes contribuir:

- Mejorando documentación
- Añadiendo código
- Proponiendo mejoras

---

## 📄 LICENCIA

MIT License — Uso libre con atribución educativa

---

## 🚀 FILOSOFÍA DEL PROYECTO

Este proyecto no es solo un CanSat.

Es una simulación completa de una misión aeroespacial:

- Diseño de sistemas
- Integración hardware/software
- Validación experimental
- Documentación técnica

---

## 👨‍🚀 EQUIPO

**TORREX SPACE**  
IES Gonzalo Torrente Ballester (Santa Marta de Tormes, Salamanca)
