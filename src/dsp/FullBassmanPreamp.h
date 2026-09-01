#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <chowdsp_wdf/chowdsp_wdf.h>
#include <dsp/Circuit.h>
#include <dsp/TriodeQuadricWDF.h>
#include <dsp/ToneStack.h>
#include <dsp/PowerSupply.h>

inline const float BASSMAN_PREAMP_POWER_SUPPLY_RSAG = 200.0f;
inline const float BASSMAN_PREAMP_POWER_SUPPLY_CSAG = 47e-6f;

using namespace chowdsp::wdft;

template <typename T>
class FullBassmanPreampCircuitT : public Circuit<T>
{
public:
    FullBassmanPreampCircuitT(): Circuit<T>()
    {
        params.resize((int)Param::Count, T(0.0f));
        controls.resize((int)Control::Count, T(0.0f));
        monitors.resize((int)Monitoring::Count);

        setDefaultParam();
        setDefaultControl();

        this->setOutputGain(T(4e-3));

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
        
        V2, Rg2, Rk2, Rp2, E2,

        V3, Rk3, RBass, RMid_plus, RMid_minus, RTreble_plus, RTreble_minus, C1, C2, C3, R4, Rout, E3,
    
        Count 
    };
    enum class Param : int 
    {
        // V1
        V1, Ri1 ,Rg1,Ci1,Rk1,Ck1,E1,Rp1,Cp1, RVol,
        // V2
        V2, Rg2 ,Rk2, Rp2, E2,
        // V3
        V3, Rg3 , Rk3, E3, 
        RBass, RMid, RTreble, C1, C2, C3, R4, // tone stack
        Count 
    };
    enum class Control : int 
    {
        Volume, Bass, Mid, Treble, Count 
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


        setParam((int)Param::Rg2, T(270.0e3f));
        setParam((int)Param::Rk2, T(1.0e3f  ));
        setParam((int)Param::E2,  T(325.0f  ));
        setParam((int)Param::Rp2, T(100.0e3f));

        setParam((int)Param::Rg3, T(1.0e3f  ));
        setParam((int)Param::Rk3, T(100.0e3f));
        setParam((int)Param::E3,  T(325.0f  ));

        setParam((int)Param::C1,      T(0.25e-9));      
        setParam((int)Param::C2,      T(22.0e-9));      
        setParam((int)Param::C3,      T(22.0e-9));      
        setParam((int)Param::R4,      T(56.0e3 ));      
        setParam((int)Param::RBass,   T(250e3f ));    
        setParam((int)Param::RMid,    T(10e3f  ));     
        setParam((int)Param::RTreble, T(250e3f )); 

