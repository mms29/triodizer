#pragma once

#include <chowdsp_wdf/chowdsp_wdf.h>
#include <dsp/TriodeQuadricWDF.h>
#include <dsp/Circuit.h>
#include <dsp/ToneStack.h>
#include <dsp/SpringModel.h>

#include <cmath>

using namespace chowdsp::wdft;

template <typename T, typename PortType>
class IdealTransformerT final : public BaseWDF
{
public:
    IdealTransformerT (PortType& port, T turnsRatio)
        : child (port),
          N (turnsRatio)
    {
        child.connectToParent (this);
        calcImpedance();
    }

    inline void calcImpedance() override
    {
        // reflect impedance through transformer
        wdf.R = N * N * child.wdf.R;
        wdf.G = (T)1 / wdf.R;
    }

    inline void incident (T x) noexcept
    {
        wdf.a = x;

        // parent -> child
        child.incident (x / N);
    }

    inline T reflected() noexcept
    {
        // child -> parent
        wdf.b = N * child.reflected();

        return wdf.b;
    }

    void setTurnsRatio (T newRatio)
    {
        N = newRatio;
        propagateImpedanceChange();
    }

    PortType& child;
    WDFMembers<T> wdf;

private:
    T N;
};

class TwinReverbCircuit : public Circuit<float>
{
public:
    TwinReverbCircuit(): Circuit<float>()
    {
        params.resize((int)Param::Count, 0.0f);
        controls.resize((int)Control::Count, 0.0f);
        monitors.resize((int)Monitoring::Count, 0.0f);

        // Init params
        setDefaultParam();
        setDefaultControl();

        // Dummy resistances 
        w_Vin.setResistanceValue(1.0e3F);
        w_V_Rg2.setResistanceValue(1.0e3F);
        w_TS.setR3_plus(1.0e3F);
        w_V_Rg3.setResistanceValue(1.0e3f);
        w_V_Rg4.setResistanceValue(1.0e3f);

    };

    enum class Monitoring : int 
    {
        VDCk1, VDCk2, VDCk3, VDCk4, VDCk5, 
        VDCp1, VDCp2, VDCp3, VDCp4, VDCp5, 
        VACk1, VACk2, VACk3, VACk4, VACk5, 
        VACp1, VACp2, VACp3, VACp4, VACp5, 
        Ik1  , Ik2  , Ik3  , Ik4  , Ik5  , 
        Count 
    };
    enum class Param : int 
    {
        // V1
        V1, Rg1, Ri1, Rk1, Ck1,E1,Rp1, 

        // Tone Stack
        C1, C2, C3, R4, RTreble, RBass, RMid,
        
        // Volume
        RVol, Cbright,

        // V2
        V2, Rk2, Ck2, Rp2, E2, Cp2, Ra2, Cfilt,

        // V3
        V3, Rk3, Ck3, TR3, E3, Rtank,

        // V4
        V4, Rg4, Rk4, Ck4, Rp4, E4, Cp4, RVerb,

        // V5
        V5, Rdry, Rwet, Rg5, Rk5, Ck5, Rp5, E5, Cp5, Rout,

