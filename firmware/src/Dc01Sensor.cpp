#include "Dc01Sensor.h"
#include "Config.h"

Dc01Sensor::Dc01Sensor(Stream& stream) : stream_(stream) {}

void Dc01Sensor::begin() {}

uint8_t Dc01Sensor::checksum(const uint8_t* frame) const {
  return static_cast<uint8_t>((frame[0] + frame[1] + frame[2]) & 0xFF);
}

void Dc01Sensor::poll(Dc01Data& data) {
  while (stream_.available() > 0) {
    consumeByte(static_cast<uint8_t>(stream_.read()), data);
  }
  if (data.health.lastUpdateMs > 0 && millis() - data.health.lastUpdateMs > SENSOR_STALE_MS) {
    data.health.online = false;
    data.health.message = "stale";
  }
}

void Dc01Sensor::consumeByte(uint8_t value, Dc01Data& data) {
  if (index_ == 0 && value != 0xA5) {
    return;
  }

  frame_[index_++] = value;
  if (index_ < sizeof(frame_)) {
    return;
  }

  index_ = 0;
  if (checksum(frame_) != frame_[3]) {
    data.health.errors++;
    data.health.online = false;
    data.health.message = "checksum error";
    return;
  }

  data.pmUgM3 = static_cast<uint16_t>(frame_[1]) * 128 + frame_[2];
  data.health.frames++;
  data.health.online = true;
  data.health.lastUpdateMs = millis();
  data.health.message = "ok";
}
