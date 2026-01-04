# 🌐 Multi-Cloud Integration Guide

**BlackRoad CEO Hub - Cloud Services Integration**

## 📋 Overview

This guide covers integrating **four cloud platforms** with your ESP32 CEO Hub:

1. **Cloudflare** - Edge computing, CDN, OTA updates
2. **DigitalOcean** - Backend API (droplet 159.65.43.12)
3. **Hugging Face** - AI inference
4. **Enclave AI** - Secure AI processing

---

## 🎯 Quick Start

### Configured Services

✅ **Cloudflare** - Ready (OTA server configured)
✅ **DigitalOcean** - Ready (droplet IP: 159.65.43.12)
⚠️  **Hugging Face** - Requires API token
⚠️  **Enclave AI** - Requires API key

### Configuration File

All cloud settings are in: `src/cloud_config.h`

```cpp
#include "cloud_config.h"

// Cloudflare OTA
String firmware_url = CF_FIRMWARE_URL;

// DigitalOcean API
String metrics_url = DO_METRICS_ENDPOINT;

// Hugging Face AI
String sentiment_url = HF_SENTIMENT_URL;

// Enclave AI
String secure_chat_url = ENCLAVE_SECURE_CHAT_URL;
```

---

## 1️⃣ CLOUDFLARE INTEGRATION

### Features
- ✅ OTA firmware updates
- ✅ CDN for fast delivery
- ✅ Edge caching
- ⏳ KV storage (optional)
- ⏳ D1 database (optional)

### OTA Update Implementation

```cpp
#include <HTTPUpdate.h>
#include "cloud_config.h"

void checkCloudflareOTA() {
  if (!ENABLE_CLOUDFLARE) return;

  Serial.println("Checking Cloudflare for firmware updates...");

  // Fetch manifest
  HTTPClient http;
  http.begin(CF_OTA_MANIFEST_URL);
  int httpCode = http.GET();

  if (httpCode == 200) {
    String manifest = http.getString();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, manifest);

    String latestVersion = doc["version"];
    String currentVersion = "v2.0.0";  // From build

    if (latestVersion != currentVersion) {
      Serial.println("New version available: " + latestVersion);
      performOTAUpdate();
    }
  }

  http.end();
}

void performOTAUpdate() {
  WiFiClient client;
  t_httpUpdate_return ret = httpUpdate.update(client, CF_FIRMWARE_URL);

  switch(ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("OTA failed: %s\n", httpUpdate.getLastErrorString().c_str());
      addNotification("OTA Update Failed", COLOR_RED);
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("No updates available");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("Update successful - Rebooting...");
      addNotification("Update Complete!", COLOR_GREEN);
      ESP.restart();
      break;
  }
}
```

### Cloudflare KV Storage (Optional)

```cpp
void saveToCloudflareKV(String key, String value) {
  HTTPClient http;
  String url = String(CF_KV_API_URL) + "/values/" + key;

  http.begin(url);
  http.addHeader("Authorization", "Bearer " + String(CF_API_TOKEN));
  http.addHeader("Content-Type", "text/plain");

  int httpCode = http.PUT(value);
  Serial.printf("KV Save: %d\n", httpCode);

  http.end();
}

String getFromCloudflareKV(String key) {
  HTTPClient http;
  String url = String(CF_KV_API_URL) + "/values/" + key;

  http.begin(url);
  http.addHeader("Authorization", "Bearer " + String(CF_API_TOKEN));

  int httpCode = http.GET();
  String value = "";

  if (httpCode == 200) {
    value = http.getString();
  }

  http.end();
  return value;
}
```

---

## 2️⃣ DIGITALOCEAN INTEGRATION

### Features
- ✅ Backend API at 159.65.43.12:8080
- ✅ Metrics collection
- ✅ Event logging
- ✅ Command execution
- ✅ WebSocket real-time

### Metrics Upload

