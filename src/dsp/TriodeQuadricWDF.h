
#pragma once

#include <chowdsp_wdf/chowdsp_wdf.h>
#include <cmath>
#include <array>
#include <dsp/Circuit.h>

using namespace chowdsp::wdft;


struct TriodeParams
{
    const char* name;
    double kp;
    double kp2;
    double kpg;
};
static const TriodeParams triodeTable[] =
{
    // name        kp            kp2           kpg
{ "12AX7"     ,    9.455e-06f,    6.053e-08f,    1.157e-05f },
{ "12AX7A"    ,    6.141e-06f,    4.612e-08f,    9.320e-06f },
{ "12AX7ASYL" ,    5.539e-06f,    4.450e-08f,    8.532e-06f },
{ "12AT7"     ,    4.263e-05f,    2.562e-07f,    2.838e-05f },
{ "12AU7"     ,    5.916e-05f,    3.438e-07f,    1.268e-05f },
{ "12AY7"     ,    1.930e-05f,    7.972e-08f,    6.880e-06f },
{ "12AZ7"     ,    3.979e-05f,    2.465e-07f,    2.743e-05f },
{ "12BH7A"    ,    1.056e-04f,    6.533e-07f,    2.519e-05f },
{ "6AN8T"     ,    5.914e-05f,    3.436e-07f,    1.267e-05f },
{ "6DJ8"      ,    1.541e-04f,    8.853e-07f,    4.898e-05f },
{ "7025"      ,    6.130e-06f,    3.925e-08f,    7.918e-06f },
{ "SV6N1P"    ,    4.243e-05f,    3.823e-07f,    2.625e-05f },
{ "ECC83"     ,    5.943e-06f,    4.254e-08f,    8.191e-06f },
{ "ECC81"     ,    3.728e-05f,    1.429e-07f,    1.685e-05f }
};
static const TriodeParams parallelTriodeTable[] =
{
    // name        kp            kp2           kpg
{ "12AX7"     ,    1.896e-05f,    1.208e-07f,    2.311e-05f },
{ "12AX7A"    ,    1.211e-05f,    9.304e-08f,    1.874e-05f },
{ "12AX7ASYL" ,    1.120e-05f,    8.826e-08f,    1.687e-05f },
{ "12AT7"     ,    8.513e-05f,    5.127e-07f,    5.676e-05f },
{ "12AU7"     ,    1.186e-04f,    6.858e-07f,    2.531e-05f },
{ "12AY7"     ,    3.857e-05f,    1.596e-07f,    1.377e-05f },
{ "12AZ7"     ,    7.962e-05f,    4.921e-07f,    5.477e-05f },
{ "12BH7A"    ,    2.118e-04f,    1.304e-06f,    5.032e-05f },
{ "6AN8T"     ,    1.186e-04f,    6.856e-07f,    2.530e-05f },
{ "6DJ8"      ,    3.080e-04f,    1.772e-06f,    9.801e-05f },
{ "7025"      ,    1.221e-05f,    7.872e-08f,    1.586e-05f },
{ "SV6N1P"    ,    8.514e-05f,    7.634e-07f,    5.245e-05f },
{ "ECC83"     ,    1.186e-05f,    8.522e-08f,    1.640e-05f },
{ "ECC81"     ,    7.485e-05f,    2.838e-07f,    3.354e-05f }
};

/**
 * Quadric surface model implementation.
 *
 * Template parameters:
 *   T            – sample type 
 *   PortGType    – type of the grid port (must provide reflected() / incident())
 *   PortKType    – cathode port
 *   PortPType    – plate port
 */
template <typename T, typename PortGType, typename PortKType, typename PortPType>
class TriodeQuadricWDF final : public RootWDF
{
public:
    TriodeQuadricWDF (PortGType& pg, PortKType& pk, PortPType& pp)
        : port_g (pg), port_k (pk), port_p (pp)
    {
        port_g.connectToParent (this);
        port_k.connectToParent (this);
        port_p.connectToParent (this);

        monitor.size = MONITOR_TUBE_COUNT;
    }

