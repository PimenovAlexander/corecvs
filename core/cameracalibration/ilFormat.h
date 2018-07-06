#ifndef ILFORMAT_H
#define ILFORMAT_H

#include "core/camerafixture/fixtureScene.h"

namespace corecvs {

/**
 *   https://portal.lanit-tercom.com/projects/topcon/wiki/IL
 **/

class ILFormat
{
public:
    ILFormat();

    FixtureScene *load(const std::string &dirPath);

    bool save(FixtureScene *scene, const std::string &dirPath);

    /* some file formats */

    /**
     * Function that loads from following format
     *
     * \f[ s \quad c_x \quad c_y \quad i \quad n_0 \quad n_2 \quad ... \quad n_i \f]
     *
     * \f[ p = (u, v) \f]
     * \f[ (u_1, v_1)=(u, v)/s-(cx,cy) \quad r = \vert| (u1, v1) \vert|  \f]
     * \f[ P(r) = n_0 + 0 \cdot r + n_2 r^2 + n_3 r^3 + \cdots + n_i r^{i} \f]
     *  Output ray
     * \f[ d = (u_1, v_1, P(r)) \f]
     *
     * To match our format in OmnidirectionalProjection
     *
     * \f[ (P(r) / n_0) = 1 + 0 \cdot r + {n_2 \over n_0} r^2 + {n_3 \over n_0} r^3 + \cdots + {n_i \over n_0} r^{i} \f]
     *
     * \f[ d = (u_1, v_1, P(r)) \sim (u_1 / n_0, v_1 / n_0, P(r) / n_0) \f]
     * \f[ (u_1 / n_0, v_1 / n_0)= ((u, v)/s-(c_x,c_y)) / n_0 = (u-s c_x, v-s c_y) / (s  n_0) \f]
     * \f[ r / n_0 = \vert| (u_1, v_1) \vert| = \vert| (u_1/n_0, v_1/n_0) \vert| \f]
     *
     * Now we have
     * \f[ (P(r) / n_0) = 1 + 0 \cdot {r \over n_0} + {n_2 \over n_0} n_0^2 {r^2 \over n_0^2} + {n_3 \over n_0} n_0^3 {r^3 \over n_0^3} + \cdots + {n_i \over n_0} n_0^i {r^i \over n_0^i} \f]
     * And we expect to have the following
     * \f[ (P(r) / n_0) = (P^1(r / n_0) / n_0) = 1 + 0 \cdot r + n_2^1 r^2 + n_3^1 r^3 + \cdots + n_i^1 r^{i} \f]
     *
     *
     * So \f{eqnarray*}
     *   p_{principal} &=& (s c_x, s c_y) \\
     *               f &=&  s  n_0      \\
     *            n_0^1  :&=& n_2  n_0  \\
     *            n_1^1  :&=& n_3  n_0^2  \\
     *                   &\cdots&             \\
     *            n_i^1  :&=& n_{i+2}  n_0^{i-1}
     * \f}
     *
     *
     **/

    static const std::string IL_OMNIDIRECTIONAL;
    static const std::string IL_PROJECTIVE;

    /*interfacing to streams*/
    static CameraProjection *loadIntrisics(std::istream &filename);
    static void              saveIntrisics(const CameraProjection &projection, std::ostream &stream);

    /*interfacing to files*/
    static CameraProjection *loadIntrisics(const std::string &filename);
    static void              saveIntrisics(const CameraProjection &projection, const std::string &stream);


    /* Helper to serialise Affine3DQ*/
    static void      saveExtrinsics(const Affine3DQ &transform, std::ostream &stream);
    static Affine3DQ loadExtrinsics(std::istream &stream);



};

} // namespace corecvs

#endif // ILFORMAT_H
