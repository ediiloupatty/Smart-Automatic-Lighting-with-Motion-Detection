#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <RTClib.h>

// ==========================================
// KONFIGURASI PENGGUNA
// ==========================================
const char* WIFI_SSID = "No Internet Connection";
const char* WIFI_PASS = "Loupatty143";

// ==========================================
// DEKLARASI HARDWARE & OBJEK
// ==========================================
#define PIR_PIN    D6
#define RELAY_PIN  D5

ESP8266WebServer server(80);
RTC_DS3231 rtc;

// ==========================================
// VARIABEL GLOBAL STATUS SISTEM
// ==========================================
String statusGerakan = "Aman";
String statusSistem  = "Aktif";
char timeBuffer[10]  = "00:00:00";
bool rtcValid        = false;

// ==========================================
// VARIABEL TIMER (ANTI-NOISE & WARMUP)
// ==========================================
unsigned long lastMotionTime = 0;
const unsigned long HOLD_TIME = 5000;    // Relay ditahan 5 detik setelah gerakan hilang
const unsigned long WARMUP_TIME = 20000; // Sensor PIR butuh 20 detik kalibrasi saat pertama nyala
bool isMotionActive = false;

// ==========================================
// DEKLARASI FUNGSI-FUNGSI UTAMA (MODULAR)
// ==========================================

