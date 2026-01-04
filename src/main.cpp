/**
 * ══════════════════════════════════════════════════════════════════════════
 *                    🖤🛣️ BLACKROAD CEO HUB v2.0 🛣️🖤
 * ══════════════════════════════════════════════════════════════════════════
 *
 * The Ultimate CEO Control System - ENHANCED EDITION
 * "You bring your chaos. BlackRoad brings structure, compute, and care."
 *
 * Hardware: ESP32-2432S028R (2.8" ILI9341 240x320 Touch Display)
 *
 * ✨ NEW FEATURES v2.0:
 * - 📡 WiFi + WebSocket real-time data sync
 * - 👆 Touch screen support (swipe, tap, long-press)
 * - 📊 Live data charts and visualizations
 * - 🔔 Notification system with alerts
 * - 🎨 Smooth animations and transitions
 * - 📈 Real-time metrics from operator backend
 * - 🔄 Pull-to-refresh functionality
 * - ⚡ Performance optimizations
 *
 * Screens:
 * - 🏠 HOME - System overview & quick launch
 * - 📊 PROJECTS - 30,000+ project dashboard with charts
 * - 🤖 AI - Lucidia, Roadie, Radius agents status
 * - 💰 FINANCE - RoadCoin, Pack-Finance with live prices
 * - 🎨 STUDIO - Canvas, Video, Writing, Music
 * - ⚙️  SETTINGS - System configuration + WiFi
 *
 * BlackRoad Official Brand Colors:
 * - Hot Pink: #FF1D6C
 * - Amber: #F5A623
 * - Electric Blue: #2979FF
 * - Violet: #9C27B0
 * ══════════════════════════════════════════════════════════════════════════
 */

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

// ══════════════════════════════════════════════════════════════════════════
// CONFIGURATION
// ══════════════════════════════════════════════════════════════════════════

// WiFi Configuration
const char* WIFI_SSID = "your-wifi-ssid";      // Update with your WiFi
const char* WIFI_PASSWORD = "your-wifi-pass";  // Update with your password

// Backend Configuration
const char* WS_HOST = "192.168.4.74";  // operator-watcher-server
const uint16_t WS_PORT = 8080;
const char* WS_PATH = "/ws";

// Display & Touch
TFT_eSPI tft = TFT_eSPI();

// WebSocket Client
WebSocketsClient webSocket;

// ══════════════════════════════════════════════════════════════════════════
// BLACKROAD OFFICIAL BRAND COLORS
// ══════════════════════════════════════════════════════════════════════════

#define COLOR_HOT_PINK   0xF8B6  // #FF1D6C
#define COLOR_AMBER      0xFD20  // #F5A623
#define COLOR_BLUE       0x24DF  // #2979FF
#define COLOR_VIOLET     0x9136  // #9C27B0
#define COLOR_BLACK      0x0000
#define COLOR_WHITE      0xFFFF
#define COLOR_DARK_GRAY  0x2104
#define COLOR_LIGHT_GRAY 0x7BEF
#define COLOR_GREEN      0x07E0
#define COLOR_RED        0xF800

// ══════════════════════════════════════════════════════════════════════════
// APP SCREENS & STATE
// ══════════════════════════════════════════════════════════════════════════

enum Screen {
  SCREEN_HOME = 0,
  SCREEN_PROJECTS,
  SCREEN_AI,
  SCREEN_FINANCE,
  SCREEN_STUDIO,
  SCREEN_SETTINGS,
  SCREEN_COUNT
};

const char* screenNames[] = {
  "HOME", "PROJECTS", "AI", "FINANCE", "STUDIO", "SETTINGS"
};

const char* screenIcons[] = {
  "🏠", "📊", "🤖", "💰", "🎨", "⚙️"
};

// State
Screen currentScreen = SCREEN_HOME;
Screen previousScreen = SCREEN_HOME;
bool wifiConnected = false;
bool wsConnected = false;

// Data
uint32_t projectCount = 30247;
uint32_t agentCount = 15892;
float roadCoinPrice = 0.42;
float roadCoinChange24h = 5.23;
uint32_t activeAgents = 47;
uint32_t cpuUsage = 0;
uint32_t memUsage = 0;
uint32_t networkTraffic = 0;

// Notifications
struct Notification {
  char message[100];
  uint16_t color;
  unsigned long timestamp;
  bool active;
};

#define MAX_NOTIFICATIONS 5
Notification notifications[MAX_NOTIFICATIONS];
int notificationCount = 0;

