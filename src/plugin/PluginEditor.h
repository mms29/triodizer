#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "plugin/PluginProcessor.h"

#include "schematic/SchematicPanel.h"
#include "gui/Knob.h"
#include "gui/WaveformDisplay.h"

#include "dsp/TriodeGainStage.h"



inline constexpr float  DRIVE_MIN = -20.0f;
inline constexpr float  DRIVE_MAX = 40.0f;
inline constexpr float  DRIVE_DEFAULT = 0.0f;
inline constexpr float  DRIVE_STEP = 0.1f;

inline constexpr float  GAIN_MIN = -60.0f;
inline constexpr float  GAIN_MAX = 0.0f;
inline constexpr float  GAIN_DEFAULT = 0.0f;
inline constexpr float  GAIN_STEP = 0.1f;

inline constexpr int WINDOW_WIDTH = 1200;
inline constexpr int WINDOW_HEIGHT = 920;



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
                     public SchematicPanelListener,
                     public juce::ChangeListener
{
public:
    TriodeEditor(TriodeProcessor&);
    ~TriodeEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    
    void updateSchematic();
    void changeListenerCallback(juce::ChangeBroadcaster*) override;

    void triodeTimerCallback();
    void waveformTimerCallback();

    // SchematicPanel::Listener
    void setCircuitParam (const int index, float newValue) override{
        audioProcessor.setCircuitParam (index, newValue);}
    void setCircuitControl (const int index, float newValue) override{
        audioProcessor.setCircuitControl (index, newValue);}
    float getCircuitMonitoring(const int index) override{
         return audioProcessor.getCircuitMonitoring (index);}
    float getCircuitParam(const int index) override{
         return audioProcessor.getCircuitParam (index);}
    float getCircuitControl(const int index) override{
         return audioProcessor.getCircuitControl (index);}


private:
    TriodeProcessor& audioProcessor;
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

    juce::ComboBox oversampleSelector;
    juce::Label oversampleLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
        oversampleAttachment;

    juce::ComboBox presetSelector;
    juce::Label presetLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
        presetAttachment;

    // Zoom controls
    juce::TextButton resetViewButton {"Reset View"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TriodeEditor)
};

