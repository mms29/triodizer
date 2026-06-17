#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <chowdsp_wdf/chowdsp_wdf.h>
#include "dsp/TriodeWDF.h"
#include "dsp/TriodeQuadricWDF.h"
#include <dsp/Circuit.h>
#include <dsp/BassmanToneStack.h>

using namespace chowdsp::wdft;

template <typename T>
class FullBassmanPreampCircuitT : public Circuit<T>
{
public:
    FullBassmanPreampCircuitT(): Circuit<T>()
    {
        params.resize((int)Param::Count, T(0.0f));
        controls.resize((int)Control::Count, T(0.0f));
        monitors.resize((int)Monitoring::Count, T(0.0f));

        setDefaultParam();
        setDefaultControl();
    };

    using Circuit<T>::params;
    using Circuit<T>::controls;
    using Circuit<T>::monitors;
    using Circuit<T>::getParam;

    enum class Monitoring : int 
    {
        Vk1, Vp1, Vg1, Ik1, 
        Vk2, Vp2, Vg2, Ik2, 
        Vk3, Vp3, Vg3, Ik3,
        
        Rg1_V, Rg1_I, 
        Ci1_V, Ci1_I, 
        Ri1_V, Ri1_I, 
        Rk1_V, Rk1_I, 
        Ck1_V, Ck1_I, 
        Rp1_V, Rp1_I,
        Cp1_V, Cp1_I,
        RVol_plus_V, RVol_plus_I, 
        RVol_minus_V, RVol_minus_I, 
        
        Rg2_V, Rg2_I,
        Rk2_V, Rk2_I,
        Rp2_V, Rp2_I,

        Rk3_V, Rk3_I,

        Count 
    };
    enum class Param : int 
    {
        // V1
        V1, Ri1 ,Rg1,Ci1,Rk1,Ck1,E1,Rp1,Cp1, RVol,
        // V2
        V2, Rg2 ,Rk2, Rp2, E2,
        // V3
        V3, Rg3 , Rk3, Rp3, E3, 
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
        setParam((int)Param::Rp3, T(1.0f    ));

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
                    getParam((int)Param::Rp3),
                    getParam((int)Param::Rk3),
                    getParam((int)Param::E3)); break;
            
            case (int)Param::Ri1: w_Ri1.setResistanceValue(value); break;
            case (int)Param::Rg1: w_Rg1.setResistanceValue(value); break;
            case (int)Param::Ci1: w_Vin.setCapacitanceValue(value); break;
            case (int)Param::Rk1: w_Rk1.setResistanceValue(value); break;
            case (int)Param::Ck1: w_Ck1.setCapacitanceValue(value); break;
            case (int)Param::E1:  w_E1_Rp1.setVoltage(value);  break;
            case (int)Param::Rp1: w_E1_Rp1.setResistanceValue(value); break;
            case (int)Param::Cp1: w_Cp1.setCapacitanceValue(value); break;
            case (int)Param::RVol: setControl((int)Control::Volume, T(50.0f)); break;


            case (int)Param::Rg2: w_V_Rg2.setResistanceValue(value); break;
            case (int)Param::Rk2: w_Rk2.setResistanceValue(value); break;
            case (int)Param::E2:  w_E2_Rp2.setVoltage(value);  break;
            case (int)Param::Rp2: w_E2_Rp2.setResistanceValue(value); break;

            case (int)Param::Rg3: w_V_Rg3.setResistanceValue(value); break;
            case (int)Param::Rk3: w_Rk3.setResistanceValue(value); break;
            case (int)Param::E3:  w_E3.setVoltage(value);  break;
            case (int)Param::Rp3: w_E3.setResistanceValue(value); break;


