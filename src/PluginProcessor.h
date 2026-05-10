#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "TriodeGainStage.h"

class TriodeProcessor : public juce::AudioProcessor
{
public:
    TriodeProcessor();
    ~TriodeProcessor() override;

    const juce::String getName() const override { return "Triode"; }

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    bool hasEditor() const override { return true; }
    juce::AudioProcessorEditor* createEditor() override;
    bool supportsDoublePrecisionProcessing() const override { return false; }

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    juce::AudioProcessorValueTreeState parameters;

private:
    TriodeGainStage triode[2];  // one per channel
    double sampleRate = 48000.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TriodeProcessor)
};
