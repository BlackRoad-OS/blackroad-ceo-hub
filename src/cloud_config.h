/**
 * ══════════════════════════════════════════════════════════════════════════
 *                    🖤🛣️ CLOUD SERVICES CONFIGURATION 🛣️🖤
 * ══════════════════════════════════════════════════════════════════════════
 * 
 * Multi-cloud integration for BlackRoad CEO Hub
 * Supports: Cloudflare, DigitalOcean, Hugging Face, Enclave AI
 */

#ifndef CLOUD_CONFIG_H
#define CLOUD_CONFIG_H

// ══════════════════════════════════════════════════════════════════════════
// CLOUDFLARE CONFIGURATION
// ══════════════════════════════════════════════════════════════════════════

// Cloudflare Workers / Pages
#define CF_WORKERS_URL "https://blackroad-ceo-hub-ota.pages.dev"
#define CF_API_ENDPOINT "https://api.cloudflare.com/client/v4"
#define CF_OTA_MANIFEST_URL CF_WORKERS_URL "/manifest.json"
#define CF_FIRMWARE_URL CF_WORKERS_URL "/firmware.bin"

// Cloudflare KV (optional)
#define CF_KV_NAMESPACE_ID "YOUR_KV_NAMESPACE_ID"
#define CF_KV_API_URL CF_API_ENDPOINT "/accounts/" CF_ACCOUNT_ID "/storage/kv/namespaces/" CF_KV_NAMESPACE_ID

// Cloudflare D1 (optional)
#define CF_D1_DATABASE_ID "YOUR_D1_DATABASE_ID"

// Account details (set via environment or secrets)
#ifndef CF_ACCOUNT_ID
#define CF_ACCOUNT_ID "your-cloudflare-account-id"
#endif

#ifndef CF_API_TOKEN
#define CF_API_TOKEN "your-cloudflare-api-token"
#endif

// ══════════════════════════════════════════════════════════════════════════
// DIGITALOCEAN CONFIGURATION
// ══════════════════════════════════════════════════════════════════════════

// Droplet IP
#define DO_DROPLET_IP "159.65.43.12"
#define DO_API_URL "http://" DO_DROPLET_IP ":8080"

// API endpoints
#define DO_METRICS_ENDPOINT DO_API_URL "/api/metrics"
#define DO_EVENTS_ENDPOINT DO_API_URL "/api/events"
#define DO_COMMANDS_ENDPOINT DO_API_URL "/api/commands"
#define DO_STATUS_ENDPOINT DO_API_URL "/api/status"

// WebSocket
#define DO_WS_URL "ws://" DO_DROPLET_IP ":8080/ws"

// Auth (optional)
#ifndef DO_API_KEY
#define DO_API_KEY "your-do-api-key"
#endif

// ══════════════════════════════════════════════════════════════════════════
// HUGGING FACE CONFIGURATION
// ══════════════════════════════════════════════════════════════════════════

// API endpoints
#define HF_API_URL "https://api-inference.huggingface.co/models"
#define HF_API_VERSION "v1"

// Models for CEO Hub
#define HF_MODEL_SENTIMENT "distilbert-base-uncased-finetuned-sst-2-english"
#define HF_MODEL_SUMMARIZATION "facebook/bart-large-cnn"
#define HF_MODEL_QA "deepset/roberta-base-squad2"
#define HF_MODEL_VISION "google/vit-base-patch16-224"

// Full endpoint URLs
#define HF_SENTIMENT_URL HF_API_URL "/" HF_MODEL_SENTIMENT
#define HF_SUMMARIZATION_URL HF_API_URL "/" HF_MODEL_SUMMARIZATION
#define HF_QA_URL HF_API_URL "/" HF_MODEL_QA
#define HF_VISION_URL HF_API_URL "/" HF_MODEL_VISION

// Auth token
#ifndef HF_API_TOKEN
#define HF_API_TOKEN "your-huggingface-api-token"
#endif

// ══════════════════════════════════════════════════════════════════════════
// ENCLAVE AI CONFIGURATION
// ══════════════════════════════════════════════════════════════════════════

// Enclave endpoints (secure AI processing)
#define ENCLAVE_API_URL "https://api.enclave.ai/v1"
#define ENCLAVE_INFERENCE_URL ENCLAVE_API_URL "/inference"
#define ENCLAVE_EMBEDDINGS_URL ENCLAVE_API_URL "/embeddings"
#define ENCLAVE_SECURE_CHAT_URL ENCLAVE_API_URL "/chat/secure"

// Models
#define ENCLAVE_MODEL_DEFAULT "enclave-llama-2-7b"
#define ENCLAVE_MODEL_SECURE "enclave-gpt-4-secure"

// Auth
#ifndef ENCLAVE_API_KEY
#define ENCLAVE_API_KEY "your-enclave-api-key"
#endif

// Security settings
#define ENCLAVE_USE_TLS true
#define ENCLAVE_VERIFY_CERT true
#define ENCLAVE_ENCRYPTION_LEVEL 256  // AES-256

// ══════════════════════════════════════════════════════════════════════════
// UNIFIED CLOUD SETTINGS
// ══════════════════════════════════════════════════════════════════════════

// Timeouts (milliseconds)
#define CLOUD_REQUEST_TIMEOUT 10000
#define CLOUD_CONNECTION_TIMEOUT 5000

// Retry settings
#define CLOUD_MAX_RETRIES 3
#define CLOUD_RETRY_DELAY 1000

// Update intervals (milliseconds)
#define CLOUD_METRICS_UPDATE_INTERVAL 30000   // 30 seconds
#define CLOUD_OTA_CHECK_INTERVAL 3600000       // 1 hour
#define CLOUD_STATUS_UPDATE_INTERVAL 60000     // 1 minute

// Feature flags
#define ENABLE_CLOUDFLARE true
#define ENABLE_DIGITALOCEAN true
#define ENABLE_HUGGINGFACE false  // Disabled by default (token required)
#define ENABLE_ENCLAVE_AI false   // Disabled by default (token required)

// ══════════════════════════════════════════════════════════════════════════
// HELPER MACROS
// ══════════════════════════════════════════════════════════════════════════

#define CLOUD_SERVICE_CLOUDFLARE 0x01
#define CLOUD_SERVICE_DIGITALOCEAN 0x02
#define CLOUD_SERVICE_HUGGINGFACE 0x04
#define CLOUD_SERVICE_ENCLAVE_AI 0x08

#define IS_CLOUD_ENABLED(service) ((service) && (ENABLE_##service))

#endif // CLOUD_CONFIG_H
