#ifndef TRIODEWDF_H_INCLUDED
#define TRIODEWDF_H_INCLUDED

#include <chowdsp_wdf/chowdsp_wdf.h>
#include <cmath>

using namespace chowdsp::wdft;

template <typename T, typename PortGType, typename PortKType, typename PortPType>

class TriodeWDF final : public RootWDF
{
public:
    TriodeWDF(PortGType& pg, PortKType& pk, PortPType& pp, T Vk_init, T Vp_init) 
        : port_g (pg), port_k (pk), port_p (pp), Vk (Vk_init), Vp (Vp_init)
    {
        port_g.connectToParent (this);
        port_k.connectToParent (this);
        port_p.connectToParent (this);
        setTriodeParameters ();
    }
    inline void calcImpedance() override
    {
    }

    void compute() noexcept
    {
        // 1. pull incident waves
        ag = port_g.reflected();
        ak = port_k.reflected();
        ap = port_p.reflected();
        R0g = port_g.wdf.R;
        R0k = port_k.wdf.R;
        R0p = port_p.wdf.R;

        // 2. solve nonlinearity
        solveTriode();


        T Vkak = Vk - ak;
        T Vgag = Vg - ag;
        T bpx  = R0p * (Vgag / R0g + Vkak / R0k);

        bg   = Vg + Vgag;
        bk   = Vk + Vkak;
        bp   = ap - bpx - bpx;

        Vp = ap - R0p * ((Vg - ag) / R0g + (Vk - ak) / R0k);

        port_g.incident (bg);
        port_k.incident (bk);
        port_p.incident (bp);
    }
    void solveTriodeOld() noexcept
    {
        // Vk with no grid current
        Vg = ag;
        Vk = solve_Vk(true);

        // check grid condition
        if ((Vg - Vk) <= Voff)
            return;

        int ping = 0;
        while (ping < maxPingPongIters)
        {
            T Vk_start = Vk;
            T Vg_start = Vg;
            // --- grid solve ---
            Vg = solve_Vg();

            // --- cathode solve ---
            Vk = solve_Vk(false);

            // MATLAB-style convergence check:
            T err_k = std::abs (Vk - Vk_start);
            T err_g = std::abs (Vg - Vg_start);

            if (err_k < epsVk && err_g < epsVgVk)
                break;

            ++ping;
            PPIters=ping;

        }
    }

    // T solve_Vp(){
    //     // EQ7 
    //     return ap - R0p * ((Vg - ag) / R0g + (Vk - ak) / R0k);
    // }


    T solve_Vk(bool init) noexcept
    {
        T Vk_prev;
        if(init) {
            Vk_prev = ak; 
        }else{
            Vk_prev = Vk;
        } 

        T f_prev = fVk (Vk_prev);
        T Vk_curr;
        if (std::abs(Vk_prev - ak) < eps)
            Vk_curr = Vk_prev + std::abs(f_prev);
        else
            Vk_curr = Vk_prev + f_prev;
        T f_curr = fVk (Vk_curr);

        // calculate error
        T err = std::abs(Vk_curr -Vk_prev);

        int iter = 0;

        while (err > epsVk && iter < maxVkIters)
        {
            // Secant method
            T denom = f_curr - f_prev;
            if (std::abs (denom) < eps) // Check div by zero
                break;

            T Vk_next = Vk_curr - f_curr * (Vk_curr - Vk_prev) / denom;

            // EQ.13 forces the range of Vk
            T Vk_min = ak + (R0k / R0g) * (ag - Vg);
            Vk_next = std::max(Vk_next, ak);
            Vk_next = std::max(Vk_next, Vk_min);

            // update states
            Vk_prev = Vk_curr;
            f_prev  = f_curr;

            Vk_curr = Vk_next;
            f_curr = fVk (Vk_curr);
            // update error
            err = std::abs(Vk_curr -Vk_prev);

            ++iter;
            VkIters=iter;
        }

        return Vk_curr;
    }

