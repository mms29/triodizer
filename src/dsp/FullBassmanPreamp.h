#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <chowdsp_wdf/chowdsp_wdf.h>
#include "dsp/TriodeWDF.h"
#include "dsp/TriodeQuadricWDF.h"
#include <dsp/Circuit.h>
#include <dsp/BassmanToneStack.h>

using namespace chowdsp::wdft;

class FullBassmanPreampCircuit : public Circuit<float>
{
public:
    FullBassmanPreampCircuit(): Circuit()
    {
        params.resize((int)Param::Count, 0.0f);
        controls.resize((int)Control::Count, 0.0f);
        monitors.resize((int)Monitoring::Count, 0.0f);

        setDefaultParam();
        setDefaultControl();

        w_V1.calcImpedance();
        w_V2.calcImpedance();
        w_V3.calcImpedance();
    };

    enum class Monitoring : int 
    {
        VDCk1, VDCk2, VDCk3, 
        VDCp1, VDCp2, VDCp3, 
        VACk1, VACk2, VACk3, 
        VACp1, VACp2, VACp3, 
        Ik1  , Ik2  , Ik3  , 
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
        setParam((int)Param::Ri1, 1.0e6f);
        setParam((int)Param::Rg1, 20.0e3f);
        setParam((int)Param::Ci1, 100e-9f);
        setParam((int)Param::Rk1, 1.0e3f);
        setParam((int)Param::Ck1, 250e-6f);
        setParam((int)Param::E1, 325.0f);
        setParam((int)Param::Cp1, 22e-9f);
        setParam((int)Param::Rp1, 100.0e3f);
        setParam((int)Param::RVol, 1.0e6f);


        setParam((int)Param::Rg2, 270.0e3f);
        setParam((int)Param::Rk2, 1.0e3f);
        setParam((int)Param::E2, 325.0f);
        setParam((int)Param::Rp2, 100.0e3f);

        setParam((int)Param::Rg3, 1.0e3f);
        setParam((int)Param::Rk3, 100.0e3f);
        setParam((int)Param::E3, 325.0f);
        setParam((int)Param::Rp3, 1.0e3f);

        setParam((int)Param::C1, 0.25e-9);      
        setParam((int)Param::C2, 22.0e-9);      
        setParam((int)Param::C3, 22.0e-9);      
        setParam((int)Param::R4, 56.0e3);      
        setParam((int)Param::RBass, 250e3f);    
        setParam((int)Param::RMid, 10e3f);     
        setParam((int)Param::RTreble, 250e3f); 


