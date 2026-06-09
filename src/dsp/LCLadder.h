#pragma once

#include <chowdsp_wdf/chowdsp_wdf.h>
#include <dsp/Circuit.h>


using namespace chowdsp::wdft;


template<typename T>
static T perturb(T x, T variationPercent)
{
    auto& rng = juce::Random::getSystemRandom();

    T r = (T) rng.nextFloat() * T(2.0) - T(1.0); // [-1, 1]
    auto y = x * (T(1.0) + variationPercent * r);
    std::cout << "Val = "<<juce::String(y)<<std::endl;
    return y;
}

template<typename T, typename Next>
class LCStage : public BaseWDF
{
public:
    LCStage(T l, T c, Next& next)
        : Lval(perturb(l, 1.0f))
        , Cval(perturb(c, 1.0f))
        , w_C(Lval)
        , w_L(Cval)
        , w_R(1e-5)
        , w_PI(next)
        , w_P(w_C, w_PI)
        , w_S2(w_R, w_P)
        , w_S(w_L, w_S2)
    {
        calcImpedance();
    }

    void prepare(T sampleRate)
    {
        w_C.prepare(sampleRate);
        w_L.prepare(sampleRate);

        propagateImpedanceChange();
    }

    void reset()
    {
        w_C.reset();
        w_L.reset();
    }

    void calcImpedance() override
    {
        w_S.calcImpedance();
        wdf = w_S.wdf;
    }

    void incident(T x) noexcept
    {
        w_S.incident(x);
        wdf.a = x;
    }

    T reflected() noexcept
    {
        wdf.b = w_S.reflected();
        return wdf.b;
    }

    WDFMembers<T> wdf;

private:
    T Lval;
    T Cval;

    CapacitorT<T> w_C;
    InductorT<T> w_L;
    ResistorT<T> w_R;

    PolarityInverterT<T, Next> w_PI;
    WDFParallelT<T, decltype(w_C), decltype(w_PI)> w_P;
    WDFSeriesT<T, decltype(w_R), decltype(w_P)> w_S2;
    WDFSeriesT<T, decltype(w_L), decltype(w_S2)> w_S;
};

template <typename T, typename Next>
class LCLadder : public BaseWDF
{
public:
    LCLadder(T l, T c, Next& next) :
        L(l), C(c), 
        w_S10 (L, C, next),
        w_S9 (L, C, w_S10),
        w_S8 (L, C, w_S9),
        w_S7 (L, C, w_S8),
        w_S6 (L, C, w_S7),
        w_S5 (L, C, w_S6),
        w_S4 (L, C, w_S5),
        w_S3 (L, C, w_S4),
        w_S2 (L, C, w_S3),
        w_S1 (L, C, w_S2)
    {
        calcImpedance();
    };

    inline void prepare (T sampleRate){
        w_S1.prepare( sampleRate);
        w_S2.prepare( sampleRate);
        w_S3.prepare( sampleRate);
        w_S4.prepare( sampleRate);
        w_S5.prepare( sampleRate);
        w_S6.prepare( sampleRate);
        w_S7.prepare( sampleRate);
        w_S8.prepare( sampleRate);
        w_S9.prepare( sampleRate);
        w_S10.prepare( sampleRate);
        propagateImpedanceChange();
    }

    void reset(){
        w_S1.reset();
        w_S2.reset();
        w_S3.reset();
        w_S4.reset();
        w_S5.reset();
        w_S6.reset();
        w_S7.reset();
        w_S8.reset();
        w_S9.reset();
        w_S10.reset();
    }

    inline void calcImpedance() override
    {
        w_S1.calcImpedance();
        wdf.R = w_S1.wdf.R;
        wdf.G = w_S1.wdf.G;
    }
    inline void incident (T x) noexcept
    {
        w_S1.incident (x);
        wdf.a = x;
    }
    inline T reflected() noexcept
    {
        wdf.b = w_S1.reflected();
        return wdf.b;
    }

    WDFMembers<T> wdf;

private:
    T L, C;
    LCStage<T, Next> w_S10;
    LCStage<T, decltype(w_S10)> w_S9;
    LCStage<T, decltype(w_S9)> w_S8;
    LCStage<T, decltype(w_S8)> w_S7;
    LCStage<T, decltype(w_S7)> w_S6;
    LCStage<T, decltype(w_S6)> w_S5;
    LCStage<T, decltype(w_S5)> w_S4;
    LCStage<T, decltype(w_S4)> w_S3;
    LCStage<T, decltype(w_S3)> w_S2;
    LCStage<T, decltype(w_S2)> w_S1;

};


class LCLadderCircuit : public Circuit<float>
{
public:
    LCLadderCircuit(): Circuit<float>()
    {
        params.resize((int)Param::Count, 0.0f);
        controls.resize((int)Control::Count, 0.0f);

        w_vin.setResistanceValue(0.01);

        setDefaultParam();
    };
    enum class Monitoring : int {  Count };
    enum class Param : int {L, C,Count };
    enum class Control : int {Count };

    void setDefaultParam () 
    {
    }

    void setParam (const int index, float value) override
    {
        params.at(index) = value;

        switch (index)
        {
            case (int)Param::L: break;
            case (int)Param::C: break;
            case (int)Param::Count:
            default: jassertfalse; break;
        }
    }
    void setControl (const int index, float value) override {}

    void prepare(float sr) override {
        w_lcl1.prepare(sr);
        w_lcl2.prepare(sr);
        w_lcl3.prepare(sr);
        w_lcl4.prepare(sr);
        w_Co.prepare(sr);
    }
    void reset() override {
        w_lcl1.reset();
        w_lcl2.reset();
        w_lcl3.reset();
        w_lcl4.reset();
        w_Co.reset();
    }

    float processSample(float x) override {
        w_vin.setVoltage (x);
        w_vin.incident(w_SJ.reflected());
        auto y = voltage<float> (w_Co);
        w_SJ.incident(w_vin.reflected());

        return y;

    }

    void updateMonitors() override{ }
    
private: 
    float L = 0.01f;
    float C = 1e-6f;
    CapacitorT<float> w_Co {C};
    LCLadder<float, decltype(w_Co)> w_lcl4 {L, C, w_Co};
    LCLadder<float, decltype(w_lcl4)> w_lcl3 {L, C, w_lcl4};
    LCLadder<float, decltype(w_lcl3)> w_lcl2 {L, C, w_lcl3};
    LCLadder<float, decltype(w_lcl2)> w_lcl1 {L, C, w_lcl2};
    ResistiveVoltageSourceT<float> w_vin ;
    WDFSeriesT<float, decltype(w_vin), decltype(w_lcl1)> w_SJ {w_vin, w_lcl1};
};





