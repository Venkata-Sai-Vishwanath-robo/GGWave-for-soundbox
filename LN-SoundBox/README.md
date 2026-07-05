# LN Soundbox ⚡

**A proof-of-concept that shares Lightning Network payment addresses over inaudible ultrasonic sound.**

No QR code. No NFC. No internet between devices. Just sound the human ear can't hear.

---

## What it does

1. The merchant opens the app and taps **Generate Invoice**
2. The app broadcasts their Lightning Address over **ultrasonic audio (~18–22 kHz)** — completely inaudible to humans
3. The customer opens the app on their phone, taps **Listen**
4. Within 1–2 seconds the phone decodes the address and **automatically opens the Lightning wallet** to complete the payment

The payment itself happens over the Lightning Network as normal. Only the address handoff happens via sound.

---

## How it works

### Audio transmission
Uses [ggwave](https://github.com/ggerganov/ggwave) — an open-source library that encodes data as audio using FSK (Frequency Shift Keying). This app uses `GGWAVE_PROTOCOL_ULTRASOUND_FASTEST` (protocol ID 5), which operates in the 18–22 kHz range.

At ~20 bytes/sec throughput, a typical Lightning Address like `alice@walletofsatoshi.com` transmits in under 1 second.

### Domain compression
To keep the payload short, known Lightning wallet domains are compressed to a single letter:

| Code | Domain |
|------|--------|
| W | walletofsatoshi.com |
| A | getalby.com |
| S | strike.me |
| B | blink.sv |
| Z | zbd.gg |
| P | primal.net |

So `alice@walletofsatoshi.com` transmits as `alice@W` — **7 bytes** instead of 27.

### Looping broadcast
The transmitter loops continuously with a 300ms gap between cycles. A listener joining mid-transmission waits at most one full cycle (~1s) before catching a clean decode.

### Inaudibility
A 17 kHz highpass Web Audio filter strips any audible-range components before they reach the speaker. Phone microphones capture the 18–22 kHz signal cleanly with browser audio processing (`autoGainControl`, `noiseSuppression`, `echoCancellation`) disabled.

---

## Payment modes

| Mode | How it works |
|------|-------------|
| **Lightning Address** | Broadcasts compressed address. Phone fetches BOLT11 invoice from the wallet's LNURL server. |
| **LNbits** | Broadcasts full BOLT11 (requires LNbits node; invoice length limits audio reliability). |

---

## Stack

- Vanilla JS — no framework
- [TailwindCSS](https://tailwindcss.com) CDN
- [qrcodejs](https://github.com/davidshimjs/qrcodejs) CDN
- [ggwave](https://github.com/ggerganov/ggwave) (local WASM build)
- [CoinGecko API](https://www.coingecko.com/en/api) for BTC/INR price

---

## Running locally

Requires a local server (WASM and microphone access don't work from `file://`):

```bash
npx serve .
```

Open `http://localhost:3000` on PC.
Open `http://<your-local-ip>:3000` on phone (same Wi-Fi).

For microphone on mobile, HTTPS is required — use [ngrok](https://ngrok.com) or deploy to GitHub Pages.

---

## Limitations

- Ultrasonic range varies by device — some older phones can't reproduce or capture 18–22 kHz reliably
- LNbits mode broadcasts the full BOLT11 invoice (~300+ chars) which may exceed ggwave's reliable payload length
- Lightning Address mode requires the payer's phone to have internet access to fetch the invoice
- iOS Safari may block auto-opening the wallet app — the "Open in Wallet" button serves as fallback

---

## Proof of concept

This is a proof of concept exploring offline / proximity payment initiation on the Lightning Network. It is not production software.
