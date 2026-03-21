# Lampu Otomatis Cerdas Berbasis Gerakan

Proyek ini adalah sistem **Lampu Otomatis** menggunakan sensor gerak (PIR), modul RTC, dan mikrokontroler (ESP8266). Lampu akan otomatis menyala jika mendeteksi gerakan seseorang yang lewat, khususnya pada waktu malam hari. Proyek ini didesain sebagai solusi penerangan pintar (bukan sistem keamanan), agar lampu menyala sendiri saat dibutuhkan.

## Simulasi
<p align="center">
  <img src="https://raw.githubusercontent.com/ediiloupatty/Smart-Automatic-Lighting-with-Motion-Detection/main/img/Create%20With%20Gemini%20Veo.gif" alt="Simulation">
</p>

## Diagram Blok
<p align="center">
  <img src="https://github.com/ediiloupatty/Smart-Automatic-Lighting-with-Motion-Detection/blob/main/img/smart-light.png" alt="Diagram Blok">
</p>

## Fitur Utama
- **Sensor Gerakan:** Menggunakan sensor PIR untuk mendeteksi pergerakan dan menyalakan lampu.
- **Deteksi Waktu:** Menggunakan RTC DS3231 dengan 3 mode operasi otomatis:
  1. **Mode Aktif (18:00 - 21:59):** Lampu menyala secara terus-menerus (mengabaikan sensor).
  2. **Mode Sensor Tengah Malam (22:00 - 05:59):** Sistem sensor aktif, lampu hanya menyala jika terdeteksi pergerakan di dekatnya.
  3. **Mode Siang (06:00 - 17:59):** Sistem *standby* (lampu akan ikut mati total).
- **Web Dashboard:** Antarmuka web lokal untuk memantau status gerakan, jam saat ini, dan status Relay. Juga tersedia API dalam format JSON.

## Komponen Hardware
1. Mikrokontroler ESP8266 (Mis: NodeMCU / Wemos D1 Mini)
2. Sensor Gerak PIR (Pin D6)
3. Modul Relay Actuve Low (Pin D5)
4. Modul RTC DS3231 (Pin I2C: D1 SCL, D2 SDA)

## Credits
- Project Developer: Edi
- AI Assistance: ChatGPT (OpenAI), Google Gemini
