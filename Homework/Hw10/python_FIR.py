import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


# Read the csv file
data = pd.read_csv('signals/sigD.csv', header=None)
# Extract the columns
t = data[0].values  # time
s = data[1].values  # signal

# Number of points to average
X = 461

#Load FIR Filter Coefficients
coefficients = np.loadtxt('signals/coefficients.csv')


# Storage for filtered data
s_fir = [s[0]]
avg_buffer = np.zeros(X)


for i in range(1, len(s)):
    avg_buffer = np.insert(avg_buffer, 0, s[i])[:-1]
    s_fir.append(np.dot(avg_buffer, coefficients))
    

plt.plot(t, s, 'k')
plt.xlabel('Time')
plt.ylabel('Amplitude')
plt.plot(t, s_fir, 'r')  # changed color to red for the filtered data
plt.legend(["Raw Data", "Filtered Data"])
plt.title("Low Pass FIR of sigD | Cutoff Frequency = 10 Hz, Bandwidth = 10Hz | Order = 461")
plt.grid()
plt.show()