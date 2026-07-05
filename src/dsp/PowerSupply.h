#pragma once

#include <chowdsp_wdf/chowdsp_wdf.h>

using namespace chowdsp::wdft;


template <typename T>
class PowerSupplyT : public BaseWDF
{
public:
    public:
        explicit PowerSupplyT (T R, T C) :
        w_VR(R), w_C(C), w_ESR(T(10.0f)), w_SC(w_C, w_ESR), w_P(w_SC, w_VR) 
        {
            calcImpedance();
        }

        /** Prepares the capacitor to operate at a new sample rate */
        void prepare (T sampleRate)
        {
            w_C.prepare(sampleRate);
        }

        /** Resets the capacitor state */
        void reset()
        {
            w_C.reset();
        }

        /** Sets the capacitance value of the WDF capacitor, in Farads. */
        void setCapacitanceValue (T newC)
        {
            w_C.setCapacitanceValue(newC);
        }

        /** Sets the resistance value of the series resistor, in Ohms. */
        void setResistanceValue (T newR)
        {
            w_VR.setResistanceValue(newR);
        }

        /** Sets the voltage of the voltage source, in Volts */
        void setVoltage (T newV) { 
            w_VR.setVoltage(newV);
        }


        /** Computes the impedance 
         */
        inline void calcImpedance() override
        {
            w_P.calcImpedance();
            wdf.R = w_P.wdf.R;
            wdf.G = w_P.wdf.G;
        }

        /** Accepts an incident wave . */
        inline void incident (T x) noexcept
        {
            w_P.incident (x);
            wdf.a = x;
        }

        /** Propogates a reflected wave. */
        inline T reflected() noexcept
        {
            wdf.b = w_P.reflected();
            return wdf.b;
        }

        WDFMembers<T> wdf;

    private:
        ResistiveVoltageSourceT<T> w_VR;
        CapacitorT<T> w_C;
        ResistorT<T> w_ESR;
        WDFSeriesT<T, decltype(w_C), decltype(w_ESR)> w_SC;
        WDFParallelT<T, decltype(w_SC), decltype(w_VR)> w_P;
};