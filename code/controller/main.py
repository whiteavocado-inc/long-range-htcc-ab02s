#pip install pyserial pycryptodome python-dotenv

import time, os, threading, sys, base64
import serial.tools.list_ports
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad, unpad
from dotenv import load_dotenv

load_dotenv()
def env(index): return os.getenv(index)

baud = int(env("BAUD"))
portName = ""
nickName = env("NICKNAME")
            

def cls():
    if (os.name == "nt"): os.system("cls")
    else: os.system("clear")
    
def encrypt(txt: str) -> str:
    data = txt.encode("utf-8")
    c = int(env("AES_KEY_COUNT"))
    for i in range(c):
        key = env(f"AES_KEY_{ i + 1 }").encode("utf-8")
        iv = os.urandom(16)
        cipher = AES.new(key, AES.MODE_CBC, iv)
        data = iv + cipher.encrypt(pad(data, AES.block_size))
        data = base64.b64encode(data)
        
    return data.decode("utf-8")

def decrypt(txt: str) -> str:
    data = txt.encode("utf-8")
    c = int(env("AES_KEY_COUNT"))
    for i in reversed(range(c)):
        key = env(f"AES_KEY_{ i + 1 }").encode("utf-8")
        data = base64.b64decode(data)
        iv = data[:16]
        encrypted = data[16:]
        cipher = AES.new(key, AES.MODE_CBC, iv)
        data = cipher.decrypt(encrypted)
        if i == 0: data = unpad(data, AES.block_size)
        
    return data.decode("utf-8")
    

while True:
    cls()
    print("Welcome to LoRa controller! (q to exit)\n\nPlease select a device based on index:\n")

    devices = []

    i = -1

    for p in list(serial.tools.list_ports.comports()):
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



while True:
    cls()
    e = "Invalid option"
    choise = -1
    try: choise = int(input("Which mode do you want to use?\n\n[0: receive | 1: send]\n> "))
        
    except Exception:
        print(e)
        continue
    
    if (choise != 0 and choise != 1):
        print(e)
        continue
    
    break

try:
    cls()
    ser = serial.Serial(portName, baud, timeout=1)
    print(f"Connected to '{portName}'\n")
    
except:
    print(f"Could not connect to { portName }")
    exit(1)

#Receive
if (choise == 0):
    print("[receive]\n")
    try:
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
        exit(1)
#

#Send
print("[send]\n")
while True:
    try:
        msg = input(f"you ({ nickName }) > ")
        msg = f"{ nickName }: { msg }"
        msg = encrypt(msg)
        ser.write((msg + "\n").encode())
    
    except Exception:
        print("Input error")
#