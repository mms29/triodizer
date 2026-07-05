#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "gui/Knob.h"

#include "constants/SchematicConstants.h"
#include "dsp/Circuit.h"
#include "dsp/TriodeQuadricWDF.h"
// #include "dsp/BassmanPreamp.h"
#include "dsp/FullBassmanPreamp.h"
#include "dsp/DualRectifierPreamp.h"
// #include "dsp/LCLadder.h"
// #include "dsp/SpringModel.h"
#include "dsp/TwinReverb.h"

// ==============================================================================
// WaveformBuffer: Ring buffer for waveform display visualization
// size must be a power of 2 for efficient wrapping with bitwise AND
class WaveformBuffer
{
public:
    void push (float sample) noexcept
    {
        buffer[writeIndex] = sample;
        writeIndex = (writeIndex + 1) & (size - 1);
    }

    void getLastBlock (float* dest, int numSamples) const noexcept
    {
        auto idx = writeIndex.load (std::memory_order_relaxed);
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

// ==============================================================================
// TubeLabProcessor: Main audio processor class for the TubeLab plugin
// Handles audio processing, parameter management, and circuit state
class TubeLabProcessor : public juce::AudioProcessor,
                        public juce::ChangeBroadcaster
{
public:
    TubeLabProcessor();
    ~TubeLabProcessor() override;

    const juce::String getName() const override { return "TubeLab"; }

    // Audio processor overrides
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    // Preset management
    void updatePreset();
    int getCurrentPreset() const { return currentPreset; }

    // Oversampling management
    void updateOversampler();
    void buildOversampler();
    double getBaseSampleRate() {return sampleRate;}

    // State information
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    void loadCircuitState (const juce::ValueTree& t);
    juce::ValueTree saveCircuitState() const;

    // Editor creation
    bool hasEditor() const override { return true; }
    juce::AudioProcessorEditor* createEditor() override;
    bool supportsDoublePrecisionProcessing() const override { return false; }

    // Circuit interface
    void prepareCircuit (double sr);
    void resetCircuit();
    void buildCircuit();
    bool circuitReady() const;

    const MonitorValuef& getCircuitMonitoring (const int index, const int ch = 0) const;
    float getCircuitParam (const int index, const int ch = 0) const;
    float getCircuitControl (const int index, const int ch = 0) const;
    void setCircuitParam (const int index, float value);
    void setCircuitControl (const int index, float value);
    void updateCircuitMonitoring ( const int ch=0);

    // Waveform display access
    const WaveformBuffer& getWaveformInputBuffer() const noexcept { return waveformInputBuffer; }
    const WaveformBuffer& getWaveformOutputBuffer() const noexcept { return waveformOutputBuffer; }

    // Parameter state (public for editor access)
    juce::AudioProcessorValueTreeState parameters;

private:
#ifdef XSIMD_HPP
    std::unique_ptr<Circuit<xsimd::batch<float>>> circuit;  // One circuit with SIMD channels
#else
    std::unique_ptr<Circuit<float>> circuit[2];  // One per channel
#endif

    double sampleRate = 48000.0;
    double oversampleRate = 48000.0;
    int blockSize = 512;
    int oversamplingStages = -1;

    // Waveform ring buffers for continuous display
    WaveformBuffer waveformInputBuffer;
    WaveformBuffer waveformOutputBuffer;

    // Oversampling
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;

    // Preset selection
    int currentPreset = PRESET_DEFAULT;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TubeLabProcessor)
};