const http = require("http");
const fs = require("fs");
const path = require("path");

const PORT = Number(process.env.PORT || 5178);
const FRONTEND_DIR = path.resolve(__dirname, "../frontend");
const LOG_FILE = path.resolve(__dirname, "readings.jsonl");
const startedAt = Date.now();
const memoryHistory = [];
let lastPersistedAt = 0;

function sampleState() {
  const t = Date.now();
  const seconds = Math.floor((t - startedAt) / 1000);
  return {
    firmware: "local-simulator",
    uptime_ms: t - startedAt,
    gps: {
      health: {
        online: true,
        frames: seconds,
        errors: 0,
        last_update_ms: t - startedAt,
        message: seconds > 8 ? "fix" : "warming up",
      },
      fix: seconds > 8,
      latitude: 30.2741 + Math.sin(seconds / 20) * 0.0002,
      longitude: 120.1551 + Math.cos(seconds / 22) * 0.0002,
      altitude_m: 38.5 + Math.sin(seconds / 10) * 1.2,
      speed_kmh: 0.8 + Math.abs(Math.sin(seconds / 6)) * 2,
      satellites: Math.min(14, 4 + Math.floor(seconds / 3)),
      utc_time: new Date(t).toISOString().slice(11, 19).replaceAll(":", ""),
    },
    tvoc: {
      health: {
        online: seconds > 5,
        frames: Math.max(0, seconds - 5),
        errors: 0,
        last_update_ms: t - startedAt,
        message: seconds > 5 ? "ok" : "preheating",
      },
      tvoc_mg_m3: 0.12 + Math.sin(seconds / 9) * 0.018,
      ch2o_mg_m3: 0.018 + Math.cos(seconds / 11) * 0.004,
      co2_ppm: 420 + Math.round(Math.sin(seconds / 13) * 18),
    },
    dc01: {
      health: {
        online: seconds > 3,
        frames: Math.max(0, seconds - 3),
        errors: 0,
        last_update_ms: t - startedAt,
        message: seconds > 3 ? "ok" : "waiting",
      },
      pm_ug_m3: 18 + Math.round(Math.abs(Math.sin(seconds / 7)) * 16),
    },
  };
}

function compactReading(state) {
  return {
    timestamp: new Date().toISOString(),
    uptime_ms: state.uptime_ms,
    tvoc_mg_m3: state.tvoc.tvoc_mg_m3,
    ch2o_mg_m3: state.tvoc.ch2o_mg_m3,
    co2_ppm: state.tvoc.co2_ppm,
    pm_ug_m3: state.dc01.pm_ug_m3,
    gps_fix: state.gps.fix,
    latitude: state.gps.latitude,
    longitude: state.gps.longitude,
    satellites: state.gps.satellites,
  };
}

function persistReading(state) {
  const now = Date.now();
  if (now - lastPersistedAt < 1000) return;
  lastPersistedAt = now;

  const reading = compactReading(state);
  memoryHistory.push(reading);
  if (memoryHistory.length > 600) memoryHistory.shift();
  fs.appendFile(LOG_FILE, `${JSON.stringify(reading)}\n`, (err) => {
    if (err) console.error("Failed to persist reading:", err.message);
  });
}

function readHistory(limit = 300) {
  if (!fs.existsSync(LOG_FILE)) return memoryHistory.slice(-limit);
  const text = fs.readFileSync(LOG_FILE, "utf8").trim();
  if (!text) return memoryHistory.slice(-limit);
  const rows = text
    .split(/\r?\n/)
    .slice(-limit)
    .map((line) => {
      try {
        return JSON.parse(line);
      } catch {
        return null;
      }
    })
    .filter(Boolean);
  if (rows.length === 0) return memoryHistory.slice(-limit);
  return rows;
}

function average(rows, key) {
  const values = rows.map((row) => row[key]).filter((value) => typeof value === "number");
  if (values.length === 0) return null;
  return values.reduce((sum, value) => sum + value, 0) / values.length;
}

function summary(rows) {
  return {
    count: rows.length,
    first_timestamp: rows[0]?.timestamp ?? null,
    last_timestamp: rows.at(-1)?.timestamp ?? null,
    averages: {
      tvoc_mg_m3: average(rows, "tvoc_mg_m3"),
      ch2o_mg_m3: average(rows, "ch2o_mg_m3"),
      co2_ppm: average(rows, "co2_ppm"),
      pm_ug_m3: average(rows, "pm_ug_m3"),
    },
  };
}

function contentType(filePath) {
  const ext = path.extname(filePath);
  if (ext === ".html") return "text/html; charset=utf-8";
  if (ext === ".css") return "text/css; charset=utf-8";
  if (ext === ".js") return "text/javascript; charset=utf-8";
  if (ext === ".json") return "application/json; charset=utf-8";
  return "application/octet-stream";
}

function sendJson(res, status, value) {
  res.writeHead(status, {
    "Content-Type": "application/json; charset=utf-8",
    "Access-Control-Allow-Origin": "*",
    "Cache-Control": "no-store",
  });
  res.end(JSON.stringify(value));
}

const server = http.createServer((req, res) => {
  const url = new URL(req.url, `http://${req.headers.host}`);

  if (url.pathname === "/api/status" || url.pathname === "/api/latest") {
    const state = sampleState();
    persistReading(state);
    sendJson(res, 200, state);
    return;
  }

  if (url.pathname === "/api/history") {
    const limit = Math.max(1, Math.min(2000, Number(url.searchParams.get("limit") || 300)));
    sendJson(res, 200, { readings: readHistory(limit) });
    return;
  }

  if (url.pathname === "/api/summary") {
    const limit = Math.max(1, Math.min(2000, Number(url.searchParams.get("limit") || 300)));
    const rows = readHistory(limit);
    sendJson(res, 200, summary(rows));
    return;
  }

  const requested = url.pathname === "/" ? "/index.html" : url.pathname;
  const filePath = path.resolve(FRONTEND_DIR, `.${requested}`);
  if (!filePath.startsWith(FRONTEND_DIR)) {
    sendJson(res, 403, { error: "forbidden" });
    return;
  }

  fs.readFile(filePath, (err, data) => {
    if (err) {
      sendJson(res, 404, { error: "not_found" });
      return;
    }
    res.writeHead(200, {
      "Content-Type": contentType(filePath),
      "Cache-Control": "no-store",
    });
    res.end(data);
  });
});

server.listen(PORT, () => {
  console.log(`DroneAQ simulator running at http://localhost:${PORT}`);
});
