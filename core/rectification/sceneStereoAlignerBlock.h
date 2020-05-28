#ifndef SCENE_STEREO_ALIGNER_BLOCK_H
#define SCENE_STEREO_ALIGNER_BLOCK_H

#include "filters/newstyle/newStyleBlock.h"
#include "xml/generated/sceneStereoAlignerBlockBase.h"

#include "camerafixture/fixtureCamera.h"

namespace corecvs {

class SceneStereoAlignerBlock : public SceneStereoAlignerBlockBase, public NewStyleBlock
{
public:
    SceneStereoAlignerBlock();


    virtual int operator ()();

    ~SceneStereoAlignerBlock() {
    }

    FixtureCamera *camera1 = NULL;
    FixtureCamera *camera2 = NULL;

    ProjectiveTransform  leftTransform = ProjectiveTransform::Identity();
    ProjectiveTransform rightTransform = ProjectiveTransform::Identity();

    Polygon leftArea;
    Polygon rightArea;
};

} // namespace corecvs

#endif // SCENE_STEREO_ALIGNER_BLOCK_H
