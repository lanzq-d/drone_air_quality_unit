# 硬件连接与上电指南

## 重要电气限制

ESP32 GPIO 是 3.3 V 逻辑输入，不能直接承受 5 V。

资料显示 TVOC-CO2 和 DC01 的 UART TX 可能是 5 V TTL。因此在没有电平转换器或实测证明 TX 高电平不超过 3.3 V 前，不要把：

- TVOC TX 接 ESP32 GPIO16
- DC01 TX 接 ESP32 GPIO32

## 供电结构

```text
无人机电池
  -> 分电盘
    -> 5 V DC-DC 降压模块
      -> ESP32 VIN/5V
      -> GPS VCC
      -> TVOC 5V
      -> DC01 VCC
```

所有模块必须共地：

```text
ESP32 GND = GPS GND = TVOC GND = DC01 GND = DC-DC GND
```

5 V DC-DC 建议额定电流不低于 2 A，输入电压必须覆盖无人机电池满电电压。

## ESP32 引脚规划

| 模块 | 模块引脚 | ESP32 引脚 | 说明 |
|---|---|---:|---|
| GPS | TX | GPIO26 | GPS 输出到 ESP32，优先验证 |
| GPS | RX | GPIO27 | ESP32 输出到 GPS，可先不接 |
| GPS | VCC | 5V/VIN | 上电前确认 GPS 板载稳压 |
| GPS | GND | GND | 共地 |
| TVOC | TX | GPIO16 | 必须先经过电平转换或确认 3.3 V |
| TVOC | RX | GPIO17 | 初期可不接 |
| TVOC | 5V | 5V | 需预热 5 至 10 分钟 |
| TVOC | GND | GND | 共地 |
| DC01 | TXD | GPIO32 | 必须先经过电平转换或确认 3.3 V |
| DC01 | RXD | GPIO33 | 初期可不接 |
| DC01 | VCC | 5V | 先确认 UART 模式 |
| DC01 | GND | GND | 共地 |

## 第一次上电顺序

1. 不接 TVOC TX、DC01 TX。
2. 只接 ESP32 USB，烧录默认固件。
3. 连接 ESP32 SoftAP，确认网页可打开。
4. 只接 GPS 的 VCC/GND/TX，观察定位数据。
5. TVOC 单独接 5 V/GND，预热后测 TX 高电平。
6. DC01 单独接 5 V/GND，确认输出模式并测 TX 高电平。
7. 接入电平转换器后，再连接 TVOC TX 和 DC01 TX。

## 飞行前检查

- ESP32、传感器、降压模块固定牢靠。
- 传感器进气口不被机身遮挡。
- 所有 GND 共地。
- 5 V 输出稳定。
- TVOC/DC01 UART 电平安全。
- 地面静态运行至少 10 分钟无重启、无明显丢帧。
- GPS 已定位后再开始采样任务。
