const apiStatus = document.getElementById("apiStatus");
const apiText = document.getElementById("apiText");
const moduleList = document.getElementById("moduleList");
const chart = document.getElementById("trendChart");
const chartContext = chart.getContext("2d");
const history = [];

const limits = {
  tvoc: { good: 0.6, warn: 1.0, unit: "mg/m3", label: "TVOC" },
  ch2o: { good: 0.1, warn: 0.2, unit: "mg/m3", label: "甲醛" },
  co2: { good: 1000, warn: 1500, unit: "ppm", label: "CO2" },
  pm: { good: 35, warn: 75, unit: "ug/m3", label: "颗粒物" },
};

const fields = {
  tvoc: document.getElementById("tvocValue"),
  ch2o: document.getElementById("ch2oValue"),
  co2: document.getElementById("co2Value"),
  pm: document.getElementById("pmValue"),
  lat: document.getElementById("latValue"),
  lon: document.getElementById("lonValue"),
  sat: document.getElementById("satValue"),
  speed: document.getElementById("speedValue"),
  gps: document.getElementById("gpsState"),
  uptime: document.getElementById("uptimeValue"),
  grade: document.getElementById("gradeValue"),
  analysis: document.getElementById("analysisText"),
  cards: {
    tvoc: document.getElementById("tvocCard"),
    ch2o: document.getElementById("ch2oCard"),
    co2: document.getElementById("co2Card"),
    pm: document.getElementById("pmCard"),
  },
  hints: {
    tvoc: document.getElementById("tvocHint"),
    ch2o: document.getElementById("ch2oHint"),
    co2: document.getElementById("co2Hint"),
    pm: document.getElementById("pmHint"),
  },
};

function fmt(value, digits = 2) {
  if (typeof value !== "number" || Number.isNaN(value)) return "--";
  return value.toFixed(digits);
}

function setConnection(ok, text) {
  apiStatus.classList.toggle("online", ok);
  apiText.textContent = text;
}

function moduleCard(name, health) {
  const online = Boolean(health?.online);
  return `
    <article class="module ${online ? "online" : ""}">
      <header>
        <strong>${name}</strong>
        <span class="state">${online ? "online" : "offline"}</span>
      </header>
      <p>frames ${health?.frames ?? 0} / errors ${health?.errors ?? 0} / ${health?.message ?? "unknown"}</p>
    </article>
  `;
}

function levelFor(key, value) {
  const limit = limits[key];
  if (typeof value !== "number") return "unknown";
  if (value <= limit.good) return "good";
  if (value <= limit.warn) return "warn";
  return "bad";
}

function applyLevel(key, value) {
  const level = levelFor(key, value);
  const limit = limits[key];
  fields.cards[key].classList.remove("good", "warn", "bad", "unknown");
  fields.cards[key].classList.add(level);
  const text = level === "good" ? "正常" : level === "warn" ? "偏高" : level === "bad" ? "超限" : "未知";
  fields.hints[key].textContent = `${text} / 参考：小于 ${limit.good} ${limit.unit}`;
  return { key, level, value, ...limit };
}

function analysisFor(items, state) {
  const rank = { unknown: 0, good: 1, warn: 2, bad: 3 };
  const worst = items.reduce((max, item) => (rank[item.level] > rank[max.level] ? item : max), items[0]);
  const badItems = items.filter((item) => item.level === "bad");
  const warnItems = items.filter((item) => item.level === "warn");

  if (!state.gps.fix) {
    return {
      grade: "等待定位",
      text: "GPS 尚未定位，空气质量数据可先用于地面调试，但飞行轨迹还不能形成有效采样点。",
    };
  }
  if (badItems.length > 0) {
    return {
      grade: "异常",
      text: `${badItems.map((item) => item.label).join("、")} 已超过参考范围，建议停留复测并记录当前位置。`,
    };
  }
  if (warnItems.length > 0) {
    return {
      grade: "关注",
      text: `${warnItems.map((item) => item.label).join("、")} 接近或高于舒适参考值，建议观察趋势是否持续升高。`,
    };
  }
  return {
    grade: "良好",
    text: `当前主要指标处于参考范围内，最高关注项为 ${worst.label}。`,
  };
}

