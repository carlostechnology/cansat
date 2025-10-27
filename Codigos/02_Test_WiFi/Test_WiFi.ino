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

#include "WiFi.h" // Biblioteca para manejar WiFi en ESP32

// ------------------- Configuración -------------------
void setup() {
  Serial.begin(115200); // Activar monitor serie para ver resultados
  Serial.println("Iniciando escaneo WiFi...");

  WiFi.mode(WIFI_STA); // Modo estación (cliente WiFi)
  WiFi.disconnect();   // Desconectar de cualquier red previa
  delay(100);          // Pequeña espera para estabilidad
}

// ------------------- Bucle principal -------------------
void loop() {
  Serial.println("Escaneando...");
  int n = WiFi.scanNetworks(); // Nº de redes encontradas
  
  if (n == 0) {
    Serial.println("No se encontraron redes :(");
  } else {
    Serial.println("Redes encontradas:");
    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i)); // Nombre de la red
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i)); // Intensidad de señal (dBm)
      Serial.print(" dBm) ");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "Abierta" : "Encriptada");
      delay(10); // Pausa breve para evitar saturar el monitor serie
    }
  }

  Serial.println("");
  delay(5000); // Repetir escaneo cada 5 s
}
