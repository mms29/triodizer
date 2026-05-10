#include "PluginEditor.h"

//==============================================================================
TriodeEditor::TriodeEditor(TriodeProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Drive slider
    driveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    driveSlider.setRange(0.1, 20.0, 0.1);
    driveSlider.setValue(p.parameters.getRawParameterValue("drive")->load());
    driveSlider.addListener(this);
    addAndMakeVisible(driveSlider);

    driveLabel.setText("Drive", juce::dontSendNotification);
    driveLabel.attachToComponent(&driveSlider, false);
    addAndMakeVisible(driveLabel);

    // Gain slider
    gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    gainSlider.setRange(0.0, 2.0, 0.01);
    gainSlider.setValue(p.parameters.getRawParameterValue("gain")->load());
    gainSlider.addListener(this);
    addAndMakeVisible(gainSlider);

    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.attachToComponent(&gainSlider, false);
    addAndMakeVisible(gainLabel);

    setSize(400, 200);
}

TriodeEditor::~TriodeEditor() = default;

//==============================================================================
void TriodeEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawFittedText("Triode Simulator", 0, 20, getWidth(), 30,
                      juce::Justification::centred, 1);
}

void TriodeEditor::resized()
{
    auto area = getLocalBounds().reduced(20);
    area.removeFromTop(50); // space for title

    auto row = area.removeFromTop(120);
    driveSlider.setBounds(row.removeFromLeft(120).reduced(10));
    gainSlider.setBounds(row.removeFromLeft(120).reduced(10));
}

//==============================================================================
void TriodeEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &driveSlider)
    {
        audioProcessor.parameters.getParameter("drive")->setValueNotifyingHost(
            audioProcessor.parameters.getParameter("drive")->convertTo0to1(
                (float)driveSlider.getValue()));
    }
    else if (slider == &gainSlider)
    {
        audioProcessor.parameters.getParameter("gain")->setValueNotifyingHost(
            audioProcessor.parameters.getParameter("gain")->convertTo0to1(
                (float)gainSlider.getValue()));
    }
}
