"""
Title: Filter design IMU
  Created: 2/11/2019
  Author: Toby Baker
"""

import matplotlib.pyplot as plt
import numpy as np
from scipy import signal
from scipy.fftpack import fft

class Data:
    def __init__(self, data):
        self.data = data
        self.dataFT = fft(data)
        self.mag = 2.0/len(data) * abs(self.dataFT[0:len(data)//2])

    def filter(self, taps):
        output = np.zeros(len(self.data)-len(taps))
        for i in range(0, len(self.data)-len(taps)):
            output[i] = self.data[i:i+len(taps)].dot(np.transpose(taps))
        return output


LEN_DATA = 1024
F_SAMPLE = 4000
dt = 1/F_SAMPLE
time = np.linspace(0, LEN_DATA*dt, LEN_DATA)
time_long = np.linspace(0, 4096*dt, 4096)
freq = np.linspace(0.0, F_SAMPLE//2, LEN_DATA//2)
freq_long = np.linspace(0, F_SAMPLE//2, 4096//2)

data = np.genfromtxt("../accelerometer_calibration/4kHz.txt", delimiter=',')
x_long = np.genfromtxt("../accelerometer_calibration/4kHz_x.txt", delimiter=',')
x_long = Data(x_long)
x_data = Data(data[:, 0])
y_data = Data(data[:, 1])
z_data = Data(data[:, 2])

numtaps = 30

taps = signal.firwin(numtaps, 4/F_SAMPLE, window='blackman')
filtered_x = signal.lfilter(taps, 1.0, x_long.data)

#------------------------------------------------
# Plot the FIR filter coefficients.
#------------------------------------------------

plt.figure(1)
plt.plot(taps, 'bo-', linewidth=2)
plt.title('Filter Coefficients (%d taps)' % numtaps)
plt.grid(True)

#------------------------------------------------
# Plot the magnitude response of the filter.
#------------------------------------------------

plt.figure(2)
plt.clf()
w, h = signal.freqz(taps, worN=8000)
plt.plot((w/np.pi)*F_SAMPLE//2, np.abs(h), linewidth=2)
plt.xlabel('Frequency (Hz)')
plt.title('Frequency Response')
plt.ylabel('Gain')
plt.ylim(-0.05, 1.05)
plt.xlim(0, 300)
plt.grid(True)

#------------------------------------------------
# Plot the original and filtered signals.
#------------------------------------------------

# The phase delay of the filtered signal.
delay = 0.5 * (numtaps-1) / F_SAMPLE

plt.figure(3)
# Plot the original signal
plt.plot(time_long, x_long.data, 'b-', alpha=0.2)
# Plot the filtered signal, shifted to compensate for the phase delay.
plt.plot(time_long-delay, filtered_x, 'r-')
# Plot just the "good" part of the filtered signal.  The first N-1
# samples are "corrupted" by the initial conditions.
plt.plot(time_long[numtaps-1:]-delay, filtered_x[numtaps-1:], 'g-')
plt.plot(time_long[numtaps:]-delay, x_long.filter(taps), 'r-')
plt.ylim([1.2, 1.6])
plt.xlabel('t')
plt.grid(True)

plt.figure(4)
plt.plot(time[numtaps:], x_data.filter(taps), 'b-')
plt.plot(time[numtaps:], y_data.filter(taps), 'g-')
plt.plot(time[numtaps:], z_data.filter(taps), 'r-')
plt.plot(time, x_data.data, 'r-', alpha=0.2)
plt.plot(time, y_data.data, 'b-', alpha=0.2)
plt.plot(time, z_data.data, 'g-', alpha=0.2)

print("{", end='')
for i, coeff in enumerate(taps):
    print(coeff, end='')
    if (i % 4 == 0) and (i !=0):
        print(',')
    else:
        print(',', end='')
print("}")

# 2Hz, 50 Coefficients
#[-6.74184202e-19  7.23168838e-05  2.94312721e-04  6.80624788e-04
  # 1.25404475e-03  2.04323985e-03  3.07979211e-03  4.39476698e-03
  # 6.01505838e-03  7.95977580e-03  1.02369404e-02  1.28407397e-02
  # 1.57495575e-02  1.89249456e-02  2.23116458e-02  2.58387011e-02
  # 2.94216243e-02  3.29655233e-02  3.63690183e-02  3.95287309e-02
  # 4.23440888e-02  4.47221621e-02  4.65822437e-02  4.78599001e-02
  # 4.85102462e-02  4.85102462e-02  4.78599001e-02  4.65822437e-02
  # 4.47221621e-02  4.23440888e-02  3.95287309e-02  3.63690183e-02
  # 3.29655233e-02  2.94216243e-02  2.58387011e-02  2.23116458e-02
  # 1.89249456e-02  1.57495575e-02  1.28407397e-02  1.02369404e-02
  # 7.95977580e-03  6.01505838e-03  4.39476698e-03  3.07979211e-03
  # 2.04323985e-03  1.25404475e-03  6.80624788e-04  2.94312721e-04
  # 7.23168838e-05 -6.74184202e-19]

# 4Hz, 30 Coefficients
# {-1.1390375091067432E-18,0.00035259598715081734,0.001477766783703178,0.00355893500529667,
# 0.006843359038215591,0.01156690107151631,0.01787102176489292,0.025729190352121574,
# 0.03489853523601125,0.044908166421298065,0.05508897695608773,0.06464209335547222,
# 0.07273590538888708,0.07861610666014734,0.08171044597919938,0.08171044597919938,
# 0.07861610666014733,0.07273590538888705,0.06464209335547222,0.05508897695608773,
# 0.04490816642129805,0.03489853523601124,0.025729190352121564,0.017871021764892907,
# 0.011566901071516297,0.006843359038215583,0.003558935005296659,0.001477766783703178,
# 0.00035259598715081734,-1.1390375091067432E-18}


plt.show()