        w_V1.setTubeLabParameters(
            1.014e-5f, 5.498e-8f, 1.076e-5f,
            getParam((int)Param::Rp1),
            getParam((int)Param::Rk1),
            getParam((int)Param::E1)
        );
        w_V2.setTubeLabParameters(
            1.014e-5f, 5.498e-8f, 1.076e-5f,
            getParam((int)Param::Rp2),
            getParam((int)Param::Rk2),
            getParam((int)Param::E2)
        );
        w_V3.setTubeLabParameters(
            1.014e-5f, 5.498e-8f, 1.076e-5f,
            getParam((int)Param::Rp3),
            getParam((int)Param::Rk3),
            getParam((int)Param::E3)
        );

    }
    void setDefaultControl () 
    { 
        setControl((int)Control::Volume, 50.0f);    
        setControl((int)Control::Bass, 50.0f);    
        setControl((int)Control::Mid, 50.0f);     
        setControl((int)Control::Treble, 50.0f); 
    }
    void setParam (const int index, float value) override
    {
        params.at(index) = value;
        switch (index)
        {

            case (int)Param::V1: break;//TODO
            case (int)Param::V2: break;//TODO
            case (int)Param::V3: break;//TODO
            
            case (int)Param::Ri1: w_Ri1.setResistanceValue(value); break;
            case (int)Param::Rg1: w_Rg1.setResistanceValue(value); break;
            case (int)Param::Ci1: w_Vin.setCapacitanceValue(value); break;
            case (int)Param::Rk1: w_Rk1.setResistanceValue(value); break;
            case (int)Param::Ck1: w_Ck1.setCapacitanceValue(value); break;
            case (int)Param::E1:  w_E1_Rp1.setVoltage(value);  break;
            case (int)Param::Rp1: w_E1_Rp1.setResistanceValue(value); break;
            case (int)Param::Cp1: w_Cp1.setCapacitanceValue(value); break;
            case (int)Param::RVol: setControl((int)Control::Volume, 50.0f); break;


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
            case (int)Param::RBass:     setControl((int)Control::Bass, 50.0f); break; 
            case (int)Param::RMid:      setControl((int)Control::Mid, 50.0f); break; 
            case (int)Param::RTreble:  setControl((int)Control::Treble, 50.0f); break; 

            case (int)Param::Count:
            default:
                jassertfalse;
                break;
        }
    }
    void setControl (const int index, float value) override
    {
        controls.at(index) = value;


        switch (index)
        {
            case (int)Control::Volume: 
            {
                float controlVal = getParam((int)Param::RVol);
                auto ratio = (100.0f-value)/100.0f; 
                w_Ro1_plus.setResistanceValue(controlVal*ratio);
                w_Ro1_minus.setResistanceValue(controlVal*(1.0f - ratio));
                break;
            }
            case (int)Control::Bass: 
            {
                float controlVal = getParam((int)Param::RBass);
                // auto ratio = std::pow((100.0f-value)/100.0f, 3.0f); // audio taper
                auto ratio = (100.0f-value)/100.0f;
                w_bts.setR2(controlVal*(1.0f - ratio));
                break;
            }
            case (int)Control::Treble: 
            {

                float controlVal = getParam((int)Param::RTreble);
                // auto ratio = std::pow((100.0f-value)/100.0f, 3.0f); // audio taper
                auto ratio = (100.0f-value)/100.0f;
                w_bts.setR1_plus( controlVal*ratio);
                w_bts.setR1_minus( controlVal*(1.0f - ratio));
                break;
            }
            case (int)Control::Mid: 
            {
                float controlVal = getParam((int)Param::RMid);
                auto ratio = (100.0f-value)/100.0f; 
                w_bts.setR3_plus( controlVal*ratio);
                w_bts.setR3_minus( controlVal*(1.0f - ratio));
                break;
            }
            default: jassertfalse; break;
        }
    }
    void updateMonitors() override{
        // ======================
        // K1
        // ======================
        {
            auto& vdc = monitors[(int)Monitoring::VDCk1];
            auto& vac = monitors[(int)Monitoring::VACk1];

            const float x = getVk1();

            vdc = lowPass(x, vdc);
            vac = variance(x, vdc, vac);
        }

        // ======================
        // K2
        // ======================
        {
            auto& vdc = monitors[(int)Monitoring::VDCk2];
            auto& vac = monitors[(int)Monitoring::VACk2];

            const float x = getVk2();

            vdc = lowPass(x, vdc);
            vac = variance(x, vdc, vac);
        }

        // ======================
        // K3
        // ======================
        {
            auto& vdc = monitors[(int)Monitoring::VDCk3];
            auto& vac = monitors[(int)Monitoring::VACk3];

            const float x = getVk3();

            vdc = lowPass(x, vdc);
            vac = variance(x, vdc, vac);
        }

        // ======================
        // P1
        // ======================
        {
            auto& vdc = monitors[(int)Monitoring::VDCp1];
            auto& vac = monitors[(int)Monitoring::VACp1];

            const float x = getVp1();

            vdc = lowPass(x, vdc);
            vac = variance(x, vdc, vac);
        }

        // ======================
        // P2
        // ======================
        {
            auto& vdc = monitors[(int)Monitoring::VDCp2];
            auto& vac = monitors[(int)Monitoring::VACp2];

            const float x = getVp2();

            vdc = lowPass(x, vdc);
            vac = variance(x, vdc, vac);
        }

        // ======================
        // P3
        // ======================
        {
            auto& vdc = monitors[(int)Monitoring::VDCp3];
            auto& vac = monitors[(int)Monitoring::VACp3];

            const float x = getVp3();

            vdc = lowPass(x, vdc);
            vac = variance(x, vdc, vac);
        }

        // ======================
        // CURRENT (DC only)
        // ======================
        {
            auto& ik1 = monitors[(int)Monitoring::Ik1];
            ik1 = lowPass(getIk1(), ik1);
        }

        {
            auto& ik2 = monitors[(int)Monitoring::Ik2];
            ik2 = lowPass(getIk2(), ik2);
        }

        {
            auto& ik3 = monitors[(int)Monitoring::Ik3];
            ik3 = lowPass(getIk3(), ik3);
        }
    }


