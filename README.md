# Cathodyne

Cathodyne is a playground for audio nerds and curious musicians who like turning knobs, dirty sounds, and messing with electronic circuits.

![Cathodyne](screenshot.png) 

## Features

- Interactive electronic schematics, to experiment with component values and tube types
- Real-time circuit simulation, to hear the effect of every change you make
- Tube stages, diode clippers, and classic amplifier circuits to explore

## Under the hood

Instead of approximating the whole circuit as a simple effect, Cathodyne simulates the entire electronic circuits using Wave Digital Filters (WDF) as its audio engine.
It simulates the voltages and currents flowing through the individual components. So when you change a resistor, a tube, or a diode, you're actually changing the circuit. The goal? Get as close as possible to the real sound — without needing a soldering iron.

## Acknoledgements

This project builds upon some great open-source tools, thanks a lot to them! 

- chowdsp_wdf
- JUCE framework