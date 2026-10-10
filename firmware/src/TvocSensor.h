#pragma once

#include <Arduino.h>
#include "SensorData.h"

class TvocSensor {
public:
  explicit TvocSensor(Stream& stream);
  void begin();
  void poll(TvocData& data);

private:
  Stream& stream_;
  uint8_t frame_[9] = {};
  uint8_t index_ = 0;
  uint8_t checksum(const uint8_t* frame) const;
  void consumeByte(uint8_t value, TvocData& data);
};
