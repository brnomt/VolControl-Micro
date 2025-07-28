// VolControl - For Arduino Pro Micro (ATmega32U4)
// Author: brnomt
// Made with a lot of Vibe Coding, So some coments are made in Spanish and some in English. (This comment is also written by cursor, bc I am lazy to write it again)

// This code is for a volume control device using an SSD1306 OLED display and an EC11 encoder
// It allows you to change the volume of a device and switch between devices using a button.

// The reason the Encoder uses F13 key, is because it is a key that does not have a function in Windows (iirc), so it will not interfere with anything.
// The F13 key is used to switch between devices, so you can use it to switch between devices without having to use the mouse.
// This is used to change the device via SoundSwitch software. Made by @Belphemur on Github.

// SoundSwitch repo:
// https://github.com/Belphemur/SoundSwitch



//Wiring:

// Encoder CLK -> Pin 6
// Encoder  DT -> Pin 7
// Encoder  SW -> Pin 8 

// OLED Stays as default, so SDA and SCL go where they should go. In my case 2 for SDA and 3 for SCL.
// OLED VCC -> 3.3V or 5V (depends on the model)

//General VCC and GND are connected to the Arduino VCC and GND respectively.

//You can modify the code to suit your needs, or do whatever you want with it.
//If you modify it, i'd be nice if you could share your modifications with me, so I can learn from them and improve my code <3.

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Encoder.h>
#include <HID-Project.h> // Requiere el HID-Project de NicoHood

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Encoder
Encoder myEncoder(7, 6);  // DT, CLK
const int swPin = 8;

// Estado
int volumen = 50;
long posicionEncoder = 0;
String dispositivoAudio = "No device";

unsigned long ultimoCambioVolumen = 0;
const int debounceTiempo = 50;

// Botón
int estadoBoton;
int ultimoEstadoBoton = HIGH;
unsigned long ultimoDebounceTime = 0;
const int debounceDelay = 50;

// Serial buffer
String inputString = "";
bool stringComplete = false;

void actualizarPantalla() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0, 0);
  display.println("Output:");
  display.println(dispositivoAudio.substring(0, 21));
  
  display.drawLine(0, 20, 128, 20, SSD1306_WHITE);
  
  int anchoBarra = map(volumen, 0, 100, 0, 118);
  display.fillRect(5, 25, anchoBarra, 20, SSD1306_WHITE);
  display.drawRect(5, 25, 118, 20, SSD1306_WHITE);
  
  display.setCursor(0, 50);
  display.print("Vol: ");
  display.print(volumen);
  display.print("%");

  display.display();
}

void cambiarVolumen(int delta) {
  volumen += delta * 2;
  volumen = constrain(volumen, 0, 100);
  
  if (delta > 0) {
    Consumer.write(MEDIA_VOLUME_UP);
  } else {
    Consumer.write(MEDIA_VOLUME_DOWN);
  }

  ultimoCambioVolumen = millis();
}

void switchDevice() {
  Keyboard.press(KEY_F13);
  delay(10);
  Keyboard.release(KEY_F13);
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
      return;
    }
    if (inChar != '\r') {
      inputString += inChar;
    }
  }
}

void procesarSerial() {
  if (!stringComplete) return;

  if (inputString.startsWith("DEVICE:")) {
    dispositivoAudio = inputString.substring(7);
    dispositivoAudio.trim();
    actualizarPantalla();
  }
  else if (inputString.startsWith("VOLUME:")) {
    volumen = inputString.substring(7).toInt();
    actualizarPantalla();
  }

  inputString = "";
  stringComplete = false;
}

void setup() {
  Serial.begin(115200);
  pinMode(swPin, INPUT_PULLUP);
  
  // HID
  Keyboard.begin();
  Consumer.begin();

  // Pantalla
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Error OLED"));
    while (true);
  }

  actualizarPantalla();
}

void loop() {
  serialEvent();
  procesarSerial();

  long nuevaPosicion = myEncoder.read() / 4;
  if (nuevaPosicion != posicionEncoder) {
    int diferencia = nuevaPosicion - posicionEncoder;
    if (millis() - ultimoCambioVolumen > debounceTiempo) {
      cambiarVolumen(diferencia);
      actualizarPantalla();
    }
    posicionEncoder = nuevaPosicion;
  }

  int lecturaBoton = digitalRead(swPin);
  if (lecturaBoton != ultimoEstadoBoton) {
    ultimoDebounceTime = millis();
  }

  if ((millis() - ultimoDebounceTime) > debounceDelay) {
    if (lecturaBoton != estadoBoton) {
      estadoBoton = lecturaBoton;
      if (estadoBoton == LOW) {
        switchDevice();
        actualizarPantalla();
      }
    }
  }

  ultimoEstadoBoton = lecturaBoton;
  delay(10);
}
