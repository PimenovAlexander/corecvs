/**
 * \file cascadeClassifier.h
 * \brief This file should hold the header for cascade classifier
 *
 * \ingroup cppcorefiles
 * \date Jun 24, 2010
 * \author alexander
 */

#ifndef CASCADECLASSIFIER_H_
#define CASCADECLASSIFIER_H_

#include "utils/global.h"

#include "buffers/g12Buffer.h"
#include "buffers/mipmapPyramid.h"
namespace corecvs {

class CascadeClassifier
{
public:
    //AbstractMidmapPyramid<G12Buffer> *

    CascadeClassifier();
    virtual ~CascadeClassifier();
};


} //namespace corecvs
#endif /* CASCADECLASSIFIER_H_ */

