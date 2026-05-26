#include <gui/Knob.h>

Knob::Knob(juce::AudioProcessorValueTreeState& apvts,
                  const juce::String& parameterID,
                  const juce::String& labelText,
                  double min,
                  double max,
                  double step,
                  const juce::String& suffix)
{
    init(labelText, min, max, step, suffix);        

    attachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts,
            parameterID,
            slider);
}

Knob::Knob(ValueChangedCallback cb,
                             const juce::String& labelText,
                             double min,
                             double max,
                             double step,
                             const juce::String& suffix) : callback(std::move(cb))
{
    init(labelText, min, max, step, suffix);

    slider.onValueChange = [this]()
    {
        if (callback)
            callback((float) slider.getValue());
    };
}

void Knob::init(const juce::String& labelText,
                         double min,
                         double max,
                         double step,
                         const juce::String& suffix)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    slider.setRange(min, max, step);
    slider.setTextValueSuffix(suffix);
    addAndMakeVisible(slider);

    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}

void Knob::resized()
{
    auto area = getLocalBounds();
    auto labelArea = area.removeFromTop(10);
    label.setBounds(labelArea);
    slider.setBounds(area);
}

juce::Slider& Knob::getSlider()
{
    return slider;
}
