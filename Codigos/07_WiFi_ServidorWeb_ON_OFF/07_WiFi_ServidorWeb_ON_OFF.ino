/***************************************************
  Proyecto: CANSAT - Servidor Web ON/OFF (ESP32)
  Versión: v1.0 - 27/10/2025
  Autor: Equipo CANSAT
  Centro: IES Gonzalo Torrente Ballester (Salamanca)

  Descripción:
  La ESP32 se conecta a una red WiFi y levanta un servidor web
  en el puerto 80 que permite encender y apagar un LED mediante
  botones desde un navegador web.

  Licencia: MIT
***************************************************/

#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "TU_SSID";
const char* password = "TU_PASSWORD";

const int LED_PIN = 25;  // LED en GPI25
WebServer server(80);

// Plantilla HTML en flash (PROGMEM)
const char INDEX_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html><html><head><meta charset='utf-8'>
<title>ESP32 WebServer</title>
<style>
  body{font-family:Arial;margin:2em}
  button{padding:1em;margin:.5em;font-size:1.2em}
  .estado{font-weight:bold}
</style>
</head><body>
  <h2>Servidor Web en ESP32</h2>
  <p>LED en GPIO2: <span class="estado">%STATE%</span></p>
  <p>
    <a href="/on"><button>Encender</button></a>
    <a href="/off"><button>Apagar</button></a>
  </p>
</body></html>
)HTML";

// Genera la página reemplazando %STATE% por el estado real del LED
String html() {
  String s = FPSTR(INDEX_HTML); // copia desde PROGMEM a String
  s.replace("%STATE%", digitalRead(LED_PIN) ? "ENCENDIDO" : "APAGADO");
  return s;
}

void handleRoot() { server.send(200, "text/html", html()); }
void handleOn()   { digitalWrite(LED_PIN, HIGH); server.sendHeader("Location", "/"); server.send(303); }
void handleOff()  { digitalWrite(LED_PIN, LOW);  server.sendHeader("Location", "/"); server.send(303); }

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);
  Serial.print("Conectando a WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\n✅ Conectado a WiFi");
  Serial.print("IP local: "); Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.begin();
  Serial.println("Servidor web iniciado en puerto 80");
}

void loop() {
  server.handleClient();
}
