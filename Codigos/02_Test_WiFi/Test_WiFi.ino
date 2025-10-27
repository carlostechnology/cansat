/***************************************************
  Proyecto: CANSAT - Test de conectividad WiFi en ESP32
  Versión: v1.0 - 27/10/2025
  Autor: Equipo CANSAT
  Centro: IES Gonzalo Torrente Ballester (Salamanca)

  Descripción:
  Escanea redes WiFi cercanas e imprime en el Monitor Serial
  el nombre de la red (SSID), la intensidad de señal (RSSI),
  y si está abierta o encriptada. Comprueba el correcto
  funcionamiento del módulo WiFi del ESP32.

  Hardware:
  - ESP32-WROOM-32 (WiFi integrado 2.4 GHz)

  Objetivo educativo:
  Verificar la conectividad inalámbrica del ESP32 y la
  funcionalidad de la interfaz de comunicación serie.

  Licencia: MIT
***************************************************/

#include "WiFi.h"

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando escaneo WiFi...");
  
  // Inicia WiFi en modo estación (como un dispositivo normal)
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
}

void loop() {
  Serial.println("Escaneando...");
  int n = WiFi.scanNetworks();
  
  if (n == 0) {
    Serial.println("No se encontraron redes :(");
  } else {
    Serial.println("Redes encontradas:");
    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));         // Nombre de la red
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));         // Intensidad de señal
      Serial.print(" dBm) ");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "Abierta" : "Encriptada");
      delay(10);
    }
  }
  Serial.println("");

  // Repetir escaneo cada 5 segundos
  delay(5000);
}
