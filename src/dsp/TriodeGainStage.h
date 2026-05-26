#ifndef TRIODEGAINSTAGE_H_INCLUDED
#define TRIODEGAINSTAGE_H_INCLUDED

#include <juce_gui_basics/juce_gui_basics.h>
#include <array>
#include <cmath>

#include <chowdsp_wdf/chowdsp_wdf.h>
#include "dsp/TriodeWDF.h"


using namespace chowdsp::wdft;

class TriodeGainStage
{
    public:
        TriodeGainStage() = default;

        void prepare (double sampleRate)
        {
            w_Vi.prepare ((float) sampleRate);
            w_Ck.prepare ((float) sampleRate);
            w_Co.prepare ((float) sampleRate);
            w_Cp.prepare ((float) sampleRate);
            w_Triode.prepare ((float) sampleRate);

            float duration = 0.1f;
            for (int i = 0; i < (int) sampleRate *duration; ++i)
            {
                auto y = processSample(0.0f);
            }
        }

        void reset()
        {
            w_Vi.reset();
            w_Ck.reset();
            w_Co.reset();
            w_Cp.reset();
            // w_Cg.reset();
        }

        inline float processSample (float x)
        {
            w_Vi.setVoltage (x);
            w_E_Rp.setVoltage(E);

            w_Triode.compute();
            auto y = voltage<float> (w_Ro2);

            return y* gainOutput;
        }

        // Runtime setters — updates the WDF node and re-propagates impedance
        void setRi (float v) { Ri = v;  w_Ri.setResistanceValue(Ri); }
        void setRg (float v) { Rg = v;  w_Rg.setResistanceValue(Rg); }
        void setCi (float v) { Ci = v;  w_Vi.setCapacitanceValue(Ci); }
        void setRk (float v) { Rk = v;  w_Rk.setResistanceValue(Rk); }
        void setCk (float v) { Ck = v;  w_Ck.setCapacitanceValue(Ck); }
        void setE  (float v) { E  = v;   w_E_Rp.setVoltage(E); }
        void setRp (float v) { Rp = v;  w_E_Rp.setResistanceValue(Rp); }
        void setCo (float v) { Co = v;  w_Co.setCapacitanceValue(Co); }
        void setRo1(float v) { Ro1 =v; w_Ro1.setResistanceValue(Ro1); }
        void setRo2(float v) { Ro2 =v; w_Ro2.setResistanceValue(Ro2); }
        void setCp (float v) { Cp = v;  w_Cp.setCapacitanceValue(Cp); }

        void setParam (const juce::String& paramName, float value)
        {
            if      (paramName == "Ri") setRi(value);
            else if (paramName == "Rg") setRg(value);
            else if (paramName == "Ci") setCi(value);
            else if (paramName == "Rk") setRk(value);
            else if (paramName == "Ck") setCk(value);
            else if (paramName == "B+") setE(value);
            else if (paramName == "Rp") setRp(value);
            else if (paramName == "Co") setCo(value);
            else if (paramName == "Ro1") setRo1(value);
            else if (paramName == "Ro2") setRo2(value);
            else if (paramName == "Cp") setCp(value);
            else
                jassertfalse;

            std::cout << paramName << ": " << value << std::endl;
        }

        // Accessor methods for monitoring internal WDF variables
        float getCathodeVoltage() const { return -w_Triode.getVgkAcc(); }
        float getPlateVoltage() const { return w_Triode.getVpkAcc() - w_Triode.getVgkAcc(); }

        // float getGridVoltage() const { return voltage<float> (w_Ri); }
        // float getTriodeVg() const { return w_Triode.getVg(); }
        // float getTriodeVk() const { return w_Triode.getVk(); }
        // float getTriodeVp() const { return w_Triode.getVp(); }
        // float getVgIters() const { return w_Triode.getVgIters(); }
        // float getVkIters() const { return w_Triode.getVkIters(); }
        // float getPPIters() const { return w_Triode.getPPIters(); }
        // float getGridCurrent() const { return w_Triode.getAk(); } // Approximation
        // float getCathodeCurrent() const { return -w_Triode.getAk(); } // Approximation
        // float getPlateCurrent() const { return (w_Triode.getAp() - w_Triode.getVp()) / w_Triode.getR0p(); }

        // float getGain() const noexcept { return w_Triode.getTransconductance() * Ro ; }
        // float getVgkAcc() const noexcept { return w_Triode.getVgkAcc(); }
        // float getVpkAcc() const noexcept { return w_Triode.getVpkAcc(); }

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
    float Ro1 = 0.5e6f ;
    float Ro2 = 0.5e6f ;
    float Cp = 100e-12f;

    float gainOutput = 1.0f/18.8f;


    // Cathode Circuit (connect triode to PJk)
    ResistorT<float> w_Rk { Rk };
    CapacitorT<float> w_Ck { Ck};

    WDFParallelT<float, decltype (w_Rk), decltype (w_Ck)> w_PJk { w_Rk, w_Ck };

    // Plate Circuit (connect triode to PJp)
    ResistiveVoltageSourceT<float> w_E_Rp {Rp};
    CapacitorT<float> w_Co { Co};
    CapacitorT<float> w_Cp { Cp};
    ResistorT<float> w_Ro1 { Ro1 };
    ResistorT<float> w_Ro2 { Ro2 };

    WDFSeriesT<float, decltype (w_Ro1), decltype (w_Ro2)> w_SJo2 { w_Ro1, w_Ro2 };
    WDFSeriesT<float, decltype (w_Co), decltype (w_SJo2)> w_SJo { w_Co, w_SJo2 };
    PolarityInverterT<float, decltype (w_SJo)> w_PIp { w_SJo };
    WDFParallelT<float, decltype (w_Cp), decltype (w_PIp)> w_PJp2 { w_Cp, w_PIp};
    WDFParallelT<float, decltype (w_E_Rp), decltype (w_PJp2)> w_PJp { w_E_Rp, w_PJp2};
    // WDFParallelT<float, decltype (w_E_Rp), decltype (w_PIp)> w_PJp { w_E_Rp, w_PIp};


    // Grid Circuit (connect triode to PIg)
    ResistorT<float> w_Rg { Rg };
    ResistorT<float> w_Ri { Ri };
    // CapacitorT<float> w_Cg { Cg};
    CapacitiveVoltageSourceT<float> w_Vi { Ci };

    PolarityInverterT<float, decltype (w_Vi)> w_PIi { w_Vi };
    WDFParallelT<float, decltype (w_Ri), decltype (w_PIi)> w_PJi { w_Ri, w_PIi };
    WDFSeriesT<float, decltype (w_Rg), decltype (w_PJi)> w_SJg { w_Rg, w_PJi };
    // WDFParallelT<float, decltype (w_Cg), decltype (w_SJg)> w_PJg {w_Cg, w_SJg };
    // PolarityInverterT<float, decltype (w_PJg)> w_PIg { w_PJg };
    PolarityInverterT<float, decltype (w_SJg)> w_PIg { w_SJg };

    // Triode WDF
    float Vk_init = 0.5 * Rk / (Rp + Rk);
    float Vp_init = 0.5 * (E + Rk / (Rp + Rk));
    TriodeWDF<float, decltype(w_PIg), decltype(w_PJk), decltype(w_PJp)>
        w_Triode{w_PIg, w_PJk, w_PJp, Vk_init, Vp_init};

};
#endif // TRIODEGAINSTAGE_H_INCLUDED