function pushHistory(state) {
  history.push({
    time: Date.now(),
    tvoc: state.tvoc.tvoc_mg_m3,
    ch2o: state.tvoc.ch2o_mg_m3,
    co2: state.tvoc.co2_ppm,
    pm: state.dc01.pm_ug_m3,
  });
  while (history.length > 60) history.shift();
}

function drawSeries(points, key, color, maxValue) {
  if (points.length < 2) return;
  const { width, height } = chart;
  const pad = 34;
  chartContext.beginPath();
  chartContext.strokeStyle = color;
  chartContext.lineWidth = 3;
  points.forEach((point, index) => {
    const x = pad + (index / (points.length - 1)) * (width - pad * 2);
    const y = height - pad - (Math.min(point[key], maxValue) / maxValue) * (height - pad * 2);
    if (index === 0) chartContext.moveTo(x, y);
    else chartContext.lineTo(x, y);
  });
  chartContext.stroke();
}

function drawChart() {
  const { width, height } = chart;
  chartContext.clearRect(0, 0, width, height);
  chartContext.fillStyle = "#ffffff";
  chartContext.fillRect(0, 0, width, height);
  chartContext.strokeStyle = "#d5e0e6";
  chartContext.lineWidth = 1;
  for (let i = 1; i < 5; i++) {
    const y = (height / 5) * i;
    chartContext.beginPath();
    chartContext.moveTo(30, y);
    chartContext.lineTo(width - 16, y);
    chartContext.stroke();
  }
  drawSeries(history, "tvoc", "#2d6a7a", limits.tvoc.warn);
  drawSeries(history, "ch2o", "#8b5a2b", limits.ch2o.warn);
  drawSeries(history, "co2", "#4b64a1", limits.co2.warn);
  drawSeries(history, "pm", "#c54d38", limits.pm.warn);
}

function render(state) {
  fields.tvoc.textContent = fmt(state.tvoc.tvoc_mg_m3, 3);
  fields.ch2o.textContent = fmt(state.tvoc.ch2o_mg_m3, 3);
  fields.co2.textContent = state.tvoc.co2_ppm ?? "--";
  fields.pm.textContent = state.dc01.pm_ug_m3 ?? "--";
  fields.lat.textContent = fmt(state.gps.latitude, 7);
  fields.lon.textContent = fmt(state.gps.longitude, 7);
  fields.sat.textContent = state.gps.satellites ?? "--";
  fields.speed.textContent = `${fmt(state.gps.speed_kmh, 1)} km/h`;
  fields.gps.textContent = state.gps.fix ? "已定位" : "未定位";
  fields.uptime.textContent = `${Math.floor((state.uptime_ms ?? 0) / 1000)} s`;

  const levels = [
    applyLevel("tvoc", state.tvoc.tvoc_mg_m3),
    applyLevel("ch2o", state.tvoc.ch2o_mg_m3),
    applyLevel("co2", state.tvoc.co2_ppm),
    applyLevel("pm", state.dc01.pm_ug_m3),
  ];
  const analysis = analysisFor(levels, state);
  fields.grade.textContent = analysis.grade;
  fields.analysis.textContent = analysis.text;

  pushHistory(state);
  drawChart();

  moduleList.innerHTML = [
    moduleCard("GPS", state.gps.health),
    moduleCard("TVOC-CO2", state.tvoc.health),
    moduleCard("DC01", state.dc01.health),
  ].join("");
}

async function refresh() {
  try {
    const response = await fetch("/api/latest", { cache: "no-store" });
    if (!response.ok) throw new Error(`HTTP ${response.status}`);
    const state = await response.json();
    render(state);
    setConnection(true, "已连接");
  } catch (error) {
    setConnection(false, "未连接");
  }
}

setInterval(refresh, 1000);
refresh();
