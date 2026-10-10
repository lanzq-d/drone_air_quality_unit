#include "GpsSensor.h"
#include "Config.h"
#include <stdlib.h>
#include <string.h>

GpsSensor::GpsSensor(Stream& stream) : stream_(stream) {}

void GpsSensor::begin() {}

void GpsSensor::poll(GpsData& data) {
  while (stream_.available() > 0) {
    consumeByte(static_cast<char>(stream_.read()), data);
  }
  if (data.health.lastUpdateMs > 0 && millis() - data.health.lastUpdateMs > SENSOR_STALE_MS) {
    data.health.online = false;
    data.health.message = "stale";
  }
}

void GpsSensor::consumeByte(char value, GpsData& data) {
  if (value == '\r') {
    return;
  }
  if (value == '\n') {
    if (index_ > 0) {
      line_[index_] = '\0';
      parseLine(line_, data);
      index_ = 0;
    }
    return;
  }
  if (index_ < sizeof(line_) - 1) {
    line_[index_++] = value;
  } else {
    index_ = 0;
    data.health.errors++;
    data.health.message = "line overflow";
  }
}

bool GpsSensor::validNmeaChecksum(const char* line) const {
  if (line[0] != '$') {
    return false;
  }
  const char* star = strchr(line, '*');
  if (star == nullptr || strlen(star) < 3) {
    return false;
  }

  uint8_t sum = 0;
  for (const char* p = line + 1; p < star; ++p) {
    sum ^= static_cast<uint8_t>(*p);
  }
  char expected[3] = {star[1], star[2], '\0'};
  return sum == static_cast<uint8_t>(strtoul(expected, nullptr, 16));
}

char* GpsSensor::tokenAt(char* line, uint8_t wanted) {
  uint8_t index = 0;
  char* token = strtok(line, ",");
  while (token != nullptr) {
    if (index == wanted) {
      return token;
    }
    index++;
    token = strtok(nullptr, ",");
  }
  return nullptr;
}

double GpsSensor::parseCoordinate(const char* value, const char* hemisphere) const {
  if (value == nullptr || hemisphere == nullptr || value[0] == '\0') {
    return 0;
  }

  const double raw = atof(value);
  const int degrees = static_cast<int>(raw / 100);
  const double minutes = raw - degrees * 100;
  double decimal = degrees + minutes / 60.0;
  if (hemisphere[0] == 'S' || hemisphere[0] == 'W') {
    decimal = -decimal;
  }
  return decimal;
}

void GpsSensor::parseLine(char* line, GpsData& data) {
  if (!validNmeaChecksum(line)) {
    data.health.errors++;
    data.health.message = "checksum error";
    return;
  }

  data.health.frames++;
  data.health.online = true;
  data.health.lastUpdateMs = millis();
  data.health.message = "nmea";

  char copy[128] = {};
  strncpy(copy, line, sizeof(copy) - 1);

  if (strstr(line, "GGA") != nullptr) {
    char* time = tokenAt(copy, 1);
    char copy2[128] = {};
    strncpy(copy2, line, sizeof(copy2) - 1);
    char* lat = tokenAt(copy2, 2);
    char copy3[128] = {};
    strncpy(copy3, line, sizeof(copy3) - 1);
    char* ns = tokenAt(copy3, 3);
    char copy4[128] = {};
    strncpy(copy4, line, sizeof(copy4) - 1);
    char* lon = tokenAt(copy4, 4);
    char copy5[128] = {};
    strncpy(copy5, line, sizeof(copy5) - 1);
    char* ew = tokenAt(copy5, 5);
    char copy6[128] = {};
    strncpy(copy6, line, sizeof(copy6) - 1);
    char* quality = tokenAt(copy6, 6);
    char copy7[128] = {};
    strncpy(copy7, line, sizeof(copy7) - 1);
    char* sats = tokenAt(copy7, 7);
    char copy8[128] = {};
    strncpy(copy8, line, sizeof(copy8) - 1);
    char* alt = tokenAt(copy8, 9);

    data.utcTime = time == nullptr ? "" : String(time);
    data.fix = quality != nullptr && atoi(quality) > 0;
    data.latitude = parseCoordinate(lat, ns);
    data.longitude = parseCoordinate(lon, ew);
    data.satellites = sats == nullptr ? 0 : static_cast<uint8_t>(atoi(sats));
    data.altitudeM = alt == nullptr ? 0 : atof(alt);
    data.health.message = data.fix ? "fix" : "no fix";
    return;
  }

  if (strstr(line, "RMC") != nullptr) {
    char copyStatus[128] = {};
    strncpy(copyStatus, line, sizeof(copyStatus) - 1);
    char* status = tokenAt(copyStatus, 2);
    char copySpeed[128] = {};
    strncpy(copySpeed, line, sizeof(copySpeed) - 1);
    char* speedKnots = tokenAt(copySpeed, 7);
    if (status != nullptr) {
      data.fix = status[0] == 'A';
    }
    if (speedKnots != nullptr) {
      data.speedKmh = atof(speedKnots) * 1.852;
    }
  }
}