        Count 
    };
    enum class Control : int 
    {
        Volume, Bass, Treble, Mid, Reverb, Count
    };
    void setDefaultParam () 
    {
        //V1
        setParam((int)Param::Ri1, 68e3f);
        setParam((int)Param::Rg1, 1e6f);
        setParam((int)Param::Rk1, 1.5e3f);
        setParam((int)Param::Ck1, 25e-6f);
        setParam((int)Param::E1, 410.0f);
        setParam((int)Param::Rp1, 100.0e3f);

        //Tone stack
        setParam((int)Param::C1, 250e-12f);      
        setParam((int)Param::C2, 0.1e-6f);      
        setParam((int)Param::C3, 0.047e-6f);      
        setParam((int)Param::R4, 100.0e3f);      
        setParam((int)Param::RBass, 250e3f);    
        setParam((int)Param::RMid, 10e3f);     
        setParam((int)Param::RTreble, 250e3f); 

        // Vol
        setParam((int)Param::Cbright, 120e-12f);
        setParam((int)Param::RVol, 1.0e6f);

        //V2
        setParam((int)Param::Rk2, 1.5e3f);
        setParam((int)Param::Ck2, 25e-6f);
        setParam((int)Param::E2, 410.0f);
        setParam((int)Param::Rp2, 100.0e3f);
        setParam((int)Param::Cp2, 22e-9);
        setParam((int)Param::Cfilt, 500e-12);
        setParam((int)Param::Ra2, 1e6f);

        //V3
        setParam((int)Param::Rk3, 2200.0f);
        setParam((int)Param::Ck3, 25e-6f);
        setParam((int)Param::E3, 458.0f);
        setParam((int)Param::TR3, 53.0f);
        setParam((int)Param::Rtank, 8.0f);

        //V4
        setParam((int)Param::Rg4, 220.0e3f);
        setParam((int)Param::Rk4, 1.5e3f);
        setParam((int)Param::Ck4, 25e-6f);
        setParam((int)Param::E4, 410.0f);
        setParam((int)Param::Rp4, 100.0e3f);
        setParam((int)Param::Cp4, 3e-9);
        setParam((int)Param::RVerb, 100e3f);

        //V5
        setParam((int)Param::Rdry, 3.3e6f);
        setParam((int)Param::Rwet, 470e3f);
        setParam((int)Param::Rg5, 220e3f);
        setParam((int)Param::Rk5, 1.5e3f);
        setParam((int)Param::Ck5, 25e-6f);
        setParam((int)Param::E5, 410.0f);
        setParam((int)Param::Rp5, 100.0e3f);
        setParam((int)Param::Cp5, 100e-9);
        setParam((int)Param::Rout, 1e6f);

        // Triodes
        setParam((int)Param::V1, 10.0F); 
        setParam((int)Param::V2, 10.0F); 
        setParam((int)Param::V3, 3.0F); 
        setParam((int)Param::V4, 10.0F); 
        setParam((int)Param::V5, 10.0F); 
    }
    void setDefaultControl () 
    { 
        setControl((int)Control::Volume, 50.0f);    
        setControl((int)Control::Bass, 50.0f);    
        setControl((int)Control::Mid, 50.0f);    
        setControl((int)Control::Treble, 50.0f);    
        setControl((int)Control::Reverb, 50.0f);    
    }
    void setParam (const int index, float value) override
    {
        params.at(index) = value;
        switch (index)
        {
            case (int)Param::V1: w_V1.setParams((int) value,
                getParam((int)Param::Rp1),
                getParam((int)Param::Rk1),
                getParam((int)Param::E1)); break;
            case (int)Param::V2: w_V2.setParams((int) value,
                getParam((int)Param::Rp2),
                getParam((int)Param::Rk2),
                getParam((int)Param::E2)); break;
            case (int)Param::V3: w_V3.setParams((int) value,
                22e3f,
                getParam((int)Param::Rk3),
                getParam((int)Param::E3), 
                true); break;
            case (int)Param::V4: w_V4.setParams((int) value,
                getParam((int)Param::Rp4),
                getParam((int)Param::Rk4),
                getParam((int)Param::E4)); break;
            case (int)Param::V5: w_V5.setParams((int) value,
                getParam((int)Param::Rp4),
                getParam((int)Param::Rk4),
                getParam((int)Param::E4)); break;
            
            case (int)Param::Rg1: w_Rg1.setResistanceValue(value); break;
            case (int)Param::Ri1: w_Ri1.setResistanceValue(value); break;
            case (int)Param::Rk1: w_Rk1.setResistanceValue(value); break;
            case (int)Param::Ck1: w_Ck1.setCapacitanceValue(value); break;
            case (int)Param::E1:  w_E1_Rp1.setVoltage(value);  break;
            case (int)Param::Rp1: w_E1_Rp1.setResistanceValue(value); break;

            case (int)Param::C1:       w_TS.setC1(value); break;
            case (int)Param::C2:       w_TS.setC2(value); break;
            case (int)Param::C3:       w_TS.setC3(value); break;
            case (int)Param::R4:       w_TS.setR4(value); break;
            case (int)Param::RBass:     setControl((int)Control::Bass, 50.0f); break; 
            case (int)Param::RMid:      setControl((int)Control::Mid, 50.0f); break; 
            case (int)Param::RTreble:  setControl((int)Control::Treble, 50.0f); break; 

            case (int)Param::Cbright: w_CBright.setCapacitanceValue(value); break;
            case (int)Param::RVol: setControl((int)Control::Volume, 50.0f); break;

            case (int)Param::Rk2: w_Rk2.setResistanceValue(value); break;
            case (int)Param::Ck2: w_Ck2.setCapacitanceValue(value); break;
            case (int)Param::E2:  w_E2_Rp2.setVoltage(value);  break;
            case (int)Param::Rp2: w_E2_Rp2.setResistanceValue(value); break;
            case (int)Param::Cp2: w_Cp2.setCapacitanceValue(value); break;
            case (int)Param::Ra2: w_Ra2.setResistanceValue(value); break;
            case (int)Param::Cfilt: w_Cfilt.setCapacitanceValue(value); break;


            case (int)Param::Rk3: w_Rk3.setResistanceValue(value); break;
            case (int)Param::Ck3: w_Ck3.setCapacitanceValue(value); break;
            case (int)Param::E3:  w_E3_Rp3.setVoltage(value);  break;
            case (int)Param::Rtank: w_Rtank.setResistanceValue(value); break;
            case (int)Param::TR3: {
                w_E3_Rp3.setResistanceValue(1e3f);
                w_Lmag.setInductanceValue(95.5);
                w_Treverb.setTurnsRatio(value);
                break;
            }

            case (int)Param::Rg4: w_Rg4.setResistanceValue(value); break;
            case (int)Param::Rk4: w_Rk4.setResistanceValue(value); break;
            case (int)Param::Ck4: w_Ck4.setCapacitanceValue(value); break;
            case (int)Param::E4:  w_E4_Rp4.setVoltage(value);  break;
            case (int)Param::Rp4: w_E4_Rp4.setResistanceValue(value); break;
            case (int)Param::Cp4: w_Cp4.setCapacitanceValue(value); break;
            case (int)Param::RVerb: setControl((int)Control::Reverb, 50.0f); break;


            case (int)Param::Rdry: w_Vdry.setResistanceValue(value); w_Rdry.setResistanceValue(value); break;
            case (int)Param::Rwet: w_Vwet.setResistanceValue(value); break;
            case (int)Param::Rg5: w_Rg5.setResistanceValue(value); break;
            case (int)Param::Rk5: w_Rk5.setResistanceValue(value); break;
            case (int)Param::Ck5: w_Ck5.setCapacitanceValue(value); break;
            case (int)Param::E5:  w_E5_Rp5.setVoltage(value);  break;
            case (int)Param::Rp5: w_E5_Rp5.setResistanceValue(value); break;
            case (int)Param::Cp5: w_Cp5.setCapacitanceValue(value); break;
            case (int)Param::Rout: w_Rout.setResistanceValue(value); break;

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
            case (int)Control::Reverb: 
            {
                float controlVal = getParam((int)Param::RVol);
                auto ratio = (100.0f-value)/100.0f; 
                w_RVerb_plus.setResistanceValue(controlVal*ratio);
                w_RVerb_minus.setResistanceValue(controlVal*(1.0f - ratio));
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
                w_TS.setR3_minus( controlVal*ratio);
                break;
            }
            default: jassertfalse; break;
        }
    }
    void updateMonitors() override{
        // Voltages
        float x;

        auto& VDCk1 = monitors[(int)Monitoring::VDCk1];
        auto& VACk1 = monitors[(int)Monitoring::VACk1];
        x = getVk1();
        VDCk1 = lowPass(x, VDCk1);
        VACk1 = variance(x, VDCk1, VACk1);

        auto& VDCp1 = monitors[(int)Monitoring::VDCp1];
        auto& VACp1 = monitors[(int)Monitoring::VACp1];
        x = getVp1();
        VDCp1 = lowPass(x, VDCp1);
        VACp1 = variance(x, VDCp1, VACp1);

        auto& VDCk2 = monitors[(int)Monitoring::VDCk2];
        auto& VACk2 = monitors[(int)Monitoring::VACk2];
        x = getVk2();
        VDCk2 = lowPass(x, VDCk2);
        VACk2 = variance(x, VDCk2, VACk2);

        auto& VDCp2 = monitors[(int)Monitoring::VDCp2];
        auto& VACp2 = monitors[(int)Monitoring::VACp2];
        x = getVp2();
        VDCp2 = lowPass(x, VDCp2);
        VACp2 = variance(x, VDCp2, VACp2);

        auto& VDCk3 = monitors[(int)Monitoring::VDCk3];
        auto& VACk3 = monitors[(int)Monitoring::VACk3];
        x = getVk3();
        VDCk3 = lowPass(x, VDCk3);
        VACk3 = variance(x, VDCk3, VACk3);

        auto& VDCp3 = monitors[(int)Monitoring::VDCp3];
        auto& VACp3 = monitors[(int)Monitoring::VACp3];
        x = getVp3();
        VDCp3 = lowPass(x, VDCp3);
        VACp3 = variance(x, VDCp3, VACp3);

        auto& VDCk4 = monitors[(int)Monitoring::VDCk4];
        auto& VACk4 = monitors[(int)Monitoring::VACk4];
        x = getVk4();
        VDCk4 = lowPass(x, VDCk4);
        VACk4 = variance(x, VDCk4, VACk4);

        auto& VDCp4 = monitors[(int)Monitoring::VDCp4];
        auto& VACp4 = monitors[(int)Monitoring::VACp4];
        x = getVp4();
        VDCp4 = lowPass(x, VDCp4);
        VACp4 = variance(x, VDCp4, VACp4);


        auto& VDCk5 = monitors[(int)Monitoring::VDCk5];
        auto& VACk5 = monitors[(int)Monitoring::VACk5];
        x = getVk5();
        VDCk5 = lowPass(x, VDCk5);
        VACk5 = variance(x, VDCk5, VACk5);

        auto& VDCp5 = monitors[(int)Monitoring::VDCp5];
        auto& VACp5 = monitors[(int)Monitoring::VACp5];
        x = getVp5();
        VDCp5 = lowPass(x, VDCp5);
        VACp5 = variance(x, VDCp5, VACp5);


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


    void prepare(float sr) override {
        w_Ck1.prepare (sr);
        w_TS.prepare(sr);
        w_CBright.prepare(sr);
        w_Ck2.prepare(sr);
        w_Cp2.prepare(sr);
        w_Lmag.prepare(sr);
        springSubCircuit.prepare(sr);
        w_Cfilt.prepare(sr);
        w_Ck4.prepare(sr);
        w_Cp4.prepare(sr);
        w_Ck5.prepare(sr);
        w_Cp5.prepare(sr);

        alpha = 1.0f / (sr * 0.5f); 
    }
    void reset() override {
        w_Ck1.reset();
        w_TS.reset();
        w_CBright.reset();
        w_Ck2.reset();
        w_Cp2.reset();
        w_Lmag.reset();
        springSubCircuit.reset();
        w_Cfilt.reset();
        w_Ck4.reset();
        w_Cp4.reset();
        w_Ck5.reset();
        w_Cp5.reset();
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
        auto V2_out = voltage<float> (w_Rdry);

        //V3
        w_V_Rg3.setVoltage (V2_out);
        w_E3_Rp3.setVoltage(getParam((int)Param::E3));
        w_V3.compute();
        auto V3_out = voltage<float>(w_Rtank);

        auto reverb_out = springSubCircuit.processSample(V3_out);

        //V4
        w_V_Rg4.setVoltage (reverb_out);
        w_E4_Rp4.setVoltage(getParam((int)Param::E4));
        w_V4.compute();
        auto V4_out = voltage<float>(w_RVerb_minus);


        //V4
        w_Vdry.setVoltage (V2_out);
        w_Vwet.setVoltage (V4_out);
        w_E5_Rp5.setVoltage(getParam((int)Param::E5));
        w_V5.compute();
        auto V5_out = voltage<float>(w_Rout);

        updateMonitors();
        return V5_out;
    }

    // Accessor methods for monitoring internal WDF variables
    // float getVk1() const { return -w_V1.getVgkAcc(); }
    // float getVp1() const { return w_V1.getVpkAcc() - w_V1.getVgkAcc(); }
    float getVk1() const { return voltage<float>(w_Rk1); }
    float getVp1() const { return voltage<float>(w_E1_Rp1); }
    float getVk2() const { return voltage<float>(w_Rk2); }
    float getVp2() const { return voltage<float>(w_E2_Rp2); }
    float getVk3() const { return voltage<float>(w_Rk3); }
    float getVp3() const { return voltage<float>(w_SJ_t3); }
    float getVk4() const { return voltage<float>(w_Rk4); }
    float getVp4() const { return voltage<float>(w_E4_Rp4); }
    float getVk5() const { return voltage<float>(w_Rk5); }
    float getVp5() const { return voltage<float>(w_E5_Rp5); }

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
        return var + alpha * (std::sqrt((x - mean)*(x - mean)) - var);
    }

