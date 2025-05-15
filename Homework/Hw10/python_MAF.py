import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


# Read the csv file
data = pd.read_csv('signals/sigd.csv', header=None)
# Extract the columns
t = data[0].values  # time
s = data[1].values  # signal

# Number of points to average
X = 100

# Storage for filtered data
s_maf = [];
avg_buffer = np.zeros(X)


for i in range(len(s)):
    avg_buffer = np.insert(avg_buffer, 0, s[i])[:-1]
    s_maf.append(np.mean(avg_buffer))

plt.plot(t, s, 'k')
plt.xlabel('Time')
plt.ylabel('Amplitude')
plt.plot(t, s_maf, 'r')  # changed color to red for the filtered data
plt.legend(["Raw Data", "Filtered Data"])
plt.title("MAF of sigA with X = 100")
plt.show()