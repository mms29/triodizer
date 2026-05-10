import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from scipy.io import wavfile
from scipy.signal import spectrogram


names = [
    "Sine_440Hz",
    "Square_440Hz",
    "Sawtooth_440Hz",
    "Sweep_20Hz-20kHz",
]
drive_val = [1,2,4,8]

audio = {n: [[],[]] for n in names}
csv = {n: [] for n in names}

for n in names:
    for d in drive_val:
        prefix = "./outputs/%s_drive_%i"%(n,d)
        sample_rate, audio_in = wavfile.read(prefix+"_input.wav")
        sample_rate, audio_out = wavfile.read(prefix+"_output.wav")
        audio[n][0].append(audio_in)
        audio[n][1].append(audio_out)

        data = pd.read_csv(prefix+"_monitoring.csv")
        data["Vgk"] = data["Grid_Voltage(V)"] - data["Cathode_Voltage(V)"]
        data["Triode_Vgk"] = data["Triode_Vg(V)"] - data["Triode_Vk(V)"]
        csv[n].append(data)


monitor = ["Grid_Voltage(V)", "Triode_Vg(V)" , "Triode_Vk(V)", "Triode_Vp(V)", "Vgk", "Triode_Vgk", "VgIters", "VkIters", "PPIters"]
# monitor = [ "Triode_Vk(V)", "Triode_Vp(V)","Vgk", "Triode_Vgk"]

nrow = 3
ncol = 3
fig, ax = plt.subplots(nrow,ncol, figsize=(15,10), layout="constrained")
for i in range(nrow):
    for j in range(ncol):
        idx= i * ncol + j 
        axij = ax[i,j]
        m =monitor[idx]
        axij.set_title(m)
        axij.axhline(0, ls="--",c="grey")

        if "Iter" in m:
            ls="."
        else:
            ls="-"
        for d,_ in enumerate(drive_val):
            axij.plot(csv["Sweep_20Hz-20kHz"][d][m][436000:436250], ls)
fig.show()



fig, ax = plt.subplots(2,2, figsize=(15,10), layout="constrained")
for i in range(2):
    for j in range(2):
        idx= i * 2 + j 
        axij = ax[i,j]
        name =names[idx]

        axij.set_title(name)
        axij.plot(audio[name][0][0][436000:438000])
        for ii, d in enumerate(drive_val) : 
            axij.plot(audio[name][1][ii][436000:438000], label="drive=%i"%d)
fig.show()


fig, ax = plt.subplots(2,2, figsize=(15,10), layout="constrained")
for i in range(2):
    for j in range(2):
        idx= i * 2 + j 
        axij = ax[i,j]
        name =names[idx]

        axij.set_title(name)
        for ii, d in enumerate(drive_val) : 
            axij.plot(csv[name][ii]["Plate_Voltage(V)"][436000:438000], label="drive=%i"%d)
fig.show()
fig, ax = plt.subplots(2,2, figsize=(15,10), layout="constrained")
for i in range(2):
    for j in range(2):
        idx= i * 2 + j 
        axij = ax[i,j]
        name =names[idx]

        axij.set_title(name)
        for ii, d in enumerate(drive_val) : 
            axij.plot(csv[name][ii]["Triode_Vp(V)"][436000:438000], label="drive=%i"%d)
fig.show()





fig, ax = plt.subplots(2,2, figsize=(15,10), layout="constrained")
for i in range(2):
    for j in range(2):
        idx= i * 2 + j 
        axij = ax[i,j]

        # Compute spectrogram
        f, t, Sxx = spectrogram(
            audio["Sweep_20Hz-20kHz"][1][idx],
            fs=sample_rate,
            window='hann',
            nperseg=2048,
            noverlap=1024,
            scaling='density',
            mode='magnitude'
        )

        # Convert to dB (better visualization)
        Sxx_db = 20 * np.log10(Sxx + 1e-12)
        Sxx_db[Sxx_db<-60] = -60

        axij.pcolormesh(t, f, Sxx_db, shading='gouraud')
        axij.set_ylabel('Frequency [Hz]')
        axij.set_xlabel('Time [s]')
        axij.set_title('Spectrogram')
        axij.set_ylim(0, sample_rate / 2)

fig.show()
