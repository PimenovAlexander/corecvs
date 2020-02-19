#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#include <random>

#include "buffers/voxels/voxelBuffer.h"

namespace corecvs {

class PerlinNoiseContainer : public  VoxelBufferBase<PerlinNoiseContainer, double>
{
public:
    PerlinNoiseContainer(int _l, int _h, int _w) :
        VoxelBufferBase(_l, _h, _w)
    {}
};

/**
 *  This is not a classical perlin noise, but similar
 **/
class PerlinNoise
{
public:
    static const int SIZE = 16;

    /**
     *  3d order hermitian polinom, needed as a smooth interpolator for [0..1]
     *  todo: move it close to lerp
     **/
    static double hermit3(double t)
    {
        return t * t * (3 - 2 * t);
    }

    static Vector3dd hermit3(Vector3dd t)
    {
        return Vector3dd(hermit3(t.x()), hermit3(t.y()), hermit3(t.z()));
    }

    PerlinNoiseContainer *n;


    PerlinNoise() {
        n = new PerlinNoiseContainer(SIZE, SIZE, SIZE);

        std::mt19937 mt;
        std::uniform_real_distribution<double> dis(0, 1);

        for (int i = 0; i < n->l; i++)
            for (int j = 0; j < n->h; j++)
                for (int k = 0; k < n->w; k++)
                    n->element(i,j,k) = dis(mt);

    }

    Vector3dd noise3d(const Vector3dd &v);
    double noise(const Vector3dd &v);

    Vector3dd turbulence3d(const Vector3dd &v, int force=4);
    double    turbulence  (const Vector3dd &v, int force=4);

    ~PerlinNoise() {
        delete_safe(n);
    }

private:
    void mapToArea(const Vector3dd &input, Vector3d32 &grid, Vector3dd &remainder);
    double query(Vector3d32 &grid, Vector3dd &r);

};

} // namespace corecvs

#endif // PERLINNOISE_H