            case (int)Param::C1:       w_bts.setC1(value); break;
            case (int)Param::C2:       w_bts.setC2(value); break;
            case (int)Param::C3:       w_bts.setC3(value); break;
            case (int)Param::R4:       w_bts.setR4(value); break;
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
                w_Ro1_plus.setResistanceValue(controlVal*(T(1.0f) - ratio));
                w_Ro1_minus.setResistanceValue(controlVal*(ratio));
                break;
            }
            case (int)Control::Bass: 
            {
                T controlVal = getParam((int)Param::RBass);
                // auto ratio = std::pow((100.0f-value)/100.0f, 3.0f); // audio taper
                w_bts.setR2(controlVal*(ratio));
                break;
            }
            case (int)Control::Treble: 
            {

                T controlVal = getParam((int)Param::RTreble);
                // auto ratio = std::pow((100.0f-value)/100.0f, 3.0f); // audio taper
                w_bts.setR1_plus( controlVal*(T(1.0f) - ratio));
                w_bts.setR1_minus( controlVal*(ratio));
                break;
            }
            case (int)Control::Mid: 
            {
                T controlVal = getParam((int)Param::RMid);
                w_bts.setR3_plus( controlVal*(T(1.0f) - ratio));
                w_bts.setR3_minus( controlVal*(ratio));
                break;
            }
            default: jassertfalse; break;
        }
    }
    void updateMonitors() override{
        monitors[(int)Monitoring::Vk1] = voltage<T>(w_Rk1);
        monitors[(int)Monitoring::Vp1] = voltage<T>(w_E1_Rp1);
        monitors[(int)Monitoring::Vg1] = voltage<T>(w_Vin);
        monitors[(int)Monitoring::Ik1] = current<T>(w_Rk1);

        monitors[(int)Monitoring::Vk2] = voltage<T>(w_Rk2) ;
        monitors[(int)Monitoring::Vp2] = voltage<T>(w_E2_Rp2);
        monitors[(int)Monitoring::Vg2] = voltage<T>(w_V_Rg2);
        monitors[(int)Monitoring::Ik2] = current<T>(w_Rk2);

        monitors[(int)Monitoring::Vk3] = voltage<T>(w_Rk3);        
        monitors[(int)Monitoring::Vp3] = voltage<T>(w_E3);
        monitors[(int)Monitoring::Vg3] = voltage<T>(w_V_Rg3);
        monitors[(int)Monitoring::Ik3] = current<T>(w_Rk3);

        monitors[(int)Monitoring::Rg1_V] = voltage<T>(w_Rg1);  
        monitors[(int)Monitoring::Rg1_I] = current<T>(w_Rg1);  
        monitors[(int)Monitoring::Ci1_V] = voltage<T>(w_Vin);  
        monitors[(int)Monitoring::Ci1_I] = current<T>(w_Vin);  
        monitors[(int)Monitoring::Ri1_V] = voltage<T>(w_Ri1);  
        monitors[(int)Monitoring::Ri1_I] = current<T>(w_Ri1);  
        monitors[(int)Monitoring::Rk1_V] = voltage<T>(w_Rk1);  
        monitors[(int)Monitoring::Rk1_I] = current<T>(w_Rk1);  
        monitors[(int)Monitoring::Ck1_V] = voltage<T>(w_Ck1);  
        monitors[(int)Monitoring::Ck1_I] = current<T>(w_Ck1);  
        monitors[(int)Monitoring::RVol_plus_V] = voltage<T>(w_Ro1_plus);  
        monitors[(int)Monitoring::RVol_plus_I] = current<T>(w_Ro1_plus);  
        monitors[(int)Monitoring::RVol_minus_V] = voltage<T>(w_Ro1_minus);  
        monitors[(int)Monitoring::RVol_minus_I] = current<T>(w_Ro1_minus);  

        monitors[(int)Monitoring::Rp1_V] = voltage<T>(w_E1_Rp1);    
        monitors[(int)Monitoring::Rp1_I] = current<T>(w_E1_Rp1);  
        monitors[(int)Monitoring::Cp1_V] = voltage<T>(w_Cp1);    
        monitors[(int)Monitoring::Cp1_I] = current<T>(w_Cp1);    
        monitors[(int)Monitoring::Rg2_V] = voltage<T>(w_V_Rg2);    
        monitors[(int)Monitoring::Rg2_I] = current<T>(w_V_Rg2);    
        monitors[(int)Monitoring::Rk2_V] = voltage<T>(w_Rk2);    
        monitors[(int)Monitoring::Rk2_I] = current<T>(w_Rk2);    
        monitors[(int)Monitoring::Rp2_V] = voltage<T>(w_E2_Rp2);    
        monitors[(int)Monitoring::Rp2_I] = current<T>(w_E2_Rp2);    

        monitors[(int)Monitoring::Rk3_V] = voltage<T>(w_Rk3);    
        monitors[(int)Monitoring::Rk3_I] = current<T>(w_Rk3);    
    }


    void prepare(T sr) override {
        w_Vin.prepare (sr);
        w_Ck1.prepare (sr);
        w_Cp1.prepare (sr);
        w_bts.prepare(sr);
    }
    void reset() override {
        w_bts.reset();
        w_Vin.reset();
        w_Ck1.reset();
        w_Cp1.reset();
    }

    T processSample(T x) override { 
        // V1
        w_Vin.setVoltage (x);
        w_E1_Rp1.setVoltage(getParam((int)Param::E1));
        w_V1.compute();
        auto V1_out = voltage<T> (w_Ro1_minus);

        //V2
        w_V_Rg2.setVoltage (V1_out);
        w_E2_Rp2.setVoltage(getParam((int)Param::E2));
        w_V2.compute();
        auto V2_out = voltage<T> (w_E2_Rp2);

        //V3
        w_V_Rg3.setVoltage (V2_out);
        w_E3.setVoltage(getParam((int)Param::E3));
        w_V3.compute();

        return w_bts.getVoltage() * outputGain;
    }


private: 

    T outputGain = T(2e-3);
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
    ResistiveVoltageSourceT<T> w_E1_Rp1 { T(0.0f) };
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
    ResistiveVoltageSourceT<T> w_E2_Rp2 { T(0.0f) };

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
    BassmanToneStack<T> w_bts {};
    WDFParallelT<T, decltype (w_Rk3), decltype (w_bts)> w_PJ_k3 { w_Rk3, w_bts};

    // Plate Circuit (connect V2 to w_PJ_p2)
    ResistiveVoltageSourceT<T> w_E3 { T(0.0f) };

    TriodeQuadricWDF<T, decltype(w_V_Rg3), decltype(w_PJ_k3), decltype(w_E3)> w_V3{
            w_V_Rg3, w_PJ_k3, w_E3
    };


};