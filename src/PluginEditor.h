#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class TriodeEditor : public juce::AudioProcessorEditor
{
public:
    TriodeEditor(TriodeProcessor&);
    ~TriodeEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    TriodeProcessor& audioProcessor;

    juce::Slider driveSlider;
    juce::Slider gainSlider;
    juce::Label driveLabel;
    juce::Label gainLabel;

    juce::ComboBox oversampleSelector;
    juce::Label oversampleLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> oversampleAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TriodeEditor)
};
