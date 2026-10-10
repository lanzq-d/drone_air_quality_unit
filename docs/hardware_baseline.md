# 固定硬件基线 v1.1

更新时间：2026-09-22

## 核心器件

| 器件 | 固定结论 |
|---|---|
| 主控 | ESP32 开发板，核心模组 ESP32-WROOM-32 |
| 气体模块 | TVOC-CO2 三合一，TVOC/CH2O/CO2 |
| 粉尘模块 | DC01；当前采用现有 D01 资料中的协议，后续以实测校验 |
| GPS | GY 系列蓝色开发板，u-blox NEO-M8N-0-10 |
| GPS 天线 | 实物标签 MP510_GPS_V02_WWXL |
| 温湿度传感器 | 未购买，不纳入系统 |

## TVOC-CO2

- 供电：5 V
- 通信：UART，9600 8N1
- 接口：GND、5V、RX、TX
- 数据帧：`2C E4 TVOC_H TVOC_L CH2O_H CH2O_L CO2_H CO2_L CHECKSUM`
- TVOC/CH2O 单位：mg/m3，按文档的 0.001 缩放
- CO2 单位：ppm
- 首次使用建议预热：5 至 10 分钟

## DC01

- 供电：5 V
- 通信：UART，9600 8N1
- 接口：GND、VCC、RXD、TXD/PWM
- UART 数据帧：`A5 DATAH DATAL SUM`
- 浓度计算：`DATAH * 128 + DATAL`
- 文档范围：5 至 2500 ug/m3
- 实际型号由用户确认是 DC01，现有 PDF 文件名为 D01，协议需实测验证

## GPS

- 核心：u-blox NEO-M8N-0-10
- 初始串口配置：9600 8N1
- 初始协议：NMEA
- 供电：按 GY-NEO-8M 类开发板默认使用稳定 5 V 输入
- 实际上电前应确认开发板 VCC 端存在板载稳压

## 电气风险

TVOC-CO2 和 DC01 资料描述为 5 V TTL UART。ESP32 GPIO 为 3.3 V 逻辑且通常不具备 5 V 容忍能力。没有分压电阻时，不能直接连接它们的 TX 到 ESP32 RX。

最低正式方案为增加一个 3.3 V/5 V UART 电平转换器。若暂时没有转换器，固件可以先完成网页、GPS和模拟数据链路，但不得连接未经确认的 5 V TX。
