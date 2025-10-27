/***************************************************
  Proyecto: CANSAT - Servidor Web ON/OFF (UI mejorada)
  Versión: v1.0 - 27/10/2025
  Autor: Equipo CANSAT
  Centro: IES Gonzalo Torrente Ballester (Salamanca)

  Descripción:
  Servidor web en ESP32 con página HTML/CSS mejorada para
  encender y apagar un LED. Muestra estado, estilo visual
  moderno y la IP de la placa en la interfaz.

  Licencia: MIT
***************************************************/

#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "TU_SSID";
const char* password = "TU_PASSWORD";

const int LED_PIN = 25;  // Cambia si usas otro GPIO
WebServer server(80);

// HTML con estilo mejorado (sin librerías externas)
const char INDEX_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html><html lang="es">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP32 Control</title>
<style>
  :root{
    --bg1:#0f172a;       /* fondo profundo */
    --bg2:#111827;       /* fondo tarjeta */
    --txt:#e5e7eb;       /* texto claro */
    --muted:#9ca3af;     /* texto secundario */
    --ok:#22c55e;        /* verde */
    --off:#ef4444;       /* rojo */
    --accent:#38bdf8;    /* cian */
  }
  *{box-sizing:border-box}
  body{
    margin:0; min-height:100vh; display:grid; place-items:center;
    background: radial-gradient(1000px 600px at 20% -10%, #1f2937 0%, transparent 60%),
                radial-gradient(900px 600px at 120% 110%, #0ea5e9 0%, transparent 55%),
                var(--bg1);
    color:var(--txt); font-family: ui-sans-serif, system-ui, -apple-system, Segoe UI, Roboto, Ubuntu;
  }
  .card{
    width:min(540px, 92vw); background:linear-gradient(180deg, #0b1220 0%, var(--bg2) 100%);
    border:1px solid #1f2937; border-radius:18px; padding:28px;
    box-shadow:0 10px 30px rgba(0,0,0,.35);
  }
  h1{margin:0 0 .25rem 0; font-size:clamp(1.4rem, 2.2vw, 1.9rem)}
  .subtitle{margin:0 0 1.2rem 0; color:var(--muted); letter-spacing:.2px}
  .row{display:flex; gap:14px; flex-wrap:wrap; align-items:center; margin:.8rem 0 1.1rem}
  .pill{
    display:inline-flex; align-items:center; gap:.5rem; padding:.35rem .7rem;
    border-radius:999px; font-weight:600; font-size:.95rem; border:1px solid #374151
  }
  .pill.ok{ color:#10b981; border-color:#064e3b; background:rgba(16,185,129,.08)}
  .pill.off{color:#f87171; border-color:#7f1d1d; background:rgba(239,68,68,.08)}
  .gpio{color:var(--muted); font-size:.95rem}
  .buttons{display:flex; gap:12px; flex-wrap:wrap}
  .btn{
    appearance:none; border:1px solid #1f2937; background:#0b1220; color:var(--txt);
    padding:.85rem 1.15rem; border-radius:12px; font-size:1.05rem; cursor:pointer;
    transition:transform .06s ease, border-color .2s ease, background .2s ease;
  }
  .btn:hover{transform:translateY(-1px); border-color:#334155}
  .btn:active{transform:translateY(0)}
  .btn.on{ background:linear-gradient(180deg, #065f46, #064e3b); border-color:#065f46 }
  .btn.off{background:linear-gradient(180deg, #7f1d1d, #6b1111); border-color:#7f1d1d}
  .footer{margin-top:1.2rem; color:var(--muted); font-size:.88rem}
  .tag{color:var(--accent); font-weight:600}
</style>
</head>
<body>
  <main class="card">
    <h1>🚀 ESP32 Control Panel</h1>
    <p class="subtitle">Servidor web local para encender/apagar un LED</p>

    <div class="row">
      <span class="gpio">Pin: <b>GPIO2</b></span>
      <span class="pill %CLASS%">LED: <span>%STATE%</span></span>
    </div>

    <div class="buttons">
      <a href="/on"><button class="btn on">Encender</button></a>
      <a href="/off"><button class="btn off">Apagar</button></a>
    </div>

    <p class="footer">Conéctate a la misma red WiFi. IP de la placa: <span class="tag">%IP%</span></p>
  </main>
</body>
</html>
)HTML";

String renderPage() {
  String html = FPSTR(INDEX_HTML);
  bool on = digitalRead(LED_PIN);
  html.replace("%STATE%", on ? "ENCENDIDO" : "APAGADO");
  html.replace("%CLASS%", on ? "ok" : "off");
  html.replace("%IP%", WiFi.localIP().toString());
  return html;
}

void handleRoot() { server.send(200, "text/html", renderPage()); }
void handleOn()   { digitalWrite(LED_PIN, HIGH); server.sendHeader("Location","/"); server.send(303); }
void handleOff()  { digitalWrite(LED_PIN, LOW);  server.sendHeader("Location","/"); server.send(303); }

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);
  Serial.print("Conectando a WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi conectado");
  Serial.print("IP: "); Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.begin();
  Serial.println("Servidor web iniciado en puerto 80");
}

void loop(){ server.handleClient(); }
