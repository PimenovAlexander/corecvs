#ifndef CORECVS_COMPLEXWAVEFILTER_H
#define CORECVS_COMPLEXWAVEFILTER_H


class ComplexWaveFilter {
public:
    double h[6];
    double g[6];
    double hi[6];
    double gi[6];
    ComplexWaveFilter() {
        // Complex Daubechies
        // Real lowpass filter

        h[0] = -0.0662912607;
        h[1] = 0.1104854346;
        h[2] = 0.6629126074;
        h[3] = 0.6629126074;
        h[4] = 0.1104854346;
        h[5] = -0.0662912607;

        // Real highpass filter
        g[5] = 0.0662912607;
        g[4] = 0.1104854346;
        g[3] = -0.6629126074;
        g[2] = 0.6629126074;
        g[1] = -0.1104854346;
        g[0] = -0.0662912607;

        // imaginary lowpass filter
        hi[0] = -0.0855816496;
        hi[1] = -0.0855816496;
        hi[2] = 0.1711632992;
        hi[3] = 0.1711632992;
        hi[4] = -0.0855816496;
        hi[5] = -0.0855816496;

        // Imaginary highpass filter
        gi[5] = -0.0855816496;
        gi[4] = 0.0855816496;
        gi[3] = 0.1711632992;
        gi[2] = -0.1711632992;
        gi[1] = -0.0855816496;
        gi[0] = 0.0855816496;
    }

};


#endif //CORECVS_COMPLEXWAVEFILTER_H