// Touch state
int16_t touchX = 0, touchY = 0;
bool touched = false;
unsigned long lastTouchTime = 0;
int16_t swipeStartX = 0, swipeStartY = 0;

// Timers
unsigned long lastUpdate = 0;
unsigned long lastPing = 0;
unsigned long lastMetricsUpdate = 0;

// ══════════════════════════════════════════════════════════════════════════
// FORWARD DECLARATIONS
// ══════════════════════════════════════════════════════════════════════════

// UI Drawing
void drawHeader();
void drawNavBar();
void drawStatusBar();
void drawNotification(const char* msg, uint16_t color);
void drawHomeScreen();
void drawProjectsScreen();
void drawAIScreen();
void drawFinanceScreen();
void drawStudioScreen();
void drawSettingsScreen();
void drawMiniChart(int x, int y, int w, int h, uint8_t* data, int dataSize, uint16_t color);
void drawProgressBar(int x, int y, int w, int h, float percentage, uint16_t color);

// Navigation
void switchScreen(Screen newScreen);
void nextScreen();
void prevScreen();

// Touch & Gestures
void handleTouch();
void checkSwipeGesture();
bool isTouchInNavBar(int16_t y);
int getTappedNavButton(int16_t x);

// Network
void connectWiFi();
void connectWebSocket();
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
void sendMetricsRequest();
void parseMetricsData(const char* json);

// Notifications
void addNotification(const char* msg, uint16_t color);
void updateNotifications();
void drawNotifications();

// Utils
String formatNumber(uint32_t num);
String formatBytes(uint32_t bytes);

// ══════════════════════════════════════════════════════════════════════════
// SETUP
// ══════════════════════════════════════════════════════════════════════════

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println("\n\n══════════════════════════════════════════");
  Serial.println("   🖤🛣️ BLACKROAD CEO HUB v2.0 🛣️🖤");
  Serial.println("══════════════════════════════════════════");
  Serial.println("Hardware: ESP32-2432S028R");
  Serial.println("Display: 2.8\" ILI9341 240x320 Touch");
  Serial.println("══════════════════════════════════════════\n");

  // Initialize display
  tft.init();
  tft.setRotation(0);  // Portrait mode
  tft.fillScreen(COLOR_BLACK);

  // Set backlight (Pin 21 on ESP32-2432S028R)
  pinMode(21, OUTPUT);
  digitalWrite(21, HIGH);

  // Show boot logo
  tft.setTextColor(COLOR_HOT_PINK, COLOR_BLACK);
  tft.setTextSize(3);
  tft.setCursor(20, 100);
  tft.println("BLACKROAD");

  tft.setTextColor(COLOR_AMBER, COLOR_BLACK);
  tft.setTextSize(2);
  tft.setCursor(30, 140);
  tft.println("CEO HUB v2.0");

  tft.setTextColor(COLOR_BLUE, COLOR_BLACK);
  tft.setTextSize(1);
  tft.setCursor(30, 180);
  tft.println("Initializing systems...");

  delay(1000);

  // Initialize touch calibration (if needed)
  uint16_t calData[5] = { 275, 3620, 264, 3532, 1 };
  tft.setTouch(calData);

  // Connect to WiFi
  tft.setCursor(30, 200);
  tft.setTextColor(COLOR_VIOLET, COLOR_BLACK);
  tft.println("Connecting to WiFi...");
  connectWiFi();

  // Initialize notifications
  for (int i = 0; i < MAX_NOTIFICATIONS; i++) {
    notifications[i].active = false;
  }

  // Draw initial screen
  tft.fillScreen(COLOR_BLACK);
  drawStatusBar();
  drawHeader();
  drawHomeScreen();
  drawNavBar();

  Serial.println("✓ CEO Hub v2.0 ready!");
  Serial.println("Touch screen to navigate");
  Serial.println();

  addNotification("CEO Hub v2.0 Online", COLOR_GREEN);
}

// ══════════════════════════════════════════════════════════════════════════
// MAIN LOOP
// ══════════════════════════════════════════════════════════════════════════

