#pragma once

#include <chowdsp_wdf/chowdsp_wdf.h>
#include <dsp/Circuit.h>
#include <dsp/Rtype.h>

using namespace chowdsp::wdft;


template <typename T>
class BassmanToneStack : public BaseWDF
{
public:
    BassmanToneStack() {
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

    inline T getVoltage() noexcept
    {
        return voltage<T> (w_R1_minus) + voltage<T> (w_R2) + voltage<T> (w_R3_plus)  + voltage<T> (w_R3_minus); 
    }

    void setR1_plus(T v)  { R1_plus = v; w_R1_plus.setResistanceValue(v); propagateImpedanceChange(); }
    void setR1_minus(T v) { R1_minus = v; w_R1_minus.setResistanceValue(v); propagateImpedanceChange(); }
    void setR2(T v)       { R2 = v; w_R2.setResistanceValue(v); propagateImpedanceChange(); }
    void setR3_plus(T v)  { R3_plus = v; w_R3_plus.setResistanceValue(v); propagateImpedanceChange(); }
    void setR3_minus(T v) { R3_minus = v; w_R3_minus.setResistanceValue(v); propagateImpedanceChange(); }
    void setC1(T v)       { C1 = v; w_C1.setCapacitanceValue(v); propagateImpedanceChange(); }
    void setC2(T v)       { C2 = v; w_C2.setCapacitanceValue(v); propagateImpedanceChange(); }
    void setC3(T v)       { C3 = v; w_C3.setCapacitanceValue(v); propagateImpedanceChange(); }
    void setR4(T v)       { R4 = v; w_R4.setResistanceValue(v); propagateImpedanceChange(); }

    T getP1(){return P1;}
    T getP2(){return P2;}
    T getP3(){return P3;}
    WDFMembers<T> wdf;

    T C1 = T(0.25e-9);
    T R4 = T(56.0e3 );
    T C2 = T(20.0e-9);
    T C3 = T(20.0e-9);

    T P1 = T(250e3f);
    T P2 = T(250e3f);
    T P3 = T(10e3f);

    T R1_plus = P1/T(2.0f);
    T R1_minus = P1/T(2.0f);
    T R2 = P2/T(2.0f);
    T R3_plus = P3/T(2.0f);
    T R3_minus = P3/T(2.0f);

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
        inline static const RTBuilderBassman builder {};

        template <typename RType>
        static T calcImpedance (RType& R)
        {
            const auto [Rb, Rc, Rd, Re, Rf, Rg] = R.getPortImpedances();
            T smat[7][7];
            T Ra = builder.buildS(&smat[0][0], Rb, Rc, Rd, Re, Rf, Rg);
            R.setSMatrixData ( smat);
            return Ra;
        }
    };

    using RType = RtypeAdaptor<T, 0, ImpedanceCalc, decltype (w_Sb), decltype (w_R4), decltype (w_C2), decltype (w_C3), decltype (w_Sf), decltype (w_R3_minus)>;
    RType R {w_Sb, w_R4, w_C2, w_C3, w_Sf, w_R3_minus};

};


template <typename T>
class BassmanToneStackCircuitT : public Circuit<T>
{
public:
    BassmanToneStackCircuitT(): Circuit<T>()
    {
        params.resize((int)Param::Count, 0.0f);
        controls.resize((int)Control::Count, 0.0f);
        monitors.resize((int)Monitoring::Count);

        setDefaultParam();
        setDefaultControl();

        this->setOutputGain(2.0f);

    };
    using Circuit<T>::params;
    using Circuit<T>::controls;
    using Circuit<T>::monitors;
    using Circuit<T>::getParam;
    using Circuit<T>::updatePortMonitor;

    enum class Monitoring : int { RBass, RMid_plus, RMid_minus, RTreble_plus, RTreble_minus, C1, C2, C3, R4, Count };
    enum class Param : int {RBass, RMid, RTreble, C1, C2, C3, R4,Count };
    enum class Control : int {Bass, Mid, Treble, Count };

