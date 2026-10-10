#pragma once

#include <Arduino.h>

#ifndef DRONE_FW_VERSION
#define DRONE_FW_VERSION "dev"
#endif

#ifndef DRONE_AP_SSID
#define DRONE_AP_SSID "DroneAQ-ESP32"
#endif

#ifndef DRONE_AP_PASSWORD
#define DRONE_AP_PASSWORD "airquality"
#endif

static constexpr uint32_t SENSOR_BAUD = 9600;

static constexpr int PIN_GPS_RX = 26;
static constexpr int PIN_GPS_TX = 27;
static constexpr int PIN_TVOC_RX = 16;
static constexpr int PIN_TVOC_TX = 17;
static constexpr int PIN_DC01_RX = 32;
static constexpr int PIN_DC01_TX = 33;

static constexpr uint32_t SENSOR_STALE_MS = 5000;