    void setParams(const int index, T Rp_val, T Rk_val, T E_val, bool parallelTriode=false) noexcept{
        if (parallelTriode){
            const TriodeParams &t = parallelTriodeTable[index];
            kp  = t.kp;
            kp2 = t.kp2;
            kpg = t.kpg;}
        else{
            const TriodeParams &t = triodeTable[index];
            kp  = t.kp;
            kp2 = t.kp2;
            kpg = t.kpg;}
        Rp = Rp_val;
        Rk = Rk_val;
        E = E_val;

        initialiseOperatingPoint();
        calcImpedance();

    }

    void initialiseOperatingPoint () noexcept
    {
        T k1 = kpg/(2*kp2) + Rp/Rk + 1;
        T k2 = k1 * (kp/kp2 + 2*E) * kp2;
        T k3 = Rk * k2 + 1;
#ifdef XSIMD_HPP
        T k1sign = xsimd::sign(k1);
        T Vk0 =(k3 - k1sign * xsimd::sqrt(T(2.0f) * k3 - T(1.0f)))/ (T(2.0f) * Rk * k1 * k1 * kp2);
#else
        T k1sign = (k1 > 0.0f) ? 1.0f : (k1 < 0.0f ? -1.0f : 0.0f);
        T Vk0 = (k3 - k1sign * std::sqrt(2*k3 - 1)) / (2 * Rk * k1 * k1 * kp2);
#endif
        T Vp0 = E - Rp/Rk * Vk0;

        Vk = Vk0;
        Vp = Vp0;
        Vg = zero;
    }
    
    inline void calcImpedance() override {
        R0g = port_g.wdf.R;
        R0k = port_k.wdf.R;
        R0p = port_p.wdf.R;

        bk_bp = R0k / R0p;
        k_eta = one / (bk_bp * (half * kpg + kp2) + kp2);
        k_delta = kp2 * k_eta * k_eta / (R0p + R0p);
#ifdef XSIMD_HPP
        k_bp_s = k_eta * xsimd::sqrt ((kp2 + kp2) / R0p);
#else
        k_bp_s = k_eta * std::sqrt ((kp2 + kp2) / R0p);
#endif
        const T bp_k = one / (R0p + R0k);
        bp_ap_0 = bp_k * (R0k - R0p);
        bp_ak_0 = bp_k * (R0p + R0p);


    }

    void compute() noexcept
    {
        // Pull incident waves from the three ports
        ag = port_g.reflected();
        ak = port_k.reflected();
        ap = port_p.reflected();


        // Closed‑form triode solution
        triodeClosedForm();

        // Update incident waves back to the ports
        port_g.incident (bg);
        port_k.incident (bk);
        port_p.incident (bp);
    }

    // Accessors for monitoring 
    T getVg() const { return Vg; }
    T getVk() const { return Vk; }
    T getVp() const { return Vp; }
    T getIk() const { return Ik; }
    T getIp() const { return Ip; }
    bool getPlateSaturation() const { return plateSaturation; }
    bool getTubeCutoff() const { return tubeCutoff; }

    const MonitorValue<T>& getMonitorValue(){ 
        auto& v = monitor.values;
        v[MONITOR_TUBE_IK] = Ik;
        v[MONITOR_TUBE_IP] = Ip;
        v[MONITOR_TUBE_VG] = Vg;
        v[MONITOR_TUBE_VK] = Vk;
        v[MONITOR_TUBE_VP] = Vp;
        v[MONITOR_TUBE_SC] = tubeCutoff;
        v[MONITOR_TUBE_SP] = plateSaturation;
        return monitor;
    }

private:

    // ---------------------------------------------------------------------
    // Closed‑form triode computation 
    // ---------------------------------------------------------------------
    void triodeClosedForm () noexcept
    {
        const T v1 = half * ap;
        const T v2 = ak + v1 * bk_bp;
        const T alpha = kpg * (ag - v2) + kp;
        const T beta  = kp2 * (v1 - v2);
        const T eta   = k_eta * (beta + beta + alpha);
        const T v3    = eta + k_delta;
        const T delta = ap + v3;

        T bp_local, bk_local, Vpk_local;

#ifdef XSIMD_HPP
        // Compute the "delta >= 0" path
        const auto deltaMask = delta >= zero;

        const T safeDelta = xsimd::max(delta, zero);

        const T bp_delta = k_bp_s * xsimd::sqrt(safeDelta) - v3 - k_delta;
        const T d_delta = bk_bp * (ap - bp_delta);
        const T bk_delta = ak + d_delta;
        const T Vpk2 = ap + bp_delta - ak - bk_delta;

        const auto cutoffMask =(kpg * (ag - ak - half * d_delta) + kp2 * Vpk2 + kp) < zero;

        const T bp_cutoff = ap;
        const T bk_cutoff = ak;
        const T Vpk_cutoff = ap - ak;

        // Result if cutoff condition is false
        const T bp_active = bp_delta;
        const T bk_active = bk_delta;
        const T Vpk_active = half * Vpk2;

        // Select cutoff/non-cutoff within delta>=0 path
        const T bp_pos = xsimd::select(cutoffMask, bp_cutoff, bp_active);
        const T bk_pos = xsimd::select(cutoffMask, bk_cutoff, bk_active);
        const T Vpk_pos = xsimd::select(cutoffMask, Vpk_cutoff, Vpk_active);

        // Result for delta<0 path
        const T bp_neg = ap;
        const T bk_neg = ak;
        const T Vpk_neg = ap - ak;

        // Select delta>=0 vs delta<0
        bp_local  = xsimd::select(deltaMask, bp_pos, bp_neg);
        bk_local  = xsimd::select(deltaMask, bk_pos, bk_neg);
        Vpk_local = xsimd::select(deltaMask, Vpk_pos, Vpk_neg);

        // Final reverse-conduction correction
        const auto reverseMask = Vpk_local < zero;

        bp_local = xsimd::select(reverseMask, bp_ap_0 * ap + bp_ak_0 * ak, bp_local);
#else
        if (delta >= zero)
        {
            bp_local = k_bp_s * std::sqrt (delta) - v3 - k_delta;
            const T d = bk_bp * (ap - bp_local);
            bk_local = ak + d;
            const T Vpk2 = ap + bp_local - ak - bk_local;

            if (kpg * (ag - ak - half * d) + kp2 * Vpk2 + kp < zero)
            {
                bp_local = ap;
                bk_local = ak;
                Vpk_local = ap - ak;
                tubeCutoff = true;
            }
            else
            {
                Vpk_local = half * Vpk2;
                tubeCutoff = false;
            }
        }
        else
        {
            bp_local = ap;
            bk_local = ak;
            Vpk_local = ap - ak;
            tubeCutoff = true;
        }

        if (Vpk_local < zero)
        {
            bp_local = bp_ap_0 * ap + bp_ak_0 * ak;
            plateSaturation = true;
        }
        else {
            plateSaturation = false;
        }
#endif
        // Incident waves for the ports 
        bg = ag;          // grid wave passes through unchanged
        bk = bk_local;
        bp = bp_local;

        Vg = (ag+bg)*half;
        Vk = (ak+bk)*half;
        Vp = (ap+bp)*half;

        Ik = (ak-bk)*half/ R0k;
        Ip = (ap-bp)*half/ R0p;
    }


    // ---------------------------------------------------------------------
    // Member variables (mirroring the original class where appropriate)
    // ---------------------------------------------------------------------
    PortGType& port_g;
    PortKType& port_k;
    PortPType& port_p;

    // Model parameters (quadric surface)
    T kp;   // constant term
    T kp2;  // linear term
    T kpg;  // grid‑voltage coefficient

    //helpful
    const T zero = static_cast<T>(0.0f);
    const T half = static_cast<T>(0.5f);
    const T one = static_cast<T>(1.0f);

    T bk_bp;
    T k_eta;
    T k_delta;
    T k_bp_s;
    T bp_ap_0;
    T bp_ak_0;


    // Circuit comps for operating point
    T Rp {zero}, Rk {zero}, E {zero};

    // Port resistances (filled each compute call)
    T R0g{}, R0k{}, R0p{};

    // Reflected waves from the ports
    T ag{}, ak{}, ap{};

    // Incident waves to be written back
    T bg{}, bk{}, bp{};

    // monitoring
    T Vg{}, Vk{}, Vp{};
    T Ik{}, Ip{};
    bool tubeCutoff = false;
    bool plateSaturation = false;
    MonitorValue<T> monitor;
};
