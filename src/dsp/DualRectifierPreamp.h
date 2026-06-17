#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <chowdsp_wdf/chowdsp_wdf.h>
#include "dsp/TriodeWDF.h"
#include "dsp/TriodeQuadricWDF.h"
#include <dsp/Circuit.h>
#include <dsp/ToneStack.h>



using namespace chowdsp::wdft;


class DualRectifierPreampCircuit : public Circuit<float>
{
public:
    DualRectifierPreampCircuit(): Circuit<float>()
    {
        params.resize((int)Param::Count, 0.0f);
        controls.resize((int)Control::Count, 0.0f);
        monitors.resize((int)Monitoring::Count, 0.0f);

        // Init params
        setDefaultParam();
        setDefaultControl();

        // Dummy resistances 
        w_Vin.setResistanceValue(1.0e3F);
        w_V_R3.setResistanceValue(1.0e3F);
        w_E5.setResistanceValue(1.0e3F);
        w_V_R5.setResistanceValue(1.0e3F);

    };

    enum class Monitoring : int 
    {

        VDCk1, VDCk2, VDCk3, VDCk4, VDCk5, 
        VDCp1, VDCp2, VDCp3, VDCp4, VDCp5, 
        Ik1  , Ik2  , Ik3  , Ik4  , Ik5  , 
        Count 
    };
    enum class Param : int 
    {
        // V1
        V1, Rg1, Rk1,Ck1,E1,Rp1,Cp1, 
        
        // First tone shaping
        Ra1, Rb1, Ca1, Cb1, Rc1, RVol, Cbright,

        // V2
        V2, Rg2, Cg2, Rk2, Ck2, Rp2, E2, Cp2, Ra2, Rb2,

        //V3 cold clipper
        V3, Rk3, Rp3, Cp3, E3, Ra3,

        //V4
        V4, Rg4, Rk4, Ck4, Rp4, E4,

        // V5
        V5, Rk5, E5,

        // Tone Stack
        C1, C2, C3, R4, RTreble, RBass, RMid,

