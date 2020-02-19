/**
 * \file classicKalman.cpp
 * \brief Add Comment Here
 *
 * \date Jun 5, 2011
 * \author alexander
 */

#include "kalman/classicKalman.h"
namespace corecvs {

ClassicKalman::~ClassicKalman()
{
    // TODO Auto-generated destructor stub
}

/**
 * Prediction for Kalman.
 *
 *  \f[x_{k|k-1} = f(x_{k-1|k-1}, u_{k-1})\f]
 *
 *  \f[ P_{k|k-1} = F_{k-1} P_{k-1|k-1} F_{k-1}^\top + Q_{k-1} \f]
 **/
void ClassicKalman::predict(void)
{
    f->operator()(x, xPredicted);
    Matrix F = f->getJacobian(xPredicted);
    P = (F * P * F.t()) + Q;
}


/**
 * Innovation for Kalman.
 *
 * Innovation or measurement residual
 * \f[ y_{k} = z_{k} - h(x_{k|k-1}) \f]
 * Innovation (or residual) covariance
 * \f[ S_{k} = H_{k} P_{k|k-1} H_{k}^\top + R_{k} \f]
 * Near-Optimal Kalman gain
 * \f[ K_{k} = P_{k|k-1} H_{k}^\top S_{k}^{-1} \f]
 * Updated state estimate
 * \f[ x_{k|k} = x_{k|k-1} + {K}_{k} y_{k} \f]
 * Updated estimate covariance
 * \f[ P_{k|k} = (I - K_{k} H_{k}) P_{k|k-1} \f]
 *
 * */
void ClassicKalman::update(void)
{
    Vector zPredicted(measureDim);
    Vector y(measureDim);

    h->operator ()(xPredicted, zPredicted);
    for (int i = 0; i < measureDim; i++)
    {
        y[i] = z[i] - zPredicted[i];
    }

    Matrix H = h->getJacobian(xPredicted);

    Matrix HT = H.t();

    Matrix S = H * P * HT + R;

    Matrix K = P * HT * S.inv();

#ifdef TRACE
    cout << "P" << endl << P << endl;
    cout << "H" << endl << H << endl;
    cout << "HT" << endl << HT << endl;
    cout << "S" << endl << S << endl;
    cout << "K" << endl << K << endl;
#endif

    x = xPredicted + K * y;
    P = (Matrix(stateDim, stateDim, 1.0) - K * H) * P;
}



} //namespace corecvs

