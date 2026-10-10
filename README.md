# Drone Air Quality Unit

独立的无人机空气质量检测单元工程。

## 固定硬件

- ESP32-WROOM-32 开发板
- TVOC-CO2 三合一空气质量模块
- DC01 粉尘传感器
- GY 系列 NEO-M8N GPS 开发板及天线
- 无独立温湿度传感器

## 重要安全结论

TVOC-CO2 和 DC01 的资料描述其 UART 输出为 5 V TTL。ESP32-WROOM-32 的 GPIO 不是 5 V 容忍输入，不能把这两个模块的 TX 直接接入 ESP32 GPIO。

在没有分压电阻时：

1. 首轮上电只完成供电测试，不连接 TVOC/DC01 的 TX 到 ESP32。
2. 正式采集必须增加逻辑电平转换器，或先实测确认模块 TX 高电平确实不超过 3.3 V。
3. 在确认电平之前，禁止进行飞行测试。

## 开发顺序

1. 核对实物和供电
2. 单独验证 GPS
3. 单独验证 TVOC
4. 单独验证 DC01
5. 集成 ESP32 数据服务
6. 开发前端页面
7. 完成静态和飞行前测试

详细资料见 `docs`、`hardware`、`firmware`、`frontend` 和 `tests`。

## 当前工程状态

- `firmware`: ESP32 PlatformIO 工程，已包含 SoftAP、HTTP API、测试网页、GPS/TVOC/DC01 驱动骨架。
- `backend`: Node.js 本地模拟后端，不依赖第三方包。
- `frontend`: 静态监控页面，可连接本地模拟后端或 ESP32 API。

## 新增文档

- 运行工程：[docs/run_guide.md](docs/run_guide.md)
- 硬件连接：[docs/hardware_connection_guide.md](docs/hardware_connection_guide.md)
- 数据存储与查看：[docs/data_storage_and_viewing.md](docs/data_storage_and_viewing.md)
- 前端监测设计：[docs/frontend_monitoring_design.md](docs/frontend_monitoring_design.md)