        setParam((int)Param::V1, T(0.0f )); 
        setParam((int)Param::V2, T(0.0f )); 
        setParam((int)Param::V3, T(0.0f )); 

    }
    void setDefaultControl () 
    { 
        setControl((int)Control::Volume, T(50.0f));    
        setControl((int)Control::Bass, T(50.0f));    
        setControl((int)Control::Mid, T(50.0f));     
        setControl((int)Control::Treble, T(50.0f)); 
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
            case (int)Param::V2: 
                w_V2.setParams((int) value, 
                    getParam((int)Param::Rp2),
                    getParam((int)Param::Rk2),
                    getParam((int)Param::E2));break;
            case (int)Param::V3: 
                w_V3.setParams((int) value, 
                    T(1.0e3f),
                    getParam((int)Param::Rk3),
                    getParam((int)Param::E3)); break;
            
            case (int)Param::Ri1: w_Ri1.setResistanceValue(value); break;
            case (int)Param::Rg1: w_Rg1.setResistanceValue(value); break;
            case (int)Param::Ci1: w_Vin.setCapacitanceValue(value); break;
            case (int)Param::Rk1: w_Rk1.setResistanceValue(value); break;
            case (int)Param::Ck1: w_Ck1.setCapacitanceValue(value); break;
            case (int)Param::E1:  w_E1.setVoltage(value);  break;
            case (int)Param::Rp1: w_Rp1.setResistanceValue(value); break;
            case (int)Param::Cp1: w_Cp1.setCapacitanceValue(value); break;
            case (int)Param::RVol: setControl((int)Control::Volume, T(50.0f)); break;


            case (int)Param::Rg2: w_V_Rg2.setResistanceValue(value); break;
            case (int)Param::Rk2: w_Rk2.setResistanceValue(value); break;
            case (int)Param::E2:  w_E2.setVoltage(value);  break;
            case (int)Param::Rp2: w_Rp2.setResistanceValue(value); break;

            case (int)Param::Rg3: w_V_Rg3.setResistanceValue(value); break;
            case (int)Param::Rk3: w_Rk3.setResistanceValue(value); break;
            case (int)Param::E3:  w_E3.setVoltage(value);  break;


            case (int)Param::C1:       w_TS.setC1(value); break;
            case (int)Param::C2:       w_TS.setC2(value); break;
            case (int)Param::C3:       w_TS.setC3(value); break;
            case (int)Param::R4:       w_TS.setR4(value); break;
            case (int)Param::RBass:     setControl((int)Control::Bass, T(50.0f)); break; 
            case (int)Param::RMid:      setControl((int)Control::Mid, T(50.0f)); break; 
            case (int)Param::RTreble:  setControl((int)Control::Treble, T(50.0f)); break; 

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
                auto r = getPotRatios(ratio, PotType::Log);

                w_Ro1_plus.setResistanceValue(controlVal * r.plus);
                w_Ro1_minus.setResistanceValue(controlVal * r.minus);
                break;
            }
            case (int)Control::Bass: 
            {
                T controlVal = getParam((int)Param::RBass);
                auto r = getPotRatios(ratio, PotType::Log);

                w_TS.setR2(controlVal * r.minus);
                break;
            }
            case (int)Control::Treble: 
            {

                T controlVal = getParam((int)Param::RTreble);
                auto r = getPotRatios(ratio, PotType::Log);

                w_TS.setR1_plus( controlVal * r.plus);
                w_TS.setR1_minus( controlVal * r.minus);
                break;
            }
            case (int)Control::Mid: 
            {
                T controlVal = getParam((int)Param::RMid);
                auto r = getPotRatios(ratio, PotType::Linear);

                w_TS.setR3_plus( controlVal * r.plus);
                w_TS.setR3_minus( controlVal * r.minus);
                break;
            }
            default: jassertfalse; break;
        }
    }
    void updateMonitors() override{

        updateTubeMonitor((int)Monitoring::V1, w_V1.getMonitorValue());
        updateTubeMonitor((int)Monitoring::V2, w_V2.getMonitorValue());
        updateTubeMonitor((int)Monitoring::V3, w_V3.getMonitorValue());

        updatePortMonitor((int)Monitoring::E1, w_E1);
        updatePortMonitor((int)Monitoring::E2, w_E2);
        updatePortMonitor((int)Monitoring::E3, w_E3);

        updatePortMonitor((int)Monitoring::Rg1, w_Rg1);
        updatePortMonitor((int)Monitoring::Ci1, w_Vin);
        updatePortMonitor((int)Monitoring::Ri1, w_Ri1);
        updatePortMonitor((int)Monitoring::Rk1, w_Rk1);
        updatePortMonitor((int)Monitoring::Ck1, w_Ck1);
        updatePortMonitor((int)Monitoring::RVol_plus, w_Ro1_plus);
        updatePortMonitor((int)Monitoring::RVol_minus, w_Ro1_minus);
        updatePortMonitor((int)Monitoring::Rp1, w_Rp1);
        updatePortMonitor((int)Monitoring::Cp1, w_Cp1);

        updatePortMonitor((int)Monitoring::Rk2, w_Rk2);
        updatePortMonitor((int)Monitoring::Rg2, w_V_Rg2);
        updatePortMonitor((int)Monitoring::Rp2, w_Rp2); 

        updatePortMonitor((int)Monitoring::Rk3, w_Rk3); 
        updatePortMonitor((int)Monitoring::RBass, w_TS.w_R2); 
        updatePortMonitor((int)Monitoring::RMid_plus, w_TS.w_R3_plus); 
        updatePortMonitor((int)Monitoring::RMid_minus, w_TS.w_R3_minus); 
        updatePortMonitor((int)Monitoring::RTreble_plus, w_TS.w_R1_plus); 
        updatePortMonitor((int)Monitoring::RTreble_minus, w_TS.w_R1_minus); 
        updatePortMonitor((int)Monitoring::C1, w_TS.w_C1); 
        updatePortMonitor((int)Monitoring::C2, w_TS.w_C2); 
        updatePortMonitor((int)Monitoring::C3, w_TS.w_C3); 
        updatePortMonitor((int)Monitoring::R4, w_TS.w_R4); 
        updatePortMonitor((int)Monitoring::Rout, w_Rload); 

    }

    void prepare(T sr) override {
        w_Vin.prepare (sr);
        w_Ck1.prepare (sr);
        w_Cp1.prepare (sr);
        w_TS.prepare(sr);
    }
    void reset() override {
        w_TS.reset();
        w_Vin.reset();
        w_Ck1.reset();
        w_Cp1.reset();
    }

    T processSample(T x) override { 
        // V1
        w_Vin.setVoltage (x);
        w_E1.setVoltage(getParam((int)Param::E1));
        w_V1.compute();
        auto V1_out = voltage<T> (w_Ro1_minus);

        //V2
        w_V_Rg2.setVoltage (V1_out);
        w_E2.setVoltage(getParam((int)Param::E2));
        w_V2.compute();
        auto V2_out = w_V2.getVp();

        //V3
        w_V_Rg3.setVoltage (V2_out);
        w_E3.setVoltage(getParam((int)Param::E3));
        w_V3.compute();

        return voltage<T>(w_Rload);
    }


