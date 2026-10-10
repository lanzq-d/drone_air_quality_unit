# 硬件接线方案

## 供电

```text
电池 -> 分电盘 -> 5 V DC-DC -> ESP32 5V/VIN
                              -> TVOC 5V
                              -> DC01 VCC
                              -> GPS VCC
```

所有模块 GND 与 ESP32 GND 共地。降压模块输入必须覆盖电池满电电压，输出稳定 5 V，建议额定电流不低于 2 A。

## 初步引脚规划

| 功能 | ESP32 引脚 | 说明 |
|---|---:|---|
| TVOC RX | GPIO16 | 传感器 TX 输入，正式使用需电平转换 |
| TVOC TX | GPIO17 | 初期可不接 |
| GPS RX | GPIO26 | GPS TX，通常可直接连接 |
| GPS TX | GPIO27 | GPS RX |
| DC01 RX | GPIO32 | 传感器 TX 输入，正式使用需电平转换 |
| DC01 TX | GPIO33 | 初期可不接 |
| USB 调试 | GPIO1/GPIO3 | 保留 |

## 无电阻时的临时方案

只能进行：

- ESP32 单独上电
- GPS 连接测试
- 网页服务器测试
- 传感器独立供电测试
- 使用 USB/示波器验证传感器串口电平

不能进行：

- 未确认电平前将 TVOC TX 接 GPIO16
- 未确认电平前将 DC01 TX 接 GPIO32
- 连接后直接进行飞行测试

## 正式连接原则

TVOC 和 DC01 的 TX 必须经过 5 V 到 3.3 V 的电平转换，或实测证明其高电平小于等于 3.3 V 后才可直连。
