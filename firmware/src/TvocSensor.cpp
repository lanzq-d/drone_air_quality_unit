#include "TvocSensor.h"
#include "Config.h"

TvocSensor::TvocSensor(Stream& stream) : stream_(stream) {}

void TvocSensor::begin() {}

uint8_t TvocSensor::checksum(const uint8_t* frame) const {
  uint16_t sum = 0;
  for (uint8_t i = 0; i < 8; ++i) {
    sum += frame[i];
  }
  return static_cast<uint8_t>(sum & 0xFF);
}

void TvocSensor::poll(TvocData& data) {
  while (stream_.available() > 0) {
    consumeByte(static_cast<uint8_t>(stream_.read()), data);
  }
  if (data.health.lastUpdateMs > 0 && millis() - data.health.lastUpdateMs > SENSOR_STALE_MS) {
    data.health.online = false;
    data.health.message = "stale";
  }
}

void TvocSensor::consumeByte(uint8_t value, TvocData& data) {
  if (index_ == 0 && value != 0x2C) {
    return;
  }
  if (index_ == 1 && value != 0xE4) {
    index_ = 0;
    return;
  }

  frame_[index_++] = value;
  if (index_ < sizeof(frame_)) {
    return;
  }

  index_ = 0;
  if (checksum(frame_) != frame_[8]) {
    data.health.errors++;
    data.health.online = false;
    data.health.message = "checksum error";
    return;
  }

  const uint16_t tvocRaw = (static_cast<uint16_t>(frame_[2]) << 8) | frame_[3];
  const uint16_t ch2oRaw = (static_cast<uint16_t>(frame_[4]) << 8) | frame_[5];
  const uint16_t co2Raw = (static_cast<uint16_t>(frame_[6]) << 8) | frame_[7];

  data.tvocMgM3 = tvocRaw * 0.001f;
  data.ch2oMgM3 = ch2oRaw * 0.001f;
  data.co2Ppm = co2Raw;
  data.health.frames++;
  data.health.online = true;
  data.health.lastUpdateMs = millis();
  data.health.message = "ok";
}