```cpp
void sendMetricsToDigitalOcean() {
  if (!ENABLE_DIGITALOCEAN) return;

  HTTPClient http;
  http.begin(DO_METRICS_ENDPOINT);
  http.addHeader("Content-Type", "application/json");

  // Build metrics JSON
  DynamicJsonDocument doc(512);
  doc["device_id"] = WiFi.macAddress();
  doc["timestamp"] = millis();
  doc["cpu_usage"] = cpuUsage;
  doc["mem_usage"] = memUsage;
  doc["network_traffic"] = networkTraffic;
  doc["wifi_rssi"] = WiFi.RSSI();
  doc["uptime"] = millis() / 1000;

  String json;
  serializeJson(doc, json);

  int httpCode = http.POST(json);

  if (httpCode == 200) {
    Serial.println("✓ Metrics sent to DigitalOcean");
  } else {
    Serial.printf("✗ Metrics failed: %d\n", httpCode);
  }

  http.end();
}
```

### Event Logging

```cpp
void logEventToDigitalOcean(String event_type, String message) {
  HTTPClient http;
  http.begin(DO_EVENTS_ENDPOINT);
  http.addHeader("Content-Type", "application/json");

  DynamicJsonDocument doc(256);
  doc["device_id"] = WiFi.macAddress();
  doc["event_type"] = event_type;
  doc["message"] = message;
  doc["timestamp"] = millis();

  String json;
  serializeJson(doc, json);

  http.POST(json);
  http.end();
}
```

### Command Execution

```cpp
void checkDigitalOceanCommands() {
  HTTPClient http;
  http.begin(DO_COMMANDS_ENDPOINT + "?device_id=" + WiFi.macAddress());

  int httpCode = http.GET();

  if (httpCode == 200) {
    String response = http.getString();
    DynamicJsonDocument doc(512);
    deserializeJson(doc, response);

    JsonArray commands = doc["commands"];
    for (JsonObject cmd : commands) {
      String action = cmd["action"];
      
      if (action == "reboot") {
        Serial.println("Command: Reboot");
        ESP.restart();
      } else if (action == "update_ota") {
        Serial.println("Command: OTA Update");
        checkCloudflareOTA();
      } else if (action == "clear_cache") {
        Serial.println("Command: Clear cache");
        // Clear local cache
      }
    }
  }

  http.end();
}
```

---

## 3️⃣ HUGGING FACE INTEGRATION

### Features
- 🤖 Sentiment analysis
- 📝 Text summarization
- ❓ Question answering
- 🖼️ Image classification

### Sentiment Analysis

```cpp
String analyzeWithHuggingFace(String text) {
  if (!ENABLE_HUGGINGFACE) return "HF disabled";

  HTTPClient http;
  http.begin(HF_SENTIMENT_URL);
  http.addHeader("Authorization", "Bearer " + String(HF_API_TOKEN));
  http.addHeader("Content-Type", "application/json");

  DynamicJsonDocument doc(256);
  doc["inputs"] = text;

  String json;
  serializeJson(doc, json);

  int httpCode = http.POST(json);
  String result = "unknown";

  if (httpCode == 200) {
    String response = http.getString();
    DynamicJsonDocument responseDoc(512);
    deserializeJson(responseDoc, response);

    JsonArray results = responseDoc[0];
    if (results.size() > 0) {
      result = results[0]["label"].as<String>();
      float score = results[0]["score"];
      Serial.printf("Sentiment: %s (%.2f)\n", result.c_str(), score);
    }
  }

  http.end();
  return result;
}
```

### Text Summarization

