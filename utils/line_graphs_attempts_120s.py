import matplotlib.pyplot as plt
import numpy as np

# Data
techniques = [
    "CPU", "AVX", "AVX2", "SPECIAL", "OpenMP", "MPI", "CUDA", "OpenCL"
]

rtx_4060 = [None, None, None, None, None, None, None, None]
gtx_1660_ti = [1429157578, 4085920632, 7764981448, 1615264917, 15047169908, 15001020376, 1405259612160, None]
i5_4210u = [841453548, 2165617912, 4262380872, 914049090, 6555845880, 6509774132, None, None]
rtx_3070 = [1371730021, 3410690388, 6797266080, 1584884714, 13762832384, 13703463816, 2402094678016, None]

# Convert None to np.nan for plotting gaps in the graph
rtx_4060 = [val if val is not None else np.nan for val in rtx_4060]
gtx_1660_ti = [val if val is not None else np.nan for val in gtx_1660_ti]
i5_4210u = [val if val is not None else np.nan for val in i5_4210u]
rtx_3070 = [val if val is not None else np.nan for val in rtx_3070]

# Plot the graph
plt.figure(figsize=(12, 6))
plt.plot(techniques, rtx_4060, marker="o", label="RTX 4060", color="blue")
plt.plot(techniques, gtx_1660_ti, marker="o", label="GTX 1660 Ti", color="green")
plt.plot(techniques, i5_4210u, marker="o", label="i5-4210U", color="red")
plt.plot(techniques, rtx_3070, marker="o", label="RTX 3070", color="orange")

# Add titles and labels
plt.title("Number of Attempts For Different Techniques - 120s", fontsize=14)
plt.xlabel("Technology", fontsize=12)
plt.ylabel("NºAttempts", fontsize=12)

# Configure legend and grid
plt.legend(title="GPU Models", fontsize=10)
plt.grid(axis="y", linestyle="--", alpha=0.7)
plt.yscale("log")  # Use logarithmic scale for better visualization of large differences

# Save the graph as a PNG file
plt.savefig("gpu_attempts_120s.png", dpi=300, bbox_inches="tight")