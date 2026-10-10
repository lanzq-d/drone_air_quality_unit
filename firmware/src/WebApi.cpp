#include "WebApi.h"
#include "Config.h"
#include <WiFi.h>

WebApi::WebApi(AirQualityState& state) : server_(80), state_(state) {}

void WebApi::begin() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(DRONE_AP_SSID, DRONE_AP_PASSWORD);

  server_.on("/", HTTP_GET, [this]() {
    server_.send(200, "text/html", html());
  });
  server_.on("/api/status", HTTP_GET, [this]() {
    sendCors();
    server_.send(200, "application/json", stateJson());
  });
  server_.on("/api/latest", HTTP_GET, [this]() {
    sendCors();
    server_.send(200, "application/json", stateJson());
  });
  server_.onNotFound([this]() {
    sendCors();
    server_.send(404, "application/json", "{\"error\":\"not_found\"}");
  });
  server_.begin();
}

void WebApi::poll() {
  server_.handleClient();
}

void WebApi::sendCors() {
  server_.sendHeader("Access-Control-Allow-Origin", "*");
  server_.sendHeader("Cache-Control", "no-store");
}

String WebApi::healthJson(const SensorHealth& health) const {
  String json = "{";
  json += "\"online\":" + String(health.online ? "true" : "false");
  json += ",\"frames\":" + String(health.frames);
  json += ",\"errors\":" + String(health.errors);
  json += ",\"last_update_ms\":" + String(health.lastUpdateMs);
  json += ",\"message\":\"" + health.message + "\"";
  json += "}";
  return json;
}

String WebApi::stateJson() const {
  String json = "{";
  json += "\"firmware\":\"" DRONE_FW_VERSION "\"";
  json += ",\"uptime_ms\":" + String(state_.uptimeMs);
  json += ",\"gps\":{";
  json += "\"health\":" + healthJson(state_.gps.health);
  json += ",\"fix\":" + String(state_.gps.fix ? "true" : "false");
  json += ",\"latitude\":" + String(state_.gps.latitude, 7);
  json += ",\"longitude\":" + String(state_.gps.longitude, 7);
  json += ",\"altitude_m\":" + String(state_.gps.altitudeM, 1);
  json += ",\"speed_kmh\":" + String(state_.gps.speedKmh, 1);
  json += ",\"satellites\":" + String(state_.gps.satellites);
  json += ",\"utc_time\":\"" + state_.gps.utcTime + "\"";
  json += "},\"tvoc\":{";
  json += "\"health\":" + healthJson(state_.tvoc.health);
  json += ",\"tvoc_mg_m3\":" + String(state_.tvoc.tvocMgM3, 3);
  json += ",\"ch2o_mg_m3\":" + String(state_.tvoc.ch2oMgM3, 3);
  json += ",\"co2_ppm\":" + String(state_.tvoc.co2Ppm);
  json += "},\"dc01\":{";
  json += "\"health\":" + healthJson(state_.dc01.health);
  json += ",\"pm_ug_m3\":" + String(state_.dc01.pmUgM3);
  json += "}}";
  return json;
}

String WebApi::html() const {
  return F("<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>"
           "<title>DroneAQ</title><style>body{font-family:Arial,sans-serif;margin:24px;background:#f6f8fb;color:#17202a}"
           "main{max-width:920px;margin:auto}.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(210px,1fr));gap:12px}"
           ".card{background:#fff;border:1px solid #d9e1ea;border-radius:8px;padding:14px}dt{color:#65758b}dd{margin:0 0 8px;font-size:24px}"
           ".ok{color:#0f7b45}.bad{color:#a33}</style></head><body><main><h1>Drone Air Quality Unit</h1>"
           "<p>SoftAP: " DRONE_AP_SSID " / API: /api/latest</p><div id='app'>Loading...</div>"
           "<script>async function tick(){const r=await fetch('/api/latest');const s=await r.json();"
           "const card=(n,ok,rows)=>`<section class='card'><h2>${n} <span class='${ok?'ok':'bad'}'>${ok?'online':'offline'}</span></h2><dl>${rows}</dl></section>`;"
           "document.getElementById('app').innerHTML=`<div class='grid'>`+"
           "card('GPS',s.gps.health.online,`<dt>Fix</dt><dd>${s.gps.fix?'Yes':'No'}</dd><dt>Lat/Lon</dt><dd>${s.gps.latitude.toFixed(6)}, ${s.gps.longitude.toFixed(6)}</dd><dt>Sats</dt><dd>${s.gps.satellites}</dd>`)+"
           "card('TVOC',s.tvoc.health.online,`<dt>TVOC</dt><dd>${s.tvoc.tvoc_mg_m3.toFixed(3)}</dd><dt>CH2O</dt><dd>${s.tvoc.ch2o_mg_m3.toFixed(3)}</dd><dt>CO2</dt><dd>${s.tvoc.co2_ppm}</dd>`)+"
           "card('DC01',s.dc01.health.online,`<dt>PM</dt><dd>${s.dc01.pm_ug_m3}</dd><dt>Status</dt><dd>${s.dc01.health.message}</dd>`)+`</div>`}"
           "setInterval(tick,1000);tick();</script></main></body></html>");
}
