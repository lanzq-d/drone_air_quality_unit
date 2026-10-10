#pragma once

#include <Arduino.h>
#include "SensorData.h"

class GpsSensor {
public:
  explicit GpsSensor(Stream& stream);
  void begin();
  void poll(GpsData& data);

private:
  Stream& stream_;
  char line_[128] = {};
  uint8_t index_ = 0;

  void consumeByte(char value, GpsData& data);
  void parseLine(char* line, GpsData& data);
  bool validNmeaChecksum(const char* line) const;
  char* tokenAt(char* line, uint8_t wanted);
  double parseCoordinate(const char* value, const char* hemisphere) const;
};
