#include "plugin/PluginEditor.h"

//==============================================================================

TubeLabEditor::TubeLabEditor (TubeLabProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      waveformTimer ([this] { waveformTimerCallback(); }, 20),
      schematicTimer ([this] { circuitTimerCallback(); }, 20),
      waveformDisplay (p)
{
    audioProcessor.addChangeListener (this);

    //==========================================================================
    // SCHEMATIC PANEL
    //==========================================================================

    schematic = std::make_unique<SchematicPanel> (this);
    updateSchematic();
    addAndMakeVisible (*schematic);

    //==========================================================================
    // WAVEFORM DISPLAY
    //==========================================================================
    waveformDisplay.setBackgroundColour (juce::Colours::black);
    waveformDisplay.setWaveformColour (juce::Colours::cyan);
    addAndMakeVisible (waveformDisplay);

    //==========================================================================
    // DRIVE
    //==========================================================================
    driveKnob = std::make_unique<Knob> (
        audioProcessor.parameters,
        "drive",
        "Drive",
        DRIVE_MIN,
        DRIVE_MAX,
        DRIVE_STEP,
        " dB");
    addAndMakeVisible (*driveKnob);

    //==========================================================================
    // GAIN
    //==========================================================================
    gainKnob = std::make_unique<Knob> (
        audioProcessor.parameters,
        "gain",
        "Gain",
        GAIN_MIN,
        GAIN_MAX,
        GAIN_STEP,
        " dB");
    addAndMakeVisible (*gainKnob);

    //==========================================================================
    // OVERSAMPLING
    //==========================================================================
    oversampleSelector.addItem ("1x", 1);
    oversampleSelector.addItem ("2x", 2);
    oversampleSelector.addItem ("4x", 3);
    oversampleSelector.addItem ("8x", 4);
    addAndMakeVisible (oversampleSelector);

    oversampleLabel.setText ("Oversample", juce::dontSendNotification);
    oversampleLabel.attachToComponent (&oversampleSelector, false);
    addAndMakeVisible (oversampleLabel);

    oversampleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        audioProcessor.parameters, "oversample", oversampleSelector);

    //==========================================================================
    // MONO STEREO
    //==========================================================================
    addAndMakeVisible (monoStereoButton);

    monoStereoAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        audioProcessor.parameters,
        "monoStereo",
        monoStereoButton);

    monoStereoButton.setClickingTogglesState (true);
    monoStereoButton.onStateChange = [this]()
    {
        if (monoStereoButton.getToggleState())
            monoStereoButton.setButtonText ("Mono");
        else
            monoStereoButton.setButtonText ("Stereo");
    };

    //==========================================================================
    // RESET VIEW
    //==========================================================================
    resetViewButton.onClick = [this] { schematic->resetView(); };
    addAndMakeVisible (resetViewButton);

    //==========================================================================
    // PRESET
    //==========================================================================
    presetSelector.addItem ("Default", PRESET_DEFAULT);
    presetSelector.addItem ("Common Cathode Stage", PRESET_COMMONCATHODE);
    presetSelector.addItem ("Fender Bassman Tone Stack", PRESET_BASSMAN_TS);
    presetSelector.addItem ("Fender Bassman Preamp Small", PRESET_BASSMAN_PREAMP_SMALL);
    presetSelector.addItem ("Fender Bassman Preamp", PRESET_BASSMAN_PREAMP);
    presetSelector.addItem ("Mesa/Boogie Dual Rectifier", PRESET_DUAL_RECTIFIER_PREAMP);
    presetSelector.addItem ("LC Ladder", PRESET_LCLADDER);
    presetSelector.addItem ("Twin reverb", PRESET_TWIN_REVERB);
    addAndMakeVisible (presetSelector);

    presetLabel.setText ("Preset", juce::dontSendNotification);
    presetLabel.attachToComponent (&presetSelector, false);
    addAndMakeVisible (presetLabel);

    presetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        audioProcessor.parameters, "preset", presetSelector);

    //==========================================================================
    // SIZE
    //==========================================================================
    setSize (WINDOW_WIDTH, WINDOW_HEIGHT);
    schematic->resetView();
}

