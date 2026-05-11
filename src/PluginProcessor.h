#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "TriodeGainStage.h"
#include <juce_dsp/juce_dsp.h>

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
    void updateOversampler();

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

    void updateWDFcircuit(juce::String paramName, float value);

    juce::AudioProcessorValueTreeState parameters;

private:
    TriodeGainStage triode[2];  // one per channel
    double sampleRate = 48000.0;
    int blockSize = 512;
    int oversamplingStages = -1;

    // Oversampling
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TriodeProcessor)
};
