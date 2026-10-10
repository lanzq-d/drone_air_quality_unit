#include <Arduino.h>
#include "Config.h"
#include "SensorData.h"
#include "GpsSensor.h"
#include "TvocSensor.h"
#include "Dc01Sensor.h"
#include "WebApi.h"

AirQualityState state;

HardwareSerial gpsSerial(1);
HardwareSerial tvocSerial(2);

#if DRONE_ENABLE_DC01
HardwareSerial dc01Serial(0);
#endif

GpsSensor gps(gpsSerial);
TvocSensor tvoc(tvocSerial);

#if DRONE_ENABLE_DC01
Dc01Sensor dc01(dc01Serial);
#endif

WebApi web(state);

void updateSimulatedSensors() {
#if DRONE_SIMULATE_DISABLED_SENSORS
  const uint32_t now = millis();
#if !DRONE_ENABLE_TVOC
  state.tvoc.tvocMgM3 = 0.120f + (now % 3000) / 100000.0f;
  state.tvoc.ch2oMgM3 = 0.020f + (now % 2000) / 200000.0f;
  state.tvoc.co2Ppm = 420 + (now / 1000) % 15;
  state.tvoc.health.online = false;
  state.tvoc.health.lastUpdateMs = now;
  state.tvoc.health.message = "simulated; hardware disabled";
#endif
#if !DRONE_ENABLE_DC01
  state.dc01.pmUgM3 = 18 + (now / 1000) % 8;
  state.dc01.health.online = false;
  state.dc01.health.lastUpdateMs = now;
  state.dc01.health.message = "simulated; hardware disabled";
#endif
#endif
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println();
  Serial.println("Drone Air Quality Unit");

#if DRONE_ENABLE_GPS
  gpsSerial.begin(SENSOR_BAUD, SERIAL_8N1, PIN_GPS_RX, PIN_GPS_TX);
  gps.begin();
  Serial.println("GPS UART enabled on GPIO26/GPIO27");
#else
  state.gps.health.message = "disabled";
#endif

#if DRONE_ENABLE_TVOC
  tvocSerial.begin(SENSOR_BAUD, SERIAL_8N1, PIN_TVOC_RX, PIN_TVOC_TX);
  tvoc.begin();
  Serial.println("TVOC UART enabled on GPIO16/GPIO17; use level shifting before connecting sensor TX");
#else
  state.tvoc.health.message = "disabled";
#endif

#if DRONE_ENABLE_DC01
  dc01Serial.begin(SENSOR_BAUD, SERIAL_8N1, PIN_DC01_RX, PIN_DC01_TX);
  dc01.begin();
  Serial.println("DC01 UART enabled on GPIO32/GPIO33; UART0 debug over USB will be limited");
#else
  state.dc01.health.message = "disabled";
#endif

  web.begin();
  Serial.print("SoftAP started: ");
  Serial.println(DRONE_AP_SSID);
  Serial.println("Open http://192.168.4.1/");
}

void loop() {
  state.uptimeMs = millis();

#if DRONE_ENABLE_GPS
  gps.poll(state.gps);
#endif
#if DRONE_ENABLE_TVOC
  tvoc.poll(state.tvoc);
#endif
#if DRONE_ENABLE_DC01
  dc01.poll(state.dc01);
#endif

  updateSimulatedSensors();
  web.poll();
  delay(2);
}
