#!/usr/bin/env python3
"""
Usage (from conda env):
  conda run -n audio-test python test_plugin.py
"""

import numpy as np
import matplotlib.pyplot as plt
from pedalboard import Pedalboard, load_plugin
import soundfile as sf
import os

# ─── Configuration ───────────────────────────────────────────────
vst3_path = "/Users/vuillemr/Library/Audio/Plug-Ins/VST3/TriodePlugin.vst3"
sample_rate = 96000
duration = 0.05  # 100ms per test tone
test_freq = 440.0  # A4

# ─── Check plugin exists ───────────────────────────────────────────
if not os.path.exists(vst3_path):
    print(f"ERROR: VST3 plugin not found at {vst3_path}")
    print("Trying AU instead...")
    vst3_path = None

# ─── Generate test signals ───────────────────────────────────────
def generate_sine(freq, duration, sr):
    t = np.linspace(0, duration, int(sr * duration), endpoint=False)
    return np.sin(2 * np.pi * freq * t).astype(np.float32)

def generate_square(freq, duration, sr):
    t = np.linspace(0, duration, int(sr * duration), endpoint=False)
    return (np.sign(np.sin(2 * np.pi * freq * t)) * 0.8).astype(np.float32)

def generate_saw(freq, duration, sr):
    t = np.linspace(0, duration, int(sr * duration), endpoint=False)
    return (2.0 * (t * freq - np.floor(t * freq + 0.5))).astype(np.float32)

def generate_sweep(duration, sr):
    """Log-frequency sweep from 20Hz to 20kHz."""
    t = np.linspace(0, duration, int(sr * duration), endpoint=False)
    freq = 20.0 * (20000.0 / 20.0) ** (t / duration)
    phase = 2 * np.pi * np.cumsum(freq) / sr
    return np.sin(phase).astype(np.float32)

# ─── Load plugin ─────────────────────────────────────────────────
print("Loading plugin...")
try:
    plugin = load_plugin(vst3_path)
    print(f"Loaded VST3: {vst3_path}")
    print("Plugin parameters:")
    print(plugin.parameters)
        
except Exception as e:
    print(f"Failed to load plugin: {e}")
    print("\nNote: For VST3/AU hosting, make sure:")
    print("  1. The plugin is built and installed")
    print("  2. pedalboard supports this plugin format on macOS")
    print("  3. You may need to grant permissions (System Settings → Privacy → Audio)")
    raise



# ─── Process and plot ────────────────────────────────────────────
def processSignal(signal, sr, d):
    """Process signal """
    # plugin.cutoff_frequency_hz = fc  # Set freq parameter
    # print(f"Set freq = {plugin.cutoff_frequency_hz}")
    plugin.drive=d
    # plugin.gain=1/d
    stereo_in = np.stack([signal, signal], axis=0)
    stereo_out = plugin(stereo_in, sr, reset=True)
    return stereo_out[0]

def plotSignals(originalSignal, processedSignals, name,d,  length=0.05):
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))

    for i in range(2):
        for j in range(2):
            idx = i*2+j
            axes[i,j].plot(originalSignal[-int(sample_rate*length):]*d[idx])
            axes[i,j].plot(processedSignals[idx][-int(sample_rate*length):])
    return fig


# ─── Run tests ─────────────────────────────────────────────────
signals = {
    "Sine 440Hz": generate_sine(test_freq, duration, sample_rate),
    "Square 440Hz": generate_square(test_freq, duration, sample_rate),
    "Sawtooth 440Hz": generate_saw(test_freq, duration, sample_rate),
    "Sweep 20Hz-20kHz": generate_sweep(duration, sample_rate),
}


drive_values = [1.0, 50.0, 100.0, 1000.0]

processedSignals = {k:[processSignal(v, sample_rate, d) for d in drive_values] for k,v in signals.items()}


for i, (name, signal) in enumerate(processedSignals.items()):
    print(f"\nProcessing: {name}...")    
    fig = plotSignals(signals[name], signal,name, drive_values)

    plot_path = f"/Users/vuillemr/PremierPlugin/outputs/{name.replace(' ', '_')}_plot.png"
    fig.savefig(plot_path, dpi=150)
    print(f"  Saved: {plot_path}")
    plt.close(fig)
