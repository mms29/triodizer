#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <array>
#include <cmath>

#include <chowdsp_wdf/chowdsp_wdf.h>
#include "dsp/TriodeWDF.h"
#include "dsp/BassmanToneStack.h"
#include "dsp/Circuit.h"


using namespace chowdsp::wdft;

class TriodeGainStage: public Circuit<float>
{
    public:
        TriodeGainStage(): Circuit<float>()
        {
            params.resize((int)Param::Count, 0.0f);
            controls.resize((int)Control::Count, 0.0f);
            monitors.resize((int)Control::Count, 0.0f);
        };
        enum class Monitoring : int { Vg, Vk, Vp, Count };
        enum class Param : int {Triode, Volume, Ri ,Rg,Ci,Rk,Ck,E,Rp,Co,Cp, Ro1, Ro2, Count };
        enum class Control : int {Volume, Count };

        void prepare (float sampleRate) override
        {
            w_Vi.prepare (sampleRate);
            w_Ck.prepare (sampleRate);
            w_Co.prepare (sampleRate);
            // w_Cp.prepare ((float) sampleRate);
            w_Triode.prepare (sampleRate);
            // w_BTS.prepare((float) sampleRate);

            float duration = 0.1f;
            for (int i = 0; i < (int) sampleRate *duration; ++i)
            {
                auto y = processSample(0.0f);
            }
        }

        void reset() override
        {
            w_Vi.reset();
            w_Ck.reset();
            w_Co.reset();
            // w_Cp.reset();
            // w_Cg.reset();
            // w_BTS.reset();
        }

        inline float processSample (float x) override
        {
            w_Vi.setVoltage (x);
            w_E_Rp.setVoltage(E);

            w_Triode.compute();
            auto y = voltage<float> (w_Ro2);
            // auto y = w_BTS.getVoltage();
            
            updateMonitors();
            return y* gainOutput;
        }

        // Runtime setters — updates the WDF node and re-propagates impedance
        void setRi (float v) { Ri = v;  w_Ri.setResistanceValue(Ri); }
        void setRg (float v) { Rg = v;  w_Rg.setResistanceValue(Rg); }
        void setCi (float v) { Ci = v;  w_Vi.setCapacitanceValue(Ci); }
        void setRk (float v) { Rk = v;  w_Rk.setResistanceValue(Rk); }
        void setCk (float v) { Ck = v;  w_Ck.setCapacitanceValue(Ck); }
        void setE  (float v) { E  = v;  w_E_Rp.setVoltage(E); }
        void setRp (float v) { Rp = v;  w_E_Rp.setResistanceValue(Rp); }
        void setTriode (float v) { }
        void setVolume (float v) {
            P1=v; 
            setRo1(v/2.0f);
            setRo2(v/2.0f); 
            setControl((int)Control::Volume, 50.0f ); 
        }
        void setCo (float v) { Co = v;  w_Co.setCapacitanceValue(Co); }
        void setRo1(float v) { Ro1 =v;  w_Ro1.setResistanceValue(Ro1); }
        void setRo2(float v) { Ro2 =v;  w_Ro2.setResistanceValue(Ro2); }
        // void setCp (float v) { Cp = v;  w_Cp.setCapacitanceValue(Cp); }


        void setParam (const int index, float value) override
        {
            switch (index)
            {
                case (int)Param::Ri: setRi(value); break;
                case (int)Param::Rg: setRg(value); break;
                case (int)Param::Ci: setCi(value); break;
                case (int)Param::Rk: setRk(value); break;
                case (int)Param::Ck: setCk(value); break;
                case (int)Param::E:  setE(value); break;
                case (int)Param::Rp: setRp(value); break;
                case (int)Param::Co: setCo(value); break;
                case (int)Param::Ro1: setRo1(value); break;
                case (int)Param::Ro2: setRo2(value); break;
                case (int)Param::Triode: setTriode(value); break;
                case (int)Param::Volume: setVolume(value); break;
                default: jassertfalse; break;
            }
        }
        void setControl (const int index, float value) override
        {
            switch (index)
            {
                case (int)Control::Volume: 
                {
                    auto ratio = (100.0f-value)/100.0f; 
                    setParam((int)Param::Ro1, P1*ratio);
                    setParam((int)Param::Ro2, P1*(1-ratio));
                    break;
                }
                default: jassertfalse; break;
            }
            // else if (paramName == "Co") setCo(value);
            // else if (paramName == "Volume_R1") setRo1(value);
            // else if (paramName == "Volume_R2") setRo2(value);
            // else if (paramName == "Cp") setCp(value);
        }