private: 

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
    PowerSupplyT<T> w_E1 { T(BASSMAN_PREAMP_POWER_SUPPLY_RSAG), T(BASSMAN_PREAMP_POWER_SUPPLY_CSAG) };
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

    // ==================================================================================================== 
    // =  Second stage 
    // ==================================================================================================== 

    // Grid Circuit (connect V2 to w_Rg2)
    ResistiveVoltageSourceT<T> w_V_Rg2 { T(0.0f) };

    // Cathode Circuit (connect V2 to w_PJ_k2)
    ResistorT<T> w_Rk2 { T(0.0f) };

    // Plate Circuit (connect V2 to w_PJ_p2)
    PowerSupplyT<T> w_E2 { T(BASSMAN_PREAMP_POWER_SUPPLY_RSAG), T(BASSMAN_PREAMP_POWER_SUPPLY_CSAG)  };
    PolarityInverterT<T, decltype (w_E2)> w_PI_e2 { w_E2 };
    ResistorT<T> w_Rp2 {T(0.0f)};
    WDFSeriesT<T, decltype(w_PI_e2), decltype(w_Rp2)> w_E2_Rp2 {w_PI_e2, w_Rp2};

    TriodeQuadricWDF<T, decltype(w_V_Rg2), decltype(w_Rk2), decltype(w_E2_Rp2)> w_V2{
            w_V_Rg2, w_Rk2, w_E2_Rp2
    };
    // ==================================================================================================== 
    // =  Third stage 
    // ==================================================================================================== 

    // Grid Circuit 
    ResistiveVoltageSourceT<T> w_V_Rg3 { T(0.0f) };


    // Cathode Circuit 
    ResistorT<T> w_Rk3 { T(0.0f) };
    ResistorT<T> w_Rload { T(1e6f) };
    ToneStack<T, decltype(w_Rload)> w_TS {w_Rload, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };
    WDFParallelT<T, decltype (w_Rk3), decltype (w_TS)> w_PJ_k3 { w_Rk3, w_TS};

    // Plate Circuit (connect V2 to w_PJ_p2)
    PowerSupplyT<T> w_E3 { T(BASSMAN_PREAMP_POWER_SUPPLY_RSAG), T(BASSMAN_PREAMP_POWER_SUPPLY_CSAG)  };

    TriodeQuadricWDF<T, decltype(w_V_Rg3), decltype(w_PJ_k3), decltype(w_E3)> w_V3{
            w_V_Rg3, w_PJ_k3, w_E3
    };


};