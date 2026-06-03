#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <chowdsp_wdf/chowdsp_wdf.h>
#include "dsp/TriodeWDF.h"
#include "dsp/TriodeQuadricWDF.h"
#include <dsp/Circuit.h>
#include <dsp/BassmanToneStack.h>

using namespace chowdsp::wdft;

class BassmanPreampCircuit : public Circuit
{
public:
    BassmanPreampCircuit(): Circuit()
    {
        params.resize((int)Param::Count, 0.0f);
        controls.resize((int)Control::Count, 0.0f);
        monitors.resize((int)Monitoring::Count, 0.0f);

        setDefaultParam();
        setDefaultControl();
    };

    enum class Monitoring : int 
    {
        Vk, Vp, Count 
    };
    enum class Param : int 
    {
        Triode, Ri ,Rg,Ci,Rk,Ck,E,Rp, // common cathode
        RBass, RMid, RTreble, C1, C2, C3, R4, // tone stack
        Count 
    };
    enum class Control : int 
    {
        Bass, Mid, Treble, Count 
    };
    void setDefaultParam () 
    {
        setParam((int)Param::Ri, 1.0e6f);
        setParam((int)Param::Rg, 20.0e3f);
        setParam((int)Param::Ci, 100e-9f);
        setParam((int)Param::Rk, 1.0e3f);
        setParam((int)Param::Ck, 10e-6f);
        setParam((int)Param::E, 250.0f);
        setParam((int)Param::Rp, 100.0e3f);
        
        setParam((int)Param::C1, 0.25e-9);      
        setParam((int)Param::C2, 22.0e-9);      
        setParam((int)Param::C3, 22.0e-9);      
        setParam((int)Param::R4, 56.0e3);      
        setParam((int)Param::RBass, 250e3f);    
        setParam((int)Param::RMid, 10e3f);     
        setParam((int)Param::RTreble, 250e3f); 


        w_Triode.setTubeLabParameters(
            1.014e-5f, 5.498e-8f, 1.076e-5f,
            getParam((int)Param::Rp),
            getParam((int)Param::Rk),
            getParam((int)Param::E)
        );
    }
    void setDefaultControl () 
    { 
        setControl((int)Control::Bass, 50.0f);    
        setControl((int)Control::Mid, 50.0f);     
        setControl((int)Control::Treble, 50.0f); 
    }
    void setParam (const int index, float value) override
    {
        params.at(index) = value;
        
        switch (index)
        {
            case (int)Param::Ri: w_Ri.setResistanceValue(value); break;
            case (int)Param::Rg: w_Rg.setResistanceValue(value); break;
            case (int)Param::Ci: w_Vi.setCapacitanceValue(value); break;
            case (int)Param::Rk: w_Rk.setResistanceValue(value); break;
            case (int)Param::Ck: w_Ck.setCapacitanceValue(value); break;
            case (int)Param::E:  w_E_Rp.setVoltage(value);  break;
            case (int)Param::Rp: w_E_Rp.setResistanceValue(value); break;

            case (int)Param::Triode: break;//TODO

            case (int)Param::C1:       w_bts.setC1(value); break;
            case (int)Param::C2:       w_bts.setC2(value); break;
            case (int)Param::C3:       w_bts.setC3(value); break;
            case (int)Param::R4:       w_bts.setR4(value); break;
            case (int)Param::RBass:     w_bts.setBass(value); break;
            case (int)Param::RMid:      w_bts.setMid(value); break;
            case (int)Param::RTreble:  w_bts.setTreble(value); break;

            case (int)Param::Count:
            default:
                jassertfalse;
                break;
        }
    }
    void setControl (const int index, float value) override
    {
        controls.at(index) = value;

        float controlVal = getParam(index);
        switch (index)
        {
            case (int)Control::Bass: 
            {
                auto ratio = std::pow((100.0f-value)/100.0f, 3.0f); // audio taper
                w_bts.setR2(controlVal*(1.0f - ratio));
                break;
            }
            case (int)Control::Treble: 
            {
                auto ratio = std::pow((100.0f-value)/100.0f, 3.0f); // audio taper
                w_bts.setR1_plus( controlVal*ratio);
                w_bts.setR1_minus( controlVal*(1.0f - ratio));
                break;
            }
            case (int)Control::Mid: 
            {
                auto ratio = (100.0f-value)/100.0f; 
                w_bts.setR3_plus( controlVal*ratio);
                w_bts.setR3_minus( controlVal*(1.0f - ratio));
                break;
            }
            default: jassertfalse; break;
        }
    }

    void prepare(double sr) override {
        w_Vi.prepare ((float) sr);
        w_Ck.prepare ((float) sr);
        w_bts.prepare((float) sr);

        float duration = 0.1f;
        for (int i = 0; i < (int) sr *duration; ++i)
        {
            auto y = processSample(0.0f);
        }
    }
    void reset() override {
        w_bts.reset();
        w_Vi.reset();
        w_Ck.reset();
    }

    float processSample(float x) override { 
        w_Vi.setVoltage (x);
        w_E_Rp.setVoltage(getParam((int)Param::E));
        w_Triode.compute();
        return w_bts.getVoltage();
    }
    void updateMonitors() override{

        monitors.at((int) Monitoring::Vk)= getCathodeVoltage(); 
        monitors.at((int) Monitoring::Vp)= getPlateVoltage(); 
    }

    // Accessor methods for monitoring internal WDF variables
    float getCathodeVoltage() const { return voltage<float> (w_Rk); }
    float getPlateVoltage() const { return voltage<float> (w_E_Rp); }
    float getGridVoltage() const { return voltage<float> (w_Ri); }
    float getPlateCurrent() const { return (getParam((int)Param::E)-getPlateVoltage())/getParam((int)Param::Rp) ; }
    
private: 

    // Cathode Circuit (connect triode to PJk)
    ResistorT<float> w_Rk { 0.0f };
    CapacitorT<float> w_Ck { 0.0f };
    WDFParallelT<float, decltype (w_Rk), decltype (w_Ck)> w_PJk { w_Rk, w_Ck };

    // Plate Circuit (connect triode to PJp)
    BassmanToneStack<float> w_bts {};
    ResistiveVoltageSourceT<float> w_E_Rp { 0.0f };
    PolarityInverterT<float, decltype (w_bts)> w_PIp { w_bts };
    WDFParallelT<float, decltype (w_E_Rp), decltype (w_PIp)> w_PJp { w_E_Rp, w_PIp};

    // Grid Circuit (connect triode to PIg)
    ResistorT<float> w_Rg { 0.0f };
    ResistorT<float> w_Ri { 0.0f };
    CapacitiveVoltageSourceT<float> w_Vi { 0.0f };
    PolarityInverterT<float, decltype (w_Vi)> w_PIi { w_Vi };
    WDFParallelT<float, decltype (w_Ri), decltype (w_PIi)> w_PJi { w_Ri, w_PIi };
    WDFSeriesT<float, decltype (w_Rg), decltype (w_PJi)> w_SJg { w_Rg, w_PJi };
    PolarityInverterT<float, decltype (w_SJg)> w_PIg { w_SJg };

    // Triode WDF
    // TriodeWDF<float, decltype(w_PIg), decltype(w_PJk), decltype(w_PJp)> w_Triode{
    //         w_PIg, w_PJk, w_PJp, 0.0f, 0.0f
    // };
    TriodeQuadricWDF<float, decltype(w_PIg), decltype(w_PJk), decltype(w_PJp)> w_Triode{
            w_PIg, w_PJk, w_PJp, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f
    };
};