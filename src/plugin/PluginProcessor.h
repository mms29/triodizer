#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include "dsp/TriodeGainStage.h"

class WaveformBuffer
{
public:
    void push(float sample)
    {
        buffer[writeIndex] = sample;
        writeIndex = (writeIndex + 1) & (size - 1);
    }

    void getLastBlock(float* dest, int numSamples) const
    {
        auto idx = writeIndex.load(std::memory_order_relaxed);
        for (int i = 0; i < numSamples; ++i)
        {
            idx = (idx + size - 1) & (size - 1);
            dest[numSamples - 1 - i] = buffer[idx];
        }
    }

private:
    static constexpr int size = 8192; // MUST be power of 2
    float buffer[size] {};
    std::atomic<int> writeIndex { 0 };
};

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

    // DSP monitor accessors (called from the message thread by the editor timer)
    float getPlateVoltage  (int ch = 0) const  { return triode[ch].getPlateVoltage();   }
    float getCathodeVoltage(int ch = 0) const  { return triode[ch].getCathodeVoltage(); }
    // float getGain(int ch = 0) const  { return triode[ch].getGain(); }

    juce::AudioProcessorValueTreeState parameters;

    const WaveformBuffer& getWaveformInputBuffer() const noexcept
    {
        return waveformInputBuffer;
    }
    const WaveformBuffer& getWaveformOutputBuffer() const noexcept
    {
        return waveformOutputBuffer;
    }

private:
    TriodeGainStage triode[2];  // one per channel
    double sampleRate = 48000.0;
    int blockSize = 512;
    int oversamplingStages = -1;

    // Waveform ring buffer for continuous display
    WaveformBuffer waveformInputBuffer;
    WaveformBuffer waveformOutputBuffer;

    // Oversampling
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TriodeProcessor)
};


