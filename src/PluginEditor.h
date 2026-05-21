#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "SchematicPanel.h"
#include "TriodeGainStage.h"
#include "WaveformDisplay.h"


class CallbackTimer : public juce::Timer
{
public:
    using Callback = std::function<void()>;
    CallbackTimer(Callback cb, int hz): callback(std::move(cb)){
        startTimerHz(hz);
    }

    void timerCallback() override
    {
        if (callback)callback();
    }
private:
    Callback callback;
};

class TriodeEditor : public juce::AudioProcessorEditor,
                     private SchematicPanelListener
{
public:
    TriodeEditor(TriodeProcessor&);
    ~TriodeEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void triodeTimerCallback();
    void waveformTimerCallback();

    // SchematicPanel::Listener
    void schematicParameterChanged (const juce::String& paramName,
                                    float newValue) override;

private:
    TriodeProcessor& audioProcessor;
    int timerCount = 0;

    // Waveform display for input/output comparison
    WaveformDisplay waveformDisplay;
    CallbackTimer waveformTimer;
    CallbackTimer schematicTimer;

    // Schematic panel — the interactive circuit
    std::unique_ptr<SchematicPanel> schematic;

    // Sliders for drive / gain (kept as-is)
    juce::Slider driveSlider;
    juce::Slider gainSlider;
    juce::Label  driveLabel;
    juce::Label  gainLabel;

    juce::ComboBox oversampleSelector;
    juce::Label oversampleLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        driveAttachment, gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
        oversampleAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TriodeEditor)
};

