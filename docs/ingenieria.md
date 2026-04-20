# ⚙️ Decisiones de ingeniería – CANSAT AURORA

## ⚙️ INGENIERÍA

- Arquitectura modular  
- Fusión de sensores  
- Telemetría en tiempo real  

---

## 📌 Visión general

El diseño del CANSAT AURORA se basa en principios de ingeniería orientados a la robustez, modularidad y fiabilidad, siguiendo enfoques utilizados en sistemas aeroespaciales reales.

Las decisiones adoptadas permiten:

- Separar responsabilidades entre subsistemas  
- Aumentar la tolerancia a fallos  
- Facilitar el desarrollo, pruebas y mantenimiento  
- Garantizar la integridad de los datos de misión  

---

## 🧩 Arquitectura modular

El sistema se divide en módulos independientes:

- **Control principal (Heltec ESP32-S3)**  
  Gestión de sensores, lógica de misión y comunicaciones  

- **Módulo de cámara (ESP32-CAM)**  
  Captura y almacenamiento de imágenes  

- **Subsistemas auxiliares**  
  Sensores, almacenamiento, señalización (LED, buzzer)  

### Ventajas

- Aislamiento de fallos  
- Desarrollo paralelo de subsistemas  
- Mayor escalabilidad  
- Facilidad de depuración  

---

## 🧠 Fusión de sensores

El sistema combina datos de múltiples sensores para mejorar la fiabilidad de la detección de eventos.

### Sensores implicados

- BMP280 → presión y altitud  
- MPU6050 → aceleración y rotación  
- GPS → posición y altitud absoluta  
- Sistema acústico → detección de impacto  

### Aplicaciones

- Detección de lanzamiento (LIFTOFF)  
- Identificación de apogeo  
- Confirmación de impacto y aterrizaje  

### Beneficios

- Reducción de falsos positivos  
- Mayor precisión en eventos críticos  
- Validación cruzada de datos  

---

## 📡 Telemetría en tiempo real

El sistema transmite datos continuamente durante la misión mediante LoRa.

### Características

- Frecuencia: 1 Hz  
- Banda: 868 MHz (EU ISM)  
- Comunicación punto a punto  

### Función

- Monitorización en tiempo real desde estación de tierra  
- Detección inmediata de estados de misión  
- Registro paralelo a la microSD  

---

## 💾 Redundancia de datos

Se implementa un sistema redundante de almacenamiento:

- **LoRa** → transmisión en tiempo real  
- **microSD** → almacenamiento local completo  

### Ventajas

- Recuperación de datos en caso de pérdida de señal  
- Validación posterior de telemetría  
- Análisis completo de la misión  

---

## 🔄 Integración del sistema

Los distintos módulos trabajan de forma coordinada mediante:

- Máquina de estados (FSM)  
- Comunicación UART entre control y cámara  
- Sincronización temporal de datos  
- Registro estructurado en CSV  

---

## 🧪 Filosofía de diseño

El sistema se ha desarrollado bajo los siguientes principios:

- Simplicidad funcional  
- Robustez ante fallos  
- Modularidad  
- Reproducibilidad educativa  
- Inspiración en ingeniería aeroespacial (ESA / NewSpace)  

---

## 🎯 Objetivo de las decisiones de ingeniería

- Garantizar el funcionamiento autónomo del CANSAT  
- Maximizar la fiabilidad de los datos recogidos  
- Facilitar el análisis científico posterior  
- Permitir la replicabilidad del proyecto en entorno educativo  

---

## 📄 ➡️ Decisiones de ingeniería

Este apartado recoge los criterios técnicos que han guiado el diseño del CANSAT AURORA, integrando hardware, software y metodología de misión en un sistema coherente y robusto.