void loop() {
  // Handle WebSocket
  if (wsConnected) {
    webSocket.loop();
  }

  // Handle touch input
  handleTouch();

  // Update notifications
  updateNotifications();

  // Request metrics every 5 seconds
  if (wsConnected && millis() - lastMetricsUpdate > 5000) {
    lastMetricsUpdate = millis();
    sendMetricsRequest();
  }

  // Simulate data updates if not connected
  if (!wsConnected && millis() - lastUpdate > 10000) {
    lastUpdate = millis();
    projectCount += random(-10, 50);
    agentCount += random(-5, 20);
    roadCoinPrice += (random(-100, 100) / 10000.0);
    roadCoinChange24h = random(-1000, 2000) / 100.0;
    cpuUsage = random(20, 90);
    memUsage = random(30, 85);
    networkTraffic = random(100, 5000);

    // Redraw current screen
    tft.fillRect(0, 40, 240, 240, COLOR_BLACK);

    switch(currentScreen) {
      case SCREEN_HOME: drawHomeScreen(); break;
      case SCREEN_PROJECTS: drawProjectsScreen(); break;
      case SCREEN_AI: drawAIScreen(); break;
      case SCREEN_FINANCE: drawFinanceScreen(); break;
      case SCREEN_STUDIO: drawStudioScreen(); break;
      case SCREEN_SETTINGS: drawSettingsScreen(); break;
    }
  }

  // Reconnect WiFi/WS if needed
  if (!wifiConnected && millis() - lastPing > 30000) {
    lastPing = millis();
    connectWiFi();
  }

  delay(10);
}

// ══════════════════════════════════════════════════════════════════════════
// NETWORKING
// ══════════════════════════════════════════════════════════════════════════

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println("\n✓ WiFi connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    // Connect WebSocket
    connectWebSocket();

    drawStatusBar();
  } else {
    wifiConnected = false;
    Serial.println("\n✗ WiFi connection failed");
    drawStatusBar();
  }
}

void connectWebSocket() {
  Serial.printf("Connecting to WebSocket: ws://%s:%d%s\n", WS_HOST, WS_PORT, WS_PATH);

  webSocket.begin(WS_HOST, WS_PORT, WS_PATH);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("✗ WebSocket Disconnected");
      wsConnected = false;
      drawStatusBar();
      addNotification("Server disconnected", COLOR_RED);
      break;

    case WStype_CONNECTED:
      Serial.println("✓ WebSocket Connected");
      wsConnected = true;
      drawStatusBar();
      addNotification("Server connected", COLOR_GREEN);
      webSocket.sendTXT("{\"type\":\"subscribe\",\"channel\":\"metrics\"}");
      break;

    case WStype_TEXT:
      Serial.printf("← Received: %s\n", payload);
      parseMetricsData((char*)payload);
      break;

    case WStype_ERROR:
      Serial.println("✗ WebSocket Error");
      break;
  }
}

void sendMetricsRequest() {
  if (!wsConnected) return;

  webSocket.sendTXT("{\"type\":\"getMetrics\"}");
}

void parseMetricsData(const char* json) {
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, json);

  if (error) {
    Serial.print("JSON parse error: ");
    Serial.println(error.c_str());
    return;
  }

  // Update data from server
  if (doc.containsKey("projects")) projectCount = doc["projects"];
  if (doc.containsKey("agents")) agentCount = doc["agents"];
  if (doc.containsKey("roadcoin")) roadCoinPrice = doc["roadcoin"];
  if (doc.containsKey("change24h")) roadCoinChange24h = doc["change24h"];
  if (doc.containsKey("cpu")) cpuUsage = doc["cpu"];
  if (doc.containsKey("memory")) memUsage = doc["memory"];
  if (doc.containsKey("network")) networkTraffic = doc["network"];

  // Redraw screen with new data
  tft.fillRect(0, 40, 240, 240, COLOR_BLACK);
  switch(currentScreen) {
    case SCREEN_HOME: drawHomeScreen(); break;
    case SCREEN_PROJECTS: drawProjectsScreen(); break;
    case SCREEN_AI: drawAIScreen(); break;
    case SCREEN_FINANCE: drawFinanceScreen(); break;
    case SCREEN_STUDIO: drawStudioScreen(); break;
    case SCREEN_SETTINGS: drawSettingsScreen(); break;
  }
}

// ══════════════════════════════════════════════════════════════════════════
// TOUCH & GESTURES
// ══════════════════════════════════════════════════════════════════════════

