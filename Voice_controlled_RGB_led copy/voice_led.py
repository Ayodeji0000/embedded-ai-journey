import speech_recognition as sr
import serial
import time
from difflib import get_close_matches

# Connect to Arduino
arduino = serial.Serial('/dev/tty.usbmodem101', 9600)
time.sleep(2)

# Define accepted color words and similar variations
color_aliases = {
    "red": ["red", "read", "raid", "ride"],
    "green": ["green", "grain"],
    "blue": ["blue", "blew"],
    "yellow": ["yellow"],
    "white": ["white", "why", "wide", "light"],
    "off": ["off", "of", "stop", "turn off", "shut", "dark"]
}

color_codes = {
    "red": "R",
    "green": "G",
    "blue": "B",
    "yellow": "Y",
    "white": "W",
    "off": "O"
}

recognizer = sr.Recognizer()
mic = sr.Microphone()

print("🎤 Say a color: red, green, blue, yellow, white, or off")

while True:
    with mic as source:
        recognizer.adjust_for_ambient_noise(source, duration=0.7)
        print("Listening...")
        audio = recognizer.listen(source, phrase_time_limit=3)

    try:
        command_raw = recognizer.recognize_google(audio).lower().strip()
        print(f"You said (raw): {command_raw}")

        # Try to match to one of our color aliases
        matched_color = None
        for color, aliases in color_aliases.items():
            if any(alias in command_raw for alias in aliases):
                matched_color = color
                break

        if not matched_color:
            # fallback to fuzzy matching
            all_words = [w for sub in color_aliases.values() for w in sub]
            guess = get_close_matches(command_raw, all_words, n=1, cutoff=0.6)
            if guess:
                for color, aliases in color_aliases.items():
                    if guess[0] in aliases:
                        matched_color = color
                        break

        if matched_color:
            arduino.write(color_codes[matched_color].encode())
            print(f"✅ LED set to: {matched_color.upper()}")
        else:
            print("❌ Couldn't recognize a color. Try again...")

    except sr.UnknownValueError:
        print("Could not understand audio.")
    except sr.RequestError:
        print("⚠️ Speech recognition service error.")