        // Presence
        C5, R5, RPres,
        RMas,
        Count 
    };
    enum class Control : int 
    {
        Volume, Bass, Treble, Mid, Presence, Master, Count 
    };
    void setDefaultParam () 
    {
        //V1
        setParam((int)Param::Rg1, 1e6f);
        setParam((int)Param::Rk1, 1.8e3f);
        setParam((int)Param::Ck1, 1e-6f);
        setParam((int)Param::E1, 400.0f);
        setParam((int)Param::Cp1, 20e-9f);
        setParam((int)Param::Rp1, 220.0e3f);

        //Tone Stack 1
        setParam((int)Param::Ra1, 680.0e3f);
        setParam((int)Param::Rb1, 2200.0e3f);
        setParam((int)Param::Ca1, 2e-9f);
        setParam((int)Param::Cb1, 82e-12);
        setParam((int)Param::Rc1, 2200.0e3f);

        // Vol
        setParam((int)Param::Cbright, 1e-9f);
        setParam((int)Param::RVol, 1.0e6f);

        //V2
        setParam((int)Param::Rg2, 470.0e3f);
        setParam((int)Param::Cg2, 20e-12);
        setParam((int)Param::Rk2, 1.8e3f);
        setParam((int)Param::Ck2, 1e-6);
        setParam((int)Param::E2, 400.0f);
        setParam((int)Param::Rp2, 100.0e3f);
        setParam((int)Param::Cp2, 20e-9);
        setParam((int)Param::Ra2, 470e3f);
        setParam((int)Param::Rb2, 1e6f);

        // Cold Clipper V3
        setParam((int)Param::Rk3, 39e3f);
        setParam((int)Param::Rp3, 100e3f);
        setParam((int)Param::Cp3, 20e-9f);
        setParam((int)Param::E3, 400.0f);
        setParam((int)Param::Ra3, 330e3f);

        // V4
        setParam((int)Param::Rg4, 220e3f);
        setParam((int)Param::Rk4, 1.8e3f);
        setParam((int)Param::Ck4, 1e-6f);
        setParam((int)Param::E4, 400.0f);
        setParam((int)Param::Rp4, 220e3f);

        //V5
        setParam((int)Param::Rk5, 100e3f);
        setParam((int)Param::E5, 400.0f);

        //Tone stack
        setParam((int)Param::C1, 680e-12f);      
        setParam((int)Param::C2, 20.0e-9);      
        setParam((int)Param::C3, 20.0e-9);      
        setParam((int)Param::R4, 47.0e3);      
        setParam((int)Param::RBass, 1e6f);    
        setParam((int)Param::RMid, 25e3f);     
        setParam((int)Param::RTreble, 220e3f); 

        //Presence + Master
        setParam((int)Param::R5, 22e3f); 
        setParam((int)Param::C5, 3e-9f); 
        setParam((int)Param::RPres, 25e3f); 
        setParam((int)Param::RMas, 1e6f); 

        // Triodes
        setParam((int)Param::V1, 0.0F); 
        setParam((int)Param::V2, 0.0F); 
        setParam((int)Param::V3, 0.0F); 
        setParam((int)Param::V4, 0.0F); 
        setParam((int)Param::V5, 0.0F); 
    }
    void setDefaultControl () 
    { 
        setControl((int)Control::Volume, 50.0f);    
    }
    void setParam (const int index, float value) override
    {
        params.at(index) = value;
        switch (index)
        {
            case (int)Param::V1: w_V1.setParams((int) value,
                getParam((int)Param::Rp1),
                getParam((int)Param::Rk1),
                getParam((int)Param::E1)); break;
            case (int)Param::V2: w_V2.setParams((int) value,
                getParam((int)Param::Rp2),
                getParam((int)Param::Rk2),
                getParam((int)Param::E2)); break;
            case (int)Param::V3: w_V3.setParams((int) value,
                getParam((int)Param::Rp3),
                getParam((int)Param::Rk3),
                getParam((int)Param::E3)); break;
            case (int)Param::V4: w_V4.setParams((int) value,
                getParam((int)Param::Rp4),
                getParam((int)Param::Rk4),
                getParam((int)Param::E4)); break;
            case (int)Param::V5: w_V5.setParams((int) value,
                1.0f,
                getParam((int)Param::Rk5),
                getParam((int)Param::E5)); break;
            
            case (int)Param::Rg1: w_Rg1.setResistanceValue(value); break;
            case (int)Param::Rk1: w_Rk1.setResistanceValue(value); break;
            case (int)Param::Ck1: w_Ck1.setCapacitanceValue(value); break;
            case (int)Param::E1:  w_E1_Rp1.setVoltage(value);  break;
            case (int)Param::Rp1: w_E1_Rp1.setResistanceValue(value); break;
            case (int)Param::Cp1: w_Cp1.setCapacitanceValue(value); break;

            case (int)Param::Ra1: w_Ra1.setResistanceValue(value); break;
            case (int)Param::Rb1: w_Rb1.setResistanceValue(value); break;
            case (int)Param::Ca1: w_Ca1.setCapacitanceValue(value); break;
            case (int)Param::Cb1: w_Cb1.setCapacitanceValue(value); break;
            case (int)Param::Rc1: w_Rc1.setResistanceValue(value); break;

            case (int)Param::Cbright: w_Cbright.setCapacitanceValue(value); break;
            case (int)Param::RVol: setControl((int)Control::Volume, 50.0f); break;

            case (int)Param::Rg2: w_V_Rg2.setResistanceValue(value); break;
            case (int)Param::Cg2: w_Cg2.setCapacitanceValue(value); break;
            case (int)Param::Rk2: w_Rk2.setResistanceValue(value); break;
            case (int)Param::Ck2: w_Ck2.setCapacitanceValue(value); break;
            case (int)Param::E2:  w_E2_Rp2.setVoltage(value);  break;
            case (int)Param::Rp2: w_E2_Rp2.setResistanceValue(value); break;
            case (int)Param::Cp2: w_Cp2.setCapacitanceValue(value); break;
            case (int)Param::Ra2: w_Ra2.setResistanceValue(value); break;
            case (int)Param::Rb2: w_Rb2.setResistanceValue(value); break;

            case (int)Param::Rk3: w_Rk3.setResistanceValue(value); break;
            case (int)Param::Cp3: w_Cp3.setCapacitanceValue(value); break;
            case (int)Param::E3:  w_E3_Rp3.setVoltage(value);  break;
            case (int)Param::Rp3: w_E3_Rp3.setResistanceValue(value); break;
            case (int)Param::Ra3: w_Ra3.setResistanceValue(value); break;

            case (int)Param::Rg4: w_V_Rg4.setResistanceValue(value); break;
            case (int)Param::Rk4: w_Rk4.setResistanceValue(value); break;
            case (int)Param::Ck4: w_Ck4.setCapacitanceValue(value); break;
            case (int)Param::E4:  w_E4_Rp4.setVoltage(value);  break;
            case (int)Param::Rp4: w_E4_Rp4.setResistanceValue(value); break;

            case (int)Param::Rk5: w_Rk5.setResistanceValue(value); break;
            case (int)Param::E5:  w_E5.setVoltage(value);  break;


            case (int)Param::C1:       w_TS.setC1(value); break;
            case (int)Param::C2:       w_TS.setC2(value); break;
            case (int)Param::C3:       w_TS.setC3(value); break;
            case (int)Param::R4:       w_TS.setR4(value); break;
            case (int)Param::RBass:     setControl((int)Control::Bass, 50.0f); break; 
            case (int)Param::RMid:      setControl((int)Control::Mid, 50.0f); break; 
            case (int)Param::RTreble:  setControl((int)Control::Treble, 50.0f); break; 

            case (int)Param::R5:       w_R5.setResistanceValue(value); break;
            case (int)Param::C5:       w_C5.setCapacitanceValue(value); break;
            case (int)Param::RPres:     setControl((int)Control::Presence, 50.0f); break; 
            case (int)Param::RMas:      setControl((int)Control::Master, 50.0f); break; 

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
                w_RVol_plus.setResistanceValue(controlVal*ratio);
                w_RVol_minus.setResistanceValue(controlVal*(1.0f - ratio));
                break;
            }
            case (int)Control::Master: 
            {
                float controlVal = getParam((int)Param::RMas);
                auto ratio = (100.0f-value)/100.0f; 
                w_RMas_plus.setResistanceValue(controlVal*ratio);
                w_RMas_minus.setResistanceValue(controlVal*(1.0f - ratio));
                break;
            }
            case (int)Control::Presence: 
            {
                float controlVal = getParam((int)Param::RPres);
                auto ratio = (100.0f-value)/100.0f; 
                w_RPres.setResistanceValue(controlVal*ratio);
                break;
            }
            case (int)Control::Bass: 
            {
                float controlVal = getParam((int)Param::RBass);
                // auto ratio = std::pow((100.0f-value)/100.0f, 3.0f); // audio taper
                auto ratio = (100.0f-value)/100.0f;
                w_TS.setR2(controlVal*(1.0f - ratio));
                break;
            }
            case (int)Control::Treble: 
            {

                float controlVal = getParam((int)Param::RTreble);
                // auto ratio = std::pow((100.0f-value)/100.0f, 3.0f); // audio taper
                auto ratio = (100.0f-value)/100.0f;
                w_TS.setR1_plus( controlVal*ratio);
                w_TS.setR1_minus( controlVal*(1.0f - ratio));
                break;
            }
            case (int)Control::Mid: 
            {
                float controlVal = getParam((int)Param::RMid);
                auto ratio = (100.0f-value)/100.0f; 
                w_TS.setR3_plus( controlVal*ratio);
                w_TS.setR3_minus( controlVal*(1.0f - ratio));
                break;
            }
            default: jassertfalse; break;
        }
    }
    void updateMonitors() override{
        // Voltages
        float x;

        // auto& VDCk1 = monitors[(int)Monitoring::VDCk1];
        // x = getVk1();
        // VDCk1 = lowPass(x, VDCk1);
        // auto& VDCp1 = monitors[(int)Monitoring::VDCp1];
        // x = getVp1();
        // VDCp1 = lowPass(x, VDCp1);

        // auto& VDCk2 = monitors[(int)Monitoring::VDCk2];
        // x = getVk2();
        // VDCk2 = lowPass(x, VDCk2);
        // auto& VDCp2 = monitors[(int)Monitoring::VDCp2];
        // x = getVp2();
        // VDCp2 = lowPass(x, VDCp2);

        // auto& VDCk3 = monitors[(int)Monitoring::VDCk3];
        // x = getVk3();
        // VDCk3 = lowPass(x, VDCk3);
        // auto& VDCp3 = monitors[(int)Monitoring::VDCp3];
        // x = getVp3();
        // VDCp3 = lowPass(x, VDCp3);

        // auto& VDCk4 = monitors[(int)Monitoring::VDCk4];
        // x = getVk4();
        // VDCk4 = lowPass(x, VDCk4);
        // auto& VDCp4 = monitors[(int)Monitoring::VDCp4];
        // x = getVp4();
        // VDCp4 = lowPass(x, VDCp4);

        // auto& VDCk5 = monitors[(int)Monitoring::VDCk5];
        // x = getVk5();
        // VDCk5 = lowPass(x, VDCk5);
        // auto& VDCp5 = monitors[(int)Monitoring::VDCp5];
        // x = getVp5();
        // VDCp5 = lowPass(x, VDCp5);

        // // CURRENT 
        // auto& ik1 = monitors[(int)Monitoring::Ik1];
        // ik1 = lowPass(getIk1(), ik1);
        // auto& ik2 = monitors[(int)Monitoring::Ik2];
        // ik2 = lowPass(getIk2(), ik2);
        // auto& ik3 = monitors[(int)Monitoring::Ik3];
        // ik3 = lowPass(getIk3(), ik3);
        // auto& ik4 = monitors[(int)Monitoring::Ik4];
        // ik4 = lowPass(getIk4(), ik4);
        // auto& ik5 = monitors[(int)Monitoring::Ik5];
        // ik5 = lowPass(getIk5(), ik5);
    }


    void prepare(float sr) override {
        w_Ck1.prepare (sr);
        w_Cp1.prepare (sr);
        w_Ca1.prepare (sr);
        w_Cb1.prepare (sr);
        w_Cbright.prepare(sr);
        w_Cg2.prepare(sr);
        w_Ck2.prepare(sr);
        w_Cp2.prepare(sr);
        w_Cp3.prepare(sr);
        w_Ck4.prepare(sr);
        w_TS.prepare(sr);
        w_C5.prepare(sr);
    }
    void reset() override {
        w_Ck1.reset();
        w_Cp1.reset();
        w_Ca1.reset();
        w_Cb1.reset();
        w_Cbright.reset();
        w_Cg2.reset();
        w_Ck2.reset();
        w_Cp2.reset();
        w_Cp3.reset();
        w_Ck4.reset();
        w_TS.reset();
        w_C5.reset();
    }

    float processSample(float x) override { 
        // V1
        w_Vin.setVoltage (x);
        w_E1_Rp1.setVoltage(getParam((int)Param::E1));
        w_V1.compute();
        auto V1_out = voltage<float> (w_RVol_minus);

        //V2
        w_V_Rg2.setVoltage (V1_out);
        w_E2_Rp2.setVoltage(getParam((int)Param::E2));
        w_V2.compute();
        auto V2_out = voltage<float> (w_Rb2);

        //V3
        w_V_R3.setVoltage (V2_out);
        w_E3_Rp3.setVoltage(getParam((int)Param::E3));
        w_V3.compute();
        auto V3_out = voltage<float> (w_Ra3);

        //V4
        w_V_Rg4.setVoltage (V3_out);
        w_E4_Rp4.setVoltage(getParam((int)Param::E4));
        w_V4.compute();
        auto V4_out = voltage<float> (w_E4_Rp4);

        //V5
        w_V_R5.setVoltage (V4_out);
        w_E5.setVoltage(getParam((int)Param::E5));
        w_V5.compute();
        auto V5_out = voltage<float> (w_Rk5);

        updateMonitors();
        return voltage<float>(w_RMas_minus)* outputGain;
    }

    // Accessor methods for monitoring internal WDF variables
    // float getVk1() const { return -w_V1.getVgkAcc(); }
    // float getVp1() const { return w_V1.getVpkAcc() - w_V1.getVgkAcc(); }
    float getVk1() const { return voltage<float>(w_Rk1); }
    float getVp1() const { return voltage<float>(w_E1_Rp1); }
    float getVk2() const { return voltage<float>(w_Rk2); }
    float getVp2() const { return voltage<float>(w_E2_Rp2); }
    float getVk3() const { return voltage<float>(w_Rk3); }
    float getVp3() const { return voltage<float>(w_E3_Rp3); }
    float getVk4() const { return voltage<float>(w_Rk4); }
    float getVp4() const { return voltage<float>(w_E4_Rp4); }
    float getVk5() const { return voltage<float>(w_Rk5); }
    float getVp5() const { return voltage<float>(w_E5); }

    float getIk1() const { return current<float>(w_Rk1); }
    float getIk2() const { return current<float>(w_Rk2); }
    float getIk3() const { return current<float>(w_Rk3); }
    float getIk4() const { return current<float>(w_Rk4); }
    float getIk5() const { return current<float>(w_Rk5); }

