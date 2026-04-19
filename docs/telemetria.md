# 📡 Sistema de telemetría – CANSAT AURORA

## 📡 TELEMETRÍA

- Transmisión en tiempo real  
- Registro en CSV  
- Análisis posterior  

---

## 📌 Visión general

El sistema de telemetría del CANSAT AURORA permite monitorizar el vuelo en tiempo real y garantizar el registro completo de todos los datos para su análisis posterior.

Se basa en dos pilares fundamentales:
- Comunicación inalámbrica mediante LoRa  
- Almacenamiento local en microSD  

---

## 📶 Transmisión en tiempo real

- Tecnología: **LoRa (868 MHz)**  
- Frecuencia asignada: **863.950 MHz**  
- Frecuencia de envío: **1 Hz (1 paquete por segundo)**  

Cada segundo, el sistema envía un paquete con el estado completo del CANSAT.

---

## 📊 Datos transmitidos

La telemetría incluye:

- Tiempo de misión  
- Temperatura (°C)  
- Presión (hPa)  
- Altitud (m)  
- Aceleración (X, Y, Z)  
- Velocidad angular (giroscopio)  
- Datos GPS:
  - Latitud  
  - Longitud  
  - Altitud  
  - Número de satélites  
- Estado de la misión  
- Eventos detectados  
- CRC16 (control de errores)  

---

## 🧾 Ejemplo de trama LoRa

T:12345;TEMP:22.5;P:1013.2;ALT:120.3;AX:0.02;AY:-0.01;AZ:9.81;GPS:40.97,-5.66;EV:APOGEE;CRC:AB12

---

## 🛰️ Estación de tierra
- Recepción mediante Heltec LoRa RX
- Lectura de datos con CoolTerm
- Visualización en tiempo real
- Guardado de datos recibidos

---

## 🔄 Flujo completo de datos
- Sensores → ESP32 → Procesamiento → LoRa → Estación de tierra → CSV → Análisis  
- Backup paralelo: microSD (registro completo de la misión)

---

## 🧠 Robustez del sistema
- Uso de CRC16 para detección de errores
- Posibles pérdidas de paquetes en LoRa
- Redundancia mediante almacenamiento en microSD
- Reconstrucción completa de la misión tras el vuelo

---

## 📈 Análisis posterior

Tras la misión:

- Se descargan los datos de la microSD
- Se comparan con los datos recibidos en tierra
- Se generan gráficas:
- Altitud vs tiempo
- Temperatura vs altitud
- Eventos de misión
- Se reconstruye el perfil completo del vuelo

---

## 🎯 Objetivo del sistema
- Monitorizar el vuelo en tiempo real
- Garantizar el registro completo de datos
- Facilitar análisis científico posterior
- Detectar eventos automáticamente

---

## 📄 ➡️ Sistema de telemetría

Este documento forma parte de la documentación técnica del proyecto CANSAT AURORA.