    // ==================================================================================================== 
    // =  First stage 
    // ==================================================================================================== 

    // Grid Circuit 
    ResistorT<float> w_Rg1 { 0.0f };
    ResistorT<float> w_Ri1 { 0.0f };
    ResistiveVoltageSourceT<float> w_Vin { 0.0f };
    PolarityInverterT<float, decltype (w_Vin)> w_PI_i1 { w_Vin };
    WDFParallelT<float, decltype (w_Rg1), decltype (w_PI_i1)> w_PJ_i1 { w_Rg1, w_PI_i1 };
    PolarityInverterT<float, decltype (w_PJ_i1)> w_PI_g1 { w_PJ_i1 };
    WDFSeriesT<float, decltype (w_Ri1), decltype (w_PI_g1)> w_SJ_g1 { w_Ri1, w_PI_g1 };

    // Cathode Circuit
    ResistorT<float> w_Rk1 { 0.0f };
    CapacitorT<float> w_Ck1 { 0.0f };
    WDFParallelT<float, decltype (w_Rk1), decltype (w_Ck1)> w_PJ_k1 { w_Rk1, w_Ck1 };

    // Plate circuit
    ResistiveVoltageSourceT<float> w_E1_Rp1 { 0.0f };

    ResistorT<float> w_RVol_plus { 0.0f };
    ResistorT<float> w_RVol_minus { 0.0f };
    CapacitorT<float> w_CBright { 0.0f };

