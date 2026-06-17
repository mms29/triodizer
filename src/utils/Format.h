#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

const inline juce::String formatVoltage (float v)
{
    auto abs = std::abs(v);
    if (abs >= 1e3) return juce::String (v / 1e3, 1) + " kV";
    if (abs >= 10) return juce::String ((int)v) + " V";
    if (abs >= 1e-1) return juce::String (v, 1) + " V";
    if (abs < 1e-1 ) return juce::String (v * 1e3, 1) + " mV";
    return juce::String (v, 0);
}
const inline  juce::String formatCurrent (float v)
{
    auto abs = std::abs(v);
    if (abs >= 1) return juce::String (v, 1) + " A";
    if (abs >= 1e-4 ) return juce::String (v * 1e3, 1) + " mA";
    if (abs < 1e-4 ) return juce::String (v * 1e6, 1) + " uA";
    return juce::String (v);
}