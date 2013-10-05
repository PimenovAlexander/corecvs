#ifndef G12BUFFER3D_H
#define G12BUFFER3D_H

#include "global.h"

#include "vector3d.h"
#include "triangulator.h"
#include "abstractContiniousBuffer.h"

namespace corecvs {

typedef AbstractContiniousBuffer<SwarmPoint*, int32_t> G12Buffer3dBase;

class G12Buffer3d : public G12Buffer
{
private:
    static SwarmPoint *p0;
public:
    G12Buffer3d(int32_t h, int32_t w, bool shouldInit = true)
        : G12Buffer(h, w, shouldInit)
    { init(h, w); }

    G12Buffer3d(Vector2d<int32_t> size, bool shouldInit = true)
        : G12Buffer(size, shouldInit)
    { init(size.y(), size.x()); }

    G12Buffer3d(G12Buffer &that)
        : G12Buffer(that)
    { init(that.getH(), that.getW()); }

    G12Buffer3d(G12Buffer *that)
        : G12Buffer(that)
    { init(that->getH(), that->getW()); }

    G12Buffer3d(G12Buffer *src, int32_t x1, int32_t y1, int32_t x2, int32_t y2) :
        G12Buffer(src, x1, y1, x2, y2)
    { init(src->getH(), src->getW()); }

    G12Buffer3d(G12Buffer *src, vector<SwarmPoint> *sp) : G12Buffer(src)
    {
        init(src->getH(), src->getW());
        addPoints(sp);
    }

    G12Buffer3d(int32_t h, int32_t w, uint16_t *data) :
        G12Buffer(h, w, data)
    { init(h, w); }

    void init(int32_t h, int32_t w)
    {
        SwarmPoint* p1 = NULL;
        m3d = new G12Buffer3dBase(h, w, p1);
        mSP = NULL;
    }

    void addPoints(vector<SwarmPoint> *sp)
    {
        mSP = sp;

        if (mSP != NULL)
           for (size_t i = 0; i < mSP->size(); i++)
              m3d->element(fround((*mSP)[i].texCoor.y()),
                           fround((*mSP)[i].texCoor.x())) = &((*mSP)[i]);
//           foreach (SwarmPoint s, *mSP)
//               this->m3d->element(s.texCoor.y(), s.texCoor.x()) = &s;
    }

    G12Buffer3dBase *get3DBuffer() const
    {
        return m3d;
    }

    vector<SwarmPoint> *getSwarmPoints() const
    {
        return mSP;
    }

    void setSwarmPoints(vector<SwarmPoint> *sp)
    {
        mSP = sp;
    }

    virtual ~G12Buffer3d();

private:
    G12Buffer3dBase *m3d;
    vector<SwarmPoint> *mSP;
}; // G12Buffer3d

} //namespace corecvs

#endif // G12BUFFER3D_H
