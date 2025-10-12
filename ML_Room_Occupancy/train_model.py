import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score, confusion_matrix
import matplotlib.pyplot as plt
import seaborn as sns
import joblib
import os

# 1️⃣ Load your dataset
data = pd.read_csv("occupancy.csv")

print("First 5 rows:")
print(data.head(), "\n")
print("Total samples:", len(data))

# 2️⃣ Split into features (X) and labels (y)
X = data[["light", "motion"]]   # inputs
y = data["label"]               # target output

# 3️⃣ Split into train/test sets
X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.2, random_state=42
)

# 4️⃣ Train a Random Forest classifier
model = RandomForestClassifier(n_estimators=100, random_state=42)
model.fit(X_train, y_train)

# 5️⃣ Evaluate the model
y_pred = model.predict(X_test)
acc = accuracy_score(y_test, y_pred)
print(f"Model accuracy: {acc * 100:.2f}%")

# 6️⃣ Confusion matrix visualization
cm = confusion_matrix(y_test, y_pred)
sns.heatmap(
    cm,
    annot=True,
    fmt="d",
    cmap="Blues",
    xticklabels=["Empty", "Occupied"],
    yticklabels=["Empty", "Occupied"],
)
plt.xlabel("Predicted")
plt.ylabel("Actual")
plt.title("Room Occupancy Confusion Matrix")
plt.show()

# 7️⃣ Save the trained model using an absolute path
save_path = os.path.join(os.getcwd(), "occupancy_model.pkl")
joblib.dump(model, save_path)
print(f"✅ Model saved successfully at: {save_path}")