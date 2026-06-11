#pragma once

#include <vector>

#include <Eigen/Dense>

template<typename T, int P, int N>
class RTBuilder
{
public:
    static constexpr int Np  = P;
    static constexpr int Nn  = N;
    static constexpr int Nnp  = N+P;

    using XMatrix = Eigen::Matrix<T, Nnp, Nnp>;
    using HMatrix = Eigen::Matrix<T, Np, Nnp>;
    using VMatrix = Eigen::Matrix<T, Nnp, Np>;
    using SMatrix = Eigen::Matrix<T, Np, Np, Eigen::RowMajor>;
    using DMatrix = Eigen::DiagonalMatrix<float, Np>;

    RTBuilder()
    {
        V.setZero();
        H.setZero();

        for (int i = 0; i < Np; ++i)
        {
            V(Nn + i, i) = T(1);
            H(i, Nn + i) = T(1);
        }
    }

    void computeS() const
    {
        Eigen::PartialPivLU<XMatrix> solver(X);

        VMatrix Y = solver.solve(V);

        S = SMatrix::Identity() + 2.0f * D * H * Y;
    }
protected:
    VMatrix V;
    HMatrix H;
    mutable SMatrix S;
    mutable XMatrix X;
    mutable DMatrix D;
};


class RTBuilderBassman : RTBuilder<float, 7, 11>{
public:
    float buildS(float* outputSMatrix, float Rb, float Rc,float Rd,float Re,float Rf,float Rg) const{

        float Ra = (Rb*Rc*Rd + (Rb*Rc + Rb*Rd)*Re + (Rb*Rc + Rc*Rd + (Rb + Rc + Rd)*Re)*Rf + ((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf)*Rg)/((Rb + Rc)*Rd + (Rb + Rc + Rd)*Re + (Rb + Rc + Rd)*Rf);
        // std::cout << "Ra2="<< Ra <<std::endl; 

        X <<      1/Ra,           0,           0,           0,       -1/Ra,           0,           0,           0,           0,           0,           0,           0,           1,           1,           0,           0,           0,           0,
                     0,        1/Rc,           0,           0,           0,           0,       -1/Rc,           0,           0,           0,           0,           0,           0,           0,           1,           1,           0,           0,
                     0,           0, 1/Rb + 1/Rd,           0,           0,       -1/Rb,           0,       -1/Rd,           0,           0,           0,           0,           0,           0,           0,           0,           1,           0,
                     0,           0,           0, 1/Re + 1/Rf,           0,           0,           0,           0,       -1/Re,       -1/Rf,           0,           0,           0,           0,           0,           0,           0,           1,
                 -1/Ra,           0,           0,           0,        1/Ra,           0,           0,           0,           0,           0,           0,          -1,           0,           0,           0,           0,           0,           0,
                     0,           0,       -1/Rb,           0,           0,        1/Rb,           0,           0,           0,           0,           0,           0,          -1,           0,           0,           0,           0,           0,
                     0,       -1/Rc,           0,           0,           0,           0,        1/Rc,           0,           0,           0,           0,           0,           0,          -1,           0,           0,           0,           0,
                     0,           0,       -1/Rd,           0,           0,           0,           0,        1/Rd,           0,           0,           0,           0,           0,           0,          -1,           0,           0,           0,
                     0,           0,           0,       -1/Re,           0,           0,           0,           0,        1/Re,           0,           0,           0,           0,           0,           0,          -1,           0,           0,
                     0,           0,           0,       -1/Rf,           0,           0,           0,           0,           0,        1/Rf,           0,           0,           0,           0,           0,           0,          -1,           0,
                     0,           0,           0,           0,           0,           0,           0,           0,           0,           0,        1/Rg,           0,           0,           0,           0,           0,           0,          -1,
                     0,           0,           0,           0,          -1,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,
                     1,           0,           0,           0,           0,          -1,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,
                     1,           0,           0,           0,           0,           0,          -1,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,
                     0,           1,           0,           0,           0,           0,           0,          -1,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,
                     0,           1,           0,           0,           0,           0,           0,           0,          -1,           0,           0,           0,           0,           0,           0,           0,           0,           0,
                     0,           0,           1,           0,           0,           0,           0,           0,           0,          -1,           0,           0,           0,           0,           0,           0,           0,           0,
                     0,           0,           0,           1,           0,           0,           0,           0,           0,           0,          -1,           0,           0,           0,           0,           0,           0,           0;


        Eigen::Matrix<float, Np, 1> r;
        r << Ra, Rb, Rc, Rd, Re, Rf, Rg;
        D = DMatrix(r);
        computeS();
        std::memcpy(outputSMatrix, S.data(), S.size() * sizeof(float));
        return Ra;
    };
};
class RTBuilderToneStack : RTBuilder<float, 9, 14>{
public:

