# 🧠 Arquitectura del sistema – CANSAT AURORA

## 📌 Visión general

El sistema CANSAT AURORA está diseñado como una arquitectura distribuida basada en dos microcontroladores ESP32-S3, encargados de adquisición de datos, procesamiento, almacenamiento y comunicación en tiempo real.

Flujo principal del sistema:

Sensores → ESP32 (Heltec) → Procesamiento → LoRa → Estación de tierra → Análisis

---

## 🧩 Componentes principales

### 🔹 Controlador principal
- **Heltec WiFi LoRa 32 V3 (ESP32-S3)**
  - Lectura de sensores
  - Procesamiento de datos
  - Detección de eventos de misión
  - Transmisión LoRa
  - Registro en microSD
  - Comunicación con la cámara

---

### 🔹 Módulo de cámara
- **Freenove ESP32-S3 CAM (OV5640)**
  - Captura de imágenes en vuelo
  - Almacenamiento en microSD independiente
  - Comunicación por UART con Heltec

---

### 🔹 Sensores

- **BMP280**
  - Temperatura
  - Presión atmosférica
  - Altitud estimada

- **MPU6050**
  - Aceleración (X, Y, Z)
  - Velocidad angular (giroscopio)

- **GPS NEO-6M V2**
  - Posición (latitud, longitud)
  - Altitud GPS
  - Número de satélites

---

### 🔹 Otros componentes

- **MicroSD**
  - Registro local de datos (backup de misión)

- **Buzzer**
  - Señalización acústica de eventos

- **LEDs WS2812**
  - Indicadores visuales de estado

---

### 🔗 Interconexión del sistema

- **BMP280**, **MPU6050** y **GPS NEO-6M V2** envían datos a la **Heltec ESP32-S3**
- La **Heltec ESP32-S3**:
  - transmite telemetría por **LoRa** hacia la **estación de tierra**
  - guarda telemetría y eventos en la **microSD**
  - se comunica por **UART** con la **ESP32-CAM**
  - ejecuta el **control de misión** y la detección de eventos
- La **ESP32-CAM**:
  - captura imágenes durante la misión
  - guarda las imágenes en su **microSD**

### ⚙️ Funcionamiento del sistema

- Los sensores capturan datos continuamente  
- La Heltec procesa la información en tiempo real  
- Se detectan eventos de misión (lanzamiento, apogeo, impacto, etc.)  
- Los datos se:
  - Transmiten por LoRa (tiempo real)  
  - Guardan en microSD (backup)  
- La ESP32-CAM captura imágenes durante el descenso  
- La estación de tierra recibe la telemetría  

---

### 🚀 Filosofía de diseño

- Sistema redundante (LoRa + microSD)  
- Arquitectura modular  
- Separación de funciones (control vs cámara)  
- Diseño educativo y replicable  
- Inspirado en sistemas reales (ESA / NewSpace)  

---

### 🧪 Ventajas de la arquitectura

- Alta fiabilidad  
- Tolerancia a fallos de comunicación  
- Registro completo de misión  
- Fácil análisis posterior  
- Escalable y ampliable  
