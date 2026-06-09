#pragma once

#include <chowdsp_wdf/chowdsp_wdf.h>
#include <dsp/Circuit.h>
#include <cmath>
#include <vector>

// One-pole low-pass filter for frequency-dependent losses
class LPF
{
public:
    void setFreq(double freq, double sr)
    {
        auto omega = 2.0 * M_PI * freq / sr;
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

// Delay line with interpolated read and HF damping
// Standard waveguide component
class WaveguideDelay
{
public:
    void prepare(double sr, int maxDelay)
    {
        sampleRate = sr;
        bufferSize = maxDelay;
        buffer.resize(bufferSize);
        idx = 0;
        delay = 1000.0f;
        gain = 0.969f;
    }

    void setDelay(float d) { delay = d; }

    // Returns delayed sample, advances both read and write
    float read()
    {
        // Read behind write by delay samples
        int intDelay = (int)delay;
        float frac = delay - intDelay;

        int readPos = (idx + bufferSize - intDelay) % bufferSize;
        int readPosPrev = (readPos - 1 + bufferSize) % bufferSize;

        float out = buffer[readPos] * (1.0f - frac) + buffer[readPosPrev] * frac;
        return gain * hfFilter.process(out);
    }

    void write(float input)
    {
        buffer[idx] = input;
        idx = (idx + 1) % bufferSize;
    }

    void reset()
    {
        std::fill(buffer.begin(), buffer.end(), 0.0f);
        hfFilter.reset();
    }

private:
    std::vector<float> buffer;
    int idx = 0;          // write index, read is idx - delay
    int bufferSize = 384000;
    float delay = 1000.0f;
    float gain = 0.96f;
    double sampleRate = 44100.0;
    LPF hfFilter;
};

// Simple dual-spring reverb tank using conventional waveguide topology
class SpringTank : public Circuit<float>
{
public:
    SpringTank() : Circuit<float>() {}

    void prepare(float sr) override
    {
        sampleRate = sr;

        // One-way travel time for each spring (50-100ms typical)
        float delayMs = 60.0f;
        float delaySamps = delayMs * sr / 1000.0f;

        int maxDelay = (int)(sr * 0.2);
        delay1.prepare(sr, maxDelay);
        delay2.prepare(sr, maxDelay);

        delay1.setDelay(delaySamps * 0.99f);
        delay2.setDelay(delaySamps * 1.01f); // slight detune for width
    }

    void reset() override
    {
        delay1.reset();
        delay2.reset();
    }

    void setParam(int param, float val) override
    {
        if (param == 0) feedback = val;
    }
    void setControl(int, float) override {}
    void updateMonitors() override {}

    float processSample(float input) override
    {
        // Read delayed signals (waves returning from spring far ends)
        float r1 = delay1.read();
        float r2 = delay2.read();

        // Compute pickup output
        float output = 0.5f * (r1 - r2);

        // Re-inject into springs: input drives both, plus reflected waves
        delay1.write(input - r1 * 0.95f);
        delay2.write(input - r2 * 0.95f);

        return output;
    }

private:
    WaveguideDelay delay1;
    WaveguideDelay delay2;

    double sampleRate = 44100.0;
    float feedback = 0.95f; // decay control
};