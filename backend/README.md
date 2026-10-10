# 本地模拟后端

这个目录提供一个不依赖第三方 npm 包的本地模拟服务，用于没有 ESP32 或还没烧录固件时开发前端。

运行：

```powershell
node .\backend\server.js
```

访问：

```text
http://localhost:5178
```

接口：

- `GET /api/latest`
- `GET /api/status`
- `GET /api/history?limit=300`
- `GET /api/summary?limit=300`

返回字段与 ESP32 固件 API 保持一致。

## 数据存储

服务每秒最多保存一条紧凑读数到：

```text
backend/readings.jsonl
```

每一行是一条 JSON 记录，适合直接导入 Python、Excel、数据库或后续上传服务器。
