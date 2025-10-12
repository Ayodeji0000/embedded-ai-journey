import serial
import time
import os

# 👇 Change this to your Arduino port from `ls /dev/tty.*`
PORT = "/dev/tty.usbmodem101"
BAUD = 9600
OUT_FILE = "occupancy.csv"

def ensure_header():
    if not os.path.exists(OUT_FILE):
        with open(OUT_FILE, "w") as f:
            f.write("time_ms,light,motion,label\n")

def main():
    ensure_header()
    print(f"Connecting to {PORT} at {BAUD} baud...")
    try:
        ser = serial.Serial(PORT, BAUD, timeout=1)
    except Exception as e:
        print("ERROR: Could not open serial port:", e)
        print(" - Make sure Arduino is plugged in and Serial Monitor is closed.")
        print(" - Check the port name with: ls /dev/tty.*")
        return

    time.sleep(2)  # wait for Arduino reset
    print(f"Logging to {OUT_FILE} (press Ctrl+C to stop)\n")
    count = 0
    try:
        while True:
            line = ser.readline().decode(errors="ignore").strip()
            if not line:
                continue
            if line.startswith("time_ms"):
                continue
            parts = line.split(',')
            if len(parts) < 4:
                continue
            with open(OUT_FILE, "a") as f:
                f.write(line + "\n")
            count += 1
            if count % 10 == 0:
                print(f"Logged {count} lines. Last: {line}")
    except KeyboardInterrupt:
        print("\nStopped logging. Total lines:", count)
    finally:
        ser.close()

if __name__ == "__main__":
    main()