    WDFParallelT<float, decltype (w_RVol_plus), decltype (w_CBright)> w_PJ_vol { w_RVol_plus, w_CBright};
    WDFSeriesT<float, decltype (w_PJ_vol), decltype (w_RVol_minus)> w_SJ_vol { w_PJ_vol, w_RVol_minus};

    ToneStack<float, decltype(w_SJ_vol)> w_TS {w_SJ_vol, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };
    PolarityInverterT<float, decltype (w_TS)> w_PI_p1 { w_TS };
    WDFParallelT<float, decltype (w_E1_Rp1), decltype (w_PI_p1)> w_PJ_p1 { w_E1_Rp1, w_PI_p1};

    //V1
    TriodeQuadricWDF<float, decltype(w_SJ_g1), decltype(w_PJ_k1), decltype(w_PJ_p1)> w_V1{
            w_SJ_g1, w_PJ_k1, w_PJ_p1
    };

    // ==================================================================================================== 
    // =  Second stage 
    // ==================================================================================================== 

    // Grid Circuit 
    ResistiveVoltageSourceT<float> w_V_Rg2 { 0.0f };

    // Cathode Circuit    
    ResistorT<float> w_Rk2 { 0.0f };
    CapacitorT<float> w_Ck2 { 0.0f };
    WDFParallelT<float, decltype (w_Rk2), decltype (w_Ck2)> w_PJ_k2 { w_Rk2, w_Ck2 };

