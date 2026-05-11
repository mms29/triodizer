#include "PluginEditor.h"

//==============================================================================
TriodeEditor::TriodeEditor(TriodeProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // =====================================================
    // DRIVE
    // =====================================================
    driveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    driveSlider.setRange(0.0, 60.0, 0.1);
    driveSlider.setTextValueSuffix(" dB");
    addAndMakeVisible(driveSlider);

    driveLabel.setText("Drive", juce::dontSendNotification);
    driveLabel.attachToComponent(&driveSlider, false);
    addAndMakeVisible(driveLabel);

    // Attachment (drives parameter sync automatically)
    driveAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.parameters,
            "drive",
            driveSlider);

    // =====================================================
    // GAIN
    // =====================================================
    gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    gainSlider.setRange(-80.0, -20.0, 0.1);
    gainSlider.setTextValueSuffix(" dB");
    addAndMakeVisible(gainSlider);

    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.attachToComponent(&gainSlider, false);
    addAndMakeVisible(gainLabel);

    gainAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.parameters,
            "gain",
            gainSlider);

    // =====================================================
    // OVERSAMPLING
    // =====================================================
    oversampleSelector.addItem("1x", 1);
    oversampleSelector.addItem("2x", 2);
    oversampleSelector.addItem("4x", 3);
    oversampleSelector.addItem("8x", 4);

    addAndMakeVisible(oversampleSelector);

    oversampleLabel.setText("Oversample", juce::dontSendNotification);
    oversampleLabel.attachToComponent(&oversampleSelector, false);
    addAndMakeVisible(oversampleLabel);

    oversampleAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            audioProcessor.parameters,
            "oversample",
            oversampleSelector);

    // =====================================================
    // SIZE
    // =====================================================
    setSize(400, 250);
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

    // Position oversample controls
    auto oversampleRow = area.removeFromTop(40);
    oversampleLabel.setBounds(oversampleRow.removeFromLeft(80).reduced(10));
    oversampleSelector.setBounds(oversampleRow.removeFromLeft(120).reduced(10));
}

