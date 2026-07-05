#include <gui/Knob.h>

//==============================================================================

void GlowLookAndFeel::drawRotarySlider (juce::Graphics& g,
                        int x, int y,
                        int width, int height,
                        float sliderPos,
                        float rotaryStartAngle,
                        float rotaryEndAngle,
                        juce::Slider& slider)
{
    auto bounds = juce::Rectangle<float> (x, y, width, height);

    auto radius = juce::jmin (bounds.getWidth(),
                                bounds.getHeight()) * 0.3f;

    auto centre = bounds.getCentre();

    auto angle = rotaryStartAngle
                + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    juce::Path arc;

    arc.addCentredArc (centre.x,
                        centre.y,
                        radius,
                        radius,
                        0.0f,
                        rotaryStartAngle,
                        angle,
                        true);

    drawGlowAndCorePath (g, arc, 0.1f, getColourAmber(), getColourAmber(), false);


    juce::Path arcend;

    arcend.addCentredArc (centre.x,
                        centre.y,
                        radius,
                        radius,
                        0.0f,
                        angle,
                        rotaryEndAngle,
                        true);

    // drawGlowPath (g, arcend, 0.0f, getColourNormal(), getColourNormal(), false);
    g.setColour (getColourGrey().withAlpha(0.5f));
    g.strokePath (arcend, juce::PathStrokeType (1.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // pointer
    juce::Path pointer;

    pointer.startNewSubPath (centre);

    pointer.lineTo (centre.x + std::cos (angle - juce::MathConstants<float>::halfPi) * radius,
                    centre.y + std::sin (angle - juce::MathConstants<float>::halfPi) * radius);

    drawGlowAndCorePath (g, pointer, 0.f, getColourNormal(), getColourNormal(), false);
}

Knob::Knob (juce::AudioProcessorValueTreeState& apvts,
             const juce::String& parameterID,
             const juce::String& labelText,
             double min,
             double max,
             double step,
             const juce::String& suffix)
{
    // init (labelText, min, max, step, suffix);

    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    // slider.setLookAndFeel (&glowLF);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    slider.setRange (min, max, step);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    slider.setTextValueSuffix (suffix);
    slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    slider.setColour (juce::Slider::textBoxTextColourId, getColourNormal());
    addAndMakeVisible (slider);

    label.setText (labelText, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, getColourHighlight());
    addAndMakeVisible (label);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        apvts,
        parameterID,
        slider);
}

Knob::Knob (ValueChangedCallback cb,
             const juce::String& labelText,
             double min,
             double max,
             double step,
             const juce::String& suffix) : callback (std::move (cb))
{
    init (labelText, min, max, step, suffix);

    slider.onValueChange = [this]()
    {
        if (callback)
            callback ((float) slider.getValue());
    };
}

//==============================================================================

void Knob::init (const juce::String& labelText,
                 double min,
                 double max,
                 double step,
                 const juce::String& suffix)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setLookAndFeel (&glowLF);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    slider.setRange (min, max, step);
    // slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    // slider.setRange (min, max, step);
    // slider.setTextValueSuffix (suffix);
    // slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    // slider.setColour (juce::Slider::textBoxTextColourId, getColourNormal());
    addAndMakeVisible (slider);

    label.setText (labelText, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, getColourHighlight());
    addAndMakeVisible (label);
}

void Knob::resized()
{
    auto area = getLocalBounds();
    auto labelArea = area.removeFromTop (10);
    label.setBounds (labelArea);
    slider.setBounds (area);
}

juce::Slider& Knob::getSlider()
{
    return slider;
}