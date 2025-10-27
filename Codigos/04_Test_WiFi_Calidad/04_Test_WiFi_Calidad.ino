/***************************************************
  Proyecto: CANSAT - Medir calidad WiFi (RSSI)
  Versión: v1.0 - 27/10/2025
  Autor: Equipo CANSAT
  Centro: IES Gonzalo Torrente Ballester (Salamanca)

  Descripción:
  Muestra intensidad de señal WiFi (RSSI) al conectarse.

  Licencia: MIT
***************************************************/

#include <WiFi.h>

const char* ssid = "TU_SSID";
const char* password = "TU_PASSWORD";

void setup() {
  Serial.begin(115200);
  Serial.println("Conectando a WiFi...");

  WiFi.begin(ssid, password);

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 20) {
    delay(500);
    Serial.print(".");
    intentos++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nNo se pudo conectar. Revisa SSID/clave o red 2.4 GHz.");
  }
}

void loop() {
  // Nada; pruebas hechas en setup().
}

