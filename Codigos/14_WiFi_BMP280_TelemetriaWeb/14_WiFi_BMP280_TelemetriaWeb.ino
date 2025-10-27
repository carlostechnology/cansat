/***************************************************
  Proyecto: CANSAT - Telemetría web local (ESP32 + BMP280)
  Versión: v1.0 - 27/10/2025
  Autor: Equipo CANSAT
  Centro: IES Gonzalo Torrente Ballester (Salamanca)

  Descripción:
  Servidor web local (sin librerías externas) que muestra
  en tiempo real temperatura, presión y altitud del BMP280
  con gráficas SVG generadas en el navegador. Endpoint /data
  devuelve JSON para refresco periódico.

  Licencia: MIT
***************************************************/

#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define I2C_SDA 21
#define I2C_SCL 22

const char* WIFI_SSID = "TU_SSID";
const char* WIFI_PASS = "TU_PASSWORD";

const float SEA_LEVEL_HPA = 1024.0;
const uint32_t SAMPLE_MS  = 1000;

Adafruit_BMP280 bmp;
WebServer server(80);

// --- Página HTML generada desde el ESP32 (sin librerías externas) ---
String htmlPage() {
  return R"HTML(<!doctype html><html lang="es"><head>
<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP32 + BMP280 — Telemetría local (sin CDN)</title>
<style>
  body{font-family:system-ui,Segoe UI,Roboto,Arial,sans-serif;margin:0;padding:16px;background:#0b1320;color:#e6e9ef}
  .pill{display:inline-block;background:#24314e;color:#c6d0f5;border-radius:999px;padding:4px 10px;font-size:12px;margin-right:6px}
  .grid{display:grid;gap:12px} @media(min-width:900px){.grid{grid-template-columns:1fr 1fr}}
  .card{background:#141c2f;border:1px solid #24314e;border-radius:14px;padding:12px}
  h2{margin:0 0 8px;font-size:16px}
  svg{width:100%;height:280px;background:#0f172a;border-radius:8px}
  .axis{stroke:#334155;stroke-width:1} .line1{fill:none;stroke-width:2}
  .ok{background:#1b4332} .err{background:#6b2220}
</style>
</head><body>
<h1 style="margin:0 0 8px;font-size:18px">ESP32 + BMP280 — Telemetría local</h1>
<span class="pill" id="status">Conectando…</span>
<span class="pill" id="tminfo"></span>
<div class="grid">
  <div class="card"><h2>Temperatura (°C)</h2><svg id="gT"></svg></div>
  <div class="card"><h2>Presión (hPa)</h2><svg id="gP"></svg></div>
  <div class="card"><h2>Altitud (m)</h2><svg id="gA"></svg></div>
  <div class="card"><h2>Presión vs Altitud (doble eje)</h2><svg id="gM"></svg></div>
</div>

<script>
// --- Código JavaScript reducido ---
const MAX_POINTS=100; let xs=[],Ts=[],Ps=[],As=[]; let t0=null;
async function tick(){
  try{
    const r=await fetch('/data'); const j=await r.json();
    if(t0===null)t0=j.t; const ts=Math.round((j.t-t0)/1000);
    xs.push(ts); Ts.push(j.temp); Ps.push(j.press); As.push(j.alt);
    if(xs.length>MAX_POINTS){xs.shift();Ts.shift();Ps.shift();As.shift();}
    draw(); document.getElementById('status').textContent='OK '+new Date().toLocaleTimeString();
  }catch(e){document.getElementById('status').textContent='sin datos…';}
}
setInterval(tick,1000);

// --- Dibujado simple con SVG ---
function draw(){
  const makeLine=(svg,data,color,min,max)=>{
    const W=800,H=280,P=32; svg.innerHTML='';
    svg.setAttribute('viewBox',`0 0 ${W} ${H}`);
    let d=''; for(let i=0;i<data.length;i++){
      const x=P+(W-2*P)*i/(MAX_POINTS-1);
      const y=P+(H-2*P)*(1-(data[i]-min)/(max-min||1));
      d+=(i?'L':'M')+x+','+y;
    }
    let p=document.createElementNS('http://www.w3.org/2000/svg','path');
    p.setAttribute('d',d); p.setAttribute('stroke',color); p.setAttribute('fill','none'); p.setAttribute('stroke-width','2');
    svg.appendChild(p);
  };
  makeLine(document.getElementById('gT'),Ts,'#ef476f',Math.min(...Ts)-1,Math.max(...Ts)+1);
  makeLine(document.getElementById('gP'),Ps,'#118ab2',Math.min(...Ps)-2,Math.max(...Ps)+2);
  makeLine(document.getElementById('gA'),As,'#06d6a0',Math.min(...As)-1,Math.max(...As)+1);
  makeLine(document.getElementById('gM'),Ps,'#118ab2',Math.min(...Ps)-2,Math.max(...Ps)+2);
}
</script>
</body></html>)HTML";
}

void handleData() {
  static uint32_t last = 0;
  static float temp=0, pres=0, alt=0;
  if (millis() - last >= SAMPLE_MS) {
    temp = bmp.readTemperature();
    pres = bmp.readPressure() / 100.0;
    alt  = bmp.readAltitude(SEA_LEVEL_HPA);
    last = millis();
  }
  String json = String("{\"t\":") + millis() +
                ",\"temp\":" + String(temp,2) +
                ",\"press\":" + String(pres,2) +
                ",\"alt\":" + String(alt,1) + "}";
  server.sendHeader("Cache-Control","no-store");
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  delay(200);

  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(400000);
  if (!bmp.begin(0x76) && !bmp.begin(0x77)) {
    Serial.println("❌ BMP280 no detectado"); while(1) delay(1000);
  }
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) { Serial.print('.'); delay(500); }
  Serial.print("\nIP: "); Serial.println(WiFi.localIP());

  server.on("/", [](){ server.send(200, "text/html; charset=utf-8", htmlPage()); });
  server.on("/data", handleData);
  server.begin();
  Serial.println("Servidor web listo. Abre la IP en tu móvil.");
}

void loop() { server.handleClient(); }
