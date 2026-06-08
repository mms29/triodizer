
#pragma once

#include <chowdsp_wdf/chowdsp_wdf.h>
#include <cmath>
#include <array>

using namespace chowdsp::wdft;

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
    TriodeQuadricWDF (PortGType& pg, PortKType& pk, PortPType& pp,
                      T kp_init, T kp2_init, T kpg_init, T Rp_init, T Rk_init, T E_init)
        : port_g (pg), port_k (pk), port_p (pp),
          kp (kp_init), kp2 (kp2_init), kpg (kpg_init),
          Rp(Rp_init), Rk(Rk_init), E(E_init)
    {
        port_g.connectToParent (this);
        port_k.connectToParent (this);
        port_p.connectToParent (this);
        initialiseOperatingPoint();
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

        // std::cout <<"R0g="<<R0g<<"; R0k="<<R0p<<"; R0p="<<R0g<< std::endl;

        // Closed‑form triode solution
        triodeClosedForm();

        // Update incident waves back to the ports
        port_g.incident (bg);
        port_k.incident (bk);
        port_p.incident (bp);
    }

    void setTubeLabParameters (T kp_val, T kp2_val, T kpg_val, T Rp_val, T Rk_val, T E_val) noexcept
    {
        kp  = kp_val;
        kp2 = kp2_val;
        kpg = kpg_val;
        Rp  = Rp_val;
        Rk  = Rk_val;
        E   = E_val;
        initialiseOperatingPoint();
    }

    // Accessors for monitoring 
    T getVg() const { return Vg; }
    T getVk() const { return Vk; }
    T getVp() const { return Vp; }
    T getR0g() const { return R0g; }
    T getR0k() const { return R0k; }
    T getR0p() const { return R0p; }

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
            }
            else
            {
                Vpk_local = half * Vpk2;
            }
        }
        else
        {
            bp_local = ap;
            bk_local = ak;
            Vpk_local = ap - ak;
        }

        if (Vpk_local < zero)
        {
            bp_local = bp_ap_0 * ap + bp_ak_0 * ak;
        }
#endif
        // Incident waves for the ports 
        bg = ag;          // grid wave passes through unchanged
        bk = bk_local;
        bp = bp_local;

        Vg = ag; // grid voltage is reflected wave on grid port
        Vk = ak; // cathode voltage is reflected wave on cathode port
        Vp = ap; // plate voltage is reflected wave on plate port
    }

    // ---------------------------------------------------------------------
    // Initial operating point 
    // ---------------------------------------------------------------------
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
    T Rp; 
    T Rk; 
    T E; 

    // Port resistances (filled each compute call)
    T R0g{}, R0k{}, R0p{};

    // Reflected waves from the ports
    T ag{}, ak{}, ap{};

    // Incident waves to be written back
    T bg{}, bk{}, bp{};

    // Node voltages for monitoring
    T Vg{}, Vk{}, Vp{};

};