    T solve_Vg() noexcept
    {
        T Vg_prev = Vg;
        T f_prev  = fVg(Vg_prev);
        T Vg_curr;

        T Vg_min = ag + (R0g / R0k) * (ak - Vk);
        if (std::abs(Vg_prev - ag) < eps) Vg_curr = Vg_prev - std::abs (f_prev);
        else if (std::abs(Vg_prev - Vg_min) < eps) Vg_curr = Vg_prev + std::abs (f_prev);
        else Vg_curr = Vg_prev + f_prev;

        // EQ13 clamp
        Vg_curr = std::clamp(Vg_curr, Vg_min, ag);

        T f_curr = fVg (Vg_curr);
        T err = std::abs(Vg_curr -Vg_prev);

        int iter = 0;

        while ( err > epsVg && iter < maxVgIters)
        {
            // Secand method
            T denom = f_curr - f_prev;
            if (std::abs (denom) < eps)
                break;

            T Vg_next = Vg_curr - f_curr * (Vg_curr - Vg_prev) / denom;

            // EQ13 clamp
            Vg_min = ag + (R0g / R0k) * (ak - Vk);
            Vg_next = std::clamp(Vg_next, Vg_min, ag);

            Vg_prev = Vg_curr;
            f_prev  = f_curr;

            Vg_curr = Vg_next;
            f_curr  = fVg (Vg_curr);

            err = std::abs(Vg_curr -Vg_prev);

            ++iter;
            VgIters=iter;

        }

        return Vg_curr;
    }
    void solveTriode() noexcept
    {

        // --- Vk without grid current ---
        
        Vg = ag;
        T Vk_prev = ak; 
        T f_Vk_prev = fVk (Vk_prev);
        Vk = ak + std::abs(f_Vk_prev);

        int iter = 0;
        T err_k ;
        do 
        {
            T f_Vk = fVk (Vk);
            T df_Vk = f_Vk - f_Vk_prev;
            if (std::abs (df_Vk) < eps)
                break;

            T Vk_next = Vk - f_Vk * (Vk - Vk_prev) / df_Vk;

            Vk_next = std::max(Vk_next, ak);

            // update states
            Vk_prev = Vk;
            Vk  = Vk_next;
            f_Vk_prev = f_Vk;
            err_k   = Vk - Vk_prev;

            ++iter;
            VkIters=iter;
        } 
        while (std::abs(err_k) > epsVk && iter < maxVkIters);

        if ((Vg - Vk) <= Voff)
            return;

        // --- Grid Current ---
        err_k = ak - Vk;
        int ping = 0;
        do{

            // --- Calculate Vg ---

            T Vg_min = ag + R0g / R0k * (ak - Vk);
            T Vg_start = Vg;
            T Vg_prev = Vg;
            T f_Vg_prev  = fVg(Vg_prev);

            if (std::abs(Vg_prev - ag) < eps) 
                Vg = Vg - std::abs (f_Vg_prev);
            else if (std::abs(Vg_prev - Vg_min) < eps) 
                Vg = Vg + std::abs (f_Vg_prev);
            else 
                Vg = Vg + f_Vg_prev;
            Vg = std::clamp(Vg, Vg_min, ag);

            T err_g = Vg -Vg_prev;
            iter = 0;
            do
            {
                T f_Vg = fVg (Vg);
                T df_Vg = f_Vg - f_Vg_prev;
                if (std::abs(df_Vg) < eps)
                    break;

                T Vg_next = Vg - f_Vg * (Vg - Vg_prev) / df_Vg;

                Vg_next = std::clamp(Vg_next, Vg_min, ag);

                Vg_prev = Vg;
                Vg = Vg_next;
                f_Vg_prev  = f_Vg;
                err_g   = Vg - Vg_prev;


                ++iter;
                VgIters=iter;

            } while ( std::abs(err_g) > epsVg && iter < maxVgIters);

            err_g = Vg_start - Vg;
            if (abs(err_k) <= epsVk && abs(err_g) <= epsVg)
                break;
            
            // --- calculate Vk ---
            T Vk_min   = ak + R0k / R0g * (ag - Vg);
            T Vk_start = Vk;
            Vk_prev  = Vk;
            f_Vk_prev = fVk (Vk_prev);
            if (std::abs(Vk_prev - ak) < eps || std::abs(Vk_prev - Vk_min) < eps )
                Vk = Vk_prev + std::abs(f_Vk_prev);
            else
                Vk = Vk + f_Vk_prev;
            Vk = std::max(Vk, ak);
            Vk = std::max(Vk, Vk_min);

            err_k   = Vk - Vk_prev;
            iter = 0;
            do
            {
                T f_Vk = fVk (Vk);
                T df_Vk = f_Vk - f_Vk_prev;

                if (std::abs (df_Vk) < eps) 
                    break;

                T Vk_next = Vk - f_Vk * (Vk - Vk_prev) / df_Vk;

                Vk_next = std::max(Vk_next, ak);
                Vk_next = std::max(Vk_next, Vk_min);

                // update states
                Vk_prev = Vk;
                Vk = Vk_next;
                f_Vk_prev  = f_Vk;
                err_k = Vk -Vk_prev;

                ++iter;
                VkIters=iter;
            } while (std::abs(err_k) > epsVk && iter < maxVkIters);

            err_k = (Vk - Vk_start);

            if (err_k < epsVk && err_g < epsVg)
                break;

            ++ping;
            PPIters=ping;

        }while (ping < maxPingPongIters);
    }


    // T fVk_no_grid_current(T Vk_val) noexcept
    // {
    //     // EQ10
    //     T Vgk = Vg - Vk_val;

    //     // polynomial parameters
    //     T G_val  = ((G[3]  * Vgk + G[2])  * Vgk + G[1])  * Vgk + G[0];
    //     T mu_val = ((mu[3] * Vgk + mu[2]) * Vgk + mu[1]) * Vgk + mu[0];
    //     T h_val  = ((h[3]  * Vgk + h[2])  * Vgk + h[1])  * Vgk + h[0];

