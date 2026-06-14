#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

// ==============================================================================
// Knob: Custom rotary slider component for parameter control
class Knob : public juce::Component
{
public:
    using ValueChangedCallback = std::function<void (float)>;

    Knob (juce::AudioProcessorValueTreeState& apvts,
          const juce::String& parameterID,
          const juce::String& labelText,
          double min,
          double max,
          double step,
          const juce::String& suffix = {});

    Knob (ValueChangedCallback cb,
          const juce::String& labelText,
          double min,
          double max,
          double step,
          const juce::String& suffix = {});

    void init (const juce::String& labelText,
               double min,
               double max,
               double step,
               const juce::String& suffix);

    void resized() override;

    juce::Slider& getSlider();

    juce::String getName() const { return label.getText(); }

private:
    juce::Slider slider;
    juce::Label label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    ValueChangedCallback callback;
};