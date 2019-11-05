"""
Title: Filter design IMU
  Created: 2/11/2019
  Author: Toby Baker
"""

import matplotlib.pyplot as plt
import numpy as np
from scipy import signal

def filter(data, taps):
    output = np.zeros(len(data)-len(taps))
    for i in range(0, len(data)-len(taps)):
        output[i] = data[i:i+len(taps)].dot(np.transpose(taps))
    return np.hstack((np.ones(len(taps)),output))


LEN_DATA = 1024
F_SAMPLE = 4000
dt = 1/F_SAMPLE
time = np.linspace(0, LEN_DATA*dt, LEN_DATA)
time_long = np.linspace(0, 4096*dt, 4096)
freq = np.linspace(0.0, F_SAMPLE//2, LEN_DATA//2)
freq_long = np.linspace(0, F_SAMPLE//2, 4096//2)

cal_s = np.genfromtxt("../accelerometer_calibration/calibration.txt", delimiter=',')
cal_m = np.genfromtxt("../accelerometer_calibration/calibration_moving.txt", delimiter=',')

numtaps = 50

taps = signal.firwin(numtaps, 2/F_SAMPLE, window='blackman')
# filtered_x = signal.lfilter(taps, 1.0, x_long.data)

plt.figure(1)
# Plot the original signal
plt.plot(cal_m[:,0], 'b-', alpha=0.2)
plt.plot(filter(cal_m[:,0], taps), 'g-')
plt.plot(cal_m[:,3], 'r-')
plt.figure(2)
plt.plot(cal_m[:,1], 'b-', alpha=0.2)
plt.plot(filter(cal_m[:,1], taps), 'g-')
plt.plot(cal_m[:,4], 'r-')
plt.figure(3)
plt.plot(cal_m[:,2], 'b-', alpha=0.2)
plt.plot(filter(cal_m[:,2], taps), 'g-')
plt.plot(cal_m[:,5], 'r-')
# Plot the filtered signal, shifted to compensate for the phase delay.
plt.grid(True)

plt.show()
