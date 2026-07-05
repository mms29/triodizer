#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "plugin/PluginProcessor.h"

#include "constants/SchematicConstants.h"

#include "schematic/SchematicPanel.h"
#include "schematic/SchematicBuilder.h"
#include "gui/Knob.h"
#include "gui/Button.h"
#include "gui/Selector.h"
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
    void inspectTimerCallback();
    void waveformTimerCallback();

    // SchematicPanel::Listener interface
    void setCircuitParam (const int index, float newValue) override;
    void setCircuitControl (const int index, float newValue) override;
    const MonitorValuef& getCircuitMonitoring (const int index) override;
    void updateCircuitMonitoring () override;
    float getCircuitParam (const int index) override;
    float getCircuitControl (const int index) override;

    void updateOversampleLabels (double sampleRate)
    {
        oversampleSelector.changeItemText (1, juce::String(sampleRate/1e3, 1) + "kHz");
        oversampleSelector.changeItemText (2, juce::String(2*sampleRate/1e3, 1) + "kHz");
        oversampleSelector.changeItemText (3, juce::String(4*sampleRate/1e3, 1) + "kHz");
        oversampleSelector.changeItemText (4, juce::String(8*sampleRate/1e3, 1) + "kHz");
    }

private:
    TubeLabProcessor& audioProcessor;
    int timerCount = 0;


    // Top panel
    juce::Rectangle<int> topRect, botRect, titleRect, subtitleRect;
    GlowComboBoxLookAndFeel glowComboBoxLF;

    // Waveform display for input/output comparison
    std::unique_ptr<PathToggleButton> scopeButton;
    WaveformDisplay waveformDisplay;
    CallbackTimer waveformTimer;
    bool showScope = false;

    // Schematic panel — the interactive circuit
    std::unique_ptr<SchematicPanel> schematic;
    SchematicBuilder schematicBuilder;
    CallbackTimer schematicTimer;

    // // Sliders for drive / gain
    // std::unique_ptr<Knob> driveKnob;
    // std::unique_ptr<Knob> gainKnob;

    // Oversampling control
    juce::ComboBox oversampleSelector;
    juce::Label oversampleLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> oversampleAttachment;

    // Preset selection
    juce::ComboBox presetSelector;
    // juce::Label presetLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> presetAttachment;

    // Zoom controls
    juce::TextButton resetViewButton { "Reset View" };

    // Signal path
    std::unique_ptr<PathToggleButton> signalButton;

    // Power path
    std::unique_ptr<PathToggleButton> powerButton;

    // Inspect button
    std::unique_ptr<PathToggleButton> inspectButton;
    CallbackTimer inspectTimer;

    // Mono/stereo mode toggle
    juce::ToggleButton monoStereoButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> monoStereoAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TubeLabEditor)
};