    //     // safety: tube conductance must never vanish
    //     mu_val = std::max(mu_val, eps);

    //     // α computation 
    //     T alpha = (Vk_val - ak) / (R0k * G_val);
    //     alpha = std::max(alpha, T(0));
    //     alpha = std::cbrt(alpha * alpha);

    //     T num = R0k * (ap + mu_val * (Vg + h_val - alpha)) + R0p * ak;
    //     T den = R0p + (mu_val + T(1)) * R0k;

    //     return (num / den) - Vk_val;
    // }

    T fVk(T Vk_val) noexcept
    {
        // EQ8 
        T Vgk = Vg - Vk_val;
        T agVg = ag - Vg;

        // polynomial parameters
        T G_val  = ((G[3]  * Vgk + G[2])  * Vgk + G[1])  * Vgk + G[0];
        T mu_val = ((mu[3] * Vgk + mu[2]) * Vgk + mu[1]) * Vgk + mu[0];
        T h_val  = ((h[3]  * Vgk + h[2])  * Vgk + h[1])  * Vgk + h[0];

        G_val = std::max(G_val, eps);
        mu_val = std::max(mu_val, eps);

        // α computation 
        T alpha = (Vk_val - ak) / (R0k * G_val);
        alpha = std::max(alpha, T(0));
        alpha = std::cbrt(alpha * alpha);

        return (R0g * (R0p * ak + R0k * (ap + mu_val * (Vg + h_val - alpha))) 
         + R0k * R0p * agVg) / (R0g * (R0p + (mu_val - 1.0) * R0k)) - Vk_val;
    }
    T fVg(T Vg_val) noexcept
    {
        // EQ12 
        T agVg = ag - Vg_val;
        T akVk = ak - Vk;
        if (std::abs(agVg) < eps)
            return Voff + Vk - Vg_val;

        T beta = - (R0g / R0k * akVk / agVg + 1.0) / D;
        if (beta < 0)
        {
            return (R0k * (R0g * (ap - Vk) + R0p * ag) + R0g * R0p * akVk) / (R0k * R0p) - Vg_val;
        }
        else
        {
            beta = std::pow(beta, T(1) / K);
        }
        if (!std::isfinite(beta))
            return Voff + Vk - Vg_val;

        return (R0k * (R0g * ((Voff + Vk) * beta - Vk + ap) + R0p * ag) 
             + R0g * R0p * akVk) / (R0k * (R0g * beta + R0p)) - Vg_val;
    }

    void setTriodeParameters() noexcept
    {
        // G polynomial coefficients
        // MATLAB units:
        // G(Vgk) = G0 + G1*Vgk + G2*Vgk² + G3*Vgk³
        G[0] = (T) 1.102e-3;   // 1.102 mA
        G[1] = (T) 15.12e-6;   // 15.12 µA
        G[2] = (T)-31.56e-6;   // -31.56 µA
        G[3] = (T)-3.286e-6;   // -3.286 µA

        // μ polynomial coefficients
        // μ(Vgk) = μ0 + μ1*Vgk + μ2*Vgk² + μ3*Vgk³

        mu[0] = (T) 99.705;
        mu[1] = (T)-22.98e-3;  // -22.98 / kV = -0.02298 / V
        mu[2] = (T)-0.4489;
        mu[3] = (T)-22.27e-3;  // -22.27 / kV³ = -0.02227 / V³


        // h polynomial coefficients
        // h(Vgk) = h0 + h1*Vgk + h2*Vgk² + h3*Vgk³
        h[0] = (T) 0.6;
        h[1] = (T) 0.0;
        h[2] = (T) 0.0;
        h[3] = (T) 0.0;


        // Grid current parameters
        Voff = (T)-0.2;
        D    = (T) 0.12;
        K    = (T) 1.1;
    }

    // WDFMembers<T> wdf;
    PortGType& port_g;
    PortKType& port_k;
    PortPType& port_p;

public:
    // Getter methods for accessing private members
    T getVg() const { return Vg; }
    T getVk() const { return Vk; }
    T getVp() const { return Vp; }
    T getAk() const { return ak; }
    T getAg() const { return ag; }
    T getAp() const { return ap; }
    T getR0p() const { return R0p; }
    T getR0g() const { return R0g; }
    T getR0k() const { return R0k; }
    T getVgIters() const { return VgIters; }
    T getVkIters() const { return VkIters; }
    T getPPIters() const { return PPIters; }

private:
    std::array<T, 4> G {};
    std::array<T, 4> mu {};
    std::array<T, 4> h {};
    T D, K, Voff;

    T Vk, Vg, Vp;
    T R0k, R0g, R0p;
    T ak, ag, ap;
    T bk, bg, bp;
    int VgIters, VkIters, PPIters;

    static constexpr T epsVgVk = (T) 1.0e-6;
    static constexpr T eps = (T) 1.0e-9;

    static constexpr int maxVkIters = 20;
    static constexpr int maxVgIters = 20;

    static constexpr int maxPingPongIters = 50;
};

#endif TRIODEWDF_H_INCLUDED
