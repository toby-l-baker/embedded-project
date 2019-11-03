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
        self.mag = 2.0/LEN_DATA * abs(self.dataFT[0:LEN_DATA//2])

LEN_DATA = 1024
F_SAMPLE = 4000
dt = 1/F_SAMPLE
time = np.linspace(0, LEN_DATA*dt, LEN_DATA)
freq = np.linspace(0.0, F_SAMPLE//2, LEN_DATA//2)

data = np.genfromtxt("../accelerometer_calibration/4kHz.txt", delimiter=',')
x_data = Data(data[:, 0])
y_data = Data(data[:, 1])
z_data = Data(data[:, 2])

plt.figure()
plt.plot(time, x_data.data)
plt.plot(time, y_data.data)
plt.plot(time, z_data.data)

plt.figure()
plt.plot(freq, x_data.mag, 'r')
plt.plot(freq, y_data.mag, 'g')
plt.plot(freq, z_data.mag, 'b')

plt.show()