```cpp
String summarizeTextHF(String long_text) {
  HTTPClient http;
  http.begin(HF_SUMMARIZATION_URL);
  http.addHeader("Authorization", "Bearer " + String(HF_API_TOKEN));
  http.addHeader("Content-Type", "application/json");

  DynamicJsonDocument doc(1024);
  doc["inputs"] = long_text;
  doc["parameters"]["max_length"] = 100;
  doc["parameters"]["min_length"] = 30;

  String json;
  serializeJson(doc, json);

  int httpCode = http.POST(json);
  String summary = "";

  if (httpCode == 200) {
    String response = http.getString();
    DynamicJsonDocument responseDoc(2048);
    deserializeJson(responseDoc, response);

    summary = responseDoc[0]["summary_text"].as<String>();
  }

  http.end();
  return summary;
}
```

---

## 4️⃣ ENCLAVE AI INTEGRATION

### Features
- 🔒 Secure AI inference
- 🔐 End-to-end encryption
- 💬 Secure chat
- 🧠 Protected embeddings

### Secure Chat

```cpp
String secureChat EnclaveAI(String message) {
  if (!ENABLE_ENCLAVE_AI) return "Enclave disabled";

  HTTPClient http;
  http.begin(ENCLAVE_SECURE_CHAT_URL);
  http.addHeader("Authorization", "Bearer " + String(ENCLAVE_API_KEY));
  http.addHeader("Content-Type", "application/json");

  DynamicJsonDocument doc(512);
  doc["model"] = ENCLAVE_MODEL_SECURE;
  doc["messages"][0]["role"] = "user";
  doc["messages"][0]["content"] = message;
  doc["encryption_level"] = ENCLAVE_ENCRYPTION_LEVEL;

  String json;
  serializeJson(doc, json);

  int httpCode = http.POST(json);
  String response_text = "";

  if (httpCode == 200) {
    String response = http.getString();
    DynamicJsonDocument responseDoc(1024);
    deserializeJson(responseDoc, response);

    response_text = responseDoc["choices"][0]["message"]["content"];
  }

  http.end();
  return response_text;
}
```

---

## 🔄 UNIFIED CLOUD MANAGER

### Implementation

```cpp
class CloudManager {
private:
  unsigned long lastCFCheck = 0;
  unsigned long lastDOMetrics = 0;
  unsigned long lastCommandCheck = 0;

public:
  void begin() {
    Serial.println("🌐 Cloud Manager initialized");
    Serial.printf("  Cloudflare: %s\n", ENABLE_CLOUDFLARE ? "✓" : "✗");
    Serial.printf("  DigitalOcean: %s\n", ENABLE_DIGITALOCEAN ? "✓" : "✗");
    Serial.printf("  Hugging Face: %s\n", ENABLE_HUGGINGFACE ? "✓" : "✗");
    Serial.printf("  Enclave AI: %s\n", ENABLE_ENCLAVE_AI ? "✓" : "✗");
  }

  void loop() {
    unsigned long now = millis();

    // Cloudflare OTA check (every hour)
    if (ENABLE_CLOUDFLARE && now - lastCFCheck > CLOUD_OTA_CHECK_INTERVAL) {
      lastCFCheck = now;
      checkCloudflareOTA();
    }

    // DigitalOcean metrics (every 30 seconds)
    if (ENABLE_DIGITALOCEAN && now - lastDOMetrics > CLOUD_METRICS_UPDATE_INTERVAL) {
      lastDOMetrics = now;
      sendMetricsToDigitalOcean();
    }

    // Command check (every minute)
    if (ENABLE_DIGITALOCEAN && now - lastCommandCheck > CLOUD_STATUS_UPDATE_INTERVAL) {
      lastCommandCheck = now;
      checkDigitalOceanCommands();
    }
  }

  void testAllServices() {
    Serial.println("\n=== Testing Cloud Services ===");

    if (ENABLE_CLOUDFLARE) {
      Serial.print("Cloudflare OTA... ");
      checkCloudflareOTA();
    }

    if (ENABLE_DIGITALOCEAN) {
      Serial.print("DigitalOcean API... ");
      sendMetricsToDigitalOcean();
    }

    if (ENABLE_HUGGINGFACE) {
      Serial.print("Hugging Face... ");
      String result = analyzeWithHuggingFace("BlackRoad is amazing!");
      Serial.println(result);
    }

    if (ENABLE_ENCLAVE_AI) {
      Serial.print("Enclave AI... ");
      String response = secureChatEnclaveAI("Hello, Enclave!");
      Serial.println(response);
    }

    Serial.println("=== Tests Complete ===\n");
  }
};

CloudManager cloudManager;
```

