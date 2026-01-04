# 🖤🛣️ BlackRoad CEO Hub v2.0

**The Ultimate CEO Control System for ESP32**

[![Build](https://github.com/BlackRoad-OS/blackroad-ceo-hub/actions/workflows/build.yml/badge.svg)](https://github.com/BlackRoad-OS/blackroad-ceo-hub/actions/workflows/build.yml)
[![Release](https://github.com/BlackRoad-OS/blackroad-ceo-hub/actions/workflows/release.yml/badge.svg)](https://github.com/BlackRoad-OS/blackroad-ceo-hub/actions/workflows/release.yml)
[![OTA Server](https://github.com/BlackRoad-OS/blackroad-ceo-hub/actions/workflows/ota-server.yml/badge.svg)](https://github.com/BlackRoad-OS/blackroad-ceo-hub/actions/workflows/ota-server.yml)
[![License](https://img.shields.io/badge/License-BlackRoad%20OS-hotpink)](LICENSE)

Transform your ESP32-2432S028R into a powerful executive dashboard with real-time metrics, touch navigation, and live data sync.

**🌐 OTA Server:** [blackroad-ceo-hub-ota.pages.dev](https://blackroad-ceo-hub-ota.pages.dev)
**📦 Latest Release:** [Download Firmware](https://github.com/BlackRoad-OS/blackroad-ceo-hub/releases/latest)

## ✨ Features

### v2.0 Major Enhancements

- **📡 WiFi + WebSocket** - Real-time data sync with BlackRoad Operator backend
- **👆 Touch Support** - Full touchscreen with swipe gestures (left/right to navigate)
- **📊 Data Visualizations** - Live charts and progress bars
- **🔔 Notifications** - Alert system with color-coded messages
- **🎨 Smooth UI** - Enhanced animations and transitions
- **📈 Live Metrics** - Real-time system stats from operator
- **⚡ Performance** - Optimized rendering and memory management

### Screens

1. **🏠 HOME** - System overview with quick stats and metrics
2. **📊 PROJECTS** - 30K+ project dashboard with activity charts
3. **🤖 AI** - Agent status monitoring (Lucidia, Roadie, Athena, etc.)
4. **💰 FINANCE** - RoadCoin price tracker with 24h charts
5. **🎨 STUDIO** - Creator tools status (Canvas, Video, Music, etc.)
6. **⚙️ SETTINGS** - Network status, system info, diagnostics

### UI Components

- **Status Bar** - WiFi, WebSocket, and uptime indicators
- **Navigation Bar** - Quick access to all screens with visual feedback
- **Progress Bars** - Visual metric representation
- **Mini Charts** - 30-day activity graphs and price charts
- **Notifications** - Slide-in alerts with auto-dismiss

## 🔧 Hardware Requirements

- **Board**: ESP32-2432S028R (Cheap Yellow Display)
- **Display**: 2.8" ILI9341 (240x320 pixels)
- **Touch**: Resistive touchscreen (XPT2046)
- **Connection**: USB-C for programming and power

## 📦 Installation

### 1. Install PlatformIO

```bash
# Install PlatformIO CLI (if not already installed)
pip install platformio

# Or use PlatformIO IDE extension for VS Code
```

### 2. Clone and Configure

```bash
# Navigate to project directory
cd ~/sensecap-watcher-operator

# Update WiFi credentials in src/main.cpp (lines 48-49)
const char* WIFI_SSID = "your-wifi-ssid";
const char* WIFI_PASSWORD = "your-wifi-password";

# Update backend server (line 52) if needed
const char* WS_HOST = "192.168.4.74";  // operator-watcher-server IP
```

### 3. Build and Upload

```bash
# Install dependencies
pio pkg install

# Build project
pio run

# Upload to ESP32
pio run --target upload

# Monitor serial output
pio device monitor
```

## 🚀 Quick Start

### First Boot

1. **Connect ESP32** to computer via USB
2. **Upload firmware** using PlatformIO
3. **Watch boot sequence** on display:
   - BlackRoad logo appears
   - WiFi connection attempt
   - WebSocket handshake (if WiFi connected)
   - Home screen loads

### Navigation

**Touch Screen:**
- **Swipe Left** - Next screen
- **Swipe Right** - Previous screen
- **Tap Icon** - Jump to specific screen (bottom navigation bar)

**Serial Commands** (via Serial Monitor):
- `n` - Next screen
- `p` - Previous screen

### Network Configuration

The CEO Hub connects to:
1. **Your WiFi Network** - Configure SSID/password in code
2. **Operator Backend** - WebSocket server at configured IP
3. **Data Updates** - Real-time metrics every 5 seconds

If WiFi/WebSocket unavailable, the app works offline with simulated data.

## 📡 Backend Integration

### Expected WebSocket Messages

**Metrics Request (from ESP32):**
```json
{
  "type": "getMetrics"
}
```

**Metrics Response (from server):**
```json
{
  "projects": 30247,
  "agents": 15892,
  "roadcoin": 0.42,
  "change24h": 5.23,
  "cpu": 45,
  "memory": 67,
  "network": 2048
}
```

### Setting Up Backend Server

The CEO Hub expects a WebSocket server at `ws://<WS_HOST>:8080/ws`. You can use:

1. **operator-watcher-server** (Node.js) - Already available at 192.168.4.74
2. **Custom WebSocket Server** - Implement metrics endpoint

Example Node.js WebSocket server:

```javascript
const WebSocket = require('ws');
const wss = new WebSocket.Server({ port: 8080, path: '/ws' });

wss.on('connection', (ws) => {
  console.log('CEO Hub connected!');

  ws.on('message', (data) => {
    const msg = JSON.parse(data);

    if (msg.type === 'getMetrics') {
      // Send metrics data
      ws.send(JSON.stringify({
        projects: 30247,
        agents: 15892,
        roadcoin: 0.42,
        change24h: 5.23,
        cpu: 45,
        memory: 67,
        network: 2048
      }));
    }
  });
});
```

## 🎨 BlackRoad Brand Colors

Official palette (RGB565 format):

```cpp
#define COLOR_HOT_PINK   0xF8B6  // #FF1D6C - Primary brand color
#define COLOR_AMBER      0xFD20  // #F5A623 - Secondary accent
#define COLOR_BLUE       0x24DF  // #2979FF - Electric blue
#define COLOR_VIOLET     0x9136  // #9C27B0 - Purple accent
#define COLOR_BLACK      0x0000  // Background
#define COLOR_WHITE      0xFFFF  // Text
```

All UI elements follow the official BlackRoad brand system.

## 🛠️ Development

### File Structure

```
sensecap-watcher-operator/
├── src/
│   └── main.cpp           # Main application (1100+ lines)
├── platformio.ini         # PlatformIO configuration
└── README.md             # This file
```

### Key Functions

**UI Drawing:**
- `drawHomeScreen()` - System overview
- `drawProjectsScreen()` - Project dashboard with charts
- `drawAIScreen()` - Agent monitoring
- `drawFinanceScreen()` - RoadCoin tracker
- `drawStudioScreen()` - Creator tools
- `drawSettingsScreen()` - System configuration

**Components:**
- `drawProgressBar()` - Animated progress indicators
- `drawMiniChart()` - Bar chart visualization
- `drawNotification()` - Alert system

**Navigation:**
- `switchScreen()` - Screen transitions
- `handleTouch()` - Touch input processing
- `checkSwipeGesture()` - Swipe gesture detection

**Networking:**
- `connectWiFi()` - WiFi connection
- `connectWebSocket()` - WebSocket handshake
- `parseMetricsData()` - JSON data parsing

### Customization

**Add New Screen:**

```cpp
// 1. Add enum value
enum Screen {
  // ... existing screens
  SCREEN_YOUR_NEW_SCREEN,
  SCREEN_COUNT
};

// 2. Add name and icon
const char* screenNames[] = { /* ... */ "YOUR_SCREEN" };
const char* screenIcons[] = { /* ... */ "🔥" };

// 3. Implement draw function
void drawYourNewScreen() {
  int y = 60;
  tft.setTextColor(COLOR_AMBER, COLOR_BLACK);
  tft.setTextSize(2);
  tft.setCursor(30, y);
  tft.println("YOUR SCREEN");
  // ... add your content
}

// 4. Add to switch in loop()
case SCREEN_YOUR_NEW_SCREEN: drawYourNewScreen(); break;
```

## 📊 Performance

- **Boot Time**: ~3 seconds (including WiFi)
- **Screen Switch**: <100ms
- **Touch Response**: <50ms
- **Data Update**: 5 second interval
- **Memory Usage**: ~40KB RAM (240KB available)
- **CPU Usage**: <10% average

## 🐛 Troubleshooting

### Display Issues

**Blank Screen:**
- Check backlight pin (21) is HIGH
- Verify TFT_eSPI configuration in platformio.ini
- Test with TFT_eSPI examples first

**Wrong Colors:**
- Verify RGB565 color format
- Check display driver (ILI9341)

### Touch Problems

**No Touch Response:**
- Calibrate touch: Update `calData` array in setup()
- Test touch coordinates via Serial Monitor
- Verify TOUCH_CS pin (33)

**Inaccurate Touch:**
- Re-calibrate using TFT_eSPI calibration sketch
- Update calibration data in `setup()`

### Network Issues

**WiFi Won't Connect:**
- Double-check SSID and password
- Ensure 2.4GHz network (ESP32 doesn't support 5GHz)
- Check WiFi signal strength

**WebSocket Fails:**
- Verify server is running at configured IP/port
- Check firewall settings
- Monitor serial output for connection logs

**No Data Updates:**
- Confirm WebSocket is connected (green WS indicator)
- Check server is sending correct JSON format
- Monitor serial for parse errors

## 📝 Serial Monitor Output

Expected log output:

```
══════════════════════════════════════════
   🖤🛣️ BLACKROAD CEO HUB v2.0 🛣️🖤
══════════════════════════════════════════
Hardware: ESP32-2432S028R
Display: 2.8" ILI9341 240x320 Touch
══════════════════════════════════════════

Connecting to WiFi: YourNetwork
✓ WiFi connected!
IP: 192.168.4.100
Connecting to WebSocket: ws://192.168.4.74:8080/ws
✓ WebSocket Connected
✓ CEO Hub v2.0 ready!
Touch screen to navigate

Touch: x=120, y=300
→ Screen: PROJECTS
Swipe LEFT
→ Screen: AI
← Received: {"projects":30247,"agents":15892,...}
📢 Notification: Server connected
```

## 🚢 Deployment

### Production Build

```bash
# Build optimized firmware
pio run --environment esp32dev

# Flash to device
pio run --target upload

# Verify
pio device monitor --baud 115200
```

### OTA Updates (Future)

Future versions will support over-the-air updates via WiFi.

## 📖 Version History

### v2.0.0 (2026-01-03)
- ✨ Complete rewrite with enhanced features
- 📡 WiFi + WebSocket real-time sync
- 👆 Full touch support with swipe gestures
- 📊 Data visualizations (charts, progress bars)
- 🔔 Notification system
- 🎨 Official BlackRoad brand colors
- ⚡ Performance optimizations

### v1.0.0 (Previous)
- Basic 6-screen interface
- Serial navigation only
- Simulated data
- Static display

## 🤝 Contributing

This is part of the BlackRoad OS ecosystem. Contributions welcome!

## 📄 License

BlackRoad OS, Inc. - All Rights Reserved

---

**"You bring your chaos. BlackRoad brings structure, compute, and care."**

🖤🛣️ **The road remembers everything.** 🛣️🖤