    // Plate Circuit
    ResistiveVoltageSourceT<float> w_E2_Rp2 { 0.0f };
    CapacitorT<float> w_Cp2 { 0.0f };
    CapacitorT<float> w_Cfilt { 0.0f };
    ResistorT<float> w_Ra2 { 0.0f };
    ResistorT<float> w_Rdry { 0.0f };
    WDFSeriesT<float, decltype (w_Ra2), decltype (w_Cfilt)> w_SJ_filt { w_Ra2, w_Cfilt};
    WDFParallelT<float, decltype (w_SJ_filt), decltype (w_Rdry)> w_PJ_dry2 { w_SJ_filt, w_Rdry};
    WDFSeriesT<float, decltype (w_PJ_dry2), decltype (w_Cp2)> w_SJ_p2 { w_PJ_dry2, w_Cp2 };
    PolarityInverterT<float, decltype (w_SJ_p2)> w_PI_p2 { w_SJ_p2 };
    WDFParallelT<float, decltype (w_E2_Rp2), decltype (w_PI_p2)> w_PJ_p2 { w_E2_Rp2, w_PI_p2};

    TriodeQuadricWDF<float, decltype(w_V_Rg2), decltype(w_PJ_k2), decltype(w_PJ_p2)> w_V2{
            w_V_Rg2, w_PJ_k2, w_PJ_p2
    };


