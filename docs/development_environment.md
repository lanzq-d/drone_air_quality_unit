# 开发环境

## 固件

推荐使用：

- Visual Studio Code
- PlatformIO 扩展
- Espressif 32 平台
- Arduino framework
- USB 串口驱动

固件目标：

- ESP32-WROOM-32
- 读取 TVOC-CO2
- 读取 DC01
- 读取 NEO-M8N
- 提供 HTTP API 和 WebSocket/SSE 实时数据

## 前端

推荐使用：

- Node.js LTS
- npm
- Vite
- React
- TypeScript

前端开发阶段可使用本地模拟数据；部署阶段将前端静态文件由 ESP32 提供，或由独立开发服务器提供。

## 第一个开发聊天需要准备

- ESP32 通过 USB 连接电脑
- Windows 设备管理器中的 COM 端口号
- GPS 开发板照片中 VCC/RX/TX/GND 的实际方向
- DC01 连接器线序
- TVOC 模块连接器线序
- 5 V DC-DC 是否已经具备
- 是否已经有可测量 UART 电平的万用表或示波器