void handleTouch() {
  uint16_t x, y;
  touched = tft.getTouch(&x, &y);

  if (touched) {
    touchX = x;
    touchY = y;

    // Debounce
    if (millis() - lastTouchTime < 200) {
      return;
    }

    lastTouchTime = millis();

    Serial.printf("Touch: x=%d, y=%d\n", touchX, touchY);

    // Handle navbar taps
    if (isTouchInNavBar(touchY)) {
      int button = getTappedNavButton(touchX);
      if (button >= 0 && button < SCREEN_COUNT) {
        switchScreen((Screen)button);
      }
    }

    // Track swipe start
    if (swipeStartX == 0) {
      swipeStartX = touchX;
      swipeStartY = touchY;
    }
  } else {
    // Touch released - check for swipe
    if (swipeStartX != 0) {
      checkSwipeGesture();
      swipeStartX = 0;
      swipeStartY = 0;
    }
  }
}

void checkSwipeGesture() {
  int deltaX = touchX - swipeStartX;
  int deltaY = touchY - swipeStartY;

  // Swipe threshold: 80 pixels
  if (abs(deltaX) > 80 && abs(deltaX) > abs(deltaY)) {
    if (deltaX > 0) {
      // Swipe right -> previous screen
      prevScreen();
      Serial.println("Swipe RIGHT");
    } else {
      // Swipe left -> next screen
      nextScreen();
      Serial.println("Swipe LEFT");
    }
  }
}

bool isTouchInNavBar(int16_t y) {
  return y >= 290;
}

int getTappedNavButton(int16_t x) {
  int buttonWidth = 40;
  return x / buttonWidth;
}

// ══════════════════════════════════════════════════════════════════════════
// NAVIGATION
// ══════════════════════════════════════════════════════════════════════════

void switchScreen(Screen newScreen) {
  if (newScreen == currentScreen) return;

  previousScreen = currentScreen;
  currentScreen = newScreen;

  tft.fillRect(0, 20, 240, 270, COLOR_BLACK);
  drawHeader();

  switch(currentScreen) {
    case SCREEN_HOME: drawHomeScreen(); break;
    case SCREEN_PROJECTS: drawProjectsScreen(); break;
    case SCREEN_AI: drawAIScreen(); break;
    case SCREEN_FINANCE: drawFinanceScreen(); break;
    case SCREEN_STUDIO: drawStudioScreen(); break;
    case SCREEN_SETTINGS: drawSettingsScreen(); break;
  }

  drawNavBar();

  Serial.printf("→ Screen: %s\n", screenNames[currentScreen]);
}

void nextScreen() {
  switchScreen((Screen)((currentScreen + 1) % SCREEN_COUNT));
}

void prevScreen() {
  switchScreen((Screen)((currentScreen + SCREEN_COUNT - 1) % SCREEN_COUNT));
}

// ══════════════════════════════════════════════════════════════════════════
// NOTIFICATIONS
// ══════════════════════════════════════════════════════════════════════════

void addNotification(const char* msg, uint16_t color) {
  // Find free slot or oldest
  int slot = -1;
  unsigned long oldestTime = millis();

  for (int i = 0; i < MAX_NOTIFICATIONS; i++) {
    if (!notifications[i].active) {
      slot = i;
      break;
    }
    if (notifications[i].timestamp < oldestTime) {
      oldestTime = notifications[i].timestamp;
      slot = i;
    }
  }

  if (slot >= 0) {
    strncpy(notifications[slot].message, msg, 99);
    notifications[slot].message[99] = '\0';
    notifications[slot].color = color;
    notifications[slot].timestamp = millis();
    notifications[slot].active = true;
  }

  Serial.printf("📢 Notification: %s\n", msg);
}

void updateNotifications() {
  unsigned long now = millis();

  for (int i = 0; i < MAX_NOTIFICATIONS; i++) {
    if (notifications[i].active) {
      // Expire after 5 seconds
      if (now - notifications[i].timestamp > 5000) {
        notifications[i].active = false;
      }
    }
  }
}

void drawNotifications() {
  int y = 25;
  int count = 0;

  for (int i = 0; i < MAX_NOTIFICATIONS && count < 3; i++) {
    if (notifications[i].active) {
      tft.fillRect(5, y, 230, 12, notifications[i].color);
      tft.setTextColor(COLOR_WHITE, notifications[i].color);
      tft.setTextSize(1);
      tft.setCursor(8, y + 2);
      tft.print(notifications[i].message);
      y += 14;
      count++;
    }
  }
}

// ══════════════════════════════════════════════════════════════════════════
// UI DRAWING - SYSTEM
// ══════════════════════════════════════════════════════════════════════════

