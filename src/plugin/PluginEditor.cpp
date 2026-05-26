#include "plugin/PluginEditor.h"

//==============================================================================
TriodeEditor::TriodeEditor(TriodeProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),     
    waveformTimer([this] { waveformTimerCallback(); }, 20),
    schematicTimer([this] { triodeTimerCallback(); }, 5),
    waveformDisplay(p)
{
    // =====================================================
    // SCHEMATIC PANEL
    // =====================================================
    schematic = std::make_unique<SchematicPanel>();
    buildCommonCathodeStage(*schematic);
    addAndMakeVisible (*schematic);
    schematic->setListener(this);

    // =====================================================
    // WAVEFORM DISPLAY
    // =====================================================
    waveformDisplay.setBackgroundColour (juce::Colours::black);
    waveformDisplay.setWaveformColour (juce::Colours::cyan);
    waveformDisplay.setAmplitudeScale (100.0f);
    addAndMakeVisible (waveformDisplay);
    
    // =====================================================
    // DRIVE
    // =====================================================
    driveKnob = std::make_unique<Knob>(
            audioProcessor.parameters,
            "drive",
            "Drive",
            DRIVE_MIN,
            DRIVE_MAX,
            DRIVE_STEP,
            " dB");
    addAndMakeVisible(*driveKnob);
    // =====================================================
    // GAIN
    // =====================================================
    gainKnob = std::make_unique<Knob>(
            audioProcessor.parameters,
            "gain",
            "Gain",
            GAIN_MIN,
            GAIN_MAX,
            GAIN_STEP,
            " dB");
    addAndMakeVisible(*gainKnob);
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
    schematic->setMonitorVoltage ("Vk", audioProcessor.getCathodeVoltage(ch));
}

void TriodeEditor::waveformTimerCallback()
{
    waveformDisplay.repaint();
}

//==============================================================================
void TriodeEditor::schematicParameterChanged (const juce::String& paramName, float newValue)
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
    auto bottom = area.removeFromBottom(120);


    oversampleLabel.setBounds(bottom.removeFromRight(80).reduced(0, 30));
    oversampleSelector.setBounds(bottom.removeFromRight(100).reduced(0, 30));

    driveKnob->setBounds(bottom.removeFromLeft(120).reduced(0, 10));
    bottom.removeFromLeft(20);
    gainKnob->setBounds(bottom.removeFromLeft(120).reduced(0, 10));
}