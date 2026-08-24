
#pragma once

#include <chowdsp_wdf/chowdsp_wdf.h>
#include <dsp/Circuit.h>

using namespace chowdsp::wdft;

struct diodeParam
{
    const char* name;
    float Is;
};

static const diodeParam diodeTable[] =
{
    { "1N4148",  4.0e-9f},
    { "1N914",   4.0e-9f},
    { "1N4007",  1.0e-6f},
    { "1N34A",   1.0e-6f},
    { "1N5819",  1.0e-6f},
    { "GZ34",  2.52e-9f}
};

class DiodeClipperCircuit : public Circuit<float>
{
public:
    DiodeClipperCircuit(): Circuit<float>()
    {
        params.resize((int)Param::Count, 0.0f);
        controls.resize((int)Control::Count, 0.0f);
        monitors.resize((int)Monitoring::Count);

        setDefaultParam();
        setDefaultControl();
    };
    using Circuit<float>::params;
    using Circuit<float>::controls;
    using Circuit<float>::monitors;
    using Circuit<float>::getParam;

    enum class Monitoring : int { R1, D1, C1, Count };
    enum class Param : int {Gain, R1, D1, C1, Count };
    enum class Control : int {Gain, Count };

    void setDefaultParam () 
    {
        setParam((int)Param::Gain, 100.0f);
        setParam((int)Param::R1, 4.7e3f);
        setParam((int)Param::C1, 4.7e-9f);
        setParam((int)Param::D1, 0.0f);

    }
    void setDefaultControl () 
    { 
        setControl((int)Control::Gain, 50.0f);    
    }

    void setParam (const int index, float value) override
    {
        params.at(index) = value;

        switch (index)
        {
            case (int)Param::D1: w_dp.setDiodeParameters( diodeTable[(int)value].Is, 25.85e-3, 1); break;
            case (int)Param::C1: w_C1.setCapacitanceValue(value); break;
            case (int)Param::R1: w_Vs.setResistanceValue(value); break;
            case (int)Param::Gain: setControl((int)Control::Gain, 100.0f); break;

            case (int)Param::Count:
            default:
                jassertfalse;
                break;
        }
    }
    void setControl (const int index, float value) override
    {
        controls.at(index) = value;
        auto ratio = value/100.0f;

        switch (index)
        {
            case (int)Control::Gain: 
            {
                inputGain = ratio * getParam((int)Param::Gain);
                break;
            }
            default: jassertfalse; break;
        }
    }

    void prepare(float sr) override {
        w_C1.prepare(sr);
    }
    void reset() override {
        w_C1.reset();
    }

    float processSample(float x) override {
        auto y = x*inputGain;

        w_Vs.setVoltage (y);
        w_dp.incident (w_P1.reflected());
        auto out = voltage<float> (w_dp);
        w_P1.incident (w_dp.reflected());

        return out * outputGain;

    }
    void updateMonitors() override{ 
        updatePortMonitor((int)Monitoring::D1, w_P1);
        updatePortMonitor((int)Monitoring::C1, w_C1);
        updatePortMonitor((int)Monitoring::R1, w_Vs);
    }
    
private: 
    float inputGain = 1.0f;
    float outputGain = 1e-1f;

    ResistiveVoltageSourceT<float> w_Vs;

    CapacitorT<float> w_C1 { 0.0f };
    WDFParallelT<float, decltype (w_Vs), decltype (w_C1)> w_P1 { w_Vs, w_C1 };

    // GZ34 diode pair
    DiodePairT<float, decltype (w_P1)> w_dp { w_P1, 0.0f };
};

