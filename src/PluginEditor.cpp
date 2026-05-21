#include "PluginEditor.h"

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;



//==============================================================================
TriodeEditor::TriodeEditor(TriodeProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),     
    waveformTimer([this] { waveformTimerCallback(); }, 20),
    schematicTimer([this] { triodeTimerCallback(); }, 1),
    waveformDisplay(p)
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

    driveAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.parameters, "drive", driveSlider);

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
            audioProcessor.parameters, "gain", gainSlider);

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
            audioProcessor.parameters, "oversample", oversampleSelector);

    // =====================================================
    // SCHEMATIC PANEL
    // =====================================================
    schematic = std::make_unique<SchematicPanel>();
    buildCommonCathodeStage(*schematic);
    schematic->setVisible (true);
    addAndMakeVisible (schematic.get());
    schematic->setListener (this);

    // =====================================================
    // WAVEFORM DISPLAY
    // =====================================================
    waveformDisplay.setBackgroundColour (juce::Colours::black);
    waveformDisplay.setWaveformColour (juce::Colours::cyan);
    waveformDisplay.setAmplitudeScale (1000.0f);
    addAndMakeVisible (waveformDisplay);
    

    // =====================================================
    // SIZE
    // =====================================================
    setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
}

TriodeEditor::~TriodeEditor() = default;

//==============================================================================

void TriodeEditor::triodeTimerCallback()
{
    // Read DSP state (audio-thread writes, message-thread reads — safe for floats)
    constexpr int ch = 0;
    schematic->setMonitorVoltage ("Vp", audioProcessor.getPlateVoltage  (ch));
    schematic->setMonitorVoltage ("Vg", audioProcessor.getGridVoltage   (ch));
    schematic->setMonitorVoltage ("Vk", audioProcessor.getCathodeVoltage(ch));
}

void TriodeEditor::waveformTimerCallback()
{
    waveformDisplay.repaint();
}

//==============================================================================
void TriodeEditor::schematicParameterChanged (const juce::String& paramName,
                                               float newValue)
{
    audioProcessor.updateWDFcircuit (paramName, newValue);
}

void TriodeEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void TriodeEditor::resized()
{
    auto area = getLocalBounds().reduced(4);

    // Waveform display at top
    waveformDisplay.setBounds(area.removeFromTop(120));

    // Schematic takes remaining space
    schematic->setBounds(area.removeFromTop(area.getHeight() - 100));

    // Bottom strip for drive / gain / oversample controls
    auto bottom = area.removeFromBottom(80);
    bottom.removeFromLeft(20);

    driveSlider.setBounds(bottom.removeFromLeft(120).reduced(10));
    bottom.removeFromLeft(20);
    gainSlider.setBounds(bottom.removeFromLeft(120).reduced(10));
    bottom.removeFromLeft(40);

    oversampleLabel.setBounds(bottom.removeFromLeft(80).reduced(5, 10));
    oversampleSelector.setBounds(bottom.removeFromLeft(100).reduced(5, 10));
}