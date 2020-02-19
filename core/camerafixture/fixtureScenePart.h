#ifndef FIXTURE_SCENE_PART_H
#define FIXTURE_SCENE_PART_H

#include "utils/global.h"
#include "utils/atomicOps.h"
#include "geometry/polygons.h"

namespace corecvs {

class FixtureScene;

/**
 * This class is so far just a common base for all objects in scene heap.
 * Should bring this to other file
 **/
class FixtureScenePart {

public:
    /* No particular reason for this, except to encourage leak checks */
    static atomic_int OBJECT_COUNT;

    FixtureScene *ownerScene;

    /**
     * Ok this id should be reworked...
     * Nice idea is for the id to be pointer itself.
     *
     * This can't be true in case of the moment after serialisation...
     * we need to invent a way how to make sure it is ok.
     *
     **/

     typedef uint64_t IdType;
private:
    IdType id;
public:
    IdType getObjectId() {
        if (id == 0) {
            id = reinterpret_cast<IdType>(this);
        }
        return id;
    }

    void setObjectId(IdType id) {
        this->id = id;
    }

    /* We could have copy constructors and stuff... but so far this is enough */
    FixtureScenePart(FixtureScene * owner = NULL) :
        ownerScene(owner),
        id (0)
    {
        atomic_inc_and_fetch(&OBJECT_COUNT);
    }


    virtual ~FixtureScenePart() {
        atomic_dec_and_fetch(&OBJECT_COUNT);
    }
};



} // namespace corecvs

#endif // FIXTURE_SCENE_PART_H
