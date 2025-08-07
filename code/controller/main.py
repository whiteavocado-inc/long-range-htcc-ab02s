#pip install pyserial

import time, os, threading, sys
import serial.tools.list_ports

baud = 921600
portName = ""

def inputThread(ser):
    while True:
        try:
            msg = input("> ")
            if (msg == 'q'): exit(0)
            ser.write((msg).encode())
        
        except Exception:
            print("Input error")

def cls():
    if (os.name == "nt"): os.system("cls")
    else: os.system("clear")
    

while True:
    cls()
    print("Welcome to LoRa controller! (q to exit)\n\nPlease select a device based on index:\n")

    devices = []

    i = -1

    for p in list(serial.tools.list_ports.comports()):
        if "CP2102" not in p.description: continue
        i += 1
        devices.append([i, p])
        print(f"[{i}] {p}")
        
    deviceIndex = -1
    errMsg = "Invalid index, please try again"
    
    deviceIndex = input("\n> ")
    if (deviceIndex == 'q'):
        cls()
        exit(0)
    
    try: deviceIndex = int(deviceIndex)
    except:
        print(errMsg)
        input()
        continue

    if (deviceIndex not in [d[0] for d in devices]):
        print(errMsg)
        input()
        continue
    
    for i, p in devices:
        if (i == deviceIndex):
            portName = p.device
            break
    
    break

cls()
try:
    ser = serial.Serial(portName, baud, timeout=1)
    print(f"Connected to '{portName}'\n")
    
    t = threading.Thread(target=inputThread, args=(ser,), daemon=True)
    t.start()
    
    while True:
        if ser.in_waiting:
            line = ser.readline().decode('utf-8', errors='replace').strip()
            
            sys.stdout.write('\r' + ' ' * 80 + '\r')
            print(line)
            sys.stdout.write("> ")
            sys.stdout.flush()
        
        else: time.sleep(0.1)
        
        
except Exception as e:
    print(f"Error: Device disconnected\n\n{e}")