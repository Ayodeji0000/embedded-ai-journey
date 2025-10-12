import serial
import time
import joblib
import numpy as np
import matplotlib.pyplot as plt
from collections import deque

# 👇 Change to your Arduino's actual port
PORT = "/dev/tty.usbmodem101"
BAUD = 9600

# Load trained model
model = joblib.load("occupancy_model.pkl")
print("✅ Model loaded. Starting live graph...\n")

# Connect to Arduino
try:
    ser = serial.Serial(PORT, BAUD, timeout=1)
    time.sleep(2)
except Exception as e:
    print("❌ Could not open serial port:", e)
    print(" - Make sure Arduino is plugged in and Serial Monitor is closed.")
    exit()

# Data buffers for graph (stores last 100 points)
light_data = deque(maxlen=100)
motion_data = deque(maxlen=100)
prediction_data = deque(maxlen=100)
time_data = deque(maxlen=100)

# Set up live plot
plt.ion()
fig, ax = plt.subplots(figsize=(9, 5))
line1, = ax.plot([], [], label="Light", color="gold")
line2, = ax.plot([], [], label="Motion", color="orange")
line3, = ax.plot([], [], label="Prediction (1=Occupied, 0=Empty)", color="green")
ax.set_ylim(-0.2, 1.2)
ax.set_xlabel("Time (seconds)")
ax.set_ylabel("Sensor / Prediction")
ax.legend(loc="upper right")
plt.title("Live Room Occupancy Prediction")

start_time = time.time()

# Continuous update loop
while True:
    try:
        line = ser.readline().decode(errors="ignore").strip()
        if not line or line.startswith("time_ms"):
            continue

        parts = line.split(",")
        if len(parts) < 4:
            continue

        light = int(parts[1])
        motion = int(parts[2])

        # Predict occupancy
        X = np.array([[light, motion]])
        prediction = model.predict(X)[0]

        # Append data
        current_time = time.time() - start_time
        light_data.append(light / 1023)  # normalize 0–1 for plotting
        motion_data.append(motion)
        prediction_data.append(prediction)
        time_data.append(current_time)

        # Update plot
        line1.set_data(time_data, light_data)
        line2.set_data(time_data, motion_data)
        line3.set_data(time_data, prediction_data)
        ax.set_xlim(max(0, current_time - 30), current_time + 1)  # 30s window
        plt.pause(0.05)

        # Console output
        status = "🟢 Occupied" if prediction == 1 else "⚪ Empty"
        print(f"Light={light:4d}, Motion={motion}, Prediction={status}")

    except KeyboardInterrupt:
        print("\n🛑 Live graph stopped.")
        break
    except Exception as e:
        print("⚠️ Error:", e)