    // ==================================================================================================== 
    // =  Third stage 
    // ==================================================================================================== 

    // Grid Circuit 
    ResistiveVoltageSourceT<float> w_V_Rg3 { 0.0f };

    // Cathode Circuit    
    ResistorT<float> w_Rk3 { 0.0f };
    CapacitorT<float> w_Ck3 { 0.0f };
    WDFParallelT<float, decltype (w_Rk3), decltype (w_Ck3)> w_PJ_k3 { w_Rk3, w_Ck3 };

    // Plate Circuit
    ResistiveVoltageSourceT<float> w_E3_Rp3 { 0.0f };
    InductorT<float> w_Lmag { 0.0f };
    ResistorT<float> w_Rtank { 0.0f };
    IdealTransformerT<float, decltype(w_Rtank)> w_Treverb {w_Rtank, 0.0f};
    // WDFSeriesT<float, decltype (w_Rref), decltype (w_Lref)> w_SJ_ref { w_Rref, w_Lref };
    WDFParallelT<float, decltype (w_Lmag), decltype (w_Treverb)> w_PJ_t3 { w_Lmag, w_Treverb};
    WDFSeriesT<float, decltype (w_E3_Rp3), decltype (w_PJ_t3)> w_SJ_t3 { w_E3_Rp3, w_PJ_t3 };
    PolarityInverterT<float, decltype (w_SJ_t3)> w_PI_p3 { w_SJ_t3 };
    // WDFSeriesT<float, decltype (w_E3_Rp3), decltype (w_Rref)> w_SJ_p3 { w_E3_Rp3, w_Rref };

    TriodeQuadricWDF<float, decltype(w_V_Rg3), decltype(w_PJ_k3), decltype(w_PI_p3)> w_V3{
            w_V_Rg3, w_PJ_k3, w_PI_p3
    };

