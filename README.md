# 🚀 Proyecto CANSAT — IES Gonzalo Torrente Ballester

CANSAT educativo con **ESP32**: un satélite del tamaño de una lata, capaz de
medir datos reales de **presión, temperatura, altitud, orientación**… y enviar
telemetría 🔥

Este repositorio incluye todos los **códigos Arduino IDE**, esquemas y
documentación práctica utilizados por los alumnos del **IES Gonzalo Torrente
Ballester** (Santa Marta de Tormes, Salamanca).

---

## 📚 Tutoriales del proyecto (WordPress)

Explicaciones paso a paso en el blog:

🔹 **Introducción al proyecto CANSAT: lanzamiento de satélites**  
https://tecnologiaingenieria.wordpress.com/2025/10/10/cansat-lanzamiento-de-satelites/

🔹 **ESP32: LED, WiFi y servidor web**  
https://tecnologiaingenieria.wordpress.com/2025/09/30/esp32-led-wifi-y-servidor-web/

🔹 **Sensores BMP280: presión, temperatura y altitud en ESP32**  
https://tecnologiaingenieria.wordpress.com/2025/10/11/cansat-esp32-presion-temperatura-y-altitud/

✍️ Próximamente más: MPU6050, microSD, LoRa, telemetría completa…

---

## 📂 Códigos Arduino IDE del proyecto

| Nº | Nombre | Descripción | Código |
|---:|--------|-------------|:------:|
| 01 | Test LED | Comprobación GPIO25 del ESP32 | [📄 Ver](Codigos/01_Test_LED/01_Test_LED.ino) |
| 02 | WiFi Escaneo | Escaneo de redes cercanas | [📄 Ver](Codigos/02_WiFi_Escaneo/02_WiFi_Escaneo.ino) |
| 03 | WiFi Conexión | Conexión a una red WiFi y obtención de IP | [📄 Ver](Codigos/03_WiFi_Conexion/03_WiFi_Conexion.ino) |
| 04 | WiFi Calidad señal | Intensidad de señal (RSSI) | [📄 Ver](Codigos/04_WiFi_Calidad/04_WiFi_Calidad.ino) |
| 05 | WiFi Velocidad descarga | Medición de velocidad HTTP | [📄 Ver](Codigos/05_WiFi_VelocidadDescarga/05_WiFi_VelocidadDescarga.ino) |
| 06 | WiFi AP (Punto de Acceso) | La ESP32 crea una red WiFi propia | [📄 Ver](Codigos/06_WiFi_PuntoAcceso/06_WiFi_PuntoAcceso.ino) |
| 07 | WiFi Servidor Web ON/OFF | Control LED desde navegador | [📄 Ver](Codigos/07_WiFi_ServidorWeb_ON_OFF/07_WiFi_ServidorWeb_ON_OFF.ino) |
| 08 | WiFi Servidor Web UI mejorada | Interfaz visual moderna | [📄 Ver](Codigos/08_WiFi_ServidorWeb_UI/08_WiFi_ServidorWeb_UI.ino) |
| 09 | WiFi Servidor Web + usabilidad | AJAX + estado en tiempo real | [📄 Ver](Codigos/09_WiFi_ServidorWeb_UI_Usabilidad/09_WiFi_ServidorWeb_UI_Usabilidad.ino) |
| 10 | Test BMP280 | Presión y temperatura | 🔒 Pendiente |
| 11 | Test MPU6050 | Inclinación y aceleraciones | 🔒 Pendiente |
| 12 | Test microSD | Guardar datos en memoria | 🔒 Pendiente |
| 13 | Test LoRa | Telemetría en tiempo real | 🔒 Pendiente |
| 14 | Telemetría completa | Todos los módulos integrados | 🔒 Pendiente |


🛠 Al ir subiendo cada código, activaremos los enlaces directamente aquí ✅

---

## 🧪 Hardware del CANSAT

- ESP32-WROOM-32
- Sensor BMP280 (presión/temperatura)
- Sensor MPU6050 (IMU)
- Módulo LoRa SX1278
- Tarjeta microSD + lector
- Batería y electrónica de soporte
- Estructura tipo lata de refresco

---

## 🌍 ¿Qué es un CANSAT?

> Un proyecto donde construimos un “satélite” real que se lanza en un vuelo
> simulado y envía datos durante su descenso.  
> Ciencia y tecnología **con las manos en la masa** 🛰️

---

## 👨‍🏫 Liderado por

**Carlos Sánchez Barbero**  
Coordinador TIC · Profesor de Tecnología  
IES Gonzalo Torrente Ballester — Salamanca

Blog: https://tecnologiaingenieria.wordpress.com  
Twitter/X: @carloseducacyl  

---

## 📜 Licencia

📌 MIT License (uso permitido incluso comercial con atribución)

---

## ⭐ Contribución

¡Los alumnos irán subiendo mejoras!  
Si eres docente o maker y te interesa colaborar → **abre un Issue** 🤝

## 📜 Licencia

Este repositorio se publica bajo licencia **MIT** para facilitar su uso educativo.

