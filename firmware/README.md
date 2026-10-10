# ESP32 固件

当前固件目标是先跑通 ESP32 SoftAP、HTTP API、测试网页和传感器协议解析。

## 默认安全状态

`platformio.ini` 默认只启用 GPS：

- `DRONE_ENABLE_GPS=1`
- `DRONE_ENABLE_TVOC=0`
- `DRONE_ENABLE_DC01=0`
- `DRONE_SIMULATE_DISABLED_SENSORS=1`

TVOC 和 DC01 的 UART TX 在资料中标为 5 V TTL。未确认电平或未加电平转换器前，不要把它们的 TX 接入 ESP32。

## 引脚

| 模块 | ESP32 RX | ESP32 TX | 默认 |
|---|---:|---:|---|
| GPS | GPIO26 | GPIO27 | 启用 |
| TVOC-CO2 | GPIO16 | GPIO17 | 禁用 |
| DC01 | GPIO32 | GPIO33 | 禁用 |

ESP32 只有 3 个硬件 UART，其中 USB 调试占用 UART0。当前集成固件默认使用 UART1 读取 GPS、UART2 预留给 TVOC。DC01 可单独验证，或在最终集成时改用 UART0/外接 UART 扩展/调整调试方案。

## API

- `GET /api/status`
- `GET /api/latest`
- `GET /`

SoftAP 默认：

- SSID: `DroneAQ-ESP32`
- Password: `airquality`
- 地址: `http://192.168.4.1/`

## 开发顺序

1. 安装 PlatformIO 后先烧录默认配置，确认 SoftAP 和页面。
2. 只连接 GPS，室外验证 NMEA、定位、卫星数。
3. TVOC 单独供电预热并测量 TX 高电平；安全后把 `DRONE_ENABLE_TVOC` 改为 `1`。
4. DC01 单独供电并确认 UART 模式、测量 TX 高电平；安全后做单独串口验证。
5. 最后再决定三串口同时集成方案。
