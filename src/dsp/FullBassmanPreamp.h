#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <chowdsp_wdf/chowdsp_wdf.h>
#include "dsp/TriodeWDF.h"
#include "dsp/TriodeQuadricWDF.h"
#include <dsp/Circuit.h>
#include <dsp/BassmanToneStack.h>

using namespace chowdsp::wdft;

class FullBassmanPreampCircuit : public Circuit
{
public:
    FullBassmanPreampCircuit(): Circuit()
    {
        params.resize((int)Param::Count, 0.0f);
        controls.resize((int)Control::Count, 0.0f);

        setDefaultParam();
        setDefaultControl();
    };

    enum class Monitoring : int 
    {
        Vk1, Vp1, Vk2, Vp2, Vk3, Vp3, Count 
    };
    enum class Param : int 
    {
        // V1
        V1, Ri1 ,Rg1,Ci1,Rk1,Ck1,E1,Rp1,Cp1, RVol,
        // V2
        V2, Rg2 ,Rk2, Rp2, E2,
        // V3
        V3, Rg3 , Rk3, Rp3, E3, 
        RBass, RMid, RTrebble, C1, C2, C3, R4, // tone stack
        Count 
    };
    enum class Control : int 
    {
        Volume, Bass, Mid, Trebble, Count 
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
        setParam((int)Param::Rp3, 1.0f);

        setParam((int)Param::C1, 0.25e-9);      
        setParam((int)Param::C2, 22.0e-9);      
        setParam((int)Param::C3, 22.0e-9);      
        setParam((int)Param::R4, 56.0e3);      
        setParam((int)Param::RBass, 250e3f);    
        setParam((int)Param::RMid, 10e3f);     
        setParam((int)Param::RTrebble, 250e3f); 


        w_V1.setTriodeParameters(
            1.014e-5f, 5.498e-8f, 1.076e-5f,
            getParam((int)Param::Rp1),
            getParam((int)Param::Rk1),
            getParam((int)Param::E1)
        );
        w_V2.setTriodeParameters(
            1.014e-5f, 5.498e-8f, 1.076e-5f,
            getParam((int)Param::Rp2),
            getParam((int)Param::Rk2),
            getParam((int)Param::E2)
        );
        w_V3.setTriodeParameters(
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
        setControl((int)Control::Trebble, 50.0f); 
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
            case (int)Param::RTrebble:  setControl((int)Control::Trebble, 50.0f); break; 

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
                auto ratio = std::pow((100.0f-value)/100.0f, 3.0f); // audio taper
                w_bts.setR2(controlVal*(1.0f - ratio));
                break;
            }
            case (int)Control::Trebble: 
            {

                float controlVal = getParam((int)Param::RTrebble);
                auto ratio = std::pow((100.0f-value)/100.0f, 3.0f); // audio taper
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

    void prepare(double sr) override {
        w_Vin.prepare ((float) sr);
        w_Ck1.prepare ((float) sr);
        w_Cp1.prepare ((float) sr);
        w_bts.prepare((float) sr);
        w_V1.prepare ((float) sr);
        w_V2.prepare ((float) sr);
        w_V3.prepare ((float) sr);

        float duration = 0.1f;
        for (int i = 0; i < (int) sr *duration; ++i)
        {
            auto y = processSample(0.0f);
        }
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

        // return V1_out;

        //V2
        w_V_Rg2.setVoltage (V1_out);
        w_E2_Rp2.setVoltage(getParam((int)Param::E2));
        w_V2.compute();
        auto V2_out = voltage<float> (w_E2_Rp2);

        //V3
        w_V_Rg3.setVoltage (V2_out);
        w_E3.setVoltage(getParam((int)Param::E3));
        w_V3.compute();

        return w_bts.getVoltage();
    }
    float getMonitoring(const int index) override{

        switch (index)
        {
            case (int) Monitoring::Vk1: 
                return getVk1(); 
            case (int) Monitoring::Vp1: 
                return getVp1(); 
            case (int) Monitoring::Vk2: 
                return getVk2(); 
            case (int) Monitoring::Vp2: 
                return getVp2(); 
            case (int) Monitoring::Vk3: 
                return getVk3(); 
            case (int) Monitoring::Vp3: 
                return getVp3(); 
            default: jassertfalse; 
                return 0.0f;
        }
    }

    // Accessor methods for monitoring internal WDF variables
    // float getVk1() const { return -w_V1.getVgkAcc(); }
    // float getVp1() const { return w_V1.getVpkAcc() - w_V1.getVgkAcc(); }
    float getVk1() const { return voltage<float>(w_Rk1); }
    float getVp1() const { return voltage<float>(w_E1_Rp1); }
    float getVk2() const { return voltage<float>(w_Rk2); }
    float getVp2() const { return voltage<float>(w_E2_Rp2); }
    float getVk3() const { return -w_V3.getVgkAcc(); }
    float getVp3() const { return w_V3.getVpkAcc() - w_V3.getVgkAcc(); }

    
private: 
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