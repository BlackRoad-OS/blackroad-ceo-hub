# 🚀 Deployment Guide - BlackRoad CEO Hub

**Automated GitHub Deployments & CI/CD Pipeline**

## 📊 Build Status

![Build](https://github.com/BlackRoad-OS/blackroad-ceo-hub/actions/workflows/build.yml/badge.svg)
![Release](https://github.com/BlackRoad-OS/blackroad-ceo-hub/actions/workflows/release.yml/badge.svg)
![OTA Server](https://github.com/BlackRoad-OS/blackroad-ceo-hub/actions/workflows/ota-server.yml/badge.svg)

## 🎯 Overview

The CEO Hub uses GitHub Actions for fully automated:
- ✅ **Continuous Integration** - Build on every push/PR
- 📦 **Automated Releases** - Tagged releases with firmware binaries
- 🌐 **OTA Server** - Cloudflare Pages hosting for firmware updates

## 🔄 CI/CD Workflows

### 1. Build Workflow (`.github/workflows/build.yml`)

**Triggers:**
- Push to `main` or `feature/*` branches
- Pull requests to `main`
- Manual dispatch

**Actions:**
1. Checkout code
2. Install PlatformIO
3. Build ESP32 firmware
4. Upload firmware artifacts (30 day retention)
5. Generate build summary

**Artifacts:**
- `firmware.bin` - Flash to ESP32
- `firmware.elf` - Debug symbols

### 2. Release Workflow (`.github/workflows/release.yml`)

**Triggers:**
- Push tag matching `v*.*.*` (e.g., `v2.0.0`)
- Manual dispatch with version input

**Actions:**
1. Build firmware
2. Generate release files:
   - `blackroad-ceo-hub-v*.bin`
   - `blackroad-ceo-hub-v*.elf`
   - `FLASH_INSTRUCTIONS.txt`
   - `checksums.txt` (SHA256)
3. Create GitHub Release with assets
4. Generate release notes

**Release Files:**
```
release/
├── blackroad-ceo-hub-v2.0.0.bin
├── blackroad-ceo-hub-v2.0.0.elf
├── FLASH_INSTRUCTIONS.txt
└── checksums.txt
```

### 3. OTA Server Workflow (`.github/workflows/ota-server.yml`)

**Triggers:**
- Push to `main` branch
- Manual dispatch

**Actions:**
1. Build firmware
2. Generate OTA manifest (`manifest.json`)
3. Create web interface (`index.html`)
4. Deploy to Cloudflare Pages

**Deployed At:**
- **Production:** https://blackroad-ceo-hub-ota.pages.dev
- **Firmware URL:** https://blackroad-ceo-hub-ota.pages.dev/firmware.bin
- **Manifest:** https://blackroad-ceo-hub-ota.pages.dev/manifest.json

## 🏷️ Creating Releases

### Method 1: Git Tags (Recommended)

```bash
# Create and push tag
git tag v2.0.0
git push origin v2.0.0

# GitHub Actions automatically:
# - Builds firmware
# - Creates release
# - Uploads binaries
```

### Method 2: Manual Dispatch

1. Go to **Actions** tab on GitHub
2. Select **Release Firmware**
3. Click **Run workflow**
4. Enter version (e.g., `v2.0.1`)
5. Click **Run**

### Version Format

Use semantic versioning: `vMAJOR.MINOR.PATCH`

- `v2.0.0` - Major release (breaking changes)
- `v2.1.0` - Minor release (new features)
- `v2.0.1` - Patch release (bug fixes)

## 📥 Using Released Firmware

### Download from GitHub

```bash
# Latest release
wget https://github.com/BlackRoad-OS/blackroad-ceo-hub/releases/latest/download/blackroad-ceo-hub-*.bin

# Specific version
wget https://github.com/BlackRoad-OS/blackroad-ceo-hub/releases/download/v2.0.0/blackroad-ceo-hub-v2.0.0.bin
```

### Flash to ESP32

```bash
# Using esptool
esptool.py --port /dev/cu.usbserial-110 write_flash 0x10000 blackroad-ceo-hub-*.bin

# Using PlatformIO
pio run --target upload
```

## 🌐 OTA Updates

### OTA Server Features

The automated OTA server provides:

1. **Web Interface**
   - Latest firmware info
   - Download links
   - Flash instructions
   - Changelog

2. **JSON Manifest**
   ```json
   {
     "version": "v2.0.0",
     "buildDate": "2026-01-03T22:00:00Z",
     "firmware": {
       "url": "/firmware.bin",
       "size": 1234567,
       "sha256": "abc123..."
     },
     "changelog": [...]
   }
   ```

3. **Direct Downloads**
   - Binary: `/firmware.bin`
   - Manifest: `/manifest.json`

### Using OTA in Your ESP32

```cpp
#include <HTTPUpdate.h>

void checkForUpdates() {
  WiFiClient client;
  t_httpUpdate_return ret = httpUpdate.update(client,
    "https://blackroad-ceo-hub-ota.pages.dev/firmware.bin");

  switch(ret) {
    case HTTP_UPDATE_FAILED:
      Serial.println("Update failed");
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("No updates");
      break;
    case HTTP_UPDATE_OK:
      Serial.println("Update OK - Rebooting");
      break;
  }
}
```

## 🔐 Required Secrets

Configure these in **Settings → Secrets and variables → Actions**:

| Secret | Description | Required For |
|--------|-------------|--------------|
| `CLOUDFLARE_API_TOKEN` | Cloudflare API token | OTA Server deployment |
| `CLOUDFLARE_ACCOUNT_ID` | Cloudflare account ID | OTA Server deployment |

### Getting Cloudflare Credentials

1. **API Token:**
   - Go to Cloudflare Dashboard → My Profile → API Tokens
   - Create Token → Edit Cloudflare Workers
   - Permissions: `Account.Cloudflare Pages:Edit`

2. **Account ID:**
   - Cloudflare Dashboard → Account ID (right sidebar)

## 📊 Monitoring Deployments

### GitHub Actions Dashboard

View all workflows:
```
https://github.com/BlackRoad-OS/blackroad-ceo-hub/actions
```

### Build Status

Check specific workflow:
- **Build:** `.../actions/workflows/build.yml`
- **Release:** `.../actions/workflows/release.yml`
- **OTA Server:** `.../actions/workflows/ota-server.yml`

### Cloudflare Pages

Monitor deployments:
```
https://dash.cloudflare.com → Pages → blackroad-ceo-hub-ota
```

## 🛠️ Local Development

### Test Build Locally

```bash
# Full build
pio run

# Clean build
pio run --target clean && pio run

# Upload to device
pio run --target upload

# Monitor serial
pio device monitor
```

### Simulate CI Build

```bash
# Install dependencies (like CI)
pio pkg install

# Build (like CI)
pio run

# Check size
pio run --target size
```

## 🔄 Workflow for Features

### Working on Feature Branch

```bash
# Create feature branch
git checkout -b feature/new-charts

# Work in worktree (recommended)
cd ~/ceo-hub-worktrees/feature-advanced-charts

# Make changes
vim src/chart_line.cpp

# Commit
git add .
git commit -m "feat: add line chart widget"

# Push - triggers build workflow
git push origin feature/new-charts
```

### CI builds automatically:
- ✅ Verifies code compiles
- ✅ Uploads firmware artifact
- ✅ Shows build size

### Merging to Main

```bash
# Create PR on GitHub
gh pr create --title "Add line chart widget" --body "Implements real-time line charts"

# Or merge locally
git checkout main
git merge feature/new-charts
git push origin main
```

### Releasing New Version

```bash
# After merge to main
git tag v2.1.0
git push origin v2.1.0

# GitHub Actions automatically:
# - Builds firmware
# - Creates release
# - Deploys OTA server
```

## 📈 Deployment Metrics

### Typical Build Times

| Workflow | Duration |
|----------|----------|
| Build | 2-3 minutes |
| Release | 3-4 minutes |
| OTA Server | 1-2 minutes |

### Artifact Sizes

| File | Size |
|------|------|
| firmware.bin | ~800 KB |
| firmware.elf | ~2 MB |

### Storage

- GitHub Artifacts: 30 days retention
- Releases: Permanent
- Cloudflare Pages: Unlimited

## 🚨 Troubleshooting

### Build Fails

**Check:**
1. PlatformIO version compatibility
2. Library dependencies in `platformio.ini`
3. Build log in Actions tab

**Fix:**
```bash
# Update locally
pio pkg update
pio run

# If works, push
git push
```

### Release Not Creating

**Check:**
1. Tag format: Must be `v*.*.*`
2. Permissions: Workflow has `contents: write`
3. Secrets configured

**Fix:**
```bash
# Retag if needed
git tag -d v2.0.0
git push origin :refs/tags/v2.0.0
git tag v2.0.0
git push origin v2.0.0
```

### OTA Server Not Deploying

**Check:**
1. Cloudflare secrets configured
2. Account ID correct
3. API token has Pages permissions

**Fix:**
```bash
# Test locally with wrangler
wrangler pages deploy ota-server --project-name=blackroad-ceo-hub-ota
```

## 📚 Additional Resources

- [GitHub Actions Docs](https://docs.github.com/en/actions)
- [PlatformIO CI Docs](https://docs.platformio.org/en/latest/integration/ci/index.html)
- [Cloudflare Pages Docs](https://developers.cloudflare.com/pages/)
- [ESP32 OTA Updates](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/ota.html)

## 🎯 Best Practices

1. **Always tag releases** - Provides version history
2. **Test locally first** - Verify builds before pushing
3. **Use semantic versioning** - Clear version increments
4. **Check build logs** - Monitor for warnings
5. **Update changelog** - Document changes in releases
6. **Monitor OTA server** - Ensure firmware availability

---

🖤🛣️ **"The road remembers everything."** 🛣️🖤

**Deployment System:** GitHub Actions + Cloudflare Pages
**Created by:** aria-esp32-firmware-1767499297-098ddacd
**Status:** ✅ Fully Automated