    void prepare(float sr) override {
        w_Vin.prepare (sr);
        w_Ck1.prepare (sr);
        w_Cp1.prepare (sr);
        w_bts.prepare(sr);
        alpha = 1.0f / (sr * 0.5f); 
    }
    void reset() override {
        w_bts.reset();
        w_Vin.reset();
        w_Ck1.reset();
        w_Cp1.reset();
    }

    float processSample(float x) override { 
        // V1
        w_Vin.setVoltage (x);
        w_E1_Rp1.setVoltage(getParam((int)Param::E1));
        w_V1.compute();
        auto V1_out = voltage<float> (w_Ro1_minus);

        //V2
        w_V_Rg2.setVoltage (V1_out);
        w_E2_Rp2.setVoltage(getParam((int)Param::E2));
        w_V2.compute();
        auto V2_out = voltage<float> (w_E2_Rp2);

        //V3
        w_V_Rg3.setVoltage (V2_out);
        w_E3.setVoltage(getParam((int)Param::E3));
        w_V3.compute();

        updateMonitors();
        return w_bts.getVoltage();
    }

    // Accessor methods for monitoring internal WDF variables
    // float getVk1() const { return -w_V1.getVgkAcc(); }
    // float getVp1() const { return w_V1.getVpkAcc() - w_V1.getVgkAcc(); }
    float getVk1() const { return voltage<float>(w_Rk1); }
    float getVp1() const { return voltage<float>(w_E1_Rp1); }
    float getVk2() const { return voltage<float>(w_Rk2); }
    float getVp2() const { return voltage<float>(w_E2_Rp2); }
    float getVk3() const { return voltage<float>(w_Rk3); }
    float getVp3() const { return voltage<float>(w_E3); }

    float getIk1() const { return current<float>(w_Rk1); }
    float getIk2() const { return current<float>(w_Rk2); }
    float getIk3() const { return current<float>(w_Rk3); }

private: 

// Monitoring utils
    float alpha=1.0f;

    float lowPass(float x, float mean){
        return mean + alpha * (x - mean);
    }
    float variance(float x, float mean, float var){
        return var + alpha * ((x - mean)*(x - mean) - var);
    }

    // ==================================================================================================== 
    // =  First stage 
    // ==================================================================================================== 

    // Grid Circuit (connect V1 to w_PI_g1)
    ResistorT<float> w_Rg1 { 0.0f };
    ResistorT<float> w_Ri1 { 0.0f };
    CapacitiveVoltageSourceT<float> w_Vin { 0.0f };
    PolarityInverterT<float, decltype (w_Vin)> w_PI_i1 { w_Vin };
    WDFParallelT<float, decltype (w_Ri1), decltype (w_PI_i1)> w_PJi { w_Ri1, w_PI_i1 };
    WDFSeriesT<float, decltype (w_Rg1), decltype (w_PJi)> w_SJ_g1 { w_Rg1, w_PJi };
    PolarityInverterT<float, decltype (w_SJ_g1)> w_PI_g1 { w_SJ_g1 };

    // Cathode Circuit (connect V1 to w_PJ_k1)
    ResistorT<float> w_Rk1 { 0.0f };
    CapacitorT<float> w_Ck1 { 0.0f };
    WDFParallelT<float, decltype (w_Rk1), decltype (w_Ck1)> w_PJ_k1 { w_Rk1, w_Ck1 };