    SpringTank springSubCircuit{
        80.0f, // delay ms
        0.85f, // decay
        0.7f, // Feedback
        4000.0f, // HF cutoff HZ,
        2.5e-3f
    };


    // ==================================================================================================== 
    // =  Recovery stage 
    // ==================================================================================================== 

    // Grid Circuit 
    ResistiveVoltageSourceT<float> w_V_Rg4 { 0.0f };
    ResistorT<float> w_Rg4 { 0.0f };
    WDFParallelT<float, decltype (w_Rg4), decltype (w_V_Rg4)> w_PJ_g4 { w_Rg4, w_V_Rg4 };

    // Cathode Circuit    
    ResistorT<float> w_Rk4 { 0.0f };
    CapacitorT<float> w_Ck4 { 0.0f };
    WDFParallelT<float, decltype (w_Rk4), decltype (w_Ck4)> w_PJ_k4 { w_Rk4, w_Ck4 };

    // Plate Circuit
    ResistiveVoltageSourceT<float> w_E4_Rp4 { 0.0f };
    ResistorT<float> w_RVerb_plus { 0.0f };
    ResistorT<float> w_RVerb_minus { 0.0f };
    CapacitorT<float> w_Cp4 { 0.0f };
    WDFSeriesT<float, decltype (w_RVerb_plus), decltype (w_RVerb_minus)> w_SJ_verb { w_RVerb_plus, w_RVerb_minus};
    WDFSeriesT<float, decltype (w_Cp4), decltype (w_SJ_verb)> w_SJ_p4 { w_Cp4, w_SJ_verb};
    PolarityInverterT<float, decltype (w_SJ_p4)> w_PI_p4 { w_SJ_p4 };
    WDFParallelT<float, decltype (w_E4_Rp4), decltype (w_PI_p4)> w_PJ_p4 { w_E4_Rp4, w_PI_p4};

    TriodeQuadricWDF<float, decltype(w_PJ_g4), decltype(w_PJ_k4), decltype(w_PJ_p4)> w_V4{
            w_PJ_g4, w_PJ_k4, w_PJ_p4
    };

    // ==================================================================================================== 
    // =  Mixing stage 
    // ==================================================================================================== 

    // Grid Circuit 
    ResistiveVoltageSourceT<float> w_Vdry { 0.0f };
    ResistiveVoltageSourceT<float> w_Vwet { 0.0f };
    ResistorT<float> w_Rg5 { 0.0f };
    WDFParallelT<float, decltype (w_Vdry), decltype (w_Vwet)> w_PJ_mix { w_Vdry, w_Vwet };
    WDFParallelT<float, decltype (w_Rg5), decltype (w_PJ_mix)> w_PJ_g5 { w_Rg5, w_PJ_mix };
    PolarityInverterT<float, decltype (w_PJ_g5)> w_PI_g5 { w_PJ_g5 };

    // Cathode Circuit    
    ResistorT<float> w_Rk5 { 0.0f };
    CapacitorT<float> w_Ck5 { 0.0f };
    WDFParallelT<float, decltype (w_Rk5), decltype (w_Ck5)> w_PJ_k5 { w_Rk5, w_Ck5 };

    // Plate Circuit
    ResistiveVoltageSourceT<float> w_E5_Rp5 { 0.0f };
    CapacitorT<float> w_Cp5 { 0.0f };
    ResistorT<float> w_Rout { 0.0f };
    WDFSeriesT<float, decltype (w_Cp5), decltype (w_Rout)> w_PJ_out { w_Cp5, w_Rout};
    PolarityInverterT<float, decltype (w_PJ_out)> w_PI_p5 { w_PJ_out };
    WDFParallelT<float, decltype (w_E5_Rp5), decltype (w_PI_p5)> w_PJ_p5 { w_E5_Rp5, w_PI_p5};

    TriodeQuadricWDF<float, decltype(w_PI_g5), decltype(w_PJ_k5), decltype(w_PJ_p5)> w_V5{
            w_PI_g5, w_PJ_k5, w_PJ_p5
    };

};