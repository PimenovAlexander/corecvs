#ifndef PUZZLEBLOCK_H
#define PUZZLEBLOCK_H

namespace corecvs {

class PuzzleBlock
{
public:
    int h;
    int w;
    int stride;
    uint16_t *data;

    PuzzleBlock(
        int _h,
        int _w,
        int _stride,
        uint16_t *_data
    ) :
        h(_h),
        w(_w),
        stride(_stride),
        data(_data)
    {}
};

}


#endif // PUZZLEBLOCK_H
