# Lampu Otomatis Cerdas Berbasis Gerakan

Proyek ini adalah sistem **Lampu Otomatis** menggunakan sensor gerak (PIR), modul RTC, dan mikrokontroler (ESP8266). Lampu akan otomatis menyala jika mendeteksi gerakan seseorang yang lewat, khususnya pada waktu malam hari. Proyek ini didesain sebagai solusi penerangan pintar (bukan sistem keamanan), agar lampu menyala sendiri saat dibutuhkan.

## Diagram Blok
<p align="center">
  <img src="https://github.com/ediiloupatty/Smart-Automatic-Lighting-with-Motion-Detection/blob/main/img/smart-light.png" alt="Diagram Blok">
</p>

## Fitur Utama
- **Sensor Gerakan:** Menggunakan sensor PIR untuk mendeteksi pergerakan dan menyalakan lampu.
- **Deteksi Waktu:** Menggunakan RTC DS3231. Lampu akan aktif merespons gerakan hanya di malam hari (17:00 - 04:59). Pada siang hari (05:00 - 16:59), sistem masuk ke mode standby dan lampu otomatis mati.
- **Web Dashboard:** Antarmuka web lokal untuk memantau status gerakan, jam saat ini, dan status Relay. Juga tersedia API dalam format JSON.

## Komponen Hardware
1. Mikrokontroler ESP8266 (Mis: NodeMCU / Wemos D1 Mini)
2. Sensor Gerak PIR (Pin D6)
3. Modul Relay Actuve Low (Pin D5)
4. Modul RTC DS3231 (Pin I2C: D1 SCL, D2 SDA)


## Credits
- Project Developer: Edi
- AI Assistance: ChatGPT (OpenAI), Google Gemini