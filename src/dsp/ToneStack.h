#pragma once

#include <chowdsp_wdf/chowdsp_wdf.h>
#include <dsp/Rtype.h>

using namespace chowdsp::wdft;


template <typename T, typename Next>
class ToneStack : public BaseWDF
{
public:
    ToneStack(Next& next, T R1_plus, T R1_minus, T R2, T R3_plus, T R3_minus, T C1, T C2, T C3, T R4):
        w_C1 (C1),
        w_C2 (C2),
        w_C3 (C3),
        w_R1_plus (R1_plus),
        w_R1_minus(R1_minus),
        w_R2 (R2),
        w_R3_plus (R3_plus),
        w_R3_minus(R3_minus),
        w_R4 (R4),
        w_Sb (w_R1_plus, w_C1),
        w_Sg (w_R2, w_R3_plus),
        w_next(next)
    {
        w_next.connectToParent(this);
        calcImpedance();
    };

    inline void prepare (T sampleRate){
        w_C1.prepare( sampleRate);
        w_C2.prepare( sampleRate);
        w_C3.prepare( sampleRate);
        propagateImpedanceChange();
    }
    void reset(){
        w_C1.reset();
        w_C2.reset();
        w_C3.reset();
    }

    inline void calcImpedance() override
    {
        R.calcImpedance();
        wdf.R = R.wdf.R;
        wdf.G = R.wdf.G;
    }
    inline void incident (T x) noexcept
    {
        R.incident (x);
        wdf.a = x;
    }
    inline T reflected() noexcept
    {
        wdf.b = R.reflected();
        return wdf.b;
    }

    void setR1_plus(T v)  { w_R1_plus.setResistanceValue(v); propagateImpedanceChange();}
    void setR1_minus(T v) { w_R1_minus.setResistanceValue(v);propagateImpedanceChange();}
    void setR2(T v)       { w_R2.setResistanceValue(v); propagateImpedanceChange();}
    void setR3_plus(T v)  { w_R3_plus.setResistanceValue(v); propagateImpedanceChange();}
    void setR3_minus(T v) { w_R3_minus.setResistanceValue(v); propagateImpedanceChange();}
    void setC1(T v)       { w_C1.setCapacitanceValue(v); propagateImpedanceChange();}
    void setC2(T v)       { w_C2.setCapacitanceValue(v); propagateImpedanceChange();}
    void setC3(T v)       { w_C3.setCapacitanceValue(v); propagateImpedanceChange();}
    void setR4(T v)       { w_R4.setResistanceValue(v); propagateImpedanceChange();}


    inline T getVoltage() noexcept
    {
        return voltage<T> (w_next) ; 
    }
    WDFMembers<T> wdf;

    // Port B
    CapacitorT<T> w_C1;
    ResistorT<T> w_R1_plus;
    WDFSeriesT<T, decltype (w_R1_plus), decltype (w_C1)> w_Sb;

    // Port C
    ResistorT<T> w_R1_minus;

    // Port D 
    Next& w_next;

    // Port E
    ResistorT<T> w_R4;

    // Port F
    CapacitorT<T> w_C2;

    // Port G
    ResistorT<T> w_R2;
    ResistorT<T> w_R3_plus;
    WDFSeriesT<T, decltype (w_R2), decltype (w_R3_plus)> w_Sg;

    // Port H
    CapacitorT<T> w_C3;

    // Port I
    ResistorT<T> w_R3_minus;

    struct ImpedanceCalc
    {
        inline static const RTBuilderToneStack builder {};

        template <typename RType>
        static T calcImpedance (RType& R)
        {
            const auto [Rb, Rc, Rd, Re, Rf, Rg, Rh, Ri] = R.getPortImpedances();
            T smat[9][9];
            T Ra = builder.buildS(&smat[0][0], Rb, Rc, Rd, Re, Rf, Rg, Rh, Ri);
            R.setSMatrixData ( smat);
            return Ra;
        }
    };

    using RType = RtypeAdaptor<T, 0, ImpedanceCalc, 
        decltype (w_Sb), // B
        decltype (w_R1_minus), // C
        decltype (w_next), // D
        decltype (w_R4), // E
        decltype (w_C2), // F
        decltype (w_Sg), // G
        decltype (w_C3), // H
        decltype (w_R3_minus) // I
    >;
    RType R {w_Sb, w_R1_minus, w_next, w_R4, w_C2, w_Sg, w_C3, w_R3_minus};

};