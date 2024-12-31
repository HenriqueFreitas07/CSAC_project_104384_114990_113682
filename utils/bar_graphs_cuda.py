import matplotlib.pyplot as plt
import numpy as np

# Data
time_intervals = [120, 180, 240, 300]
gpu_names = ["RTX 4060", "GTX 1660 Ti", "RTX 3070"]

# Attempts data (per time interval for each GPU)
attempts_data = [
    [0, 1405259612160, 2402094678016],  # 120 seconds
    [1954210119680, 2108963160064, 3700986740736],  # 180 seconds
    [0, 2803808337920, 5003569790976],  # 240 seconds
    [3175994097664, 3476977352704, 6072681103360],  # 300 seconds
]

# Coins data (per time interval for each GPU)
coins_data = [
    [0, 312, 567],  # 120 seconds
    [446, 507, 847],  # 180 seconds
    [0, 647, 1144],  # 240 seconds
    [700, 797, 1382],  # 300 seconds
]

# Create numpy arrays for easier plotting
attempts = np.array(attempts_data).T  # Transpose for bar grouping
coins = np.array(coins_data).T  # Transpose for bar grouping

# Plot CUDA Attempts
fig1, ax1 = plt.subplots(figsize=(10, 6))
bar_width = 20
x_positions = np.array(time_intervals) * 1.5

for i, gpu in enumerate(gpu_names):
    ax1.bar(x_positions + i * bar_width, attempts[i], width=bar_width, label=gpu)

ax1.set_title("Number of Attempts For Each GPU", fontsize=14)
ax1.set_xlabel("Time (s)", fontsize=12)
ax1.set_ylabel("NºAttempts", fontsize=12)
ax1.set_xticks(x_positions + bar_width)
ax1.set_xticklabels(time_intervals)
ax1.legend(title="GPU Models")
ax1.grid(axis="y", linestyle="--", alpha=0.7)

# Save the Attempts graph
plt.savefig("cuda_attempts.png", dpi=300, bbox_inches="tight")

# Plot CUDA Coins
fig2, ax2 = plt.subplots(figsize=(10, 6))

for i, gpu in enumerate(gpu_names):
    ax2.bar(x_positions + i * bar_width, coins[i], width=bar_width, label=gpu)

ax2.set_title("Number of Coins Mined By Each GPU", fontsize=14)
ax2.set_xlabel("Time (s)", fontsize=12)
ax2.set_ylabel("NºCoins", fontsize=12)
ax2.set_xticks(x_positions + bar_width)
ax2.set_xticklabels(time_intervals)
ax2.legend(title="GPU Models")
ax2.grid(axis="y", linestyle="--", alpha=0.7)

# Save the Coins graph
plt.savefig("cuda_coins.png", dpi=300, bbox_inches="tight")

