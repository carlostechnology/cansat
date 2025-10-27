/***************************************************
  Proyecto: CANSAT - Conexión a WiFi en ESP32
  Versión: v1.0 - 30/9/2025
  Autor: Equipo CANSAT
  Centro: IES Gonzalo Torrente Ballester (Salamanca)

  Descripción:
  Conecta el ESP32 a una red WiFi y muestra por el
  Monitor Serie si la conexión ha sido correcta y la
  dirección IP asignada.

  Hardware:
  - ESP32-WROOM-32 (WiFi integrado 2.4 GHz)

  Objetivo educativo:
  Configurar el ESP32 como cliente WiFi y verificar la
  conectividad local antes de usar servicios web o MQTT.

  Licencia: MIT
***************************************************/

#include <WiFi.h>  // Biblioteca de gestión WiFi en ESP32

// ------------------- Configuración WiFi -------------------
// ⚠️ Cambiar por los datos de tu red WiFi
const char* ssid     = "TU_SSID";     // Nombre de la red WiFi
const char* password = "TU_PASSWORD"; // Contraseña de la red

// ------------------- Configuración -------------------
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\nConectando a la red WiFi...");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // Intentar conexión

  // Esperar hasta estar conectado
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi conectado");
  Serial.print("Dirección IP asignada: ");
  Serial.println(WiFi.localIP());
}

// ------------------- Bucle principal -------------------
void loop() {
  // Nada aquí por ahora: solo conexión WiFi
  // Se usará más adelante para servidor web / MQTT
}
