/***************************************************
  Proyecto: CANSAT - ESP32 como Punto de Acceso WiFi (AP)
  Versión: v1.0 - 27/10/2025
  Autor: Nombre del alumno/equipo
  Centro: IES Gonzalo Torrente Ballester (Salamanca)

  Descripción:
  Configura la ESP32 como un punto de acceso WiFi (AP),
  creando su propia red inalámbrica local.
  Permite conectar móviles u otros dispositivos para
  comunicación directa sin router.

  Licencia: MIT
***************************************************/

#include <WiFi.h>

const char* ssid = "ESP32_AP";
const char* password = "contraseña_AP";

void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  Serial.println("Punto de acceso iniciado");
  Serial.print("Nombre de red: ");
  Serial.println(ssid);
  Serial.print("IP local: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  //Lógica de la aplicación
}