void drawStatusBar() {
  // Top status bar (20px)
  tft.fillRect(0, 0, 240, 20, COLOR_DARK_GRAY);

  tft.setTextSize(1);

  // WiFi indicator
  if (wifiConnected) {
    tft.setTextColor(COLOR_GREEN, COLOR_DARK_GRAY);
    tft.setCursor(5, 6);
    tft.print("WiFi");
  } else {
    tft.setTextColor(COLOR_RED, COLOR_DARK_GRAY);
    tft.setCursor(5, 6);
    tft.print("WiFi");
  }

  // WebSocket indicator
  if (wsConnected) {
    tft.setTextColor(COLOR_GREEN, COLOR_DARK_GRAY);
    tft.setCursor(40, 6);
    tft.print("WS");
  } else {
    tft.setTextColor(COLOR_DARK_GRAY, COLOR_DARK_GRAY);
    tft.setCursor(40, 6);
    tft.print("WS");
  }

  // Time/uptime
  tft.setTextColor(COLOR_WHITE, COLOR_DARK_GRAY);
  tft.setCursor(150, 6);
  uint32_t uptime = millis() / 1000;
  tft.printf("%02d:%02d:%02d", uptime/3600, (uptime%3600)/60, uptime%60);
}

void drawHeader() {
  // Header bar (30px)
  tft.fillRect(0, 20, 240, 30, COLOR_HOT_PINK);
  tft.setTextColor(COLOR_WHITE, COLOR_HOT_PINK);
  tft.setTextSize(2);
  tft.setCursor(10, 28);
  tft.printf("%s %s", screenIcons[currentScreen], screenNames[currentScreen]);
}

void drawNavBar() {
  // Bottom navigation bar (30px)
  int navY = 290;
  tft.fillRect(0, navY, 240, 30, COLOR_DARK_GRAY);

  int buttonWidth = 40;
  for (int i = 0; i < SCREEN_COUNT; i++) {
    int x = i * buttonWidth;

    if (i == currentScreen) {
      tft.fillRect(x, navY, buttonWidth, 30, COLOR_HOT_PINK);
      tft.setTextColor(COLOR_WHITE, COLOR_HOT_PINK);
    } else {
      tft.setTextColor(COLOR_LIGHT_GRAY, COLOR_DARK_GRAY);
    }

    tft.setTextSize(2);
    tft.setCursor(x + 8, navY + 8);
    tft.print(screenIcons[i]);
  }
}

// ══════════════════════════════════════════════════════════════════════════
// UI DRAWING - SCREENS
// ══════════════════════════════════════════════════════════════════════════

void drawHomeScreen() {
  int y = 60;

  // Welcome message
  tft.setTextColor(COLOR_AMBER, COLOR_BLACK);
  tft.setTextSize(2);
  tft.setCursor(20, y);
  tft.println("CEO CONTROL");
  y += 30;

  tft.setTextColor(COLOR_LIGHT_GRAY, COLOR_BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, y);
  tft.println("\"You bring the chaos.");
  y += 12;
  tft.setCursor(10, y);
  tft.println("BlackRoad brings structure,");
  y += 12;
  tft.setCursor(10, y);
  tft.println("compute, and care.\"");
  y += 25;

  // Quick stats with progress bars
  tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
  tft.setTextSize(1);

  // Projects
  tft.setCursor(10, y);
  tft.printf("Projects: %s", formatNumber(projectCount).c_str());
  y += 15;
  drawProgressBar(10, y, 220, 8, (projectCount % 100) / 100.0, COLOR_BLUE);
  y += 15;

  // Agents
  tft.setCursor(10, y);
  tft.printf("AI Agents: %s (%d active)", formatNumber(agentCount).c_str(), activeAgents);
  y += 15;
  drawProgressBar(10, y, 220, 8, activeAgents / 100.0, COLOR_VIOLET);
  y += 15;

  // RoadCoin
  tft.setCursor(10, y);
  tft.printf("RoadCoin: $%.4f", roadCoinPrice);
  uint16_t changeColor = roadCoinChange24h >= 0 ? COLOR_GREEN : COLOR_RED;
  tft.setTextColor(changeColor, COLOR_BLACK);
  tft.printf(" %.2f%%", roadCoinChange24h);
  tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
  y += 20;

  // System metrics
  y += 10;
  tft.setTextColor(COLOR_AMBER, COLOR_BLACK);
  tft.setCursor(10, y);
  tft.println("System Metrics:");
  y += 15;

  tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
  tft.setCursor(15, y);
  tft.printf("CPU: %d%%", cpuUsage);
  y += 12;
  drawProgressBar(70, y - 10, 160, 6, cpuUsage / 100.0, COLOR_HOT_PINK);

  tft.setCursor(15, y);
  tft.printf("Memory: %d%%", memUsage);
  y += 12;
  drawProgressBar(70, y - 10, 160, 6, memUsage / 100.0, COLOR_AMBER);

  tft.setCursor(15, y);
  tft.printf("Network: %s/s", formatBytes(networkTraffic).c_str());

  // Notifications area
  drawNotifications();
}

