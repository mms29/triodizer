#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class TriodeEditor : public juce::AudioProcessorEditor,
                      public juce::Slider::Listener
{
public:
    TriodeEditor(TriodeProcessor&);
    ~TriodeEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void sliderValueChanged(juce::Slider*) override;

private:
    TriodeProcessor& audioProcessor;

    juce::Slider driveSlider;
    juce::Slider gainSlider;
    juce::Label driveLabel;
    juce::Label gainLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TriodeEditor)
};
