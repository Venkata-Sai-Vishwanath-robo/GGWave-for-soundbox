/*
 * UPI SoundBox — ESP32 + DFPlayer Mini
 * UPI ID: dvsv2004@okaxis  (transmitted as "dvsv2004@A")
 *
 * Wiring:
 *   ESP32 5V     → DFPlayer VCC  (100µF cap between VCC and GND for clean power)
 *   ESP32 GND    → DFPlayer GND
 *   ESP32 GPIO17 → DFPlayer RX
 *   ESP32 GPIO16 → DFPlayer TX
 *   DFPlayer SPK_1 → Speaker (+)
 *   DFPlayer SPK_2 → Speaker (−)
 *
 * SD card:
 *   FAT32 formatted, file named 0001.wav in root
 *   Generate 0001.wav from the UPI Soundbox website with ID: dvsv2004@okaxis
 *
 * Library: "DFRobot DFPlayer Mini" — install via Arduino Library Manager
 */

#include <DFRobotDFPlayerMini.h>

#define RX_PIN   16
#define TX_PIN   17
#define LED_PIN   2
#define VOLUME   28    // 0–30
#define GAP_MS  350    // gap between cycles (matches website broadcast timing)

HardwareSerial dfSerial(2);
DFRobotDFPlayerMini player;

void ledBlink(int n, int ms = 100) {
  for (int i = 0; i < n; i++) {
    digitalWrite(LED_PIN, HIGH); delay(ms);
    digitalWrite(LED_PIN, LOW);  delay(ms);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  dfSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  delay(500);

  if (!player.begin(dfSerial, true, true)) {
    Serial.println("DFPlayer init failed. Check:");
    Serial.println("  - SD card inserted (FAT32)");
    Serial.println("  - 0001.wav in SD root");
    Serial.println("  - Wiring: GPIO16=RX, GPIO17=TX");
    while (true) { ledBlink(3, 80); delay(500); }
  }

  player.volume(VOLUME);
  player.EQ(DFPLAYER_EQ_NORMAL);

  Serial.println("UPI SoundBox ready — looping dvsv2004@okaxis");
  ledBlink(2, 150);
}

void loop() {
  player.play(1);
  digitalWrite(LED_PIN, HIGH);

  // Wait for playback to finish
  while (true) {
    if (player.available() && player.readType() == DFPlayerPlayFinished) break;
    delay(10);
  }

  digitalWrite(LED_PIN, LOW);
  delay(GAP_MS);
}