        void updateMonitors() override{

            monitors.at((int) Monitoring::Vk)= getCathodeVoltage(); 
            monitors.at((int) Monitoring::Vp)= getPlateVoltage(); 
        }

        // Accessor methods for monitoring internal WDF variables
        float getCathodeVoltage() const { return -w_Triode.getVgkAcc(); }
        float getPlateVoltage() const { return w_Triode.getVpkAcc() - w_Triode.getVgkAcc(); }
        float getGridVoltage() const { return voltage<float> (w_Ri); }
        float getPlateCurrent() const { return (E-getPlateVoltage())/Rp ; }

    private:

    
    float Ri = 1.0e6f ;
    float Rg = 20.0e3f ;
    float Ci = 100e-9f;
    // float Cg = 50e-12f;

    float Rk =  1.0e3f ;
    float Ck =  10e-6f ;

    float E = 250.0f;
    float Rp =  100.0e3f ;
    float Co = 100e-9f;
    float P1 = 1.0e6f;
    float Ro1 = P1/2.0f;
    float Ro2 = P1/2.0f;
    // float Cp = 100e-12f;

    float gainOutput = 1.0f/18.8f;


    // Cathode Circuit (connect triode to PJk)
    ResistorT<float> w_Rk { Rk };
    CapacitorT<float> w_Ck { Ck};

    WDFParallelT<float, decltype (w_Rk), decltype (w_Ck)> w_PJk { w_Rk, w_Ck };

    // Plate Circuit (connect triode to PJp)
    ResistiveVoltageSourceT<float> w_E_Rp {Rp};
    // CapacitorT<float> w_Cp { Cp};
    CapacitorT<float> w_Co { Co};
    ResistorT<float> w_Ro1 { Ro1 };
    ResistorT<float> w_Ro2 { Ro2 };

    WDFSeriesT<float, decltype (w_Ro1), decltype (w_Ro2)> w_SJo2 { w_Ro1, w_Ro2 };
    WDFSeriesT<float, decltype (w_Co), decltype (w_SJo2)> w_SJo { w_Co, w_SJo2 };
    // BassmanToneStack<float> w_BTS;
    // PolarityInverterT<float, decltype (w_BTS)> w_PIp { w_BTS };
    PolarityInverterT<float, decltype (w_SJo)> w_PIp { w_SJo };
    // WDFParallelT<float, decltype (w_Cp), decltype (w_PIp)> w_PJp2 { w_Cp, w_PIp};
    // WDFParallelT<float, decltype (w_E_Rp), decltype (w_PJp2)> w_PJp { w_E_Rp, w_PJp2};
    WDFParallelT<float, decltype (w_E_Rp), decltype (w_PIp)> w_PJp { w_E_Rp, w_PIp};


    // Grid Circuit (connect triode to PIg)
    ResistorT<float> w_Rg { Rg };
    ResistorT<float> w_Ri { Ri };
    CapacitiveVoltageSourceT<float> w_Vi { Ci };

    PolarityInverterT<float, decltype (w_Vi)> w_PIi { w_Vi };
    WDFParallelT<float, decltype (w_Ri), decltype (w_PIi)> w_PJi { w_Ri, w_PIi };
    WDFSeriesT<float, decltype (w_Rg), decltype (w_PJi)> w_SJg { w_Rg, w_PJi };
    PolarityInverterT<float, decltype (w_SJg)> w_PIg { w_SJg };

    // Triode WDF
    float Vk_init = 0.5 * Rk / (Rp + Rk);
    float Vp_init = 0.5 * (E + Rk / (Rp + Rk));
    TriodeWDF<float, decltype(w_PIg), decltype(w_PJk), decltype(w_PJp)>
        w_Triode{w_PIg, w_PJk, w_PJp, Vk_init, Vp_init};

};
