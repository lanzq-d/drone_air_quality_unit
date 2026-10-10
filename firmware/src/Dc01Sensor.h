#pragma once

#include <Arduino.h>
#include "SensorData.h"

class Dc01Sensor {
public:
  explicit Dc01Sensor(Stream& stream);
  void begin();
  void poll(Dc01Data& data);

private:
  Stream& stream_;
  uint8_t frame_[4] = {};
  uint8_t index_ = 0;
  uint8_t checksum(const uint8_t* frame) const;
  void consumeByte(uint8_t value, Dc01Data& data);
};
