#pragma once

#include "utils/Glow.h"
class PathToggleButton : public juce::Button
{
public:
    PathToggleButton (juce::Path p,
                      juce::String labelText,
                      juce::Colour offColour,
                      juce::Colour onColour)
        : juce::Button ("PathToggle"),
          shape (std::move (p)),
          text (std::move (labelText)),
          off (offColour),
          on (onColour)
    {
        setClickingTogglesState (true);
    }

    void paintButton (juce::Graphics& g,
                      bool isMouseOver ,
                      bool) override;

private:
    juce::Path shape;
    juce::String text;
    juce::Colour off, on;
};



juce::Path createSineWavePath (juce::Rectangle<float> bounds,
                               float cycles = 2.0f,
                               float amplitudeRatio = 0.35f);