void updateRTCStatus() {
  DateTime now = rtc.now();

  // Jika tahun <= 2000, artinya komunikasi I2C gagal atau baterai RTC habis
  if (now.year() <= 2000) {
    rtcValid = false;
    strcpy(timeBuffer, "ERROR");
  } else {
    rtcValid = true;
    sprintf(timeBuffer, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  }
}

void updateMotionStatus() {
  // BLOKIR SENSOR SAAT BARU NYALA (Mencegah Relay Nyala karena PIR Kalibrasi)
  if (millis() < WARMUP_TIME) {
    isMotionActive = false;
    statusGerakan = "Memulai Sensor..."; // Tampilkan status ini di Web
    return;
  }

  int currentMotion = digitalRead(PIR_PIN);

  if (currentMotion == HIGH) {
    isMotionActive = true;
    lastMotionTime = millis();
  } else {
    if (millis() - lastMotionTime > HOLD_TIME) {
      isMotionActive = false;
    }
  }
}

void updateRelayState() {
  int jamSaatIni = 0;

  if (rtcValid) {
    DateTime now = rtc.now();
    jamSaatIni = now.hour();
  }

  // ===============================
  // MODE MALAM (17:00 - 04:59) Atau Fail-Safe jika RTC Error
  // ===============================
  if (!rtcValid || jamSaatIni >= 17 || jamSaatIni < 5) {
    statusSistem = "Mode Malam (Keamanan Aktif)";

    if (millis() >= WARMUP_TIME) {
      if (isMotionActive) {
        statusGerakan = "TERDETEKSI!"; // Menggunakan string ini agar di web warnanya merah (sesuai handleRoot)
        digitalWrite(RELAY_PIN, HIGH);   // Relay ON (Active Low)
      } else {
        statusGerakan = "Aman";
        digitalWrite(RELAY_PIN, LOW);  // Relay OFF
      }
    }
  }
  // ===============================
  // MODE SIANG (05:00 - 16:59)
  // ===============================
  else {
    statusSistem = "Mode Siang (Standby)";

    if (millis() >= WARMUP_TIME) {
      if (isMotionActive) {
        statusGerakan = "Gerakan Terdeteksi (Siang)";
      } else {
        statusGerakan = "Aman (Siang)";
      }
    }

    // Relay selalu mati saat siang
    digitalWrite(RELAY_PIN, LOW);  // OFF
  }
}

void handleRoot() {
  updateRTCStatus();

  String html = "<html><head><meta http-equiv='refresh' content='2'>";
  html += "<style>body{font-family:Arial; text-align:center; background:#1e1e1e; color:#fff; padding-top:20px;}";
  html += ".box{padding:20px; display:inline-block; border-radius:10px; font-weight:bold; font-size:1.5em; margin:10px; box-shadow: 0 4px 8px rgba(0,0,0,0.5);}</style></head><body>";
  html += "<h1>Sistem Keamanan Pintar</h1>";

  if (!rtcValid) {
    html += "<h2 style='color:#ff4444;'>Peringatan: Modul RTC Error / Tidak Terbaca! <br> Sistem masuk ke Fail-Safe Mode</h2>";
  } else {
    html += "<h2>Jam Sistem: <span style='color:#00e676;'>" + String(timeBuffer) + "</span></h2>";
  }

  if (statusSistem == "Mode Siang (Standby)") {
    html += "<h3 style='color:#ffbb33;'>Status: " + statusSistem + "</h3>";
    html += "<div class='box' style='background:#424242;'>SENSOR: " + statusGerakan + "</div>";
    html += "<p style='color:#9e9e9e;'>Lampu Relay: MATI (Dinonaktifkan)</p>";
  } else {
    html += "<h3 style='color:#33b5e5;'>Status: " + statusSistem + "</h3>";
    
    if (statusGerakan == "TERDETEKSI!") {
      html += "<div class='box' style='background:#ff4444;'>ALARM: ADA GERAKAN!</div>";
      html += "<p style='color:#ff4444; font-weight:bold;'>Lampu Relay: MENYALA</p>";
    } else {
      String bgColor = (statusGerakan == "Memulai Sensor...") ? "#33b5e5" : "#00C851";
      html += "<div class='box' style='background:" + bgColor + ";'>STATUS: " + statusGerakan + "</div>";
      html += "<p style='color:#00C851;'>Lampu Relay: MATI</p>";
    }
  }

  html += "<br><br><a href='/api' style='color:#ffbb33; text-decoration:none;'>[ Lihat Data API JSON ]</a>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleApi() {
  updateRTCStatus();

  String relayState = (digitalRead(RELAY_PIN) == HIGH) ? "ON" : "OFF";

  String json = "{";
  json += "\"jam\":\"" + String(timeBuffer) + "\",";
  json += "\"rtc_status\":\"" + String(rtcValid ? "OK" : "ERROR") + "\",";
  json += "\"sistem\":\"" + statusSistem + "\",";
  json += "\"gerakan\":\"" + statusGerakan + "\",";
  json += "\"relay\":\"" + relayState + "\"";
  json += "}";

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

// ==========================================
// FUNGSI SETUP (DIJALANKAN SEKALI)
// ==========================================
void setup() {
  Serial.begin(115200);
  Wire.begin(D2, D1); 

  // Trik Mencegah Relay Kedip/Menyala Saat Booting
  digitalWrite(RELAY_PIN, LOW); 
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);

  if (!rtc.begin()) {
    Serial.println("\n[ERROR] RTC tidak ditemukan! Cek kabel.");
    rtcValid = false;
  } else {
    rtcValid = true;
  }

  Serial.print("\nMenghubungkan ke WiFi: ");
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int wifiRetry = 0;
  
  // Tunggu koneksi WiFi maksimal 10 detik (20 x 500ms)
  while (WiFi.status() != WL_CONNECTED && wifiRetry < 20) {
    delay(500);
    Serial.print(".");
    wifiRetry++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[SUKSES] WiFi Terhubung!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n[WARNING] Gagal terhubung ke WiFi. Sistem berjalan secara offline.");
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/api", HTTP_GET, handleApi);
  server.begin();
  
  Serial.println("Web Server Berjalan.");
}

// ==========================================
// FUNGSI LOOP (DIJALANKAN TERUS MENERUS)
// ==========================================
void loop() {
  server.handleClient(); 
  
  updateRTCStatus();     
  updateMotionStatus();  
  updateRelayState();    

  delay(10); 
}