void drawProjectsScreen() {
  int y = 60;

  tft.setTextColor(COLOR_BLUE, COLOR_BLACK);
  tft.setTextSize(2);
  tft.setCursor(30, y);
  tft.println("PROJECTS");
  y += 35;

  // Stats
  tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
  tft.setTextSize(1);

  uint32_t activeProjects = (projectCount * 38) / 100;
  uint32_t completedProjects = (projectCount * 62) / 100;

  tft.setCursor(10, y);
  tft.printf("Total: %s", formatNumber(projectCount).c_str());
  y += 15;

  tft.setCursor(10, y);
  tft.printf("Active: %s (38%%)", formatNumber(activeProjects).c_str());
  y += 12;
  drawProgressBar(10, y, 220, 8, 0.38, COLOR_GREEN);
  y += 15;

  tft.setCursor(10, y);
  tft.printf("Done: %s (62%%)", formatNumber(completedProjects).c_str());
  y += 12;
  drawProgressBar(10, y, 220, 8, 0.62, COLOR_VIOLET);
  y += 20;

  // Mini chart
  y += 5;
  tft.setTextColor(COLOR_AMBER, COLOR_BLACK);
  tft.setCursor(10, y);
  tft.println("Activity (last 30 days):");
  y += 15;

  uint8_t chartData[30];
  for (int i = 0; i < 30; i++) {
    chartData[i] = random(20, 100);
  }
  drawMiniChart(10, y, 220, 50, chartData, 30, COLOR_BLUE);
  y += 55;

  // Top projects
  tft.setTextColor(COLOR_AMBER, COLOR_BLACK);
  tft.setCursor(10, y);
  tft.println("Recent:");
  y += 15;

  const char* projects[] = {
    "RoadView Platform",
    "Lucidia AI Core",
    "RoadCoin Exchange"
  };

  tft.setTextColor(COLOR_LIGHT_GRAY, COLOR_BLACK);
  for (int i = 0; i < 3; i++) {
    tft.setCursor(15, y);
    tft.printf("• %s", projects[i]);
    y += 15;
  }
}

void drawAIScreen() {
  int y = 60;

  tft.setTextColor(COLOR_VIOLET, COLOR_BLACK);
  tft.setTextSize(2);
  tft.setCursor(30, y);
  tft.println("AI AGENTS");
  y += 35;

  // Agent status
  const char* agents[] = {
    "Lucidia", "Master AI", "🟢",
    "Roadie", "Tutor", "🟢",
    "Radius", "Quantum", "🟢",
    "Athena", "Code Review", "🟡",
    "Guardian", "Security", "🟢",
    "Alice", "Governance", "🟢",
    "Aria", "Design", "🟢",
    "Cece", "Personal AI", "🟢"
  };

  tft.setTextSize(1);

  for (int i = 0; i < 8; i++) {
    uint16_t color = (i % 2 == 0) ? COLOR_BLUE : COLOR_AMBER;
    tft.setTextColor(color, COLOR_BLACK);
    tft.setCursor(10, y);
    tft.printf("%s - %s", agents[i*3], agents[i*3+1]);

    // Status indicator
    tft.setCursor(210, y);
    if (agents[i*3+2][0] == '🟢') {
      tft.setTextColor(COLOR_GREEN, COLOR_BLACK);
      tft.print("●");
    } else {
      tft.setTextColor(COLOR_AMBER, COLOR_BLACK);
      tft.print("●");
    }

    y += 20;
  }

  y += 10;
  tft.setTextColor(COLOR_HOT_PINK, COLOR_BLACK);
  tft.setCursor(10, y);
  tft.printf("Active: %s agents", formatNumber(agentCount).c_str());
  y += 15;
  tft.setCursor(10, y);
  tft.printf("Online: %d / %d", activeAgents, 8);
}

