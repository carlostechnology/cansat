/***************************************************
  Proyecto: CANSAT - Test de hardware (LED en ESP32)
  Versión: v1.0 - 27/10/2025
  Autor: Equipo CANSAT
  Centro: IES Gonzalo Torrente Ballester (Salamanca)

  Descripción:
  Comprobación del funcionamiento del pin GPIO25 del ESP32
  mediante el parpadeo de un LED externo.

  Hardware:
  - ESP32-WROOM-32
  - LED conectado al GPIO25 con resistencia

  Objetivo educativo:
  Validar que el microcontrolador está correctamente
  programado y que la salida digital funciona.

  Licencia: MIT
***************************************************/

// Pin del LED a testear
int led = 25; // Usaremos el pin GPIO25

// ------------------- Configuración -------------------
void setup() {
  pinMode(led, OUTPUT); // Configurar GPIO25 como salida
}

// ------------------- Bucle principal -------------------
void loop() {
  digitalWrite(led, HIGH); // Encender LED
  delay(1000);             // Esperar 1 segundo
  digitalWrite(led, LOW);  // Apagar LED
  delay(1000);             // Esperar 1 segundo
}
