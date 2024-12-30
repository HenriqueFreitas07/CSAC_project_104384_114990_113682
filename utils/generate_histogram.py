import matplotlib.pyplot as plt
import numpy as np

# Histogram data
powers = np.array([
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33
])
occurrences = np.array([
    1429595418, 714797812, 357423072, 178720347, 89356156, 44670167, 
    22333690, 11174039, 5586503, 2794137, 1399170, 697614, 348340, 
    174245, 87448, 43691, 21705, 10944, 5324, 2736, 1380, 659, 365, 174, 
    97, 48, 18, 16, 6, 4, 1, 1, 1
])

# Calculate expected probabilities
expected_probs = 1 / (2 ** powers)
expected_occurrences = expected_probs * sum(occurrences)

# Plot the histogram and expected distribution
plt.figure(figsize=(12, 6))
plt.bar(powers, occurrences, label="Observed Occurrences", alpha=0.7)
plt.plot(powers, expected_occurrences, label=r"Expected $\frac{1}{2^n}$", color="red", marker='o')

# Add labels and legend
plt.xlabel("Power (n)")
plt.ylabel("Occurrences")
plt.title("Histogram of DETI Coin Powers vs Expected Distribution (log scale) - 5 min")
plt.yscale("log")  # Log scale to handle wide range of values
plt.legend()
plt.grid(axis="y", which="both", linestyle="--", alpha=0.7)

# Save the plot to a file
plt.savefig("deti_coin_histogram.png")
