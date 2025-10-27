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

#include <WiFi.h>

const char* ssid     = "TU_SSID";     // Nombre de tu red WiFi
const char* password = "TU_PASSWORD"; // Contraseña de tu WiFi

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Conectando...");
  }

  Serial.println();
  Serial.println("WiFi conectado");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Nada en el bucle, solo comprobamos conexión
}