    void setDefaultParam () 
    {
        setParam((int)Param::C1, T(0.25e-9));      
        setParam((int)Param::C2, T(22.0e-9));      
        setParam((int)Param::C3, T(22.0e-9));      
        setParam((int)Param::R4, T(56.0e3));      
        setParam((int)Param::RBass, T(250e3f));    
        setParam((int)Param::RMid, T(10e3f));     
        setParam((int)Param::RTreble, T(250e3f)); 
    }
    void setDefaultControl () 
    { 
        setControl((int)Control::Bass, T(50.0f));    
        setControl((int)Control::Mid, T(50.0f));     
        setControl((int)Control::Treble, T(50.0f)); 
    }
    // Runtime setters — updates the WDF node and re-propagates impedance

    void setParam (const int index, T value) override
    {
        params.at(index) = value;

        switch (index)
        {
            case (int)Param::C1:       w_bts.setC1(value); break;
            case (int)Param::C2:       w_bts.setC2(value); break;
            case (int)Param::C3:       w_bts.setC3(value); break;
            case (int)Param::R4:       w_bts.setR4(value); break;
            case (int)Param::RBass:     setControl((int)Control::Bass, T(50.0f)); break;
            case (int)Param::RMid:     setControl((int)Control::Mid, T(50.0f)); break;
            case (int)Param::RTreble:  setControl((int)Control::Treble, T(50.0f)); break;

            case (int)Param::Count:
            default:
                jassertfalse;
                break;
        }
    }
    void setControl (const int index, T value) override
    {
        controls.at(index) = value;
        auto ratio = value/T(100.0f);

        switch (index)
        {
            case (int)Control::Bass: 
            {
                T controlVal = getParam((int)Param::RBass);
                auto r = getPotRatios(ratio, PotType::Log);

                w_bts.setR2(controlVal*r.minus);
                break;
            }
            case (int)Control::Treble: 
            {
                T controlVal = getParam((int)Param::RTreble);
                auto r = getPotRatios(ratio, PotType::Log);

                w_bts.setR1_plus( controlVal * r.plus);
                w_bts.setR1_minus( controlVal * r.minus);
                break;
            }
            case (int)Control::Mid: 
            {        
                T controlVal = getParam((int)Param::RMid);
                auto r = getPotRatios(ratio, PotType::Linear);

                w_bts.setR3_plus( controlVal * r.plus);
                w_bts.setR3_minus( controlVal * r.minus);
                break;
            }
            default: jassertfalse; break;
        }
    }

    void prepare(T sr) override {
        w_bts.prepare(sr);
    }
    void reset() override {
        w_bts.reset();
    }

    T processSample(T x) override {
        w_vin.setVoltage (x);
        w_vin.incident(w_bts.reflected());
        w_bts.incident(w_vin.reflected());

        return w_bts.getVoltage();

    }

    void updateMonitors() override{
        updatePortMonitor((int)Monitoring::RBass, w_bts.w_R2); 
        updatePortMonitor((int)Monitoring::RMid_plus, w_bts.w_R3_plus); 
        updatePortMonitor((int)Monitoring::RMid_minus, w_bts.w_R3_minus); 
        updatePortMonitor((int)Monitoring::RTreble_plus, w_bts.w_R1_plus); 
        updatePortMonitor((int)Monitoring::RTreble_minus, w_bts.w_R1_minus); 
        updatePortMonitor((int)Monitoring::C1, w_bts.w_C1); 
        updatePortMonitor((int)Monitoring::C2, w_bts.w_C2); 
        updatePortMonitor((int)Monitoring::C3, w_bts.w_C3); 
        updatePortMonitor((int)Monitoring::R4, w_bts.w_R4); 
     }
    
private: 
    BassmanToneStack<T> w_bts {};
    IdealVoltageSourceT<T, decltype(w_bts)> w_vin {w_bts};
};

