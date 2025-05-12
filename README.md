# 🎛️ VolControl

> A DIY volume knob & device switcher for Windows  
> Built with Arduino Pro Micro + Python ✨

---

## 🔍 Overview

VolControl transforms an **Arduino Pro Micro (ATmega32U4)**, an **SSD1306 OLED** screen, and an **EC11 rotary encoder** into a sleek volume controller.  
- 🔊 **Rotate** the encoder → Adjust system volume  
- 🎧 **Press** the encoder → Switch audio device (via F13 key + SoundSwitch)  
- 💡 F13 is chosen because it’s unused (i think) in Windows, avoiding conflicts

---

## 🛠️ Hardware

- Arduino Pro Micro (ATmega32U4)  
- SSD1306 OLED display (128×64, I2C)  
- EC11 rotary encoder with push-button  
- Jumper wires / breadboard (or custom PCB)
- 3D Printed case (Optional) 

---

## 💾 Software

### 1. Arduino Sketch

1. Open `Main.cpp` in the PlatformIO.  
2. Verify/adjust I2C pins and encoder pins if needed.  
3. Upload to the Pro Micro.

### 2. Python Controller (PC)

A Python script reads serial data and controls Windows volume using **PyCAW**.

1. Install **Python 3.6+**  
2. (Optional) Create & activate a virtual environment:

   ```bash
   python -m venv venv
   # Linux/macOS
   source venv/bin/activate
   # Windows
   venv\Scripts\activate
   ```

3. Install dependencies:

   ```bash
   pip install -r requirements.txt
   ```

4. Plug in VolControl via USB and run:

   ```bash
   python volcontrol.py
   ```

---

## 🚀 Usage

- 🔊 **Rotate encoder** → Increase / Decrease volume  
- 🎧 **Press encoder** → Change device

---

## 📦 Prerequisites

### Arduino Libraries

- Adafruit_SSD1306  
- Adafruit_GFX
- Encoder
- HID-Project

### Python Packages

Provided in `requirements.txt`:

```
comtypes
pycaw
pyaudio
pyserial
```

### Windows Software

- **SoundSwitch** by [@Belphemur](https://github.com/Belphemur/SoundSwitch)

---
