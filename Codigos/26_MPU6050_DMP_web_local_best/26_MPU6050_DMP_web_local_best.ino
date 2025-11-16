/***************************************************
  Proyecto: CANSAT - Telemetría web local (ESP32 + MPU6050)
  Módulo: Orientación 3D con DMP (yaw, pitch, roll)
  Versión: v1.1 - 15/11/2025
  Autor: Equipo CANSAT
  Centro: IES Gonzalo Torrente Ballester (Salamanca)

  Descripción:
  Servidor web local que muestra en tiempo real la
  orientación 3D del CANSAT obtenida con el DMP del
  MPU6050: yaw, pitch y roll en grados.

  El ESP32 se conecta a una red WiFi (con clave o
  abierta) y actúa como servidor HTTP. Desde el móvil
  o el ordenador abrimos la IP del ESP32 y vemos tres
  gráficas SVG (yaw, pitch, roll) que se actualizan
  periódicamente mediante peticiones AJAX a /data.

  Para usar red abierta → WIFI_PASS = "" (cadena vacía).
  Para usar red con clave → WIFI_PASS = "tu_clave".

  Licencia: MIT
***************************************************/

#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

// ─ I2C (MPU6050) ─
#define I2C_SDA 21
#define I2C_SCL 22

// ─ WiFi ─
const char* WIFI_SSID = "*******";   // cambia esto
const char* WIFI_PASS = "";                   // "" si es abierta, o la contraseña

// ─ Frecuencia de actualización enviada al navegador (ms) ─
const uint32_t SAMPLE_MS = 200;   // 5 Hz aprox.

// ─ Objetos globales ─
WebServer server(80);
MPU6050 mpu(0x68);

// Estado DMP / FIFO
bool     dmpReady    = false;
uint8_t  devStatus   = 0;
uint16_t packetSize  = 0;
uint16_t fifoCount   = 0;
uint8_t  fifoBuffer[64];

Quaternion   q;
VectorFloat  gravity;
float        ypr[3];        // yaw, pitch, roll (radianes)

// Última orientación en grados
volatile float yawDeg   = 0.0;
volatile float pitchDeg = 0.0;
volatile float rollDeg  = 0.0;

