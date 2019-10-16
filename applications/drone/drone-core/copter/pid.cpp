#include <iostream>
#include <cmath>
#include "pid.h"
#include <limits>

using namespace std;

class PIDImpl
{
    public:
        PIDImpl( double max, double min, double Kp, double Kd, double Ki );
        ~PIDImpl();
        double calculate( double dt, double setpoint, double pv );

    private:
        double _dt;
        double _max;
        double _min;
        double _Kp;
        double _Kd;
        double _Ki;
        double _pre_error;
        double _integral;
};


PID::PID( double max, double min, double Kp, double Kd, double Ki )
{
    pimpl = new PIDImpl(max,min,Kp,Kd,Ki);
}

PID::PID( double Kp, double Kd, double Ki )
{
    pimpl = new PIDImpl(std::numeric_limits<double>::max(), std::numeric_limits<double>::min(), Kp, Kd, Ki);
}

double PID::calculate( double dt, double wantedPoint, double currentPoint )
{
    return pimpl->calculate(dt, wantedPoint, currentPoint);
}

PID::~PID()
{
    delete pimpl;
}


/**
 * Implementation
 */
PIDImpl::PIDImpl( double max, double min, double Kp, double Kd, double Ki ) :
    _dt(0),
    _max(max),
    _min(min),
    _Kp(Kp),
    _Kd(Kd),
    _Ki(Ki),
    _pre_error(0),
    _integral(0)
{
}

double PIDImpl::calculate( double dt, double wantedPoint, double currentPoint )
{
    _dt = dt;

    // Calculate error
    double error = wantedPoint - currentPoint;

    // Proportional term
    double Pout = _Kp * error;

    // Integral term
    _integral += error * _dt;
    double Iout = _Ki * _integral;

    // Derivative term
    double derivative = (error - _pre_error) / _dt;
    double Dout = _Kd * derivative;

    // Calculate total output
    double output = Pout + Iout + Dout;

    // Restrict to max/min
    if( output > _max )
        output = _max;
    else if( output < _min )
        output = _min;

    // Save error to previous error
    _pre_error = error;

    return output;
}

PIDImpl::~PIDImpl()
{
}
