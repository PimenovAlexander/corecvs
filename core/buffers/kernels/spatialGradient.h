#ifndef SPATIALGRADIENT_H_
#define SPATIALGRADIENT_H_

/**
 * \file spatialGradient.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Sep 22, 2010
 * \author alexander
 */

#include "utils/global.h"

#include "buffers/g12Buffer.h"
#include "buffers/abstractBuffer.h"
#include "math/vector/vector3d.h"
#include "buffers/integralBuffer.h"

#include "buffers/kernels/fastkernel/vectorTraits.h"
namespace corecvs {


template<typename ContanierType, int inputNumber, int outputNumber>
class ScalarAlgebraVector33d
{
public:
    typedef TraitGeneric<ContanierType> TraitVector33;
    typedef ScalarAlgebraMulti<TraitVector33, TraitVector33, inputNumber, outputNumber> Type;
};


/**
 * This class holds the spatial gradient matrix square elements
 *
 * \f{eqnarray*}
 * V_1 &=&  \left( \frac {\delta A(x,y)} {\delta x} \right)^2 \\
 * V_2 &=&  \left( \frac {\delta A(x,y)} {\delta y} \right)^2 \\
 * V_3 &=& \frac {\delta A(x,y)} {\delta x} \frac {\delta A(x,y)} {\delta y}
 * \f}
 *
 * TODO: Use mapper
 *
 **/
template<typename ContainerType>
class AbstractSpatialGradient : public AbstractBuffer<ContainerType, int32_t>
{
public:
    typedef ContainerType                             InternalContainerType;
    typedef typename ContainerType::InnerElementType  InternalDoubleType;

    template <int inputNumber, int outputNumber>
    using SpacialGradientScalarAlgebra = ScalarAlgebraVector33d<ContainerType, inputNumber, outputNumber>;


    AbstractSpatialGradient(int h, int w) : AbstractBuffer<ContainerType, int32_t>(h, w) {}
    AbstractSpatialGradient(const Vector2d<int32_t> &size) : AbstractBuffer<ContainerType, int32_t>(size) {}

    AbstractSpatialGradient(G12Buffer *input);

    G12Buffer* findCornerPoints(double scaler = G12Buffer::BUFFER_MAX_VALUE, int apperture = 5);
};

extern template class AbstractSpatialGradient<Vector3dd>;
extern template class AbstractSpatialGradient<Vector3df>;


typedef AbstractSpatialGradient<Vector3dd> SpatialGradient;
typedef AbstractSpatialGradient<Vector3df> SpatialGradientF;

typedef IntegralBuffer<Vector3dd, Vector3dd, int32_t>  SpatialGradientIntegralBuffer;


} //namespace corecvs
#endif /* SPATIALGRADIENT_H_ */

