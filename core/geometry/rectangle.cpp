/**
 * \file rectangle.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Nov 12, 2010
 * \author alexander
 */

#include "rectangle.h"
namespace corecvs {


/**
 * \brief This function clips a line to a rectangular window.
 *
 * This function uses the popular Cohen-Sutherland algorithm.
 * See http://en.wikipedia.org/wiki/Cohen-Sutherland for details
 *
 * \author this code is based on code from wikipedia
 *
 * \param[in,out] lineStart on enter - the start of the line, on exit - the start of the clipped line.
 * \param[in,out] lineEnd   on enter - the end of the line, on exit - the end of the clipped line.
 *
 * \return CLIP_HAD_INTERSECTIONS if there was an intersection
 *         CLIP_NO_INTERSECTIONS otherwise
 */

template<>
int Rectangle<int>::clipCohenSutherland ( Vector2d32 &lineStart, Vector2d32 &lineEnd ) const
{
    int code_a, code_b, code; /* codes of the ends */
    Vector2d32 *c;

    // (0,0)-(821,617) : (-3,-1) : (0,1)
#ifdef DEEP_TRACE
    printf("(%d,%d)+(%d,%d) : (%d,%d) : (%d,%d)\n",
            corner.x(), corner.y(),
            size.x(), size.y(),
            lineStart.x(), lineStart.y(),
            lineEnd.x(), lineEnd.y());
    fflush(stdout);
#endif

    code_a = getPointCode(lineStart);
    code_b = getPointCode(lineEnd);

    /* while there are outliers */
    while (code_a || code_b) {
        /* check for no intersection */
        if (code_a & code_b)
            return CLIP_NO_INTERSECTIONS;

        /* take one point */
        if (code_a) {
            code = code_a;
            c = &lineStart;
        } else {
            code = code_b;
            c = &lineEnd;
        }

        if (code & LEFT_CODE) {
            c->y() += (lineStart.y() - lineEnd.y()) * (corner.x() - c->x()) / (lineStart.x() - lineEnd.x());
            c->x() = corner.x();
        } else if (code & RIGHT_CODE) {
            c->y() += (lineStart.y() - lineEnd.y()) * (corner.x() + size.x() - c->x()) / (lineStart.x() - lineEnd.x());
            c->x() = corner.x() + size.x();
        } else if (code & BOTTOM_CODE) {
            c->x() += (lineStart.x() - lineEnd.x()) * (corner.y() - c->y()) / (lineStart.y() - lineEnd.y());
            c->y() = corner.y();
        } else if (code & TOP_CODE) {
            c->x() += (lineStart.x() - lineEnd.x()) * (corner.y() + size.y() - c->y()) / (lineStart.y() - lineEnd.y());
            c->y() = corner.y() + size.y();
        }

        if (code == code_a)
            code_a = getPointCode(lineStart);
        else
            code_b = getPointCode(lineEnd);
    }

#ifdef ASSERT
/*       if (a->x < x1 || a->x > x2 || a->y < y1 || a->y > y2)
           ASSERT_FAIL("OOPS1");
       if (b->x < x1 || b->x > x2 || b->y < y1 || b->y > y2)
           ASSERT_FAIL("OOPS2");*/
#endif

    return CLIP_HAD_INTERSECTIONS;
}

} //namespace corecvs

