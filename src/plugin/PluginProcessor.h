#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include "dsp/Circuit.h"
#include "dsp/BassmanPreamp.h"
#include "dsp/FullBassmanPreamp.h"


const int PRESET_DEFAULT = 1;
const int PRESET_COMMONCATHODE = 2;
const int PRESET_BASSMAN_TS = 3;
const int PRESET_BASSMAN_PREAMP_SMALL = 4;
const int PRESET_BASSMAN_PREAMP = 5;

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

class TriodeProcessor : public juce::AudioProcessor,
                        public juce::ChangeBroadcaster
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

    //PRESET
    void updatePreset();
    int getCurrentPreset() const {return currentPreset;}

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


    inline float getCircuitMonitoring  (const int index, const int ch = 0) const  { 
        return circuit[ch]->getMonitoring(index);   
    }
    inline float getCircuitParam  (const int index, const int ch = 0) const  { 
        return circuit[ch]->getParam(index);   
    }
    inline float getCircuitControl  (const int index, const int ch = 0) const  { 
        return circuit[ch]->getControl(index);   
    }
    inline void setCircuitParam(const int index, float value){
        for (int ch = 0; ch < 2; ++ch) {
            circuit[ch]->setParam(index, value);
        }
    }
    inline void setCircuitControl(const int index, float value){
        for (int ch = 0; ch < 2; ++ch) {
            circuit[ch]->setControl(index, value);

        }
    }

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
    std::unique_ptr<Circuit> circuit[2];  // one per channel
    double sampleRate = 48000.0;
    double oversampleRate = 48000.0;
    int blockSize = 512;
    int oversamplingStages = -1;

    // Waveform ring buffer for continuous display
    WaveformBuffer waveformInputBuffer;
    WaveformBuffer waveformOutputBuffer;

    // Oversampling
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;

    // Preset
    int currentPreset = PRESET_DEFAULT;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TriodeProcessor)
};


