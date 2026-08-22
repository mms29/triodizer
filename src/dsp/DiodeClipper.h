
#pragma once

#include <chowdsp_wdf/chowdsp_wdf.h>
#include <dsp/Circuit.h>

using namespace chowdsp::wdft;


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

    enum class Monitoring : int { D1,  Count };
    enum class Param : int {D1, Count };
    enum class Control : int {Gain, Count };

    void setDefaultParam () 
    {
        Vs.setResistanceValue(1e3f);
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
            case (int)Param::D1: break;

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
                gain = ratio * MAX_GAIN;
                break;
            }
            default: jassertfalse; break;
        }
    }

    void prepare(float sr) override {
    }
    void reset() override {
    }

    float processSample(float x) override {
        auto y = x*gain;

        Vs.setVoltage (y);
        dp.incident (Vs.reflected());
        auto out = voltage<float> (dp);
        Vs.incident (dp.reflected());
        return out;

    }
    void updateMonitors() override{ }
    
private: 
    float gain = 1.0f;
    const float MAX_GAIN = 100.0f;

    ResistiveVoltageSourceT<float> Vs;

    // GZ34 diode pair
    DiodePairT<float, decltype (Vs)> dp { Vs, 2.52e-9f };
};

