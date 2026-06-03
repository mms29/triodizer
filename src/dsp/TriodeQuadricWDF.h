/*
 * TriodeQuadricWDF.h
 *
 * Implements the Quadric Surface Model of a triode (see triode_stage_quadric.m).
 * This version replaces the iterative Cardarilli solution with a closed‑form
 * computation, eliminating the inner Newton‑Raphson loops and thus reducing
 * per‑sample CPU cost.
 *
 * The class mirrors the interface of TriodeWDF so it can be swapped in the
 * existing circuit classes without further changes.
 */

#ifndef TRIODEQUADRICWDF_H_INCLUDED
#define TRIODEQUADRICWDF_H_INCLUDED

#include <chowdsp_wdf/chowdsp_wdf.h>
#include <cmath>
#include <array>

using namespace chowdsp::wdft;

/**
 * Quadric surface model implementation.
 *
 * Template parameters:
 *   T            – sample type (float or double)
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
        // Connect ports to the WDF tree
        port_g.connectToParent (this);
        port_k.connectToParent (this);
        port_p.connectToParent (this);
        // Compute static operating point (Vk0, Vp0) – same as MATLAB script
        initialiseOperatingPoint();
    }

    // No‑op – the model does not need to compute impedance.
    inline void calcImpedance() override {}

    /** Process one sample. Must be called after the incident waves have been
     *  pulled from the ports (as done in the original TriodeWDF).
     */
    void compute() noexcept
    {
        // 1. Pull incident waves from the three ports
        ag = port_g.reflected();
        ak = port_k.reflected();
        ap = port_p.reflected();
        R0g = port_g.wdf.R;
        R0k = port_k.wdf.R;
        R0p = port_p.wdf.R;

        // 2. Closed‑form triode solution
        // The helper returns bg, bk, bp (grid, cathode, plate incident waves)
        triodeClosedForm();

        // 3. Update incident waves back to the ports
        port_g.incident (bg);
        port_k.incident (bk);
        port_p.incident (bp);
    }

    /** Set the quadratic model parameters.
     *  The values are taken from the MATLAB implementation:
     *   kp  – constant term of the triode characteristic
     *   kp2 – linear term coefficient
     *   kpg – grid‑voltage coefficient
     */
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

    // Accessors for monitoring (same as in the original class)
    T getVg() const { return Vg; }
    T getVk() const { return Vk; }
    T getVp() const { return Vp; }
    T getR0g() const { return R0g; }
    T getR0k() const { return R0k; }
    T getR0p() const { return R0p; }

private:
    // ---------------------------------------------------------------------
    // 1)  Closed‑form triode computation (port‑wave formulation)
    // ---------------------------------------------------------------------
    void triodeClosedForm () noexcept
    {
        // The MATLAB `triode` function returns bg (grid wave = ag), bk, bp.
        // bg is simply ag, so we only need to compute bk and bp.
        // All calculations follow the MATLAB code verbatim, using T as the
        // scalar type.

        // Pre‑compute a few constants that depend only on the port resistances
        const T bk_bp = R0k / R0p;
        const T k_eta = static_cast<T>(1) / (bk_bp * (static_cast<T>(0.5) * kpg + kp2) + kp2);
        const T k_delta = kp2 * k_eta * k_eta / (R0p + R0p);
        const T k_bp_s = k_eta * std::sqrt ((kp2 + kp2) / R0p);
        const T bp_k = static_cast<T>(1) / (R0p + R0k);
        const T bp_ap_0 = bp_k * (R0k - R0p);
        const T bp_ak_0 = bp_k * (R0p + R0p);

        const T v1 = static_cast<T>(0.5) * ap;
        const T v2 = ak + v1 * bk_bp;
        const T alpha = kpg * (ag - v2) + kp;
        const T beta  = kp2 * (v1 - v2);
        const T eta   = k_eta * (beta + beta + alpha);
        const T v3    = eta + k_delta;
        const T delta = ap + v3;

        T bp_local, bk_local, Vpk_local;
        if (delta >= static_cast<T>(0))
        {
            bp_local = k_bp_s * std::sqrt (delta) - v3 - k_delta;
            const T d = bk_bp * (ap - bp_local);
            bk_local = ak + d;
            const T Vpk2 = ap + bp_local - ak - bk_local;

            if (kpg * (ag - ak - static_cast<T>(0.5) * d) + kp2 * Vpk2 + kp < static_cast<T>(0))
            {
                bp_local = ap;
                bk_local = ak;
                Vpk_local = ap - ak;
            }
            else
            {
                Vpk_local = static_cast<T>(0.5) * Vpk2;
            }
        }
        else
        {
            bp_local = ap;
            bk_local = ak;
            Vpk_local = ap - ak;
        }

        if (Vpk_local < static_cast<T>(0))
        {
            bp_local = bp_ap_0 * ap + bp_ak_0 * ak;
        }

        // Incident waves for the ports (grid wave is unchanged)
        bg = ag;          // grid wave passes through unchanged
        bk = bk_local;
        bp = bp_local;

        // Update internal node voltages for monitoring – same definitions as
        // the original Cardarilli version.
        Vg = ag; // grid voltage is reflected wave on grid port
        Vk = ak; // cathode voltage is reflected wave on cathode port
        Vp = ap; // plate voltage is reflected wave on plate port
    }

    // ---------------------------------------------------------------------
    // 2)  Initial operating point – computes Vk0 and Vp0 from the quadric
    //     formulas (see triode_stage_quadric.m). This is called from the
    //     constructor and whenever the model parameters are changed.
    // ---------------------------------------------------------------------
    void initialiseOperatingPoint () noexcept
    {
        T k1 = kpg/(2*kp2) + Rp/Rk + 1;
        T k2 = k1 * (kp/kp2 + 2*E) * kp2;
        T k3 = Rk * k2 + 1;
        T k1sign = (k1 > 0.0f) ? 1.0f : (k1 < 0.0f ? -1.0f : 0.0f);
        T Vk0 = (k3 - k1sign * std::sqrt(2*k3 - 1)) / (2 * Rk * k1 * k1 * kp2);
        T Vp0 = E - Rp/Rk * Vk0;

        Vk = Vk0;
        Vp = Vp0;
        Vg = static_cast<T>(0);
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

    // No iterative counters needed for the quadric model
    static constexpr T eps = static_cast<T>(1e-9);
};

#endif // TRIODEQUADRICWDF_H_INCLUDED
