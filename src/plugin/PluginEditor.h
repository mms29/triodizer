#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "plugin/PluginProcessor.h"

#include "constants/SchematicConstants.h"

#include "schematic/SchematicPanel.h"
#include "schematic/SchematicBuilder.h"
#include "gui/Knob.h"
#include "gui/WaveformDisplay.h"

// ==============================================================================
// CallbackTimer: Timer utility that invokes a callback at specified frequency
class CallbackTimer : public juce::Timer
{
public:
    using Callback = std::function<void()>;

    CallbackTimer (Callback cb, int hz) : callback (std::move (cb))
    {
        startTimerHz (hz);
    }

    void timerCallback() override
    {
        if (callback)
            callback();
    }

private:
    Callback callback;
};

// ==============================================================================
// TubeLabEditor: Main editor component for the TubeLab plugin
// Manages the schematic panel, controls, and waveform display
class TubeLabEditor : public juce::AudioProcessorEditor,
                     public SchematicPanelListener,
                     public juce::ChangeListener
{
public:
    TubeLabEditor (TubeLabProcessor&);
    ~TubeLabEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void updateSchematic();
    void changeListenerCallback (juce::ChangeBroadcaster*) override;

    void circuitTimerCallback();
    void waveformTimerCallback();

    // SchematicPanel::Listener interface
    void setCircuitParam (const int index, float newValue) override;
    void setCircuitControl (const int index, float newValue) override;
    float getCircuitMonitoring (const int index) override;
    float getCircuitParam (const int index) override;
    float getCircuitControl (const int index) override;

private:
    TubeLabProcessor& audioProcessor;
    int timerCount = 0;

    // Waveform display for input/output comparison
    WaveformDisplay waveformDisplay;
    CallbackTimer waveformTimer;
    CallbackTimer schematicTimer;

    // Schematic panel — the interactive circuit
    std::unique_ptr<SchematicPanel> schematic;
    SchematicBuilder schematicBuilder;

    // Sliders for drive / gain
    std::unique_ptr<Knob> driveKnob;
    std::unique_ptr<Knob> gainKnob;

    // Oversampling control
    juce::ComboBox oversampleSelector;
    juce::Label oversampleLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> oversampleAttachment;

    // Preset selection
    juce::ComboBox presetSelector;
    juce::Label presetLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> presetAttachment;

    // Zoom controls
    juce::TextButton resetViewButton { "Reset View" };

    // Mono/stereo mode toggle
    juce::ToggleButton monoStereoButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> monoStereoAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TubeLabEditor)
};