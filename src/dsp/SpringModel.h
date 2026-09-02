#pragma once

#include <chowdsp_wdf/chowdsp_wdf.h>
#include <dsp/Circuit.h>
#include <cmath>
#include <algorithm>

// Low-pass filter for HF damping
class SpringFilter
{
public:
    void prepare(double sr, float cutoff)
    {
        double omega = 2.0 * 3.14f * cutoff / sr;
        g = std::exp(-omega);
    }

    float process(float x)
    {
        float y = g * yPrev + (1.0f - g) * x;
        yPrev = y;
        return y;
    }

    void reset() { yPrev = 0.0f; }

private:
    float g = 0.99f;
    float yPrev = 0.0f;
};

// Single interpolated delay line for one spring
class SpringLine
{
public:
    void prepare(double sr, int maxDelaySamples, float delaySamps, float decay, float hfCutoff)
    {
        sampleRate = sr;
        size = maxDelaySamples;
        buffer.resize(size);
        std::fill(buffer.begin(), buffer.end(), 0.0f);
        delay = delaySamps;
        decayFactor = decay;
        hfFilter.prepare(sr, hfCutoff);
        idx = 0;
    }

    void setDelay(float d) { delay = d; }

    void setDecay(float d) { decayFactor = d; }

    void setHfCutoff(float cutoff) { hfFilter.prepare(sampleRate, cutoff); }

    float getDelay() const { return delay; }

    // Read delayed sample with HF damping
    float read()
    {
        int intDelay = (int)std::floor(delay);
        float frac = delay - intDelay;

        int readIdx = (idx + size - intDelay) % size;
        int readIdxPrev = (readIdx - 1 + size) % size;

        float delayed = buffer[readIdx] * (1.0f - frac) + buffer[readIdxPrev] * frac;

        // Apply HF rolloff with low-pass filter
        float damped = hfFilter.process(delayed);

        return damped * decayFactor;
    }

    // Write sample to buffer
    void write(float x)
    {
        buffer[idx] = x;
        idx = (idx + 1) % size;
    }

    void reset()
    {
        std::fill(buffer.begin(), buffer.end(), 0.0f);
        hfFilter.reset();
        idx = 0;
    }

private:
    std::vector<float> buffer;
    int idx = 0;
    int size = 48000;
    double sampleRate = 48000.0;
    float delay = 1000.0f;
    float decayFactor = 0.9f;
    SpringFilter hfFilter;
};


// Spring reverb tank using conventional waveguide topology
// Models two coupled springs with differential pickup
class SpringTank : public Circuit<float>
{
public:
    SpringTank(float delayMs=65.0f, float decay=0.85f, float feedback=0.7f, float hfCutoff=4000.0f, float gain = 2.5e-3) : 
        Circuit<float>(), 
        baseDelayMs(delayMs), feedback(feedback), hfCutoff(hfCutoff), decayFactor(decay), gain(gain)
    {}

    void prepare(float sr) override
    {
        sampleRate = sr;
        int maxDelaySamples= (int)(sr * maxDelayMs/ 1000.0f);
        // Two springs with detuned delays for chaotic behavior
        // Guitar spring tanks: ~40-80ms round-trip delay
        float baseDelaySamps = baseDelayMs * sr / 1000.0f;

        line1.prepare(sr, maxDelaySamples, baseDelaySamps * 1.00f, decayFactor * 1.03f, hfCutoff * 1.0f);
        line2.prepare(sr, maxDelaySamples, baseDelaySamps * 1.05f, decayFactor * 1.0f , hfCutoff * 0.9f);
    }

    void reset() override
    {
        line1.reset();
        line2.reset();
    }

    void setFeedback (float v) { feedback = v; }

    void setDecay (float v)
    {
        decayFactor = v;
        line1.setDecay (v * 1.03f);
        line2.setDecay (v);
    }

    void setDelayMs (float v)
    {
        baseDelayMs = juce::jlimit (1.0f, maxDelayMs, v);
        line1.setDelay (baseDelayMs * sampleRate / 1000.0f);
        line2.setDelay (baseDelayMs * 1.05f * sampleRate / 1000.0f);
    }

    void setHfCutoff (float v)
    {
        hfCutoff = v;
        line1.setHfCutoff (v);
        line2.setHfCutoff (v * 0.9f);
    }

    void setParam(int param, float val) override
    {
        if (param == 0) feedback = val;
    }

    void setControl(int, float) override {}
    void updateMonitors() override {}

    float processSample(float input) override
    {
        // Read returning waves from both springs
        float w1 = line1.read();
        float w2 = line2.read();

        // Differential pickup - characteristic of spring reverbs
        float output = 0.5f * (w1 - w2);

        // Write excitation + feedback to both springs
        float fbDrive = input * gain + feedback * output;
        line1.write(fbDrive);
        line2.write(fbDrive);

        return output ;
    }

private:
    SpringLine line1;
    SpringLine line2;

    float maxDelayMs = 200.0f;
    float sampleRate = 48000.0f;
    float feedback, hfCutoff, baseDelayMs, decayFactor, gain;

};