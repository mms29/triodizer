# Cathodyne

Cathodyne is an electronic circuit playground for audio nerds, and curious musicians who like turning knobs and to see what happens. Just circuits, sound, and knobs to mess with.

![Cathodyne](screenshot.png)

## Under the hood

Cathodyne uses Wave Digital Filters (WDF) to power its audio engine.

Instead of approximating the whole circuit as a simple effect, it simulates the voltages and currents flowing through the individual components.

So when you change a resistor, tube, or diode, you're actually changing the circuit.

The goal? Get as close as possible to the real thing — without needing a soldering iron.

## Acknoledgements

This project builds upon some great opensource tools, thanks a lot to them! 

- chowdsp_wdf
- JUCE framework

## Citations 

- D'Angelo, S., Pakarinen, J., & Valimaki, V. (2012). New family of wave-digital triode models. IEEE transactions on audio, speech, and language processing, 21(2), 313-321.
 - Zhang, J., & Smith III, J. O. (2018, September). Real-time wave digital simulation of cascaded vacuum tube amplifiers using modified blockwise method. In Proc. 21st Int. Conf. Digital Audio Effects (DAFx-18). Aveiro: University of Aveiro.
- Giampiccolo, R., D'Angelo, S., Bernardini, A., & Sarti, A. (2023). A quadric surface model of vacuum tubes for virtual analog applications. In Proceedings of the 26th International Conference on Digital Audio Effects (DAFx23) (pp. 296-303).
