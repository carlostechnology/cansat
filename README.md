# 🚀 Proyecto CANSAT — IES Gonzalo Torrente Ballester

CANSAT educativo con **ESP32**: un satélite del tamaño de una lata
capaz de medir datos reales de **presión, temperatura, altitud,
orientación y telemetría** en un vuelo simulado 🌍🛰️

Este repositorio incluye todos los **programas Arduino para ESP32**, esquemas
y documentación práctica utilizados por los alumnos del
**IES Gonzalo Torrente Ballester** (Santa Marta de Tormes, Salamanca).

---

## 📚 Tutoriales del proyecto (WordPress)

Explicaciones paso a paso:

🔹 CANSAT: lanzamiento de satélites y objetivos del proyecto  
https://tecnologiaingenieria.wordpress.com/2025/10/10/cansat-lanzamiento-de-satelites/

🔹 ESP32: LED, WiFi y servidor web  
https://tecnologiaingenieria.wordpress.com/2025/09/30/esp32-led-wifi-y-servidor-web/

🔹 Sensor BMP280: presión, temperatura y altitud  
https://tecnologiaingenieria.wordpress.com/2025/10/11/cansat-esp32-presion-temperatura-y-altitud/

📌 Más tutoriales próximamente: MPU6050, LoRa, microSD, telemetría completa…

---

## 📂 Códigos Arduino IDE del proyecto

| Nº | Nombre | Descripción | Código |
|---:|--------|-------------|:------:|
| 01 | Test LED | Comprobación GPIO25 del ESP32 | [📄 Ver](Codigos/01_Test_LED/01_Test_LED.ino) |
| 02 | WiFi Escaneo | Escaneo de redes cercanas | [📄 Ver](Codigos/02_WiFi_Escaneo/02_WiFi_Escaneo.ino) |
| 03 | WiFi Conexión | Conexión a WiFi + IP local | [📄 Ver](Codigos/03_WiFi_Conexion/03_WiFi_Conexion.ino) |
| 04 | WiFi Calidad señal | Intensidad de señal (RSSI) | [📄 Ver](Codigos/04_WiFi_Calidad/04_WiFi_Calidad.ino) |
| 05 | WiFi Velocidad descarga | Test de descarga HTTP | [📄 Ver](Codigos/05_WiFi_VelocidadDescarga/05_WiFi_VelocidadDescarga.ino) |
| 06 | WiFi AP (Punto de Acceso) | La ESP32 crea su propia red WiFi | [📄 Ver](Codigos/06_WiFi_PuntoAcceso/06_WiFi_PuntoAcceso.ino) |
| 07 | WiFi Servidor Web ON/OFF | Control LED desde navegador | [📄 Ver](Codigos/07_WiFi_ServidorWeb_ON_OFF/07_WiFi_ServidorWeb_ON_OFF.ino) |
| 08 | WiFi Servidor Web UI mejorada | Interfaz visual moderna HTML/CSS | [📄 Ver](Codigos/08_WiFi_ServidorWeb_UI/08_WiFi_ServidorWeb_UI.ino) |
| 09 | WiFi Web UI + usabilidad | AJAX + estado real en JSON | [📄 Ver](Codigos/09_WiFi_ServidorWeb_UI_Usabilidad/09_WiFi_ServidorWeb_UI_Usabilidad.ino) |
| 10 | Test BMP280 | Temperatura, presión y altitud | [📄 Ver](Codigos/10_Test_BMP280/10_Test_BMP280.ino) |
| 11 | BMP280 CSV para Excel | Datos en formato tiempo-temp-pres-alt | [📄 Ver](Codigos/11_BMP280_CSV/11_BMP280_CSV.ino) |
| 12 | BMP280 CSV España | Salida con ; y coma decimal compatible Excel ES | [📄 Ver](Codigos/12_BMP280_CSV_ES/12_BMP280_CSV_ES.ino) |
| 13 | BMP280 + microSD CSV (España) | Registro de datos real en vuelo | [📄 Ver](Codigos/13_BMP280_SD_CSV_ES/13_BMP280_SD_CSV_ES.ino) |
| 14 | WiFi + BMP280 Telemetría web | Gráficas SVG en navegador, JSON /data | [📄 Ver](Codigos/14_WiFi_BMP280_TelemetriaWeb/14_WiFi_BMP280_TelemetriaWeb.ino) |
| X | Test LoRa SX1278 | Telemetría inalámbrica | 🔒 Pendiente |
| X | Telemetría completa | Todos los módulos integrados | 🔒 Pendiente |

📌 Próximas actualizaciones semanales ✅

---

## 🧪 Hardware del CANSAT

- ESP32-WROOM-32
- Sensor BMP280 (presión + temperatura)
- Sensor MPU6050 (IMU 6DoF)
- Módulo LoRa SX1278
- Tarjeta microSD + lector
- Batería y electrónica de soporte
- Estructura tipo lata de refresco

---

## 👨‍🚀 ¿Qué es un CANSAT?

Un “satélite” escolar que se lanza en un vuelo simulado y envía datos durante el descenso.  
Aprendizaje activo de electrónica, programación y física ✅

---

## 👨‍🏫 Proyecto coordinado por

**Carlos Sánchez Barbero**  
Profesor de Tecnología y Digitalización — Coordinador TIC  
IES Gonzalo Torrente Ballester (Salamanca)

Blog: https://tecnologiaingenieria.wordpress.com  
Twitter/X: @carloseducacyl

---

## 📝 Licencia

📌 MIT License — Uso libre con atribución educativa

---

## ⭐ Contribuciones

¡Este proyecto está abierto a colaboración!  
Si eres docente o maker y quieres participar → **abre un Issue** 🤝

