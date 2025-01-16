import pandas as pd
import matplotlib.pyplot as plt

# Data
powers = [
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33
]
occurrences = [
    1429595418, 714797812, 357423072, 178720347, 89356156, 44670167,
    22333690, 11174039, 5586503, 2794137, 1399170, 697614, 348340,
    174245, 87448, 43691, 21705, 10944, 5324, 2736, 1380, 659, 365,
    174, 97, 48, 18, 16, 6, 4, 1, 1, 1
]

# Create a DataFrame
data = pd.DataFrame({
    "n": powers,
    "Occurrences": occurrences
})

# Plot the table
fig, ax = plt.subplots(figsize=(8, len(powers) * 0.4))
ax.axis('off')  # Turn off the axes
table = ax.table(
    cellText=data.values,
    colLabels=data.columns,
    cellLoc='center',
    loc='center',
    bbox=[0, 0, 1, 1]  # [left, bottom, width, height]
)
table.auto_set_font_size(False)
table.set_fontsize(10)
table.auto_set_column_width(col=list(range(len(data.columns))))

# Make header bold and bigger
for (i, j), cell in table.get_celld().items():
    if i == 0:  # Header row
        cell.set_text_props(fontweight="bold", fontsize=12)

plt.savefig("histogram_table_bold_headers.png", dpi=300, bbox_inches='tight')  # Save the table as an image