    // Plate Circuit (connect V1 to w_PJ_p1)
    ResistiveVoltageSourceT<float> w_E1_Rp1 { 0.0f };
    CapacitorT<float> w_Cp1 { 0.0f };
    ResistorT<float> w_Ro1_plus { 0.0f };
    ResistorT<float> w_Ro1_minus { 0.0f };
    WDFSeriesT<float, decltype (w_Ro1_minus), decltype (w_Ro1_plus)> w_SJ_o1 { w_Ro1_minus, w_Ro1_plus };
    WDFSeriesT<float, decltype (w_Cp1), decltype (w_SJ_o1)> w_SJ_p1 { w_Cp1, w_SJ_o1 };
    PolarityInverterT<float, decltype (w_SJ_p1)> w_PI_p1 { w_SJ_p1 };
    WDFParallelT<float, decltype (w_E1_Rp1), decltype (w_PI_p1)> w_PJ_p1 { w_E1_Rp1, w_PI_p1};

    TriodeQuadricWDF<float, decltype(w_PI_g1), decltype(w_PJ_k1), decltype(w_PJ_p1)> w_V1{
            w_PI_g1, w_PJ_k1, w_PJ_p1, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f
    };

    // ==================================================================================================== 
    // =  Second stage 
    // ==================================================================================================== 

    // Grid Circuit (connect V2 to w_Rg2)
    ResistiveVoltageSourceT<float> w_V_Rg2 { 0.0f };

    // Cathode Circuit (connect V2 to w_PJ_k2)
    ResistorT<float> w_Rk2 { 0.0f };

    // Plate Circuit (connect V2 to w_PJ_p2)
    ResistiveVoltageSourceT<float> w_E2_Rp2 { 0.0f };

    TriodeQuadricWDF<float, decltype(w_V_Rg2), decltype(w_Rk2), decltype(w_E2_Rp2)> w_V2{
            w_V_Rg2, w_Rk2, w_E2_Rp2, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f
    };
    // ==================================================================================================== 
    // =  Third stage 
    // ==================================================================================================== 

    // Grid Circuit 
    ResistiveVoltageSourceT<float> w_V_Rg3 { 0.0f };


    // Cathode Circuit 
    ResistorT<float> w_Rk3 { 0.0f };
    BassmanToneStack<float> w_bts {};
    WDFParallelT<float, decltype (w_Rk3), decltype (w_bts)> w_PJ_k3 { w_Rk3, w_bts};

    // Plate Circuit (connect V2 to w_PJ_p2)
    ResistiveVoltageSourceT<float> w_E3 { 0.0f };

