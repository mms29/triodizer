#pragma once

#include <chowdsp_wdf/chowdsp_wdf.h>
#include <dsp/Circuit.h>

class SpringLine
{
public:
    void prepare(double sr, int maxDelay)
    {
        fs = sr;
        delayBuffer.resize(maxDelay);
        idx = 0;
    }

    void setParams(float delaySamps, float loss)
    {
        D = delaySamps;
        g = loss;
    }

    float process(float x)
    {
        float y = delayBuffer[(idx + size - (int)D) % size];

        // write input with loss
        delayBuffer[idx] = x + y * g;

        idx = (idx + 1) % size;

        return y;
    }

private:
    std::vector<float> delayBuffer;
    int idx = 0;
    int size = 48000;
    float D = 1000;
    float g = 0.95f;
    double fs = 44100.0;
};

struct SpringJunction
{
    float process(float a1, float a2)
    {
        float s = 0.5f * (a1 + a2);

        y1 = s - a1;  // reflected wave back to line 1
        y2 = s - a2;  // reflected wave back to line 2

        return s;
    }

    float y1, y2;
};


class SpringTank : public Circuit<float>
{
public:
    SpringTank() : Circuit<float>()
    {
        line1.setParams(1000.0f, 0.96f);
        line2.setParams(1203.0f, 0.94f); // slight detune = spring chaos
    }
    void prepare(float sr) override
    {
        line1.prepare(sr, 48000);
        line2.prepare(sr, 48000);
    }
    void reset() override {}
    void setParam(int, float) override {}
    void setControl(int, float) override {}
    void updateMonitors() override {}


    float processSample(float input) override
    {
        // excite both lines
        float a1 = input + feedback * fb1;
        float a2 = input + feedback * fb2;

        // wave propagation
        float w1 = line1.process(a1);
        float w2 = line2.process(a2);

        // scattering junction
        float s = junction.process(w1, w2);

        // feedback
        fb1 = w1;
        fb2 = w2;

        return s;
    }

private:
    SpringLine line1;
    SpringLine line2;

    SpringJunction junction;

    float fb1 = 0.0f;
    float fb2 = 0.0f;
    float feedback = 0.9f;
};