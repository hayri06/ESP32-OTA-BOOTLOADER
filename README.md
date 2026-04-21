
---

## 🔧 Requirements

- ESP32 board
- Arduino IDE / PlatformIO
- AWS IoT Core account
- S3 bucket (for firmware hosting)

---

## 🔐 AWS Setup

1. Create a Thing in AWS IoT Core
2. Generate certificates:
   - Device Certificate
   - Private Key
   - Root CA (AmazonRootCA1)
3. Attach IoT policy:
   - `iot:Connect`
   - `iot:Publish`
   - `iot:Subscribe`

---

## 🌐 Configuration

Update the following variables:

```cpp
const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";

const char* awsEndpoint = "YOUR_ENDPOINT.iot.region.amazonaws.com";
const char* clientID = "ESP32_CLIENT";

String host = "your-bucket.s3.amazonaws.com";
String bootloaderName = "/firmware.bin";
int port = 80;

🔄 OTA Update Flow
Device connects to WiFi
Device connects to AWS IoT
If both connections are active:
execOTA() is triggered
Firmware is downloaded via HTTP
Update is written using Update.writeStream()
Device reboots after successful update

⚠️ Important Notes

OTA file must be:

Content-Type: application/octet-stream
Ensure correct partition scheme (OTA enabled)
Avoid recursive retry in OTA (can crash device)
Use HTTPS for production

🛠 Improvements (TODO)
 MQTT-based OTA trigger
 HTTPS OTA (WiFiClientSecure)
 Progress callback (%)
 FreeRTOS task separation
 Retry mechanism with limits