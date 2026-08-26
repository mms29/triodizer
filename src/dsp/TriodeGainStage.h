#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <chowdsp_wdf/chowdsp_wdf.h>
#include <dsp/Circuit.h>
#include <dsp/TriodeQuadricWDF.h>
#include <dsp/PowerSupply.h>

inline const float POWER_SUPPLY_RSAG = 200.0f;
inline const float POWER_SUPPLY_CSAG = 47e-6f;

using namespace chowdsp::wdft;

template <typename T>
class TriodeGainStageCircuitT : public Circuit<T>
{
public:
    TriodeGainStageCircuitT(): Circuit<T>()
    {
        params.resize((int)Param::Count, T(0.0f));
        controls.resize((int)Control::Count, T(0.0f));
        monitors.resize((int)Monitoring::Count);

        setDefaultParam();
        setDefaultControl();

    };
    using Circuit<T>::params;
    using Circuit<T>::controls;
    using Circuit<T>::monitors;
    using Circuit<T>::getParam;
    using Circuit<T>::updateTubeMonitor;
    using Circuit<T>::updatePortMonitor;

    enum class Monitoring : int 
    {
        V1, Rg1, Ci1, Ri1, Rk1, Ck1, Rp1,Cp1,RVol_plus, RVol_minus, E1,
        
        Count 
    };
    enum class Param : int 
    {
        // V1
        Gain, V1, Ri1 ,Rg1,Ci1,Rk1,Ck1,E1,Rp1,Cp1, RVol,

        Count 
    };
    enum class Control : int 
    {
        Gain, Volume, Count 
    };
    void setDefaultParam () 
    {
        
        setParam((int)Param::Ri1,  T(1.0e6f  ));
        setParam((int)Param::Rg1,  T(20.0e3f ));
        setParam((int)Param::Ci1,  T(100e-9f ));
        setParam((int)Param::Rk1,  T(1.0e3f  ));
        setParam((int)Param::Ck1,  T(250e-6f ));
        setParam((int)Param::E1,   T(325.0f  ));
        setParam((int)Param::Cp1,  T(22e-9f  ));
        setParam((int)Param::Rp1,  T(100.0e3f));
        setParam((int)Param::RVol, T(1.0e6f  ));

        setParam((int)Param::V1, T(0.0f )); 

        setParam((int)Param::Gain, T(20.0f )); 
    }
    void setDefaultControl () 
    { 
        setControl((int)Control::Volume, T(50.0f));    
        setControl((int)Control::Gain, T(50.0f));    
    }
    void setParam (const int index, T value) override
    {
        params.at(index) = value;
        switch (index)
        {

            case (int)Param::V1: 
                w_V1.setParams((int) value, 
                    getParam((int)Param::Rp1),
                    getParam((int)Param::Rk1),
                    getParam((int)Param::E1)); break;
            case (int)Param::Ri1: w_Ri1.setResistanceValue(value); break;
            case (int)Param::Rg1: w_Rg1.setResistanceValue(value); break;
            case (int)Param::Ci1: w_Vin.setCapacitanceValue(value); break;
            case (int)Param::Rk1: w_Rk1.setResistanceValue(value); break;
            case (int)Param::Ck1: w_Ck1.setCapacitanceValue(value); break;
            case (int)Param::E1:  w_E1.setVoltage(value);  break;
            case (int)Param::Rp1: w_Rp1.setResistanceValue(value); break;
            case (int)Param::Cp1: w_Cp1.setCapacitanceValue(value); break;
            case (int)Param::RVol: setControl((int)Control::Volume, T(50.0f)); break;
            case (int)Param::Gain: setControl((int)Control::Gain, T(100.0f)); break;

            case (int)Param::Count:
            default:
                jassertfalse;
                break;
        }
    }
    void setControl (const int index, T value) override
    {
        controls.at(index) = value;
        auto ratio = value/T(100.0f); 

        switch (index)
        {
            case (int)Control::Volume: 
            {
                T controlVal = getParam((int)Param::RVol);
                w_Ro1_plus.setResistanceValue(controlVal*(T(1.0f) - ratio));
                w_Ro1_minus.setResistanceValue(controlVal*(ratio));
                break;
            }
            case (int)Control::Gain: 
            {
                inputGain = ratio *  getParam((int)Param::Gain);
                break;
            }
            default: jassertfalse; break;
        }
    }
    void updateMonitors() override{

        updateTubeMonitor((int)Monitoring::V1, w_V1.getMonitorValue());

        updatePortMonitor((int)Monitoring::E1, w_E1);
        updatePortMonitor((int)Monitoring::Rg1, w_Rg1);
        updatePortMonitor((int)Monitoring::Ci1, w_Vin);
        updatePortMonitor((int)Monitoring::Ri1, w_Ri1);
        updatePortMonitor((int)Monitoring::Rk1, w_Rk1);
        updatePortMonitor((int)Monitoring::Ck1, w_Ck1);
        updatePortMonitor((int)Monitoring::RVol_plus, w_Ro1_plus);
        updatePortMonitor((int)Monitoring::RVol_minus, w_Ro1_minus);
        updatePortMonitor((int)Monitoring::Rp1, w_Rp1);
        updatePortMonitor((int)Monitoring::Cp1, w_Cp1);
    }

