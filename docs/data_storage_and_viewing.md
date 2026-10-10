# 数据存储与查看

## 后端能否存储数据

可以。当前本地模拟后端已经加入轻量级存储，启动后每秒最多保存一条读数。

存储文件：

```text
D:\drone_air_quality_unit\backend\readings.jsonl
```

格式是 JSON Lines：一行一条记录。相比普通 JSON 数组，它更适合持续追加和后续导入分析工具。

## 如何生成数据

启动本地后端：

```powershell
cd D:\drone_air_quality_unit
node .\backend\server.js
```

浏览器打开前端：

```text
http://localhost:5178
```

前端每秒请求一次 `/api/latest`，后端会同步追加一条记录。

## 如何查看最新数据

浏览器访问：

```text
http://localhost:5178/api/latest
```

## 如何查看历史数据

查看最近 100 条：

```text
http://localhost:5178/api/history?limit=100
```

返回结构：

```json
{
  "readings": [
    {
      "timestamp": "2026-09-29T01:45:02.000Z",
      "uptime_ms": 821,
      "tvoc_mg_m3": 0.12,
      "ch2o_mg_m3": 0.022,
      "co2_ppm": 420,
      "pm_ug_m3": 18,
      "gps_fix": false,
      "latitude": 30.2741,
      "longitude": 120.1553,
      "satellites": 4
    }
  ]
}
```

## 如何查看汇总

查看最近 300 条的平均值：

```text
http://localhost:5178/api/summary?limit=300
```

## 如何直接打开存储文件

PowerShell：

```powershell
Get-Content .\backend\readings.jsonl -Tail 20
```

如果要导入 Excel，后续可以增加 CSV 导出接口。目前 JSONL 更适合开发阶段保留完整字段。

## 后续真实设备存储方案

当前 ESP32 固件只提供实时 API，还没有在板端持久化。推荐路线：

1. 调试阶段：电脑后端轮询 ESP32 `/api/latest` 并保存。
2. 飞行阶段轻量方案：ESP32 内存缓存最近 N 条，落地后导出。
3. 飞行阶段完整方案：增加 SD 卡模块或把数据通过 Wi-Fi/串口发给伴随设备保存。

如果不加 SD 卡，ESP32 断电后板端缓存会丢失。因此正式采样建议使用电脑端、手机端或独立记录模块保存数据。
