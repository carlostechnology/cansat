/***************************************************
  Proyecto: CANSAT - Telemetría web local (ESP32 + MPU6050)
  Módulo: Orientación 3D con DMP (yaw, pitch, roll)
  Versión: v1.0 - 15/11/2025
  Autor: Equipo CANSAT
  Centro: IES Gonzalo Torrente Ballester (Salamanca)

  Descripción:
  Servidor web local (sin librerías externas) que muestra
  en tiempo real la orientación 3D del CANSAT obtenida
  con el DMP (Digital Motion Processor) del MPU6050:
  yaw, pitch y roll en grados.

  El ESP32 actúa como servidor HTTP en la red WiFi del
  centro o del laboratorio. La página principal muestra
  tres gráficas SVG (yaw, pitch, roll) que se actualizan
  periódicamente mediante peticiones AJAX al endpoint /data,
  que devuelve un JSON con la última orientación medida.

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

const char* WIFI_SSID = "NOMBRE_RED";
const char* WIFI_PASS = "";  // "" si es abierta, o tu contraseña si la tiene

...

WiFi.mode(WIFI_STA);

if (strlen(WIFI_PASS) == 0) {
  // Red abierta
  WiFi.begin(WIFI_SSID);
} else {
  // Red con contraseña
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
}


// ─ Frecuencia de actualización enviada al navegador (ms) ─
const uint32_t SAMPLE_MS = 200;   // 5 Hz aprox.

// ─ Objetos globales ─
WebServer server(80);
MPU6050 mpu(0x68);

// Estado DMP
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
<title>ESP32 + MPU6050 — Orientación 3D (telemetría local)</title>
<style>
  body{font-family:system-ui,Segoe UI,Roboto,Arial,sans-serif;margin:0;padding:16px;background:#0b1320;color:#e6e9ef}
  .pill{display:inline-block;background:#24314e;color:#c6d0f5;border-radius:999px;padding:4px 10px;font-size:12px;margin-right:6px}
  .grid{display:grid;gap:12px}
  @media(min-width:900px){.grid{grid-template-columns:1fr 1fr}}
  .card{background:#141c2f;border:1px solid #24314e;border-radius:14px;padding:12px}
  h2{margin:0 0 8px;font-size:16px}
  svg{width:100%;height:260px;background:#0f172a;border-radius:8px}
</style>
</head><body>
<h1 style="margin:0 0 8px;font-size:18px">ESP32 + MPU6050 — Orientación 3D (yaw, pitch, roll)</h1>
<span class="pill" id="status">Conectando…</span>
<span class="pill" id="tminfo"></span>

<div class="grid">
  <div class="card"><h2>Yaw (°) — giro horizontal</h2><svg id="gYaw"></svg></div>
  <div class="card"><h2>Pitch (°) — cabeceo</h2><svg id="gPitch"></svg></div>
  <div class="card"><h2>Roll (°) — alabeo</h2><svg id="gRoll"></svg></div>
</div>

<script>
const MAX_POINTS = 150;
let xs=[], yawArr=[], pitchArr=[], rollArr=[];
let t0=null;

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

    draw();
    document.getElementById('status').textContent='OK '+new Date().toLocaleTimeString();
    document.getElementById('tminfo').textContent='t = '+ts+' s';
  }catch(e){
    document.getElementById('status').textContent='sin datos…';
  }
}

setInterval(tick, 200);

// Dibujado simple con SVG
function draw(){
  const makeLine=(svg,data,color,min,max)=>{
    const W=800,H=260,P=32;
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

  const yawMin   = Math.min(...yawArr,   -180);
  const yawMax   = Math.max(...yawArr,    180);
  const pitchMin = Math.min(...pitchArr, -90);
  const pitchMax = Math.max(...pitchArr,  90);
  const rollMin  = Math.min(...rollArr,  -90);
  const rollMax  = Math.max(...rollArr,   90);

  makeLine(document.getElementById('gYaw'),   yawArr,   '#ffb703', yawMin,   yawMax);
  makeLine(document.getElementById('gPitch'), pitchArr, '#06d6a0', pitchMin, pitchMax);
  makeLine(document.getElementById('gRoll'),  rollArr,  '#118ab2', rollMin,  rollMax);
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
    // Overflow: limpiamos y salimos
    mpu.resetFIFO();
    Serial.println(F("# Aviso: FIFO overflow, reseteando."));
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
    // Enviamos igualmente la última lectura, aunque no sea "nueva"
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
  Serial.println("\n# ESP32 + MPU6050 — Telemetría web de orientación (DMP)");

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

  // Offsets de tu calibración (ajusta para tu sensor)
  mpu.setXAccelOffset(-1062);
  mpu.setYAccelOffset( 2636);
  mpu.setZAccelOffset( 1330);
  mpu.setXGyroOffset(   104);
  mpu.setYGyroOffset(   -14);
  mpu.setZGyroOffset(    -8);

  if (devStatus == 0) {
    mpu.setDMPEnabled(true);
    dmpReady   = true;
    packetSize = mpu.dmpGetFIFOPacketSize();
    Serial.println(F("# DMP iniciado correctamente."));
  } else {
    Serial.print(F("# ❌ Error al iniciar DMP. devStatus = "));
    Serial.println(devStatus);
    while (1) delay(1000);
  }

  // WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print(F("# Conectando a WiFi"));
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.print(F("\n# Conectado. IP: "));
  Serial.println(WiFi.localIP());

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
  // 1) Actualizamos la IMU (vaciamos FIFO y calculamos yaw/pitch/roll)
  updateIMU();

  // 2) Atendemos peticiones HTTP
  server.handleClient();

  delay(5); // pequeña pausa de cortesía
}

