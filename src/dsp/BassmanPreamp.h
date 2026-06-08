#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <chowdsp_wdf/chowdsp_wdf.h>
#include "dsp/TriodeWDF.h"
#include "dsp/TriodeQuadricWDF.h"
#include <dsp/Circuit.h>
#include <dsp/BassmanToneStack.h>

using namespace chowdsp::wdft;

template <typename T>
class BassmanPreampCircuitT : public Circuit<T>
{
public:
    BassmanPreampCircuitT(): Circuit<T>()
    {
        params.resize((int)Param::Count, 0.0f);
        controls.resize((int)Control::Count, 0.0f);
        monitors.resize((int)Monitoring::Count, 0.0f);

        setDefaultParam();
        setDefaultControl();
    };
    using Circuit<T>::params;
    using Circuit<T>::controls;
    using Circuit<T>::monitors;
    using Circuit<T>::getParam;

    enum class Monitoring : int 
    {
        Vk, Vp, Ip,
        Count 
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
        setParam((int)Param::Ri, T(1.0e6f  ));
        setParam((int)Param::Rg, T(20.0e3f ));
        setParam((int)Param::Ci, T(100e-9f ));
        setParam((int)Param::Rk, T(1.0e3f  ));
        setParam((int)Param::Ck, T(10e-6f  ));
        setParam((int)Param::E,  T(250.0f  ));
        setParam((int)Param::Rp, T(100.0e3f));
        setParam((int)Param::C1, T(0.25e-9 ));      
        setParam((int)Param::C2, T(22.0e-9 ));      
        setParam((int)Param::C3, T(22.0e-9 ));      
        setParam((int)Param::R4, T(56.0e3  ));      
        setParam((int)Param::RBass,   T(250e3f));    
        setParam((int)Param::RMid,    T(10e3f ));     
        setParam((int)Param::RTreble, T(250e3f)); 


        w_Triode.setTubeLabParameters(
            T(1.014e-5f), T(5.498e-8f), T(1.076e-5f),
            getParam((int)Param::Rp),
            getParam((int)Param::Rk),
            getParam((int)Param::E)
        );
    }
    void setDefaultControl () 
    { 
        setControl((int)Control::Bass, T(50.0f));    
        setControl((int)Control::Mid, T(50.0f));     
        setControl((int)Control::Treble, T(50.0f)); 
    }
    void setParam (const int index, T value) override
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
    void setControl (const int index, T value) override
    {
        controls.at(index) = value;
        T controlVal = getParam(index);
        auto ratio = value/T(100.0f);

        switch (index)
        {
            case (int)Control::Bass: 
            {
                w_bts.setR2(controlVal*(ratio));
                break;
            }
            case (int)Control::Treble: 
            {
                w_bts.setR1_plus( controlVal*(T(1.0f) - ratio));
                w_bts.setR1_minus( controlVal*ratio);
                break;
            }
            case (int)Control::Mid: 
            {
                w_bts.setR3_plus( controlVal*(T(1.0f) - ratio));
                w_bts.setR3_minus( controlVal*ratio);
                break;
            }
            default: jassertfalse; break;
        }
    }

    void prepare(T sr) override {
        w_Vi.prepare (sr);
        w_Ck.prepare (sr);
        w_bts.prepare(sr);

        // float duration = 0.1f;
        // for (int i = 0; i < (int) sr *duration; ++i)
        // {
        //     auto y = processSample(zero);
        // }
    }
    void reset() override {
        w_bts.reset();
        w_Vi.reset();
        w_Ck.reset();
    }

    T processSample(T x) override { 
        w_Vi.setVoltage (x);
        w_E_Rp.setVoltage(getParam((int)Param::E));
        w_Triode.compute();
        updateMonitors();
        return w_bts.getVoltage();
    }
    void updateMonitors() override{

        monitors.at((int) Monitoring::Vk)= getCathodeVoltage(); 
        monitors.at((int) Monitoring::Vp)= getPlateVoltage(); 
        monitors.at((int) Monitoring::Ip)= getPlateCurrent(); 
    }

    // Accessor methods for monitoring internal WDF variables
    T getCathodeVoltage() const { return voltage<T> (w_Rk); }
    T getPlateVoltage() const { return voltage<T> (w_E_Rp); }
    T getGridVoltage() const { return voltage<T> (w_Ri); }
    T getPlateCurrent() const { return (getParam((int)Param::E)-getPlateVoltage())/getParam((int)Param::Rp) ; }
    
private: 
    const T zero = static_cast<T>(0.0f);
    const T one = static_cast<T>(1.0f);

    // Cathode Circuit (connect triode to PJk)
    ResistorT<T> w_Rk { zero };
    CapacitorT<T> w_Ck { zero };
    WDFParallelT<T, decltype (w_Rk), decltype (w_Ck)> w_PJk { w_Rk, w_Ck };

    // Plate Circuit (connect triode to PJp)
    BassmanToneStack<T> w_bts {};
    ResistiveVoltageSourceT<T> w_E_Rp { zero };
    // PolarityInverterT<T, decltype (w_bts)> w_PIp { w_bts };
    WDFParallelT<T, decltype (w_E_Rp), decltype (w_bts)> w_PJp { w_E_Rp, w_bts};

    // Grid Circuit (connect triode to PIg)
    ResistorT<T> w_Rg { zero };
    ResistorT<T> w_Ri { zero };
    CapacitiveVoltageSourceT<T> w_Vi { zero };
    PolarityInverterT<T, decltype (w_Vi)> w_PIi { w_Vi };
    WDFParallelT<T, decltype (w_Ri), decltype (w_PIi)> w_PJi { w_Ri, w_PIi };
    WDFSeriesT<T, decltype (w_Rg), decltype (w_PJi)> w_SJg { w_Rg, w_PJi };
    PolarityInverterT<T, decltype (w_SJg)> w_PIg { w_SJg };

    // Triode WDF
    // TriodeWDF<T, decltype(w_PIg), decltype(w_PJk), decltype(w_PJp)> w_Triode{
    //         w_PIg, w_PJk, w_PJp, 0.0f, 0.0f
    // };
    TriodeQuadricWDF<T, decltype(w_PIg), decltype(w_PJk), decltype(w_PJp)> w_Triode{
            w_PIg, w_PJk, w_PJp, zero,zero,zero,zero,zero,zero
    };
};