#ifndef __OTA_H_
#define __OTA_H_

#include <Arduino.h>
#include <WiFiClient.h>
#include <Update.h>

// ================== GLOBAL DEĞİŞKENLER ==================

// OTA bağlantı client
extern WiFiClient clientUp;

// Sunucu bilgileri
extern String host;
extern int port;

// İndirilecek binary
extern String bin;
extern String bootloaderName;

// OTA kontrol değişkenleri
extern long contentLength;
extern bool isValidContentType;

// ================== FONKSİYONLAR ==================

// Header parse yardımcı fonksiyon
String getHeaderValue(String header, String headerName);

// OTA ana fonksiyonu
void execOTA();

#endif