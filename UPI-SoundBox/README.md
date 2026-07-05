# UPI Soundbox 🔊

Transmit a UPI ID over audio so a customer's phone can decode it automatically — no QR code scanning needed. The merchant's ESP32 plays an encoded audio loop through a speaker; the customer opens the website on their phone, taps Listen, and their UPI app opens with the payment pre-filled.

Built on [ggwave](https://github.com/ggerganov/ggwave) — FSK audio encoding that works through any speaker and phone mic.

---

## How it works

```
Merchant device (ESP32 + speaker)          Customer phone
─────────────────────────────────          ──────────────
Powers on → plays encoded audio loop  →   Opens website → taps Listen
                                       →   Mic decodes UPI ID from sound
                                       →   UPI app opens automatically
```

The UPI ID is compressed before encoding:
`dvsv2004@okaxis` → `dvsv2004@A` (common handles replaced with a single letter)
This keeps the audio packet short and reliable.

---

## Bill of Materials

| Component | Details | Est. Cost |
|-----------|---------|-----------|
| ESP32 Dev Kit | 30-pin or 38-pin | ₹350–450 |
| DFPlayer Mini | MP3/WAV player module | ₹120–150 |
| Speaker | 4Ω or 8Ω, 3–5W, with wire leads | ₹60–100 |
| Micro SD card | 2–32 GB, FAT32 | ₹60–80 |
| Capacitor | 100µF 25V electrolytic | ₹5 |
| Jumper wires | Male-to-male | ₹10 |
| USB cable | Micro USB or USB-C (for ESP32) | already have |

**Total: ~₹600–800**

No resistors needed — ESP32 outputs 3.3V logic which DFPlayer RX accepts directly.

---

## Wiring

```
┌─────────────────────────────────────────────┐
│                  ESP32                       │
│                                             │
│  5V  ────────────────────── DFPlayer VCC   │
│                                    │        │
│                               [100µF cap]  │
│                               + to VCC     │
│                               - to GND     │
│                                    │        │
│  GND ────────────────────── DFPlayer GND   │
│  GPIO17 (TX2) ────────────── DFPlayer RX   │
│  GPIO16 (RX2) ────────────── DFPlayer TX   │
│                                             │
└─────────────────────────────────────────────┘

DFPlayer SPK_1 ────── Speaker (+)
DFPlayer SPK_2 ────── Speaker (−)
```

### Important notes
- **Capacitor** goes between DFPlayer **VCC and GND** only — it smooths the power supply and eliminates audio pops. Do **not** connect it to the speaker terminals.
- **Speaker** connects directly to SPK_1 and SPK_2 — no capacitor, no resistor needed. The DFPlayer output is a differential (BTL) amplifier designed to drive speakers directly.
- **No resistor** needed on the GPIO17 → RX line — ESP32's 3.3V logic is within DFPlayer's input range.

### DFPlayer Mini pin reference
```
        ┌────────────┐
   VCC  │ 1        2 │  RX  ← ESP32 GPIO17
    TX  │ 3        4 │  DAC_R
 DAC_L  │ 5        6 │  SPK_1  → Speaker (+)
   GND  │ 7        8 │  SPK_2  → Speaker (−)
   ...  │ 9  ...  10 │  GND
        └────────────┘
```

---

## Part 1 — Website setup (GitHub Pages)

The website handles both broadcasting (merchant) and listening (customer). It runs entirely in the browser — no backend needed.

### Deploy to GitHub Pages

1. Fork or clone this repo to your GitHub account
2. Go to repo **Settings → Pages**
3. Under **Source** select **GitHub Actions**
4. Push to `main` — the site deploys automatically via `.github/workflows/static.yml`
5. Your site will be live at:
   ```
   https://<your-username>.github.io/<repo-name>/
   ```

### Run locally (for testing)

Requires Node.js installed:
```bash
npx serve .
```
Open `http://localhost:3000` in your browser.

> **Note:** Microphone access and WebAssembly require either `localhost` or HTTPS. GitHub Pages provides HTTPS automatically.

---

## Part 2 — Generate the audio file

The audio file encodes your UPI ID as ggwave FSK tones. It is generated on the website and stored on the SD card.

1. Open the website (local or GitHub Pages)
2. Enter your UPI ID (e.g. `dvsv2004@okaxis`)
3. Tap **Broadcast UPI** — the broadcast screen opens
4. Tap **Download WAV for SD Card**
5. The file `upi.wav` downloads automatically
6. Rename it to **`0001.wav`**

> When you change your UPI ID, repeat these steps and replace `0001.wav` on the SD card. No re-flashing the ESP32 needed.

---

## Part 3 — SD card setup

1. Insert SD card into your computer
2. Format it as **FAT32** (right-click → Format in Windows; Disk Utility on Mac)
3. Copy `0001.wav` to the **root** of the SD card (not inside any folder)
4. Eject and insert into the DFPlayer Mini's micro SD slot

---

## Part 4 — Arduino IDE setup

### Install Arduino IDE
Download from [arduino.cc/en/software](https://www.arduino.cc/en/software) (version 2.x recommended).

### Add ESP32 board support
1. Open Arduino IDE → **File → Preferences**
2. In **Additional Board Manager URLs** paste:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Go to **Tools → Board → Board Manager**
4. Search `esp32` → install **esp32 by Espressif Systems**

### Install DFPlayer library
1. **Tools → Manage Libraries**
2. Search `DFRobot DFPlayer Mini`
3. Install **DFRobot DFPlayer Mini** by DFRobot

### Board settings
| Setting | Value |
|---------|-------|
| Board | ESP32 Dev Module |
| Upload Speed | 921600 |
| Flash Size | 4MB (32Mb) |
| Partition Scheme | Default 4MB with spiffs |
| Port | whichever COM port appears when ESP32 is plugged in |

---

## Part 5 — Flash the ESP32

1. Open `esp32/upi_soundbox/upi_soundbox.ino` in Arduino IDE
2. Connect ESP32 to computer via USB
3. Select the correct port under **Tools → Port**
4. Click **Upload** (→ arrow button)
5. Wait for `Done uploading`

If upload fails, hold the **BOOT** button on the ESP32 while clicking Upload, release once uploading starts.

---

## Part 6 — Power on and test

1. Insert the SD card into DFPlayer Mini
2. Power the ESP32 (USB to computer or any 5V USB adapter)
3. The LED (GPIO2) will blink twice on startup
4. Audio plays immediately and loops with a 350ms gap between cycles
5. Open Serial Monitor (115200 baud) to see status:
   ```
   UPI SoundBox ready — looping dvsv2004@okaxis
   ```

---

## Using the Listen mode (customer side)

1. Customer opens the website on their phone
2. Taps **Listen** (top right)
3. Taps **Start Listening** — mic permission prompt appears, allow it
4. Points phone mic toward the speaker
5. UPI ID is decoded automatically → UPI app opens with payment pre-filled

Works with Google Pay, PhonePe, Paytm, and any UPI app that handles `upi://` deep links.

---

## Handle compression table

Common UPI handles are compressed to a single letter to keep the audio packet short and fast:

| Code | Handle |
|------|--------|
| A | @okaxis |
| Y | @ybl (PhonePe) |
| S | @oksbi |
| P | @paytm |
| I | @okicici |
| U | @upi |
| L | @apl (Amazon Pay) |
| X | @axl |
| H | @okhdfcbank |
| M | @ibl |
| B | @barodampay |
| N | @naviaxis |
| T | @postbank |
| K | @ikwik |

If your handle is not in the table, the full UPI ID is transmitted as-is.

---

## Audio protocol

| Setting | Details |
|---------|---------|
| Library | ggwave (FSK encoding) |
| Default mode | Audible — standard tones, works through any speaker |
| Silent mode | 18–22 kHz ultrasonic — inaudible, for phone-to-phone use |
| Packet size | ~10 chars for compressed UPI ID — fast and reliable |
| Loop gap | 350ms between cycles — listener can decode on any cycle |

Switch between audible and silent in the website Settings screen.

---

## Troubleshooting

### DFPlayer does not initialise (LED blinks 3 times)
- Check SD card is inserted firmly
- Confirm SD is FAT32 formatted
- Confirm `0001.wav` is in the SD root (not in a subfolder)
- Check wiring: GPIO17 → RX, GPIO16 → TX, 5V → VCC, GND → GND

### No sound from speaker
- Check speaker wires are in SPK_1 and SPK_2 (not DAC pins)
- Try increasing volume: change `#define VOLUME 28` to `30` in the sketch

### Phone cannot decode the audio
- Make sure phone mic is pointed toward the speaker
- Keep distance under 1 metre for reliable decoding
- Switch website to **Audible** mode in Settings (not Silent/ultrasonic)
- Disable any phone case that covers the microphone
- Make sure browser mic permission is granted

### Website microphone does not work
- Must be served over HTTPS or localhost — file:// will not work
- Use GitHub Pages or `npx serve` locally
- Check browser mic permission in site settings

### Upload to ESP32 fails
- Hold BOOT button on ESP32 during upload
- Try a different USB cable (some are charge-only, no data)
- Lower upload speed to 115200 in Tools menu

---

## Repository structure

```
UPI-SoundBox/
├── index.html                  # Website (merchant broadcast + customer listen)
├── lib/
│   └── ggwave.js               # Audio encoding/decoding (WebAssembly)
├── esp32/
│   └── upi_soundbox/
│       ├── upi_soundbox.ino    # Arduino sketch for ESP32 + DFPlayer Mini
│       └── data/               # Place upi.wav here (if using LittleFS approach)
└── .github/
    └── workflows/
        └── static.yml          # GitHub Pages auto-deploy
```

---

## License

MIT