    TriodeQuadricWDF<float, decltype(w_V_Rg3), decltype(w_PJ_k3), decltype(w_E3)> w_V3{
            w_V_Rg3, w_PJ_k3, w_E3, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f
    };


};



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

        w_V1.calcImpedance();
        w_V2.calcImpedance();
        w_V3.calcImpedance();
    };

    using Circuit<T>::params;
    using Circuit<T>::controls;
    using Circuit<T>::monitors;
    using Circuit<T>::getParam;

    enum class Monitoring : int 
    {
        VDCk1, VDCk2, VDCk3, 
        VDCp1, VDCp2, VDCp3, 
        VACk1, VACk2, VACk3, 
        VACp1, VACp2, VACp3, 
        Ik1  , Ik2  , Ik3  , 
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


        w_V1.setTubeLabParameters(
            T(1.014e-5f), T(5.498e-8f), T(1.076e-5f),
            getParam((int)Param::Rp1),
            getParam((int)Param::Rk1),
            getParam((int)Param::E1)
        );
        w_V2.setTubeLabParameters(
            T(1.014e-5f), T(5.498e-8f), T(1.076e-5f),
            getParam((int)Param::Rp2),
            getParam((int)Param::Rk2),
            getParam((int)Param::E2)
        );
        w_V3.setTubeLabParameters(
            T(1.014e-5f), T(5.498e-8f), T(1.076e-5f),
            getParam((int)Param::Rp3),
            getParam((int)Param::Rk3),
            getParam((int)Param::E3)
        );

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

            case (int)Param::V1: break;//TODO
            case (int)Param::V2: break;//TODO
            case (int)Param::V3: break;//TODO
            
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
        // ======================
        // K1
        // ======================
        {
            auto& vdc = monitors[(int)Monitoring::VDCk1];
            auto& vac = monitors[(int)Monitoring::VACk1];

            const T x = getVk1();

            vdc = lowPass(x, vdc);
            vac = variance(x, vdc, vac);
        }

        // ======================
        // K2
        // ======================
        {
            auto& vdc = monitors[(int)Monitoring::VDCk2];
            auto& vac = monitors[(int)Monitoring::VACk2];

            const T x = getVk2();

            vdc = lowPass(x, vdc);
            vac = variance(x, vdc, vac);
        }

        // ======================
        // K3
        // ======================
        {
            auto& vdc = monitors[(int)Monitoring::VDCk3];
            auto& vac = monitors[(int)Monitoring::VACk3];

            const T x = getVk3();

            vdc = lowPass(x, vdc);
            vac = variance(x, vdc, vac);
        }

        // ======================
        // P1
        // ======================
        {
            auto& vdc = monitors[(int)Monitoring::VDCp1];
            auto& vac = monitors[(int)Monitoring::VACp1];

            const T x = getVp1();

            vdc = lowPass(x, vdc);
            vac = variance(x, vdc, vac);
        }

        // ======================
        // P2
        // ======================
        {
            auto& vdc = monitors[(int)Monitoring::VDCp2];
            auto& vac = monitors[(int)Monitoring::VACp2];

            const T x = getVp2();

            vdc = lowPass(x, vdc);
            vac = variance(x, vdc, vac);
        }

        // ======================
        // P3
        // ======================
        {
            auto& vdc = monitors[(int)Monitoring::VDCp3];
            auto& vac = monitors[(int)Monitoring::VACp3];

            const T x = getVp3();

            vdc = lowPass(x, vdc);
            vac = variance(x, vdc, vac);
        }

        // ======================
        // CURRENT (DC only)
        // ======================
        {
            auto& ik1 = monitors[(int)Monitoring::Ik1];
            ik1 = lowPass(getIk1(), ik1);
        }

        {
            auto& ik2 = monitors[(int)Monitoring::Ik2];
            ik2 = lowPass(getIk2(), ik2);
        }

        {
            auto& ik3 = monitors[(int)Monitoring::Ik3];
            ik3 = lowPass(getIk3(), ik3);
        }
    }


    void prepare(T sr) override {
        w_Vin.prepare (sr);
        w_Ck1.prepare (sr);
        w_Cp1.prepare (sr);
        w_bts.prepare(sr);
        alpha = 1.0f / (sr * 0.5f); 
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

        updateMonitors();
        return w_bts.getVoltage();
    }

    // Accessor methods for monitoring internal WDF variables
    // T getVk1() const { return -w_V1.getVgkAcc(); }
    // T getVp1() const { return w_V1.getVpkAcc() - w_V1.getVgkAcc(); }
    T getVk1() const { return voltage<T>(w_Rk1); }
    T getVp1() const { return voltage<T>(w_E1_Rp1); }
    T getVk2() const { return voltage<T>(w_Rk2); }
    T getVp2() const { return voltage<T>(w_E2_Rp2); }
    T getVk3() const { return voltage<T>(w_Rk3); }
    T getVp3() const { return voltage<T>(w_E3); }

    T getIk1() const { return current<T>(w_Rk1); }
    T getIk2() const { return current<T>(w_Rk2); }
    T getIk3() const { return current<T>(w_Rk3); }

private: 

// Monitoring utils
    T alpha=1.0f;

    T lowPass(T x, T mean){
        return mean + alpha * (x - mean);
    }
    T variance(T x, T mean, T var){
        return var + alpha * ((x - mean)*(x - mean) - var);
    }

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
            w_PI_g1, w_PJ_k1, w_PJ_p1, T(0.0f),T(0.0f),T(0.0f),T(0.0f),T(0.0f),T(0.0f)
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
            w_V_Rg2, w_Rk2, w_E2_Rp2, T(0.0f),T(0.0f),T(0.0f),T(0.0f),T(0.0f),T(0.0f)
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
            w_V_Rg3, w_PJ_k3, w_E3, T(0.0f),T(0.0f),T(0.0f),T(0.0f),T(0.0f),T(0.0f)
    };


};