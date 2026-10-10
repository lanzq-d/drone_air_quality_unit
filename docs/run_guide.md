# 工程运行指南

## 目录说明

```text
backend/    本地模拟后端，负责电脑端调试、模拟数据、历史数据存储
frontend/   静态前端页面，可由本地后端或 ESP32 提供
firmware/   ESP32 PlatformIO 固件工程
hardware/   接线规划
docs/       项目文档和资料
```

## 电脑端模拟运行

适合没有连接 ESP32 时开发前端和验证数据展示。

1. 打开 PowerShell。
2. 进入工程目录：

```powershell
cd D:\drone_air_quality_unit
```

3. 启动本地服务：

```powershell
node .\backend\server.js
```

4. 浏览器打开：

```text
http://localhost:5178
```

5. 查看接口：

```text
http://localhost:5178/api/latest
http://localhost:5178/api/history?limit=100
http://localhost:5178/api/summary?limit=100
```

## ESP32 固件运行

推荐使用 VS Code + PlatformIO。

1. 安装 VS Code。
2. 安装 PlatformIO 扩展。
3. 用 VS Code 打开：

```text
D:\drone_air_quality_unit\firmware
```

4. 连接 ESP32 USB。
5. 在 Windows 设备管理器确认 COM 端口。
6. PlatformIO 中选择 `esp32dev` 环境并执行 Upload。
7. 烧录后打开串口监视器，波特率 `115200`。
8. 电脑或手机连接 Wi-Fi：

```text
SSID: DroneAQ-ESP32
Password: airquality
```

9. 浏览器打开：

```text
http://192.168.4.1/
```

## 当前默认固件行为

默认配置只启用 GPS，TVOC 和 DC01 硬件读取关闭：

```ini
DRONE_ENABLE_GPS=1
DRONE_ENABLE_TVOC=0
DRONE_ENABLE_DC01=0
DRONE_SIMULATE_DISABLED_SENSORS=1
```

这样做是为了避免在 UART 电平未确认前，把 5 V TTL 输出直接接入 ESP32。

确认 TVOC 或 DC01 的 TX 高电平安全，或接入 5 V/3.3 V 电平转换模块后，再逐个改为 `1`。

## 推荐开发顺序

1. 只烧录 ESP32，确认 SoftAP 和网页。
2. 只连接 GPS，确认 NMEA 输出、定位和卫星数。
3. TVOC 单独供电预热，测量 TX 高电平，再接入 ESP32。
4. DC01 单独供电，确认 UART 模式和 TX 高电平，再接入 ESP32。
5. 完成三模块集成和长时间静态测试。