// ─────────────────────────────────────────────
// Página HTML (telemetría yaw/pitch/roll)
// ─────────────────────────────────────────────
String htmlPage() {
  return R"HTML(<!doctype html><html lang="es"><head>
<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP32 + MPU6050 — Orientación 3D</title>
<style>
  :root{
    --bg:#020617;
    --bg-card:#02091b;
    --bg-panel:#0b1220;
    --accent:#38bdf8;
    --accent2:#22c55e;
    --accent3:#f97316;
    --text:#e5e7eb;
    --muted:#9ca3af;
    --border:#1f2937;
  }
  *{box-sizing:border-box;margin:0;padding:0}
  body{
    font-family:system-ui,Segoe UI,Roboto,Arial,sans-serif;
    background:radial-gradient(circle at top,#082f49 0,#020617 55%,#000 100%);
    color:var(--text);
    padding:14px;
  }
  header{
    display:flex;
    flex-wrap:wrap;
    justify-content:space-between;
    align-items:flex-start;
    gap:8px;
    margin-bottom:12px;
  }
  h1{
    font-size:18px;
    font-weight:600;
  }
  h1 span{
    display:block;
    font-size:12px;
    font-weight:400;
    color:var(--muted);
  }
  .pill{
    display:inline-flex;
    align-items:center;
    gap:6px;
    background:rgba(15,23,42,.85);
    border:1px solid rgba(148,163,184,.4);
    border-radius:999px;
    padding:4px 10px;
    font-size:11px;
    color:var(--text);
    white-space:nowrap;
  }
  .pill-dot{
    width:8px;height:8px;border-radius:999px;
    background:#22c55e;
    box-shadow:0 0 8px rgba(34,197,94,.8);
  }
  .layout{
    display:grid;
    gap:12px;
  }
  @media(min-width:900px){
    .layout{
      grid-template-columns:minmax(0,1.2fr) minmax(0,1fr);
      align-items:start;
    }
  }
  .panel{
    background:var(--bg-panel);
    border-radius:18px;
    border:1px solid rgba(15,23,42,.9);
    box-shadow:0 18px 40px rgba(15,23,42,.8);
    padding:12px;
  }
  .panel-header{
    display:flex;
    justify-content:space-between;
    align-items:center;
    margin-bottom:8px;
    gap:8px;
  }
  .panel-header h2{
    font-size:14px;
    font-weight:500;
  }
  .mini-label{
    font-size:11px;
    color:var(--muted);
  }
  .values-grid{
    display:grid;
    grid-template-columns:repeat(3,minmax(0,1fr));
    gap:8px;
    margin-top:4px;
  }
  .value-card{
    background:linear-gradient(135deg,#020617,#02091b);
    border-radius:12px;
    padding:8px 10px;
    border:1px solid rgba(31,41,55,.8);
  }
  .value-label{
    font-size:11px;
    color:var(--muted);
    margin-bottom:2px;
  }
  .value-main{
    font-size:20px;
    font-weight:600;
  }
  .value-main span{
    font-size:11px;
    margin-left:2px;
    color:var(--muted);
    font-weight:400;
  }
  .value-yaw  {color:var(--accent3);}
  .value-pitch{color:var(--accent2);}
  .value-roll {color:var(--accent);}
  .grid-plots{
    display:grid;
    gap:10px;
    margin-top:4px;
  }
  @media(min-width:900px){
    .grid-plots{
      grid-template-columns:repeat(3,minmax(0,1fr));
    }
  }
  .card{
    background:var(--bg-card);
    border-radius:14px;
    padding:8px;
    border:1px solid var(--border);
  }
  .card h3{
    font-size:13px;
    font-weight:500;
    margin-bottom:6px;
  }
  .card h3 span{
    display:block;
    font-size:11px;
    font-weight:400;
    color:var(--muted);
  }
  svg{
    width:100%;
    height:210px;
    background:#020617;
    border-radius:10px;
  }
</style>
</head><body>
<header>
  <div>
    <h1>ESP32 + MPU6050 — Orientación 3D
      <span>Telemetría local en tiempo real (yaw, pitch, roll)</span>
    </h1>
  </div>
  <div style="display:flex;gap:6px;flex-wrap:wrap;">
    <div class="pill"><span class="pill-dot" id="dot"></span><span id="status">Conectando…</span></div>
    <div class="pill"><span id="tminfo">t = 0 s</span></div>
  </div>
</header>

<div class="layout">
  <!-- Panel de valores actuales -->
  <section class="panel">
    <div class="panel-header">
      <h2>Orientación instantánea</h2>
      <span class="mini-label" id="ipinfo"></span>
    </div>
    <div class="values-grid">
      <div class="value-card">
        <div class="value-label">Yaw (°) — giro horizontal</div>
        <div class="value-main value-yaw" id="valYaw">0.0<span>°</span></div>
      </div>
      <div class="value-card">
        <div class="value-label">Pitch (°) — cabeceo</div>
        <div class="value-main value-pitch" id="valPitch">0.0<span>°</span></div>
      </div>
      <div class="value-card">
        <div class="value-label">Roll (°) — alabeo</div>
        <div class="value-main value-roll" id="valRoll">0.0<span>°</span></div>
      </div>
    </div>
  </section>

  <!-- Panel de gráficas -->
  <section class="panel">
    <div class="panel-header">
      <h2>Histórico reciente</h2>
      <span class="mini-label">Ventana ~30&nbsp;s (150 muestras)</span>
    </div>
    <div class="grid-plots">
      <div class="card">
        <h3>Yaw (°)<span>Rotación alrededor del eje vertical</span></h3>
        <svg id="gYaw"></svg>
      </div>
      <div class="card">
        <h3>Pitch (°)<span>Inclinación adelante / atrás</span></h3>
        <svg id="gPitch"></svg>
      </div>
      <div class="card">
        <h3>Roll (°)<span>Inclinación lateral (alabeo)</span></h3>
        <svg id="gRoll"></svg>
      </div>
    </div>
  </section>
</div>

<script>
const MAX_POINTS = 150;
let xs=[], yawArr=[], pitchArr=[], rollArr=[];
let t0=null;

function setIP(){
  const el=document.getElementById('ipinfo');
  el.textContent = location.host ? ('IP: '+location.host) : '';
}
setIP();

async function tick(){
  try{
    const r = await fetch('/data');
    const j = await r.json();
    if(t0===null) t0 = j.t;
    const ts = Math.round((j.t - t0)/1000);

    xs.push(ts);
    yawArr.push(j.yaw);
    pitchArr.push(j.pitch);
    rollArr.push(j.roll);

    if(xs.length>MAX_POINTS){
      xs.shift(); yawArr.shift(); pitchArr.shift(); rollArr.shift();
    }

    // Valores instantáneos
    document.getElementById('valYaw').firstChild.nodeValue   = j.yaw.toFixed(1);
    document.getElementById('valPitch').firstChild.nodeValue = j.pitch.toFixed(1);
    document.getElementById('valRoll').firstChild.nodeValue  = j.roll.toFixed(1);

    draw();
    document.getElementById('status').textContent='OK '+new Date().toLocaleTimeString();
    document.getElementById('tminfo').textContent='t = '+ts+' s';
    document.getElementById('dot').style.background='#22c55e';
  }catch(e){
    document.getElementById('status').textContent='sin datos…';
    document.getElementById('dot').style.background='#f97316';
  }
}

setInterval(tick, 200);

// Dibujado simple con SVG
function draw(){
  const makeLine=(svg,data,color,min,max)=>{
    const W=800,H=210,P=26;
    svg.innerHTML='';
    svg.setAttribute('viewBox',`0 0 ${W} ${H}`);
    if(data.length<2) return;
    let d='';
    for(let i=0;i<data.length;i++){
      const x=P+(W-2*P)*i/(MAX_POINTS-1);
      const y=P+(H-2*P)*(1-(data[i]-min)/(max-min||1));
      d+=(i?'L':'M')+x+','+y;
    }
    const path=document.createElementNS('http://www.w3.org/2000/svg','path');
    path.setAttribute('d',d);
    path.setAttribute('stroke',color);
    path.setAttribute('fill','none');
    path.setAttribute('stroke-width','2');
    svg.appendChild(path);
  };

  // Rango razonable para cada eje
  const yawMin   = Math.min(...yawArr,   -180);
  const yawMax   = Math.max(...yawArr,    180);
  const pitchMin = Math.min(...pitchArr, -90);
  const pitchMax = Math.max(...pitchArr,  90);
  const rollMin  = Math.min(...rollArr,  -90);
  const rollMax  = Math.max(...rollArr,   90);

  makeLine(document.getElementById('gYaw'),   yawArr,   '#fbbf24', yawMin,   yawMax);
  makeLine(document.getElementById('gPitch'), pitchArr, '#22c55e', pitchMin, pitchMax);
  makeLine(document.getElementById('gRoll'),  rollArr,  '#38bdf8', rollMin,  rollMax);
}
</script>
</body></html>)HTML";
}


// ─────────────────────────────────────────────
// Lectura del DMP: actualiza yawDeg/pitchDeg/rollDeg
// ─────────────────────────────────────────────
void updateIMU() {
  if (!dmpReady) return;

  fifoCount = mpu.getFIFOCount();
  if (fifoCount == 0) return;

  if (fifoCount == 1024) {
    // Overflow: limpiamos y salimos (sin spam por serie)
    mpu.resetFIFO();
    return;
  }

  if (fifoCount < packetSize) return;

  // Nos quedamos con el último paquete de la FIFO
  while (fifoCount >= packetSize) {
    mpu.getFIFOBytes(fifoBuffer, packetSize);
    fifoCount -= packetSize;
  }

  mpu.dmpGetQuaternion(&q, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &q);
  mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

  yawDeg   = ypr[0] * 180.0 / PI;
  pitchDeg = ypr[1] * 180.0 / PI;
  rollDeg  = ypr[2] * 180.0 / PI;
}

// ─────────────────────────────────────────────
// Endpoint /data -> JSON con la última orientación
// ─────────────────────────────────────────────
void handleData() {
  static uint32_t lastSend = 0;
  if (millis() - lastSend < SAMPLE_MS) {
    // aunque no haya nueva lectura, enviamos el último valor
  }
  lastSend = millis();

  String json = String("{\"t\":") + millis() +
                ",\"yaw\":"   + String(yawDeg,   1) +
                ",\"pitch\":" + String(pitchDeg, 1) +
                ",\"roll\":"  + String(rollDeg,  1) + "}";

  server.sendHeader("Cache-Control","no-store");
  server.send(200, "application/json", json);
}

// ─────────────────────────────────────────────
// SETUP
// ─────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println(F("\n# ESP32 + MPU6050 — Telemetría web de orientación (DMP)"));

  // I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(400000);

  // MPU6050 + DMP
  Serial.println(F("# Inicializando MPU6050..."));
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println(F("# ❌ MPU6050 no detectado"));
    while (1) delay(1000);
  }
  Serial.println(F("# MPU6050 conectado."));

  Serial.println(F("# Configurando DMP..."));
  devStatus = mpu.dmpInitialize();

  // Offsets de calibración (ajusta a tu sensor)
  mpu.setXAccelOffset(-1062);
  mpu.setYAccelOffset( 2636);
  mpu.setZAccelOffset( 1330);
  mpu.setXGyroOffset(   104);
  mpu.setYGyroOffset(   -14);
  mpu.setZGyroOffset(    -8);

  if (devStatus == 0) {
    mpu.setDMPEnabled(true);
    mpu.resetFIFO();                        // limpiamos FIFO de arranque
    dmpReady   = true;
    packetSize = mpu.dmpGetFIFOPacketSize();
    Serial.println(F("# DMP iniciado correctamente."));
  } else {
    Serial.print(F("# ❌ Error al iniciar DMP. devStatus = "));
    Serial.println(devStatus);
    while (1) delay(1000);
  }

  // WiFi (con o sin contraseña)
  WiFi.mode(WIFI_STA);
  if (strlen(WIFI_PASS) == 0) {
    // Red abierta
    WiFi.begin(WIFI_SSID);
  } else {
    // Red con clave
    WiFi.begin(WIFI_SSID, WIFI_PASS);
  }

  Serial.print(F("# Conectando a WiFi"));
  uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) {
    Serial.print('.');
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print(F("\n# Conectado. IP: "));
    Serial.println(WiFi.localIP());
  } else {
    Serial.println(F("\n# ❌ No se pudo conectar a la WiFi (timeout)."));
    // si quieres, aquí podrías montar un AP propio en vez de quedarte bloqueado
  }

  // Servidor HTTP
  server.on("/", []() {
    server.send(200, "text/html; charset=utf-8", htmlPage());
  });
  server.on("/data", handleData);
  server.begin();
  Serial.println(F("# Servidor web listo. Abre la IP en tu móvil."));
}

// ─────────────────────────────────────────────
// LOOP
// ─────────────────────────────────────────────
void loop() {
  updateIMU();           // leer DMP y actualizar yaw/pitch/roll
  server.handleClient(); // atender peticiones HTTP
  delay(5);              // pequeña pausa
}
