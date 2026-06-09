#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

// #include <xsimd/xsimd.hpp>
#include <chowdsp_wdf/chowdsp_wdf.h>
#include "dsp/Circuit.h"
#include "dsp/BassmanPreamp.h"
#include "dsp/FullBassmanPreamp.h"
#include "dsp/DualRectifierPreamp.h"
#include "dsp/LCLadder.h"


const int PRESET_DEFAULT = 1;
const int PRESET_COMMONCATHODE = 2;
const int PRESET_BASSMAN_TS = 3;
const int PRESET_BASSMAN_PREAMP_SMALL = 4;
const int PRESET_BASSMAN_PREAMP = 5;
const int PRESET_DUAL_RECTIFIER_PREAMP = 6;
const int PRESET_LCLADDER = 7;

class OnePoleLPF
{
public:
    void prepare(float sampleRate, float cutoffHz, float noiseGain)
    {
        float x = std::exp(-2.0f * 3.14f * cutoffHz / sampleRate);
        a = x;
        b = 1.0f - x;
        gain = std::sqrt(sampleRate / 44100.0f)*noiseGain;
    }
    float process(float in)
    {
        z = in * b + z * a;
        return z*gain;
    }
private:
    float a = 0.0f, b = 1.0f, z = 0.0f;
    float gain;
};
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

class TubeLabProcessor : public juce::AudioProcessor,
                        public juce::ChangeBroadcaster
{
public:
    TubeLabProcessor();
    ~TubeLabProcessor() override;

    const juce::String getName() const override { return "TubeLab"; }

    // Overrides
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
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    // Preset
    void updatePreset();
    int getCurrentPreset() const {return currentPreset;}

    // Oversampler
    void updateOversampler();
    void buildOversampler();

    // State information
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    void loadCircuitState (const juce::ValueTree& t);
    juce::ValueTree saveCircuitState() const;

    // Editor
    bool hasEditor() const override { return true; }
    juce::AudioProcessorEditor* createEditor() override;
    bool supportsDoublePrecisionProcessing() const override { return false; }

    // Circuit
    void prepareCircuit(double sr);
    void resetCircuit();
    void buildCircuit();
    bool circuitReady() const;
    float getCircuitMonitoring  (const int index, const int ch = 0) const;
    float getCircuitParam  (const int index, const int ch = 0) const;
    float getCircuitControl  (const int index, const int ch = 0) const;
    void setCircuitParam(const int index, float value);
    void setCircuitControl(const int index, float value);

    // Wave display
    const WaveformBuffer& getWaveformInputBuffer() const noexcept {return waveformInputBuffer;}
    const WaveformBuffer& getWaveformOutputBuffer() const noexcept {return waveformOutputBuffer;}

    //noise
    inline float whiteNoise() {return rng.nextFloat() * 2.0f - 1.0f;}

    // parameters
    juce::AudioProcessorValueTreeState parameters;

private:
    #ifdef XSIMD_HPP
    std::unique_ptr<Circuit<xsimd::batch<float>>> circuit;  // one circuit with SIMD channels
    #else
    std::unique_ptr<Circuit<float>> circuit[2];  // one per channel
    #endif
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

    //noise 
    OnePoleLPF noiseLP;
    juce::Random rng;
    float noiseGain = 1e-5;
    float noiseCutoff = 1000.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TubeLabProcessor)

};