    float buildS(float* outputSMatrix, float Rb, float Rc,float Rd,float Re,float Rf,float Rg,float Rh,float Ri) const {

        float Ra = ((Rb*Rc + (Rb + Rc)*Rd)*Re*Rf + ((Rb + Rd)*Re*Rf + (Rb*Rc + (Rb + Rc)*Rd)*Re)*Rg + ((Rb*Rc + (Rb + Rc)*Rd)*Re + (Rb*Rc + (Rb + Rc)*Rd)*Rf + (Rb*Rc + (Rb + Rc)*Rd + (Rb + Rd)*Re + (Rb + Rd)*Rf)*Rg)*Rh + ((Rb*Rc + (Rb + Rc)*Rd + (Rb + Rd)*Re)*Rf + (Rb*Rc + (Rb + Rc)*Rd + (Rb + Rd)*Re + (Rb + Rd)*Rf)*Rg + (Rb*Rc + (Rb + Rc)*Rd + (Rb + Rd)*Re + (Rb + Rd)*Rf)*Rh)*Ri)/((Rb*Rc + (Rb + Rc)*Rd + (Rc + Rd)*Re)*Rf + (Rb*Rc + (Rb + Rc)*Rd + (Rc + Rd)*Re + (Rb + Rd + Re)*Rf)*Rg + (Rb*Rc + (Rb + Rc)*Rd + (Rc + Rd)*Re + (Rc + Rd)*Rf + (Rb + Rc + Re + Rf)*Rg)*Rh + ((Rb + Rc + Re)*Rf + (Rb + Rc + Re + Rf)*Rg + (Rb + Rc + Re + Rf)*Rh)*Ri);

        X <<   1/Rg + 1/Rh,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,       -1/Rg,       -1/Rh,           0,           0,           0,           0,           0,           0,           0,           0,           0,           1,
                         0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           1,           1,           0,           1,           0,
                         0,           0, 1/Rc + 1/Rf,           0,           0,           0,           0,       -1/Rc,           0,           0,       -1/Rf,           0,           0,           0,           0,           0,           0,           0,           0,           0,           1,           0,           0,
                         0,           0,           0,        1/Rb,           0,           0,       -1/Rb,           0,           0,           0,           0,           0,           0,           0,           0,           0,           1,           1,           0,           0,           0,           0,           0,
                         0,           0,           0,           0, 1/Ra + 1/Re,       -1/Ra,           0,           0,           0,       -1/Re,           0,           0,           0,           0,           0,           1,           0,           0,           0,           0,           0,           0,           0,
                         0,           0,           0,           0,       -1/Ra,        1/Ra,           0,           0,           0,           0,           0,           0,           0,           0,          -1,           0,           0,           0,           0,           0,           0,           0,           0,
                         0,           0,           0,       -1/Rb,           0,           0,        1/Rb,           0,           0,           0,           0,           0,           0,           0,           0,          -1,           0,           0,           0,           0,           0,           0,           0,
                         0,           0,       -1/Rc,           0,           0,           0,           0,        1/Rc,           0,           0,           0,           0,           0,           0,           0,           0,          -1,           0,           0,           0,           0,           0,           0,
                         0,           0,           0,           0,           0,           0,           0,           0,        1/Rd,           0,           0,           0,           0,           0,           0,           0,           0,          -1,           0,           0,           0,           0,           0,
                         0,           0,           0,           0,       -1/Re,           0,           0,           0,           0,        1/Re,           0,           0,           0,           0,           0,           0,           0,           0,          -1,           0,           0,           0,           0,
                         0,           0,       -1/Rf,           0,           0,           0,           0,           0,           0,           0,        1/Rf,           0,           0,           0,           0,           0,           0,           0,           0,          -1,           0,           0,           0,
                     -1/Rg,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,        1/Rg,           0,           0,           0,           0,           0,           0,           0,           0,          -1,           0,           0,
                     -1/Rh,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,        1/Rh,           0,           0,           0,           0,           0,           0,           0,           0,          -1,           0,
                         0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,        1/Ri,           0,           0,           0,           0,           0,           0,           0,           0,          -1,
                         0,           0,           0,           0,           0,          -1,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,
                         0,           0,           0,           0,           1,           0,          -1,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,
                         0,           0,           0,           1,           0,           0,           0,          -1,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,
                         0,           0,           0,           1,           0,           0,           0,           0,          -1,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,
                         0,           1,           0,           0,           0,           0,           0,           0,           0,          -1,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,
                         0,           1,           0,           0,           0,           0,           0,           0,           0,           0,          -1,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,
                         0,           0,           1,           0,           0,           0,           0,           0,           0,           0,           0,          -1,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,
                         0,           1,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,          -1,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,
                         1,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,           0,          -1,           0,           0,           0,           0,           0,           0,           0,           0,           0;

        Eigen::Matrix<float, Np, 1> r;
        r << Ra, Rb, Rc, Rd, Re, Rf, Rg, Rh, Ri;
        D = DMatrix(r);
        computeS();
        std::memcpy(outputSMatrix, S.data(), S.size() * sizeof(float));
        return Ra;
    };
};
