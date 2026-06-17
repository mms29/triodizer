#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "utils/Glow.h"


class GlowComboBoxLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawComboBox (juce::Graphics& g,
                    int width,
                    int height,
                    bool isButtonDown,
                    int buttonX,
                    int buttonY,
                    int buttonW,
                    int buttonH,
                    juce::ComboBox&) override;

    void positionComboBoxText (juce::ComboBox& box, juce::Label& label) override;
    void drawLabel (juce::Graphics& g, juce::Label& label) override;

};