    void prepare(T sr) override {
        w_Vin.prepare (sr);
        w_Ck1.prepare (sr);
        w_Cp1.prepare (sr);
    }
    void reset() override {
        w_Vin.reset();
        w_Ck1.reset();
        w_Cp1.reset();
    }

    T processSample(T x) override { 
        // V1
        w_Vin.setVoltage (x*inputGain);
        w_E1.setVoltage(getParam((int)Param::E1));
        w_V1.compute();
        auto V1_out = voltage<T> (w_Ro1_minus);


        return V1_out * outputGain;
    }


private: 

    T inputGain = T(1.0f);
    T outputGain = T(2e-3f);
    // ==================================================================================================== 
    // =  First stage 
    // ==================================================================================================== 

    // Grid Circuit (connect V1 to w_PI_g1)
    ResistorT<T> w_Rg1 { T(0.0f) };
    ResistorT<T> w_Ri1 { T(0.0f) };
    CapacitiveVoltageSourceT<T> w_Vin { T(0.0f) };
    PolarityInverterT<T, decltype (w_Vin)> w_PI_i1 { w_Vin };
    WDFParallelT<T, decltype (w_Ri1), decltype (w_PI_i1)> w_PJi { w_Ri1, w_PI_i1 };
    WDFSeriesT<T, decltype (w_Rg1), decltype (w_PJi)> w_SJ_g1 { w_Rg1, w_PJi };
    PolarityInverterT<T, decltype (w_SJ_g1)> w_PI_g1 { w_SJ_g1 };

    // Cathode Circuit (connect V1 to w_PJ_k1)
    ResistorT<T> w_Rk1 { T(0.0f) };
    CapacitorT<T> w_Ck1 { T(0.0f) };
    WDFParallelT<T, decltype (w_Rk1), decltype (w_Ck1)> w_PJ_k1 { w_Rk1, w_Ck1 };

    // Plate Circuit (connect V1 to w_PJ_p1)
    PowerSupplyT<T> w_E1 { T(POWER_SUPPLY_RSAG), T(POWER_SUPPLY_CSAG) };
    PolarityInverterT<T, decltype (w_E1)> w_PI_e1 { w_E1 };
    ResistorT<T> w_Rp1 {T(0.0f)};
    WDFSeriesT<T, decltype(w_PI_e1), decltype(w_Rp1)> w_E1_Rp1 {w_PI_e1, w_Rp1};

    CapacitorT<T> w_Cp1 { T(0.0f) };
    ResistorT<T> w_Ro1_plus { T(0.0f) };
    ResistorT<T> w_Ro1_minus { T(0.0f) };
    WDFSeriesT<T, decltype (w_Ro1_minus), decltype (w_Ro1_plus)> w_SJ_o1 { w_Ro1_minus, w_Ro1_plus };
    WDFSeriesT<T, decltype (w_Cp1), decltype (w_SJ_o1)> w_SJ_p1 { w_Cp1, w_SJ_o1 };
    PolarityInverterT<T, decltype (w_SJ_p1)> w_PI_p1 { w_SJ_p1 };
    WDFParallelT<T, decltype (w_E1_Rp1), decltype (w_PI_p1)> w_PJ_p1 { w_E1_Rp1, w_PI_p1};

    TriodeQuadricWDF<T, decltype(w_PI_g1), decltype(w_PJ_k1), decltype(w_PJ_p1)> w_V1{
            w_PI_g1, w_PJ_k1, w_PJ_p1
    };

};