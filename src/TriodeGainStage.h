#ifndef TRIODEGAINSTAGE_H_INCLUDED
#define TRIODEGAINSTAGE_H_INCLUDED

#include <juce_gui_basics/juce_gui_basics.h>
#include <array>
#include <cmath>

#include <chowdsp_wdf/chowdsp_wdf.h>
#include "TriodeWDF.h"


using namespace chowdsp::wdft;

/**
 * Wave Digital Filter implementation of a vacuum tube triode gain stage.
 * Based on the Cardarilli model from:
 * "New Family of Wave-Digital Triode Models" by D'Angelo et al.
 *
 * Circuit topology: common-cathode triode stage with:
 * - Grid subcircuit: input coupling (Vi, Ci), grid resistor (Rg), input impedance (Ri)
 * - Cathode subcircuit: cathode resistor (Rk) in parallel with cathode capacitor (Ck)
 * - Plate subcircuit: plate resistor (Rp) to supply (E), output stage (Ro, Co)
 *
 * The triode is modeled as a 3-port nonlinear element connecting these subnetworks.
 * The implementation uses precomputed WDF coefficients for efficiency.
 */
class TriodeGainStage
{
    public:
        TriodeGainStage() = default;

        void prepare (double sampleRate)
        {
            w_Ci.prepare ((float) sampleRate);
            w_Ck.prepare ((float) sampleRate);
            w_Co.prepare ((float) sampleRate);

            float duration = 1.0f;
            for (int i = 0; i < (int) sampleRate *duration; ++i)
            {
                auto y = processSample(0.0f);
            }
        }

        void reset()
        {
            w_Ci.reset();
            w_Ck.reset();
            w_Co.reset();
        }

        inline float processSample (float x)
        {
            w_Vi.setVoltage (x);
            w_E_Rp.setVoltage(E);

            w_Triode.compute();
            auto y = voltage<float> (w_Ro);

            return y;
        }

        // Accessor methods for monitoring internal WDF variables
        float getGridVoltage() const { return voltage<float> (w_Ri); }
        float getCathodeVoltage() const { return voltage<float> (w_Rk); }
        float getPlateVoltage() const { return voltage<float> (w_Ro); }
        float getTriodeVg() const { return w_Triode.getVg(); }
        float getTriodeVk() const { return w_Triode.getVk(); }
        float getTriodeVp() const { return w_Triode.getVp(); }
        float getVgIters() const { return w_Triode.getVgIters(); }
        float getVkIters() const { return w_Triode.getVkIters(); }
        float getPPIters() const { return w_Triode.getPPIters(); }
        float getGridCurrent() const { return w_Triode.getAk(); } // Approximation
        float getCathodeCurrent() const { return -w_Triode.getAk(); } // Approximation
        float getPlateCurrent() const { return (w_Triode.getAp() - w_Triode.getVp()) / w_Triode.getR0p(); }

    private:

    float E = 250.0f;
    float Rp =  100.0e3f ;
    float Rk =  1.0e3f ;
    float Ck =  10e-6f ;
    float Co = 10e-9f;
    float Ro = 1.0e6f ;
    float Ri = 1.0e6f ;
    float Rg = 20.0e3f ;
    float Ci = 100e-9f;
    float Rsi = 0.1f;


    // Cathode Circuit (connect to PJk)
    ResistorT<float> w_Rk { Rk };
    CapacitorT<float> w_Ck { Ck};

    WDFParallelT<float, decltype (w_Rk), decltype (w_Ck)> w_PJk { w_Rk, w_Ck };

    // Plate Circuit (connect to PJp)
    ResistiveVoltageSourceT<float> w_E_Rp {Rp};
    CapacitorT<float> w_Co { Co};
    ResistorT<float> w_Ro { Ro };

    WDFSeriesT<float, decltype (w_Co), decltype (w_Ro)> w_SJo { w_Co, w_Ro };
    PolarityInverterT<float, decltype (w_SJo)> w_SJp { w_SJo };
    WDFParallelT<float, decltype (w_E_Rp), decltype (w_SJp)> w_PJp { w_E_Rp, w_SJp};


    // Grid Circuit (connect to PIg)
    ResistorT<float> w_Rg { Rg };
    ResistorT<float> w_Ri { Ri };
    CapacitorT<float> w_Ci { Ci};
    ResistiveVoltageSourceT<float> w_Vi { Rsi };

    WDFSeriesT<float, decltype (w_Ci), decltype (w_Vi)> w_SJi { w_Ci, w_Vi };
    PolarityInverterT<float, decltype (w_SJi)> w_PIi { w_SJi };
    WDFParallelT<float, decltype (w_Ri), decltype (w_PIi)> w_PJg { w_Ri, w_PIi };
    WDFSeriesT<float, decltype (w_Rg), decltype (w_PJg)> w_SJg { w_Rg, w_PJg };
    PolarityInverterT<float, decltype (w_SJg)> w_PIg { w_SJg };

    // Triode WDF
    float Vk_init = 0.5 * Rk / (Rp + Rk);
    float Vp_init = 0.5 * (E + Rk / (Rp + Rk));
    TriodeWDF<float, decltype (w_PIg), decltype (w_PJk), decltype (w_PJp)> 
        w_Triode {w_PIg, w_PJk, w_PJp, Vk_init, Vp_init};

};
#endif TRIODEGAINSTAGE_H_INCLUDED