### Usage in main.cpp

```cpp
void setup() {
  // ... existing setup ...

  cloudManager.begin();
  cloudManager.testAllServices();
}

void loop() {
  // ... existing loop ...

  cloudManager.loop();
}
```

---

## 🔐 Security Best Practices

### API Tokens
Store tokens securely - don't hardcode in source!

**Option 1: Build flags** (platformio.ini)
```ini
build_flags =
  -DHF_API_TOKEN=\"hf_your_token_here\"
  -DENCLAVE_API_KEY=\"enclave_your_key_here\"
  -DCF_API_TOKEN=\"cf_your_token_here\"
```

**Option 2: Separate secrets file**
```cpp
// secrets.h (add to .gitignore!)
#define HF_API_TOKEN "hf_your_token"
#define ENCLAVE_API_KEY "enclave_your_key"
#define CF_API_TOKEN "cf_your_token"
```

### HTTPS/TLS
Always use HTTPS for API calls (already configured).

---

## 📊 Integration Status Dashboard

Add to Settings screen:

```cpp
void drawCloudStatusScreen() {
  int y = 60;

  tft.setTextColor(COLOR_BLUE, COLOR_BLACK);
  tft.setTextSize(2);
  tft.setCursor(20, y);
  tft.println("CLOUD STATUS");
  y += 35;

  // Services
  const char* services[] = {
    "Cloudflare", ENABLE_CLOUDFLARE ? "●" : "○",
    "DigitalOcean", ENABLE_DIGITALOCEAN ? "●" : "○",
    "Hugging Face", ENABLE_HUGGINGFACE ? "●" : "○",
    "Enclave AI", ENABLE_ENCLAVE_AI ? "●" : "○"
  };

  tft.setTextSize(1);
  for (int i = 0; i < 4; i++) {
    tft.setTextColor(COLOR_WHITE, COLOR_BLACK);
    tft.setCursor(10, y);
    tft.print(services[i*2]);

    // Status indicator
    uint16_t color = services[i*2+1][0] == '●' ? COLOR_GREEN : COLOR_DARK_GRAY;
    tft.setTextColor(color, COLOR_BLACK);
    tft.setCursor(200, y);
    tft.print(services[i*2+1]);

    y += 20;
  }
}
```

---

## 🚀 DEPLOYMENT

### Build with Cloud Features

```bash
# Install dependencies
pio lib install "bblanchon/ArduinoJson@^7.0.0"

# Build
pio run

# Upload
pio run --target upload
```

### Enable Services

Edit `cloud_config.h`:
```cpp
#define ENABLE_CLOUDFLARE true       // ✓ Ready
#define ENABLE_DIGITALOCEAN true     // ✓ Ready
#define ENABLE_HUGGINGFACE false     // Add token first
#define ENABLE_ENCLAVE_AI false      // Add key first
```

---

## 📖 API Documentation

- **Cloudflare:** https://developers.cloudflare.com/api/
- **DigitalOcean:** https://docs.digitalocean.com/reference/api/
- **Hugging Face:** https://huggingface.co/docs/api-inference/
- **Enclave AI:** https://docs.enclave.ai/ (if available)

---

🖤🛣️ **"The road remembers everything."** 🛣️🖤

**Feature:** Multi-Cloud Integration
**Services:** Cloudflare, DigitalOcean, Hugging Face, Enclave AI
**Status:** ✅ Configuration Ready