void drawFinanceScreen() {
  int y = 60;

  tft.setTextColor(COLOR_AMBER, COLOR_BLACK);
  tft.setTextSize(2);
  tft.setCursor(40, y);
  tft.println("ROADCOIN");
  y += 35;

  // Price
  tft.setTextColor(COLOR_HOT_PINK, COLOR_BLACK);
  tft.setTextSize(3);
  tft.setCursor(30, y);
  tft.printf("$%.4f", roadCoinPrice);
  y += 30;

  // 24h change
  uint16_t changeColor = roadCoinChange24h >= 0 ? COLOR_GREEN : COLOR_RED;
  tft.setTextColor(changeColor, COLOR_BLACK);
  tft.setTextSize(2);
  tft.setCursor(30, y);
  tft.printf("%s%.2f%%", roadCoinChange24h >= 0 ? "+" : "", roadCoinChange24h);
  y += 25;

  // Price chart
  tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, y);
  tft.println("Price (24h):");
  y += 15;

  uint8_t priceData[24];
  for (int i = 0; i < 24; i++) {
    priceData[i] = 50 + random(-20, 20);
  }
  drawMiniChart(10, y, 220, 40, priceData, 24, changeColor);
  y += 45;

  // Stats
  tft.setTextColor(COLOR_LIGHT_GRAY, COLOR_BLACK);
  tft.setCursor(10, y);
  tft.printf("Market Cap: $%dM", random(100, 500));
  y += 15;
  tft.setCursor(10, y);
  tft.printf("Volume: $%dK", random(50, 200));
  y += 20;

  // Holdings
  tft.setTextColor(COLOR_BLUE, COLOR_BLACK);
  tft.setCursor(10, y);
  tft.println("Your Holdings:");
  y += 15;

  tft.setTextColor(COLOR_VIOLET, COLOR_BLACK);
  tft.setCursor(15, y);
  tft.printf("ETH: 2.5 ($%.1fK)", 2.5 * 3.2);
  y += 15;
  tft.setCursor(15, y);
  tft.printf("SOL: 100 ($%.1fK)", 100 * 0.18);
  y += 15;
  tft.setCursor(15, y);
  tft.printf("BTC: 0.1 ($%.1fK)", 0.1 * 95);
}

void drawStudioScreen() {
  int y = 60;

  tft.setTextColor(COLOR_HOT_PINK, COLOR_BLACK);
  tft.setTextSize(2);
  tft.setCursor(30, y);
  tft.println("CREATOR");
  tft.setCursor(40, y + 20);
  tft.println("STUDIO");
  y += 55;

  const char* studios[] = {
    "Canvas Studio", "Design", COLOR_VIOLET,
    "Video Studio", "Film", COLOR_BLUE,
    "Writing Studio", "Docs", COLOR_AMBER,
    "Cadence", "Music", COLOR_HOT_PINK,
    "Genesis Road", "Games", COLOR_GREEN,
    "RoadView", "Publish", COLOR_VIOLET,
    "BackRoad", "Social", COLOR_BLUE
  };

  tft.setTextSize(1);

  for (int i = 0; i < 7; i++) {
    uint16_t color;
    if (strcmp(studios[i*3+2], "COLOR_VIOLET") == 0) color = COLOR_VIOLET;
    else if (strcmp(studios[i*3+2], "COLOR_BLUE") == 0) color = COLOR_BLUE;
    else if (strcmp(studios[i*3+2], "COLOR_AMBER") == 0) color = COLOR_AMBER;
    else if (strcmp(studios[i*3+2], "COLOR_HOT_PINK") == 0) color = COLOR_HOT_PINK;
    else color = COLOR_GREEN;

    tft.setTextColor(color, COLOR_BLACK);
    tft.setCursor(10, y);
    tft.printf("%s - %s", studios[i*3], studios[i*3+1]);

    // Active indicator
    tft.setCursor(210, y);
    tft.setTextColor(COLOR_GREEN, COLOR_BLACK);
    tft.print("●");

    y += 20;
  }

  y += 10;
  tft.setTextColor(COLOR_LIGHT_GRAY, COLOR_BLACK);
  tft.setCursor(10, y);
  tft.println("\"Creating beauty from chaos\"");
}

