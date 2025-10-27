/***************************************************
  Proyecto: CANSAT - Medición de velocidad de descarga WiFi
  Versión: v1.0 - 27/10/2025
  Autor: Equipo CANSAT
  Centro: IES Gonzalo Torrente Ballester (Salamanca)

  Descripción:
  Conecta a la red WiFi (2.4GHz), realiza ping al router y a
  servidores externos, y mide la velocidad aproximada de
  descarga HTTP. Útil para diagnosticar la calidad de la red
  en diferentes ubicaciones.

  Licencia: MIT
***************************************************/

#include <WiFi.h>
#include <HTTPClient.h>
// Si instalaste la librería ESP32Ping desde ZIP:
#include <ESP32Ping.h>

const char* ssid     = "TU_SSID";
const char* password = "TU_PASSWORD";

// URL ligera para medir descarga (puedes cambiarla por otra)
const char* testUrl = "http://example.com/";

void conectarWiFi() {
  Serial.println("\nConectando a WiFi (solo 2.4 GHz)...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 40) { // ~20 s
    delay(500);
    Serial.print(".");
    intentos++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi conectado");
    Serial.print("IP: ");   Serial.println(WiFi.localIP());
    Serial.print("RSSI: "); Serial.print(WiFi.RSSI()); Serial.println(" dBm");
  } else {
    Serial.println("\n❌ No se pudo conectar. Revisa SSID/clave o usa la banda 2.4 GHz.");
  }
}

void pruebaPing() {
  if (WiFi.status() != WL_CONNECTED) return;

  IPAddress gateway = WiFi.gatewayIP();
  Serial.print("\nHaciendo ping al router "); Serial.println(gateway);

  bool okGw = Ping.ping(gateway, 5);   // 5 intentos
  if (okGw) {
    Serial.print("Ping router OK. Media: ");
    Serial.print(Ping.averageTime()); Serial.println(" ms");
  } else {
    Serial.println("Ping router FALLÓ");
  }

  Serial.println("Haciendo ping a 8.8.8.8 (Google DNS)...");
  bool okG = Ping.ping("8.8.8.8", 5);
  if (okG) {
    Serial.print("Ping 8.8.8.8 OK. Media: ");
    Serial.print(Ping.averageTime()); Serial.println(" ms");
  } else {
    Serial.println("Ping 8.8.8.8 FALLÓ");
  }
}

void pruebaDescarga() {
  if (WiFi.status() != WL_CONNECTED) return;

  Serial.print("\nDescargando: "); Serial.println(testUrl);

  HTTPClient http;
  http.begin(testUrl);
  uint32_t t0 = millis();
  int code = http.GET();
  uint32_t t1 = millis();

  if (code > 0) {
    String payload = http.getString();          // Ojo: guarda RAM; para pruebas vale
    size_t bytes = payload.length();
    float seg = (t1 - t0) / 1000.0;
    float kBps = (bytes / 1024.0) / seg;

    Serial.printf("HTTP %d | %u bytes en %.2f s -> %.2f kB/s\n",
                  code, (unsigned)bytes, seg, kBps);
  } else {
    Serial.printf("Error HTTP: %d\n", code);
  }
  http.end();
}

void setup() {
  Serial.begin(115200);
  delay(500);
  conectarWiFi();
  pruebaPing();
  pruebaDescarga();
}

void loop() {
  // Nada; pruebas hechas en setup().
}
