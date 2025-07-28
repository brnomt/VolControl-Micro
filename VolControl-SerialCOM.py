#VolControl-SerialCOM
#author: brnomt

#Made with a lot of vibe coding. So some coments are made in Spanish and some in English. (This comment is also written by cursor, bc I am lazy)
#This code is a work in progress, and it may not be fully functional or optimized.

#This python script is designed to work with a custom Arduino device called "VolControl-Micro".
#It communicates with the device over a serial port to control audio volume and display the current audio device, as well as to send volume and device information to the Arduino.

#You can modify the code to suit your needs, or do whatever you want with it.
#If you modify it, it'd be nice if you could share your modifications with me, so I can learn from them and improve my code <3.



import time
import re
import serial
import pyaudio
from comtypes import CLSCTX_ALL, CoInitialize
from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume
from serial.tools import list_ports

import serial.tools.list_ports



def get_current_speaker():
    audio = pyaudio.PyAudio()
    try:
        idx = audio.get_default_output_device_info()['index']
        return audio.get_device_info_by_index(idx)['name']
    except OSError as e:
        print(f"Error getting default output device: {e}")
        return None
    finally:
        audio.terminate()

def format_device_name(name):
    name = re.sub(r"\s*\(SteelSeries Sonar Virtual Audio Device\)", "", name)
    name = name.replace("SteelSeries Sonar", "SS")
    shortcuts = {
        "Altavoces (Razer Barracuda X 2.4)": "Razer Barracuda X",
    }
    for full, short in shortcuts.items():
        if full in name:
            name = short
            break
    if len(name) > 21:
        name = name[:18] + "…"
    return name

def get_current_volume():
    CoInitialize()
    devices = AudioUtilities.GetSpeakers()
    interface = devices.Activate(IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
    volume_iface = interface.QueryInterface(IAudioEndpointVolume)
    level = volume_iface.GetMasterVolumeLevelScalar()
    return int(level * 100)

def find_volcontrol_port():
    """
    Escanea todos los puertos serie y devuelve el primero cuyo
    descriptor contenga 'VolControl-Micro'. Si no hay ninguno,
    duerme 1 s y vuelve a intentar.
    """
    while True:
        ports = list_ports.comports()
        for p in ports:
            desc = (p.description or "") + (p.product or "") + (p.manufacturer or "")
            if "VolControl-Micro" in desc:
                return p.device  # ej. "COM3"
        print("VolControl-Micro no encontrado. Reintentando en 1 s...")
        time.sleep(1)

def main():

    for p in serial.tools.list_ports.comports():
        print(f"{p.device} → {p.description}, {p.product}, {p.manufacturer}")

    while True:
        # 1) BUSCAMOS y ABRIMOS el puerto
        # port = find_volcontrol_port()
        port = "COM21"
        try:
            arduino = serial.Serial(port, 115200, timeout=1)
            print(f"Conectado a {port}")
            last_dev = ""
            last_vol = -1

            # 2) Bucle principal de lectura+envío
            while True:
                # -- dispositivo de audio
                raw_name = get_current_speaker()
                disp = format_device_name(raw_name) if raw_name else "No audio device"

                # -- volumen
                vol = get_current_volume()

                # -- enviamos sólo si cambian
                if disp != last_dev:
                    arduino.write(f"DEVICE:{disp}\n".encode())
                    print("Enviado DEVICE:", disp)
                    last_dev = disp

                if vol != last_vol:
                    arduino.write(f"VOLUME:{vol}\n".encode())
                    print("Enviado VOLUME:", vol)
                    last_vol = vol

                time.sleep(1)

        except (serial.SerialException, OSError) as e:
            print(f"*** Desconectado o error en {port}: {e}")

        finally:
            try:
                arduino.close()
            except:
                pass
            print("Esperando reconexión de VolControl-Micro…\n")
            time.sleep(1)

if __name__ == "__main__":
    main()