private: 

    float outputGain = 2e-3f;


    // ==================================================================================================== 
    // =  First stage 
    // ==================================================================================================== 

    // Grid Circuit 
    ResistorT<float> w_Rg1 { 0.0f };
    ResistiveVoltageSourceT<float> w_Vin { 0.0f };
    PolarityInverterT<float, decltype (w_Vin)> w_PI_i1 { w_Vin };
    WDFParallelT<float, decltype (w_Rg1), decltype (w_PI_i1)> w_PJ_i1 { w_Rg1, w_PI_i1 };
    PolarityInverterT<float, decltype (w_PJ_i1)> w_PI_g1 { w_PJ_i1 };

    // Cathode Circuit
    ResistorT<float> w_Rk1 { 0.0f };
    CapacitorT<float> w_Ck1 { 0.0f };
    WDFParallelT<float, decltype (w_Rk1), decltype (w_Ck1)> w_PJ_k1 { w_Rk1, w_Ck1 };

    //Volume pot
    CapacitorT<float> w_Cbright { 0.0f };
    ResistorT<float> w_RVol_plus { 0.0f };
    ResistorT<float> w_RVol_minus { 0.0f };
    WDFParallelT<float, decltype (w_Cbright), decltype (w_RVol_plus)> w_PJ_vol { w_Cbright, w_RVol_plus};
    WDFSeriesT<float, decltype (w_RVol_minus), decltype (w_PJ_vol)> w_SJ_vol { w_RVol_minus, w_PJ_vol };

    //Parallel stack
    CapacitorT<float> w_Ca1 { 0.0f };
    CapacitorT<float> w_Cb1 { 0.0f };
    ResistorT<float> w_Ra1 { 0.0f };
    ResistorT<float> w_Rb1 { 0.0f };
    ResistorT<float> w_Rc1 { 0.0f };
    WDFParallelT<float, decltype (w_Ca1), decltype (w_Ra1)> w_PJ_stack1 { w_Ca1, w_Ra1};
    WDFParallelT<float, decltype (w_Cb1), decltype (w_PJ_stack1)> w_PJ_stack2 { w_Cb1, w_PJ_stack1};
    WDFParallelT<float, decltype (w_Rb1), decltype (w_PJ_stack2)> w_PJ_stack3 { w_Rb1, w_PJ_stack2};
    WDFSeriesT<float, decltype (w_SJ_vol), decltype (w_PJ_stack3)> w_SJ_stack1 { w_SJ_vol, w_PJ_stack3 };
    WDFParallelT<float, decltype (w_Rc1), decltype (w_SJ_stack1)> w_PJ_stack4 { w_Rc1, w_SJ_stack1};

    // Plate circuit
    CapacitorT<float> w_Cp1 { 0.0f };
    ResistiveVoltageSourceT<float> w_E1_Rp1 { 0.0f };
    WDFSeriesT<float, decltype (w_Cp1), decltype (w_PJ_stack4)> w_SJ_stack2 { w_Cp1, w_PJ_stack4 };
    PolarityInverterT<float, decltype (w_SJ_stack2)> w_PI_p1 { w_SJ_stack2 };
    WDFParallelT<float, decltype (w_E1_Rp1), decltype (w_PI_p1)> w_PJ_p1 { w_E1_Rp1, w_PI_p1};

    //V1
    TriodeQuadricWDF<float, decltype(w_PI_g1), decltype(w_PJ_k1), decltype(w_PJ_p1)> w_V1{
            w_PI_g1, w_PJ_k1, w_PJ_p1
    };

    // ==================================================================================================== 
    // =  Second stage 
    // ==================================================================================================== 

    // Grid Circuit 
    ResistiveVoltageSourceT<float> w_V_Rg2 { 0.0f };
    CapacitorT<float> w_Cg2 { 0.0f };
    PolarityInverterT<float, decltype (w_V_Rg2)> w_PI_i2 { w_V_Rg2 };
    WDFParallelT<float, decltype (w_Cg2), decltype (w_PI_i2)> w_PJ_i2 { w_Cg2, w_PI_i2 };
    PolarityInverterT<float, decltype (w_PJ_i2)> w_PI_g2 { w_PJ_i2 };

    // Cathode Circuit    
    ResistorT<float> w_Rk2 { 0.0f };
    CapacitorT<float> w_Ck2 { 0.0f };
    WDFParallelT<float, decltype (w_Rk2), decltype (w_Ck2)> w_PJ_k2 { w_Rk2, w_Ck2 };

    // Plate Circuit
    ResistiveVoltageSourceT<float> w_E2_Rp2 { 0.0f };
    CapacitorT<float> w_Cp2 { 0.0f };
    ResistorT<float> w_Ra2 { 0.0f };
    ResistorT<float> w_Rb2 { 0.0f };
    WDFSeriesT<float, decltype (w_Ra2), decltype (w_Rb2)> w_SJ_r2 { w_Ra2, w_Rb2 };
    WDFSeriesT<float, decltype (w_Cp2), decltype (w_SJ_r2)> w_SJ_p2 { w_Cp2, w_SJ_r2 };
    PolarityInverterT<float, decltype (w_SJ_p2)> w_PI_p2 { w_SJ_p2 };
    WDFParallelT<float, decltype (w_E2_Rp2), decltype (w_PI_p2)> w_PJ_p2 { w_E2_Rp2, w_PI_p2};

    TriodeQuadricWDF<float, decltype(w_V_Rg2), decltype(w_PJ_k2), decltype(w_PJ_p2)> w_V2{
            w_V_Rg2, w_PJ_k2, w_PJ_p2
    };

    // ==================================================================================================== 
    // =  Third stage 
    // ==================================================================================================== 
    // Grid Circuit 
    ResistiveVoltageSourceT<float> w_V_R3 { 0.0f };

    // Cathode Circuit    
    ResistorT<float> w_Rk3 { 0.0f };
    // Plate Circuit
    ResistiveVoltageSourceT<float> w_E3_Rp3 { 0.0f };
    CapacitorT<float> w_Cp3 { 0.0f };
    ResistorT<float> w_Ra3 { 0.0f };
    WDFSeriesT<float, decltype (w_Cp3), decltype (w_Ra3)> w_SJ_p3 { w_Cp3, w_Ra3 };
    PolarityInverterT<float, decltype (w_SJ_p3)> w_PI_p3 { w_SJ_p3 };
    WDFParallelT<float, decltype (w_E3_Rp3), decltype (w_PI_p3)> w_PJ_p3 { w_E3_Rp3, w_PI_p3};

    TriodeQuadricWDF<float, decltype(w_V_R3), decltype(w_Rk3), decltype(w_PJ_p3)> w_V3{
            w_V_R3, w_Rk3, w_PJ_p3
    };

    // ==================================================================================================== 
    // =  Fourth stage 
    // ==================================================================================================== 

    // Grid Circuit 
    ResistiveVoltageSourceT<float> w_V_Rg4 { 0.0f };

    // Cathode Circuit    
    ResistorT<float> w_Rk4 { 0.0f };
    CapacitorT<float> w_Ck4 { 0.0f };
    WDFParallelT<float, decltype (w_Rk4), decltype (w_Ck4)> w_PJ_k4 { w_Rk4, w_Ck4 };

    // Plate Circuit
    ResistiveVoltageSourceT<float> w_E4_Rp4 { 0.0f };

    TriodeQuadricWDF<float, decltype(w_V_Rg4), decltype(w_PJ_k4), decltype(w_E4_Rp4)> w_V4{
            w_V_Rg4, w_PJ_k4, w_E4_Rp4
    }; 
    // ==================================================================================================== 
    // =  Fifth stage 
    // ==================================================================================================== 

    // Grid Circuit 
    ResistiveVoltageSourceT<float> w_V_R5 { 0.0f };

    // Cathode Circuit    
    ResistorT<float> w_RMas_plus { 0.0f };
    ResistorT<float> w_RMas_minus { 0.0f };
    ResistorT<float> w_R5 { 0.0f };
    CapacitorT<float> w_C5 { 0.0f };
    ResistorT<float> w_RPres { 0.0f };
    WDFSeriesT<float, decltype (w_C5), decltype (w_RPres)> w_SJ_pres1 { w_C5, w_RPres };
    WDFSeriesT<float, decltype (w_R5), decltype (w_SJ_pres1)> w_SJ_pres2 { w_R5, w_SJ_pres1 };
    WDFSeriesT<float, decltype (w_RMas_plus), decltype (w_RMas_minus)> w_SJ_mas { w_RMas_plus, w_RMas_minus };
    WDFParallelT<float, decltype (w_SJ_mas), decltype (w_SJ_pres2)> w_PJ_out{ w_SJ_mas, w_SJ_pres2};
    ResistorT<float> w_Rk5 { 0.0f };
    ToneStack<float, decltype(w_PJ_out)> w_TS {w_PJ_out, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };
    WDFParallelT<float, decltype (w_Rk5), decltype (w_TS)> w_PJ_k5 { w_Rk5, w_TS};

    // Plate Circuit
    ResistiveVoltageSourceT<float> w_E5 { 0.0f };

    TriodeQuadricWDF<float, decltype(w_V_R5), decltype(w_PJ_k5), decltype(w_E5)> w_V5{
            w_V_R5, w_PJ_k5, w_E5
    }; 
};