#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include "SensorData.h"

class WebApi {
public:
  explicit WebApi(AirQualityState& state);
  void begin();
  void poll();

private:
  WebServer server_;
  AirQualityState& state_;

  String stateJson() const;
  String healthJson(const SensorHealth& health) const;
  String html() const;
  void sendCors();
};
