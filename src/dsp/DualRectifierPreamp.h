#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <chowdsp_wdf/chowdsp_wdf.h>
#include "dsp/TriodeWDF.h"
#include "dsp/TriodeQuadricWDF.h"
#include <dsp/Circuit.h>


using namespace chowdsp::wdft;

template <typename T>
class DualRectifierToneStack : public BaseWDF
{
public:
    DualRectifierToneStack() {
            calcImpedance();
    };

    inline void prepare (double sampleRate){
        w_C1.prepare((T) sampleRate);
        w_C2.prepare((T) sampleRate);
        w_C3.prepare((T) sampleRate);
        calcImpedance();
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

    inline T getVoltage() noexcept
    {
        // return voltage<float> (w_R1_minus) + voltage<float> (w_R2) + voltage<float> (w_R3_plus)  + voltage<float> (w_R3_minus);
         
    }

    void setR1_plus(float v)  { R1_plus = v; w_R1_plus.setResistanceValue(v); }
    void setR1_minus(float v) { R1_minus = v; w_R1_minus.setResistanceValue(v); }
    void setR2(float v)       { R2 = v; w_R2.setResistanceValue(v); }
    void setR3_plus(float v)  { R3_plus = v; w_R3_plus.setResistanceValue(v); }
    void setR3_minus(float v) { R3_minus = v; w_R3_minus.setResistanceValue(v); }
    void setC1(float v)       { C1 = v; w_C1.setCapacitanceValue(v); }
    void setC2(float v)       { C2 = v; w_C2.setCapacitanceValue(v); }
    void setC3(float v)       { C3 = v; w_C3.setCapacitanceValue(v); }
    void setR4(float v)       { R4 = v; w_R4.setResistanceValue(v); }
    void setBass(float v)       { 
        P2 = v;
        setR2(v*0.5f);    
    }
    void setTreble(float v)       { 
        P1 = v;
        setR1_plus(v*0.5f);    
        setR1_minus(v*0.5f);    
    }
    void setMid(float v)       { 
        P3 = v;
        setR3_plus(v*0.5f);    
        setR3_minus(v*0.5f);    
    }

    float getP1(){return P1;}
    float getP2(){return P2;}
    float getP3(){return P3;}
    WDFMembers<T> wdf;

private:
    T C1 = 0.25e-9;
    T R4 = 56.0e3;
    T C2 = 20.0e-9;
    T C3 = 20.0e-9;

    T P1 = 250e3f;
    T P2 = 250e3f;
    T P3 = 10e3f;

    T R1_plus = P1/2.0f;
    T R1_minus = P1/2.0f;
    T R2 = P2/2.0f;
    T R3_plus = P3/2.0f;
    T R3_minus = P3/2.0f;

    // Port B
    CapacitorT<T> w_C1 {C1};
    ResistorT<T> w_R1_plus {R1_plus};
    ResistorT<T> w_R1_minus {R1_minus};
    WDFSeriesT<T, decltype (w_R1_plus), decltype (w_R1_minus)> w_SR1 { w_R1_plus, w_R1_minus };
    WDFSeriesT<T, decltype (w_SR1), decltype (w_C1)> w_Sb { w_SR1, w_C1 };

    // Port C
    ResistorT<T> w_R4 {R4};

    // Port D
    CapacitorT<T> w_C2 {C2};

    // Port E
    CapacitorT<T> w_C3 {C3};

    // Port F
    ResistorT<T> w_R2 {R2};
    ResistorT<T> w_R3_plus {R3_plus};
    WDFSeriesT<T, decltype (w_R2), decltype (w_R3_plus)> w_Sf { w_R2, w_R3_plus };

    // Port G
    ResistorT<T> w_R3_minus {R3_minus};



    struct ImpedanceCalc
    {
        template <typename RType>
        static T calcImpedance (RType& R)
        {
            const auto [Rb, Rc, Rd, Re, Rf, Rg] = R.getPortImpedances();
            // This scattering matrix was derived using the R-Solver python script (https://github.com/jatinchowdhury18/R-Solver),
            // invoked with command: R-Solver/r_solver.py --adapt 0 --out PremierPlugin/bassman.txt PremierPlugin/src/dsp/bassman_netlist.txt
            R.setSMatrixData ( {{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               0,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           -(Rc*Rd + (Rc + Rd)*Re + Rc*Rf)/((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           -(Rb*Rd + Rb*Re + (Rb + Rd)*Rf)/((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          -(Rb*Re - Rc*Rf)/((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              -(Rb*Rd + (Rb + Rc + Rd)*Rf)/((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              -(Rc*Rd + (Rb + Rc + Rd)*Re)/((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               -1},
                                        {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                -(Rb*Rc*Rd + Rb*Rc*Rf + (Rb*Rc + Rb*Rd)*Re)/(Rb*Rc*Rd + (Rb*Rc + Rb*Rd)*Re + (Rb*Rc + Rc*Rd + (Rb + Rc + Rd)*Re)*Rf + ((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf)*Rg), -(Rb*Rb*Rc*Rd*Rd + (Rb*Rb*Rc + Rb*Rb*Rd)*Re*Re + (Rb*Rb*Rc - Rc*Rc*Rd - Rc*Rd*Rd + (Rb*Rb - Rc*Rc - 2*Rc*Rd - Rd*Rd)*Re)*Rf*Rf + (2*Rb*Rb*Rc*Rd + Rb*Rb*Rd*Rd)*Re + (2*Rb*Rb*Rc*Rd - Rc*Rc*Rd*Rd + (Rb*Rb - Rc*Rc - 2*Rc*Rd - Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc - Rc*Rd*Rd + (Rb*Rb - Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb - Rc*Rc)*Rd*Rd + (Rb*Rb - Rc*Rc - 2*Rc*Rd - Rd*Rd)*Re*Re + (Rb*Rb - Rc*Rc - 2*Rc*Rd - Rd*Rd)*Rf*Rf - 2*(Rc*Rd*Rd - (Rb*Rb - Rc*Rc)*Rd)*Re - 2*(Rc*Rd*Rd - (Rb*Rb - Rc*Rc)*Rd - (Rb*Rb - Rc*Rc - 2*Rc*Rd - Rd*Rd)*Re)*Rf)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),                                    (Rb*Rb*Rc*Rd*Rd + (Rb*Rb*Rc + Rb*Rb*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rd + 2*(Rb*Rb + Rb*Rc + Rb*Rd)*Re)*Rf*Rf + (2*Rb*Rb*Rc*Rd + Rb*Rb*Rd*Rd)*Re + (2*Rb*Rb*Rc*Rd + Rb*Rc*Rd*Rd + 2*(Rb*Rb + Rb*Rc + Rb*Rd)*Re*Re + (2*Rb*Rb*Rc + Rb*Rd*Rd + 3*(Rb*Rb + Rb*Rc)*Rd)*Re)*Rf + 2*((Rb*Rb + Rb*Rc)*Rd*Rd + (Rb*Rb + Rb*Rc + Rb*Rd)*Re*Re + (Rb*Rb + Rb*Rc + Rb*Rd)*Rf*Rf + (Rb*Rd*Rd + 2*(Rb*Rb + Rb*Rc)*Rd)*Re + (Rb*Rd*Rd + 2*(Rb*Rb + Rb*Rc)*Rd + 2*(Rb*Rb + Rb*Rc + Rb*Rd)*Re)*Rf)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),                                                                                                    (Rb*Rb*Rc*Rd*Re + (Rb*Rb*Rc + Rb*Rb*Rd)*Re*Re + (2*Rb*Rb*Rc + Rb*Rc*Rc + 2*Rb*Rc*Rd + 2*(Rb*Rb + Rb*Rc + Rb*Rd)*Re)*Rf*Rf + (2*(Rb*Rb + Rb*Rc + Rb*Rd)*Re*Re + (2*Rb*Rb*Rc + Rb*Rc*Rc)*Rd + (3*Rb*Rb*Rc + Rb*Rc*Rc + (2*Rb*Rb + 3*Rb*Rc)*Rd)*Re)*Rf + 2*((Rb*Rb + Rb*Rc)*Rd*Re + (Rb*Rb + Rb*Rc + Rb*Rd)*Re*Re + (Rb*Rb + Rb*Rc + Rb*Rd)*Rf*Rf + ((Rb*Rb + Rb*Rc)*Rd + 2*(Rb*Rb + Rb*Rc + Rb*Rd)*Re)*Rf)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),                                                                                                                                                                                                                         (Rb*Rb*Rc*Rd*Rd - (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rc*Rd)*Rf*Rf + (Rb*Rb*Rc*Rd + Rb*Rb*Rd*Rd)*Re - (Rb*Rc*Rc*Rd - Rb*Rc*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc - Rb*Rb*Rd - Rb*Rd*Rd)*Re)*Rf + 2*((Rb*Rb + Rb*Rc)*Rd*Rd + (Rb*Rd*Rd + (Rb*Rb + Rb*Rc)*Rd)*Re + (Rb*Rd*Rd + (Rb*Rb + Rb*Rc)*Rd)*Rf)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),                                                                                                                         -((2*Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (2*(Rb*Rb + Rb*Rc)*Rd*Rd + (3*Rb*Rb*Rc + 2*Rb*Rc*Rc)*Rd)*Re + (2*Rb*Rc*Rd*Rd + (2*Rb*Rb*Rc + Rb*Rc*Rc)*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + 2*Rb*Rd*Rd + (2*Rb*Rb + 3*Rb*Rc)*Rd)*Re)*Rf + 2*((Rb*Rb + Rb*Rc)*Rd*Rd + (Rb*Rd*Rd + (Rb*Rb + Rb*Rc)*Rd)*Re + (Rb*Rd*Rd + (Rb*Rb + Rb*Rc)*Rd)*Rf)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 -(Rb*Rc*Rd + Rb*Rc*Rf + (Rb*Rc + Rb*Rd)*Re)/(Rb*Rc*Rd + (Rb*Rc + Rb*Rd)*Re + (Rb*Rc + Rc*Rd + (Rb + Rc + Rd)*Re)*Rf + ((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf)*Rg)},
                                        {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                -(Rb*Rc*Rd + Rb*Rc*Re + (Rb*Rc + Rc*Rd)*Rf)/(Rb*Rc*Rd + (Rb*Rc + Rb*Rd)*Re + (Rb*Rc + Rc*Rd + (Rb + Rc + Rd)*Re)*Rf + ((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf)*Rg),                                    (Rb*Rc*Rc*Rd*Rd + (Rb*Rc*Rc + Rb*Rc*Rd)*Re*Re + (Rb*Rc*Rc + Rc*Rc*Rd + 2*(Rb*Rc + Rc*Rc + Rc*Rd)*Re)*Rf*Rf + (2*Rb*Rc*Rc*Rd + Rb*Rc*Rd*Rd)*Re + (2*Rb*Rc*Rc*Rd + Rc*Rc*Rd*Rd + 2*(Rb*Rc + Rc*Rc + Rc*Rd)*Re*Re + (2*Rb*Rc*Rc + Rc*Rd*Rd + 3*(Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + 2*((Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rc + Rc*Rc + Rc*Rd)*Re*Re + (Rb*Rc + Rc*Rc + Rc*Rd)*Rf*Rf + (Rc*Rd*Rd + 2*(Rb*Rc + Rc*Rc)*Rd)*Re + (Rc*Rd*Rd + 2*(Rb*Rc + Rc*Rc)*Rd + 2*(Rb*Rc + Rc*Rc + Rc*Rd)*Re)*Rf)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg), -(Rb*Rc*Rc*Rd*Rd + (Rb*Rc*Rc - Rb*Rb*Rd - Rb*Rd*Rd)*Re*Re + (Rb*Rc*Rc + Rc*Rc*Rd - (Rb*Rb - Rc*Rc + 2*Rb*Rd + Rd*Rd)*Re)*Rf*Rf + (2*Rb*Rc*Rc*Rd - Rb*Rb*Rd*Rd)*Re + (2*Rb*Rc*Rc*Rd + Rc*Rc*Rd*Rd - (Rb*Rb - Rc*Rc + 2*Rb*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rc*Rc - Rb*Rd*Rd - (Rb*Rb - Rc*Rc)*Rd)*Re)*Rf - ((Rb*Rb - Rc*Rc)*Rd*Rd + (Rb*Rb - Rc*Rc + 2*Rb*Rd + Rd*Rd)*Re*Re + (Rb*Rb - Rc*Rc + 2*Rb*Rd + Rd*Rd)*Rf*Rf + 2*(Rb*Rd*Rd + (Rb*Rb - Rc*Rc)*Rd)*Re + 2*(Rb*Rd*Rd + (Rb*Rb - Rc*Rc)*Rd + (Rb*Rb - Rc*Rc + 2*Rb*Rd + Rd*Rd)*Re)*Rf)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),                                                                                                   -((Rb*Rb*Rc + 2*Rb*Rc*Rc)*Rd*Re + (Rb*Rb*Rc + 2*Rb*Rc*Rc + 2*Rb*Rc*Rd)*Re*Re + (Rb*Rc*Rc + Rc*Rc*Rd + 2*(Rb*Rc + Rc*Rc + Rc*Rd)*Re)*Rf*Rf + (Rb*Rc*Rc*Rd + 2*(Rb*Rc + Rc*Rc + Rc*Rd)*Re*Re + (Rb*Rb*Rc + 3*Rb*Rc*Rc + (3*Rb*Rc + 2*Rc*Rc)*Rd)*Re)*Rf + 2*((Rb*Rc + Rc*Rc)*Rd*Re + (Rb*Rc + Rc*Rc + Rc*Rd)*Re*Re + (Rb*Rc + Rc*Rc + Rc*Rd)*Rf*Rf + ((Rb*Rc + Rc*Rc)*Rd + 2*(Rb*Rc + Rc*Rc + Rc*Rd)*Re)*Rf)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),                                                                                                                         -((Rb*Rb*Rc + 2*Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd)*Rf*Rf + (2*Rb*Rc*Rd*Rd + (Rb*Rb*Rc + 2*Rb*Rc*Rc)*Rd)*Re + (2*(Rb*Rc + Rc*Rc)*Rd*Rd + (2*Rb*Rb*Rc + 3*Rb*Rc*Rc)*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + 2*Rc*Rd*Rd + (3*Rb*Rc + 2*Rc*Rc)*Rd)*Re)*Rf + 2*((Rb*Rc + Rc*Rc)*Rd*Rd + (Rc*Rd*Rd + (Rb*Rc + Rc*Rc)*Rd)*Re + (Rc*Rd*Rd + (Rb*Rc + Rc*Rc)*Rd)*Rf)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),                                                                                                                                                                                                                         (Rb*Rc*Rc*Rd*Rd - (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rc*Rd)*Re*Re - (Rb*Rb*Rc*Rd - Rb*Rc*Rd*Rd)*Re + (Rb*Rc*Rc*Rd + Rc*Rc*Rd*Rd - (Rb*Rb*Rc + Rb*Rc*Rc - Rc*Rc*Rd - Rc*Rd*Rd)*Re)*Rf + 2*((Rb*Rc + Rc*Rc)*Rd*Rd + (Rc*Rd*Rd + (Rb*Rc + Rc*Rc)*Rd)*Re + (Rc*Rd*Rd + (Rb*Rc + Rc*Rc)*Rd)*Rf)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 -(Rb*Rc*Rd + Rb*Rc*Re + (Rb*Rc + Rc*Rd)*Rf)/(Rb*Rc*Rd + (Rb*Rc + Rb*Rd)*Re + (Rb*Rc + Rc*Rd + (Rb + Rc + Rd)*Re)*Rf + ((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf)*Rg)},
                                        {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     -(Rb*Rd*Re - Rc*Rd*Rf)/(Rb*Rc*Rd + (Rb*Rc + Rb*Rd)*Re + (Rb*Rc + Rc*Rd + (Rb + Rc + Rd)*Re)*Rf + ((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf)*Rg),                                                                                                                     (Rb*Rc*Rd*Rd*Re + (Rb*Rc*Rd + Rb*Rd*Rd)*Re*Re + (2*Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + 2*((Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + 2*((Rb + Rc)*Rd + Rd*Rd)*Re*Re + ((2*Rb + 3*Rc)*Rd*Rd + (3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + 2*((Rb + Rc)*Rd*Rd*Re + ((Rb + Rc)*Rd + Rd*Rd)*Re*Re + ((Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + ((Rb + Rc)*Rd*Rd + 2*((Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),                                                                                                                    -((Rb*Rb + 2*Rb*Rc)*Rd*Rd*Re + (2*Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rc*Rd + Rc*Rd*Rd + 2*((Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + (Rb*Rc*Rd*Rd + 2*((Rb + Rc)*Rd + Rd*Rd)*Re*Re + ((3*Rb + 2*Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc)*Rd)*Re)*Rf + 2*((Rb + Rc)*Rd*Rd*Re + ((Rb + Rc)*Rd + Rd*Rd)*Re*Re + ((Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + ((Rb + Rc)*Rd*Rd + 2*((Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),     -((Rb*Rb + 2*Rb*Rc)*Rd*Rd*Re + (Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd - (Rb*Rb*Rc + Rb*Rc*Rc - Rb*Rd*Rd)*Re*Re - (Rb*Rb*Rc + Rb*Rc*Rc - Rc*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc - Rd*Rd)*Re)*Rf*Rf + ((2*Rb*Rc + Rc*Rc)*Rd*Rd - (Rb*Rb + 2*Rb*Rc + Rc*Rc - Rd*Rd)*Re*Re - 2*(Rb*Rb*Rc + Rb*Rc*Rc - (Rb + Rc)*Rd*Rd)*Re)*Rf + (2*(Rb + Rc)*Rd*Rd*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd - (Rb*Rb + 2*Rb*Rc + Rc*Rc - Rd*Rd)*Re*Re - (Rb*Rb + 2*Rb*Rc + Rc*Rc - Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd - (Rb*Rb + 2*Rb*Rc + Rc*Rc - Rd*Rd)*Re)*Rf)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),                                                                                                    (2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rc*Rd*Rd + (Rb*Rc + Rc*Rc)*Rd)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((3*Rb*Rc + 2*Rc*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + ((Rb + 2*Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + 2*Rc*Rc)*Rd)*Re)*Rf + 2*((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + ((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + ((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Rf)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),                                                                                                   -(2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rd*Rd + (Rb*Rb + Rb*Rc)*Rd)*Re*Re + ((2*Rb*Rb + 3*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + ((2*Rb + Rc)*Rd*Rd + (2*Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + 2*((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + ((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + ((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Rf)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      -(Rb*Rd*Re - Rc*Rd*Rf)/(Rb*Rc*Rd + (Rb*Rc + Rb*Rd)*Re + (Rb*Rc + Rc*Rd + (Rb + Rc + Rd)*Re)*Rf + ((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf)*Rg)},
                                        {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         -(Rb*Rd*Re + (Rb + Rc + Rd)*Re*Rf)/(Rb*Rc*Rd + (Rb*Rc + Rb*Rd)*Re + (Rb*Rc + Rc*Rd + (Rb + Rc + Rd)*Re)*Rf + ((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf)*Rg),                                                                                                                                                                                                                                                     (Rb*Rc*Rd*Rd*Re - (Rb*Rc + Rc*Rc + Rc*Rd)*Re*Rf*Rf + (Rb*Rc*Rd + Rb*Rd*Rd)*Re*Re - ((Rb*Rc + Rc*Rc - Rb*Rd - Rd*Rd)*Re*Re + (Rc*Rc*Rd - Rc*Rd*Rd)*Re)*Rf + 2*((Rb + Rc)*Rd*Rd*Re + ((Rb + Rc)*Rd + Rd*Rd)*Re*Re + ((Rb + Rc)*Rd + Rd*Rd)*Re*Rf)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),                                                                                                                                                                        -((Rb*Rb + 2*Rb*Rc)*Rd*Rd*Re + (Rb*Rb + Rb*Rc + (2*Rb + Rc)*Rd + Rd*Rd)*Re*Rf*Rf + (2*Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + ((Rb*Rb + Rb*Rc + (3*Rb + 2*Rc)*Rd + 2*Rd*Rd)*Re*Re + (2*(Rb + Rc)*Rd*Rd + (2*Rb*Rb + 3*Rb*Rc)*Rd)*Re)*Rf + 2*((Rb + Rc)*Rd*Rd*Re + ((Rb + Rc)*Rd + Rd*Rd)*Re*Re + ((Rb + Rc)*Rd + Rd*Rd)*Re*Rf)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),                                                                                                                       ((Rb*Rc + Rc*Rc + Rc*Rd)*Re*Rf*Rf + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Re + (2*Rb*Rb*Rc + 2*Rb*Rc*Rc + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + ((Rb*Rb + 3*Rb*Rc + 2*Rc*Rc + (Rb + 2*Rc)*Rd)*Re*Re + (2*Rb*Rb*Rc + 2*Rb*Rc*Rc + (3*Rb*Rc + 2*Rc*Rc)*Rd)*Re)*Rf + 2*((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + (Rb + Rc)*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + (Rb + Rc)*Rd)*Re*Rf)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),                    ((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd - (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd)*Rf*Rf + ((2*Rb*Rc + Rc*Rc)*Rd*Rd - (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd - (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Rf)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),                                                                             ((2*Rb*Rb*Rc + 2*Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 3*Rb*Rc)*Rd)*Re*Re + (Rb*Rc*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (2*Rb*Rb*Rc + 2*Rb*Rc*Rc + Rc*Rd*Rd + (3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + 2*((Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Rf + ((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          -(Rb*Rd*Re + (Rb + Rc + Rd)*Re*Rf)/(Rb*Rc*Rd + (Rb*Rc + Rb*Rd)*Re + (Rb*Rc + Rc*Rd + (Rb + Rc + Rd)*Re)*Rf + ((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf)*Rg)},
                                        {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            -(Rc*Rd + (Rb + Rc + Rd)*Re)*Rf/(Rb*Rc*Rd + (Rb*Rc + Rb*Rd)*Re + (Rb*Rc + Rc*Rd + (Rb + Rc + Rd)*Re)*Rf + ((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf)*Rg),                                                                                                                                                                                 -((2*Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rc + Rc*Rc + (2*Rb + 3*Rc)*Rd + 2*Rd*Rd)*Re)*Rf*Rf + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rc + Rc*Rc + (Rb + 2*Rc)*Rd + Rd*Rd)*Re*Re + (2*(Rb + Rc)*Rd*Rd + (3*Rb*Rc + 2*Rc*Rc)*Rd)*Re)*Rf + 2*(((Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + ((Rb + Rc)*Rd*Rd + ((Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),                                                                                                                                                                                                                                                              ((Rb*Rc*Rd + Rc*Rd*Rd - (Rb*Rb + Rb*Rc - Rc*Rd - Rd*Rd)*Re)*Rf*Rf + (Rb*Rc*Rd*Rd - (Rb*Rb + Rb*Rc + Rb*Rd)*Re*Re - (Rb*Rb*Rd - Rb*Rd*Rd)*Re)*Rf + 2*(((Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + ((Rb + Rc)*Rd*Rd + ((Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),                                                                                                                               -((2*Rb*Rb*Rc + 2*Rb*Rc*Rc + (2*Rb*Rc + Rc*Rc)*Rd + (2*Rb*Rb + 3*Rb*Rc + Rc*Rc + (2*Rb + Rc)*Rd)*Re)*Rf*Rf + ((Rb*Rb + Rb*Rc + Rb*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + (2*Rb*Rb*Rc + 2*Rb*Rc*Rc + (2*Rb*Rb + 3*Rb*Rc)*Rd)*Re)*Rf + 2*((Rb*Rb + 2*Rb*Rc + Rc*Rc + (Rb + Rc)*Rd)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + (Rb + Rc)*Rd)*Re)*Rf)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),                                                                                       ((2*Rb*Rb*Rc + 2*Rb*Rc*Rc + Rc*Rd*Rd + (3*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + (Rb*Rc*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + (2*Rb*Rb*Rc + 2*Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 3*Rb*Rc)*Rd)*Re)*Rf + 2*((Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + ((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),                      ((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re - (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re - (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re)*Rg)/((Rb*Rb*Rc + Rb*Rc*Rc)*Rd*Rd + (Rb*Rb*Rc + Rb*Rc*Rc + Rb*Rd*Rd + (Rb*Rb + 2*Rb*Rc)*Rd)*Re*Re + (Rb*Rb*Rc + Rb*Rc*Rc + Rc*Rd*Rd + (2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf*Rf + ((Rb*Rb + 2*Rb*Rc)*Rd*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd)*Re + ((2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + 2*(Rb*Rb*Rc + Rb*Rc*Rc)*Rd + 2*(Rb*Rb*Rc + Rb*Rc*Rc + (Rb + Rc)*Rd*Rd + (Rb*Rb + 3*Rb*Rc + Rc*Rc)*Rd)*Re)*Rf + ((Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re*Re + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Rf*Rf + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd)*Re + 2*((Rb + Rc)*Rd*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc)*Rd + (Rb*Rb + 2*Rb*Rc + Rc*Rc + 2*(Rb + Rc)*Rd + Rd*Rd)*Re)*Rf)*Rg),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             -(Rc*Rd + (Rb + Rc + Rd)*Re)*Rf/(Rb*Rc*Rd + (Rb*Rc + Rb*Rd)*Re + (Rb*Rc + Rc*Rd + (Rb + Rc + Rd)*Re)*Rf + ((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf)*Rg)},
                                        {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 -((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf)*Rg/(Rb*Rc*Rd + (Rb*Rc + Rb*Rd)*Re + (Rb*Rc + Rc*Rd + (Rb + Rc + Rd)*Re)*Rf + ((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf)*Rg),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          -(Rc*Rd + (Rc + Rd)*Re + Rc*Rf)*Rg/(Rb*Rc*Rd + (Rb*Rc + Rb*Rd)*Re + (Rb*Rc + Rc*Rd + (Rb + Rc + Rd)*Re)*Rf + ((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf)*Rg),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          -(Rb*Rd + Rb*Re + (Rb + Rd)*Rf)*Rg/(Rb*Rc*Rd + (Rb*Rc + Rb*Rd)*Re + (Rb*Rc + Rc*Rd + (Rb + Rc + Rd)*Re)*Rf + ((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf)*Rg),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         -(Rb*Re - Rc*Rf)*Rg/(Rb*Rc*Rd + (Rb*Rc + Rb*Rd)*Re + (Rb*Rc + Rc*Rd + (Rb + Rc + Rd)*Re)*Rf + ((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf)*Rg),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             -(Rb*Rd + (Rb + Rc + Rd)*Rf)*Rg/(Rb*Rc*Rd + (Rb*Rc + Rb*Rd)*Re + (Rb*Rc + Rc*Rd + (Rb + Rc + Rd)*Re)*Rf + ((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf)*Rg),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             -(Rc*Rd + (Rb + Rc + Rd)*Re)*Rg/(Rb*Rc*Rd + (Rb*Rc + Rb*Rd)*Re + (Rb*Rc + Rc*Rd + (Rb + Rc + Rd)*Re)*Rf + ((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf)*Rg),                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    (Rb*Rc*Rd + (Rb*Rc + Rb*Rd)*Re + (Rb*Rc + Rc*Rd + (Rb + Rc + Rd)*Re)*Rf)/(Rb*Rc*Rd + (Rb*Rc + Rb*Rd)*Re + (Rb*Rc + Rc*Rd + (Rb + Rc + Rd)*Re)*Rf + ((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf)*Rg)}}

            );
            T Ra = (Rb*Rc*Rd + (Rb*Rc + Rb*Rd)*Re + (Rb*Rc + Rc*Rd + (Rb + Rc + Rd)*Re)*Rf + ((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf)*Rg)/((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf);
            return Ra;
        }
    };


    using RType = RtypeAdaptor<T, 0, ImpedanceCalc, decltype (w_Sb), decltype (w_R4), decltype (w_C2), decltype (w_C3), decltype (w_Sf), decltype (w_R3_minus)>;
    RType R {w_Sb, w_R4, w_C2, w_C3, w_Sf, w_R3_minus};

};



class DualRectifierPreampCircuit : public Circuit
{
public:
    DualRectifierPreampCircuit(): Circuit()
    {
        params.resize((int)Param::Count, 0.0f);
        controls.resize((int)Control::Count, 0.0f);
        monitors.resize((int)Monitoring::Count, 0.0f);

        setDefaultParam();
        setDefaultControl();
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
        Count 
    };
    enum class Control : int 
    {
        Volume,
        Bass, Treble, Mid,
         Count 
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
        w_V4.setTubeLabParameters(
            1.014e-5f, 5.498e-8f, 1.076e-5f,
            getParam((int)Param::Rp4),
            getParam((int)Param::Rk4),
            getParam((int)Param::E4)
        );
        w_V5.setTubeLabParameters(
            1.014e-5f, 5.498e-8f, 1.076e-5f,
            getParam(1.0f),
            getParam((int)Param::Rk5),
            getParam((int)Param::E5)
        );

        w_Vin.setResistanceValue(1.0F);
        w_V_R3.setResistanceValue(1.0F);
        w_E5.setResistanceValue(1.0F);
        w_V_R5.setResistanceValue(1.0F);

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

            case (int)Param::V1: break;//TODO
            case (int)Param::V2: break;//TODO
            case (int)Param::V3: break;//TODO
            case (int)Param::V4: break;//TODO
            case (int)Param::V5: break;//TODO
            
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

        auto& VDCk1 = monitors[(int)Monitoring::VDCk1];
        x = getVk1();
        VDCk1 = lowPass(x, VDCk1);
        auto& VDCp1 = monitors[(int)Monitoring::VDCp1];
        x = getVp1();
        VDCp1 = lowPass(x, VDCp1);

        auto& VDCk2 = monitors[(int)Monitoring::VDCk2];
        x = getVk2();
        VDCk2 = lowPass(x, VDCk2);
        auto& VDCp2 = monitors[(int)Monitoring::VDCp2];
        x = getVp2();
        VDCp2 = lowPass(x, VDCp2);

        auto& VDCk3 = monitors[(int)Monitoring::VDCk3];
        x = getVk3();
        VDCk3 = lowPass(x, VDCk3);
        auto& VDCp3 = monitors[(int)Monitoring::VDCp3];
        x = getVp3();
        VDCp3 = lowPass(x, VDCp3);

        auto& VDCk4 = monitors[(int)Monitoring::VDCk4];
        x = getVk4();
        VDCk4 = lowPass(x, VDCk4);
        auto& VDCp4 = monitors[(int)Monitoring::VDCp4];
        x = getVp4();
        VDCp4 = lowPass(x, VDCp4);

        auto& VDCk5 = monitors[(int)Monitoring::VDCk5];
        x = getVk5();
        VDCk5 = lowPass(x, VDCk5);
        auto& VDCp5 = monitors[(int)Monitoring::VDCp5];
        x = getVp5();
        VDCp5 = lowPass(x, VDCp5);

        // CURRENT 
        auto& ik1 = monitors[(int)Monitoring::Ik1];
        ik1 = lowPass(getIk1(), ik1);
        auto& ik2 = monitors[(int)Monitoring::Ik2];
        ik2 = lowPass(getIk2(), ik2);
        auto& ik3 = monitors[(int)Monitoring::Ik3];
        ik3 = lowPass(getIk3(), ik3);
        auto& ik4 = monitors[(int)Monitoring::Ik4];
        ik4 = lowPass(getIk4(), ik4);
        auto& ik5 = monitors[(int)Monitoring::Ik5];
        ik5 = lowPass(getIk5(), ik5);
    }


    void prepare(double sr) override {
        w_Ck1.prepare ((float) sr);
        w_Cp1.prepare ((float) sr);
        w_Ca1.prepare ((float) sr);
        w_Cb1.prepare ((float) sr);
        w_Cbright.prepare((float) sr);
        w_Cg2.prepare((float) sr);
        w_Ck2.prepare((float) sr);
        w_Cp2.prepare((float) sr);
        w_Cp3.prepare((float) sr);
        w_Ck4.prepare((float) sr);
        w_TS.prepare((float) sr);

        float duration = 0.1f;
        for (int i = 0; i < (int) sr *duration; ++i)
        {
            auto y = processSample(0.0f);
        }

        alpha = 1.0f / (sr * 0.5f); 
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
        return w_TS.getVoltage();
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
            w_PI_g1, w_PJ_k1, w_PJ_p1, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f
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
            w_V_Rg2, w_PJ_k2, w_PJ_p2, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f
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
            w_V_R3, w_Rk3, w_PJ_p3, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f
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
            w_V_Rg4, w_PJ_k4, w_E4_Rp4, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f
    }; 
    // ==================================================================================================== 
    // =  Fifth stage 
    // ==================================================================================================== 

    // Grid Circuit 
    ResistiveVoltageSourceT<float> w_V_R5 { 0.0f };

    // Cathode Circuit    
    ResistorT<float> w_Rk5 { 0.0f };
    BassmanToneStack<float> w_TS {};
    WDFParallelT<float, decltype (w_Rk5), decltype (w_TS)> w_PJ_k5 { w_Rk5, w_TS};

    // Plate Circuit
    ResistiveVoltageSourceT<float> w_E5 { 0.0f };

    TriodeQuadricWDF<float, decltype(w_V_R5), decltype(w_PJ_k5), decltype(w_E5)> w_V5{
            w_V_R5, w_PJ_k5, w_E5, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f
    }; 
};