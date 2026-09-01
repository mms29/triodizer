#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "plugin/PluginProcessor.h"

// ==============================================================================
// WaveformDisplay: Component for visualizing input/output waveforms
class WaveformDisplay : public juce::Component
{
public:
    WaveformDisplay (CathodyneProcessor& p) : processor (p) {}

    void paint (juce::Graphics& g) override;

    void setWaveformColour (juce::Colour c) { waveformColour = c; }
    void setBackgroundColour (juce::Colour c) { backgroundColour = c; }
    void setAmplitudeScale (float scale) { amplitudeScale = scale; }
    void updateOutputGain () { outputGain = processor.getWaveformOutputGain(); }

private:
    CathodyneProcessor& processor;

    static constexpr int displaySize = 1024;
    std::array<float, displaySize> inputBuffer {};
    std::array<float, displaySize> outputBuffer {};

    juce::Colour waveformColour { getColourHotRed() };
    juce::Colour backgroundColour { getColourBackground() };
    float amplitudeScale = 100.0f;
    float outputGain =1.0f;
};