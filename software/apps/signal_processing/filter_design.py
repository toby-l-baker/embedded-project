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
        for i in range(len(taps), len(self.data)):
            output[i] = data[i:i+len(taps)].dot(np.transpose(taps))
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
# x_data = Data(data[:, 0])
# y_data = Data(data[:, 1])
# z_data = Data(data[:, 2])

numtaps = 50

taps = signal.firwin(numtaps, 2/F_SAMPLE, window='blackman')
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
plt.ylim([1.2, 1.6])
plt.xlabel('t')
plt.grid(True)


plt.show()
