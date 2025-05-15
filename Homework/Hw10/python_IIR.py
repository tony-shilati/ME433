import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


# Read the csv file
data = pd.read_csv('signals/sigA.csv', header=None)
# Extract the columns
t = data[0].values  # time
s = data[1].values  # signal

# Number of points to average
X = 50

# IIR Filter Coefficients
A = 0.5
B = 0.5
# Storage for filtered data
s_iir = [s[0]]
avg_buffer = np.zeros(X)


for i in range(1, len(s)):
    avg_buffer = np.insert(avg_buffer, 0, s[i])[:-1]
    s_iir.append(A * s_iir[i-1] + B * s[i])

plt.plot(t, s, 'k')
plt.xlabel('Time')
plt.ylabel('Amplitude')
plt.plot(t, s_iir, 'r')  # changed color to red for the filtered data
plt.legend(["Raw Data", "Filtered Data"])
plt.title("MAF of sigD with A = 0.5, B = 0.5")
plt.grid()
plt.show()