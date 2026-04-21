#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

//#include "AWS_Root_CA.h"
#include "OTA.h"   // execOTA()

// ================== WIFI ==================
const char* ssid = "SSID";
const char* password = "PASSWORD";

// ================== AWS ===================
const char* awsEndpoint = "xxxxxxxxxxxxx-ats.iot.region.amazonaws.com";
const int awsPort = 8883;
const char* clientID = "ESP32_CLIENT";

// Sertifikalar 
extern const char* certificatePemCrt; // # You will add.
extern const char* privatePemKey;  // # You will add.


WiFiClient clientUp;

String host = "your-bucket.s3.amazonaws.com";  // S3 endpoint
int port = 80;

String bin;
String bootloaderName = "/firmware.bin";

long contentLength = 0;
bool isValidContentType = false;

// ================== MODE ==================
typedef enum
{
  CONNECTION_NONE,
  CONNECTION_WIFI,
  CONNECTION_ETH
} e_connstates;

unsigned char connectionMode = CONNECTION_WIFI;

// ================== CLIENT =================
WiFiClientSecure net;
PubSubClient client(net);

// ================== WIFI ==================
void connectWiFi()
{
  if (WiFi.status() == WL_CONNECTED) return;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
}

// ================== AWS ===================
void connectAWS()
{
  if (client.connected()) return;

  net.setCACert(AWS_ROOT_CA);
  net.setCertificate(certificatePemCrt);
  net.setPrivateKey(privatePemKey);

  client.setServer(awsEndpoint, awsPort);

  while (!client.connected())
  {
    if (client.connect(clientID))
    {
      // Bağlandı
    }
    else
    {
      delay(2000);
    }
  }
}

// ================== SETUP =================
void setup()
{
  Serial.begin(115200);
  delay(500);

  if (connectionMode == CONNECTION_WIFI)
  {
    connectWiFi();
    connectAWS();
  }
}

// ================== LOOP ==================
void loop()
{
  if (connectionMode == CONNECTION_WIFI)
  {
    // WiFi kontrol
    if (WiFi.status() != WL_CONNECTED)
    {
      connectWiFi();
    }

    // AWS kontrol
    if (!client.connected())
    {
      connectAWS();
    }

    client.loop();

    // ✅ Bağlantılar OK ise OTA çalıştır
    if (WiFi.status() == WL_CONNECTED && client.connected())
    {
      static bool otaDone = false;

      if (!otaDone && WiFi.status() == WL_CONNECTED && client.connected())
      {
        execOTA();
        otaDone = true;
      }
    }
  }
}