TubeLabEditor::~TubeLabEditor()
{
    audioProcessor.removeChangeListener (this);
}

//==============================================================================

void TubeLabEditor::changeListenerCallback (juce::ChangeBroadcaster*)
{
    updateSchematic();
}

void TubeLabEditor::updateSchematic()
{
    schematic->clear();

    switch (audioProcessor.getCurrentPreset())
    {
    case PRESET_COMMONCATHODE:
        // buildCommonCathodeStage(*schematic);
        break;

    case PRESET_BASSMAN_TS:
        schematicBuilder.buildBassmanToneStack (*schematic);
        break;

    case PRESET_BASSMAN_PREAMP_SMALL:
        schematicBuilder.buildBassmanPreampSmall (*schematic);
        break;

    case PRESET_BASSMAN_PREAMP:
        schematicBuilder.buildBassmanPreamp (*schematic);
        break;

    case PRESET_DUAL_RECTIFIER_PREAMP:
        schematicBuilder.buildDualRectifierPreamp (*schematic);
        break;

    case PRESET_TWIN_REVERB:
        schematicBuilder.buildTwinReverb (*schematic);
        break;

    case PRESET_LCLADDER:
    default:
        schematicBuilder.buildDefault (*schematic);
        break;
    }

    schematic->syncSchematicToCircuit();
    schematic->resized();
    schematic->resetView();
}

//==============================================================================

void TubeLabEditor::circuitTimerCallback()
{
    // TODO: Implement monitoring updates
    schematic->updateMonitoring();
}

void TubeLabEditor::waveformTimerCallback()
{
    waveformDisplay.repaint();
}

//==============================================================================

void TubeLabEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);
}

void TubeLabEditor::resized()
{
    auto area = getLocalBounds().reduced (4);

    // Waveform display at top
    waveformDisplay.setBounds (area.removeFromTop (120));

    // Schematic takes remaining space
    schematic->setBounds (area.removeFromTop (area.getHeight() - 100));

    // Bottom strip for drive / gain / oversample controls
    auto bottom = area.removeFromBottom (120);

    // Oversample
    oversampleLabel.setBounds (bottom.removeFromRight (80).reduced (0, 30));
    oversampleSelector.setBounds (bottom.removeFromRight (100).reduced (0, 30));

    // Preset
    presetLabel.setBounds (bottom.removeFromRight (80).reduced (0, 30));
    presetSelector.setBounds (bottom.removeFromRight (100).reduced (0, 30));

    // Reset view button
    resetViewButton.setBounds (bottom.removeFromRight (80).reduced (0, 30));

    // Mono stereo button
    monoStereoButton.setBounds (bottom.removeFromRight (80).reduced (0, 30));

    // Drive / gain knobs
    driveKnob->setBounds (bottom.removeFromLeft (120).reduced (0, 10));
    bottom.removeFromLeft (20);
    gainKnob->setBounds (bottom.removeFromLeft (120).reduced (0, 10));
}

//==============================================================================

void TubeLabEditor::setCircuitParam (const int index, float newValue)
{
    audioProcessor.setCircuitParam (index, newValue);
}

void TubeLabEditor::setCircuitControl (const int index, float newValue)
{
    audioProcessor.setCircuitControl (index, newValue);
}

float TubeLabEditor::getCircuitMonitoring (const int index)
{
    return audioProcessor.getCircuitMonitoring (index);
}

float TubeLabEditor::getCircuitParam (const int index)
{
    return audioProcessor.getCircuitParam (index);
}

float TubeLabEditor::getCircuitControl (const int index)
{
    return audioProcessor.getCircuitControl (index);
}