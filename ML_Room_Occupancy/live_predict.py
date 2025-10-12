import serial
import time
import joblib
import numpy as np

# 👇 Change this to your Arduino's port name
PORT = "/dev/tty.usbmodem101"
BAUD = 9600

# Load trained model
print("🔍 Loading trained model...")
model = joblib.load("occupancy_model.pkl")
print("✅ Model loaded successfully! Starting live prediction...\n")

# Connect to Arduino
try:
    ser = serial.Serial(PORT, BAUD, timeout=1)
    time.sleep(2)  # Allow Arduino to reset
except Exception as e:
    print("❌ Could not open serial port:", e)
    print(" - Make sure Arduino is plugged in and Serial Monitor is closed.")
    exit()

while True:
    try:
        line = ser.readline().decode(errors="ignore").strip()
        if not line or line.startswith("time_ms"):
            continue

        parts = line.split(",")
        if len(parts) < 4:
            continue

        # Extract LDR and PIR values
        light = int(parts[1])
        motion = int(parts[2])

        # Predict occupancy using trained model
        X = np.array([[light, motion]])
        prediction = model.predict(X)[0]

        status = "🟢 Occupied" if prediction == 1 else "⚪ Empty"
        print(f"Light={light:4d}, Motion={motion}, Prediction={status}")

    except KeyboardInterrupt:
        print("\n🛑 Stopped live prediction.")
        break
    except Exception as e:
        print("⚠️ Error:", e)