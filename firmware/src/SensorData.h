#pragma once

#include <Arduino.h>

struct SensorHealth {
  bool online = false;
  uint32_t frames = 0;
  uint32_t errors = 0;
  uint32_t lastUpdateMs = 0;
  String message = "waiting";
};

struct GpsData {
  SensorHealth health;
  bool fix = false;
  double latitude = 0;
  double longitude = 0;
  double altitudeM = 0;
  double speedKmh = 0;
  uint8_t satellites = 0;
  String utcTime;
};

struct TvocData {
  SensorHealth health;
  float tvocMgM3 = 0;
  float ch2oMgM3 = 0;
  uint16_t co2Ppm = 0;
};

struct Dc01Data {
  SensorHealth health;
  uint16_t pmUgM3 = 0;
};

struct AirQualityState {
  uint32_t uptimeMs = 0;
  GpsData gps;
  TvocData tvoc;
  Dc01Data dc01;
};