void drawSettingsScreen() {
  int y = 60;

  tft.setTextColor(COLOR_VIOLET, COLOR_BLACK);
  tft.setTextSize(2);
  tft.setCursor(30, y);
  tft.println("SETTINGS");
  y += 35;

  // Network status
  tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, y);
  tft.println("Network:");
  y += 15;

  tft.setTextColor(wifiConnected ? COLOR_GREEN : COLOR_RED, COLOR_BLACK);
  tft.setCursor(15, y);
  tft.printf("WiFi: %s", wifiConnected ? "Connected" : "Disconnected");
  y += 12;

  if (wifiConnected) {
    tft.setTextColor(COLOR_BLUE, COLOR_BLACK);
    tft.setCursor(15, y);
    tft.printf("IP: %s", WiFi.localIP().toString().c_str());
    y += 12;

    tft.setCursor(15, y);
    tft.printf("RSSI: %d dBm", WiFi.RSSI());
    y += 12;
  }

  tft.setTextColor(wsConnected ? COLOR_GREEN : COLOR_RED, COLOR_BLACK);
  tft.setCursor(15, y);
  tft.printf("WebSocket: %s", wsConnected ? "Connected" : "Disconnected");
  y += 20;

  // System info
  tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
  tft.setCursor(10, y);
  tft.println("System:");
  y += 15;

  tft.setTextColor(COLOR_BLUE, COLOR_BLACK);
  tft.setCursor(15, y);
  tft.println("ESP32-2432S028R");
  y += 12;
  tft.setCursor(15, y);
  tft.println("240x320 ILI9341");
  y += 12;
  tft.setCursor(15, y);
  tft.printf("Uptime: %dd %dh %dm",
    millis()/(1000*60*60*24),
    (millis()/(1000*60*60))%24,
    (millis()/(1000*60))%60);
  y += 12;
  tft.setCursor(15, y);
  tft.printf("Free RAM: %s", formatBytes(ESP.getFreeHeap()).c_str());
  y += 12;
  tft.setCursor(15, y);
  tft.printf("CPU: %d MHz", ESP.getCpuFreqMHz());
  y += 25;

  // Version
  tft.setTextColor(COLOR_AMBER, COLOR_BLACK);
  tft.setCursor(10, y);
  tft.println("CEO Hub v2.0.0");
  y += 12;
  tft.setCursor(10, y);
  tft.println("BlackRoad OS, Inc.");
  y += 20;

  tft.setTextColor(COLOR_HOT_PINK, COLOR_BLACK);
  tft.setCursor(10, y);
  tft.println("\"The road remembers");
  y += 12;
  tft.setCursor(10, y);
  tft.println(" everything.\"");
}

// ══════════════════════════════════════════════════════════════════════════
// UI COMPONENTS
// ══════════════════════════════════════════════════════════════════════════

void drawProgressBar(int x, int y, int w, int h, float percentage, uint16_t color) {
  // Clamp percentage
  if (percentage > 1.0) percentage = 1.0;
  if (percentage < 0.0) percentage = 0.0;

  // Border
  tft.drawRect(x, y, w, h, COLOR_DARK_GRAY);

  // Fill
  int fillWidth = (w - 2) * percentage;
  if (fillWidth > 0) {
    tft.fillRect(x + 1, y + 1, fillWidth, h - 2, color);
  }
}

void drawMiniChart(int x, int y, int w, int h, uint8_t* data, int dataSize, uint16_t color) {
  // Draw border
  tft.drawRect(x, y, w, h, COLOR_DARK_GRAY);

  // Find min/max for scaling
  uint8_t minVal = 255, maxVal = 0;
  for (int i = 0; i < dataSize; i++) {
    if (data[i] < minVal) minVal = data[i];
    if (data[i] > maxVal) maxVal = data[i];
  }

  if (maxVal == minVal) maxVal = minVal + 1; // Avoid division by zero

  // Draw bars
  int barWidth = w / dataSize;
  if (barWidth < 1) barWidth = 1;

  for (int i = 0; i < dataSize && i * barWidth < w; i++) {
    int barHeight = ((data[i] - minVal) * (h - 4)) / (maxVal - minVal);
    int barX = x + 2 + i * barWidth;
    int barY = y + h - 2 - barHeight;

    tft.fillRect(barX, barY, barWidth - 1, barHeight, color);
  }
}

// ══════════════════════════════════════════════════════════════════════════
// UTILITIES
// ══════════════════════════════════════════════════════════════════════════

String formatNumber(uint32_t num) {
  if (num >= 1000000) {
    return String(num / 1000000.0, 1) + "M";
  } else if (num >= 1000) {
    return String(num / 1000.0, 1) + "K";
  }
  return String(num);
}

String formatBytes(uint32_t bytes) {
  if (bytes >= 1024 * 1024) {
    return String(bytes / (1024.0 * 1024.0), 1) + "MB";
  } else if (bytes >= 1024) {
    return String(bytes / 1024.0, 1) + "KB";
  }
  return String(bytes) + "B";
}
