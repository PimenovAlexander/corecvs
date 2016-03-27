#ifndef SEGMENTATOR_H_
#define SEGMENTATOR_H_
/**
 * \file segmentator.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Mar 24, 2010
 * \author alexander
 */

#include <algorithm>
#include <vector>
#include <functional>

#include "global.h"

#include "vector2d.h"
#include "abstractBuffer.h"
#include "g12Buffer.h"

namespace corecvs {

using std::vector;
using std::bind2nd;
using std::equal_to;

/**
 * This is a generic class that desribes the part of the whole connected component
 *
 * It is advised that the real segment inherit form this class.
 *
 * */
template <typename ThisType>
class BaseSegment
{
public:
    /**
     * The father segment in a merge tree during the merge phase
     * TODO: Make this private
     *
     * */
    ThisType *father;

    /** Default constructor initializes father to NULL which means that this is a top element */
    BaseSegment()
    {
        father = NULL;
    }

    /** Getter for father */
    ThisType *getFather()
    {
        return father;
    }

    /** Checks if it is a child segment or not */
    bool isTop()
    {
        return (father == NULL);
    }

    /** This method returns a topmost segment in a father tree */
    ThisType *getTopFather()
    {
        ThisType *toReturn = (ThisType *)this;
        while (toReturn->getFather() != NULL)
            toReturn = toReturn->getFather();
        return toReturn;
    }

    /** This method merges two trees of child segments */
    void markMerge(ThisType *other)
    {
        ThisType *thisTopFather  = this->getTopFather();
        ThisType *otherTopFather = other->getTopFather();
        if (otherTopFather != thisTopFather)
            otherTopFather->father = thisTopFather;
    }

    void dadify()
    {
        if (!isTop())
            father = getTopFather();
    }

}; // BaseSegment

/**
 *   This class implements a generic approach to the segmentation - Connected Component Labeling
 *   Base problem statement can be seen here
 *
 *   http://en.wikipedia.org/wiki/Connected_Component_Labeling
 *   http://homepages.inf.ed.ac.uk/rbf/HIPR2/label.htm
 *
 *
 *
 **/
template <typename ThisType, typename SegmentType>
class Segmentator
{
public:

    /**
     * This class will hold the DTO for segmentation result.
     *
     *
     * */
    class SegmentationResult
    {
    public:
        /**
         *  The list of the segments
         **/
        vector<SegmentType *> *segments;
        /**
         *   The map form the point to the segment
         */
        AbstractBuffer<SegmentType *> *markup;

        /*TODO: This should be private*/
        SegmentationResult(vector<SegmentType *> *_segments, AbstractBuffer<SegmentType *> *_markup)
        {
            this->segments = _segments;
            this->markup = _markup;
        }

        ~SegmentationResult()
        {
            if (this->segments != NULL)
            {
                typename vector<SegmentType *>::iterator it;
                for (it = this->segments->begin(); it != this->segments->end(); it++) {
                    delete_safe(*it);
                }
                delete_safe(this->segments);
            }

            if (this->markup != NULL)
            {
                delete_safe(this->markup);
            }
        }

        SegmentType *segment(int i) const
        {
        	return (*segments)[i];
        }

        unsigned segmentNumber() const
        {
        	return (unsigned)segments->size();
        }

    };

    Segmentator() {}

    template<typename BufferType>
    SegmentationResult *segment (BufferType *input)
    {
        AbstractBuffer<SegmentType *> *markup = new AbstractBuffer<SegmentType *>(input->h, input->w);
        vector<SegmentType *> *segments = new vector<SegmentType *>();


        for(int i = 0; i < input->h; i++)
        {
            for(int j = 0; j < input->w; j++)
            {

                if (!((ThisType *)this)->canStartSegment(i, j, input->element(i,j)))
                    continue;

                /* TODO: Use iterator instead */
                int dx = ((ThisType *)this)->xZoneSize();
                int dy = ((ThisType *)this)->yZoneSize();
                bool addingPending = true;
                for (int k = -dy; k <= 0; k++)
                {
                    for (int l = -dx; l <= dx; l++)
                    {

                        if (k == 0 && l == 0)
                            break;

                        int x = j + l;
                        int y = i + k;

                        if (!input->isValidCoord(y, x))
                            continue;

                        if (markup->element(y,x) == NULL)
                            continue;

                        if (addingPending)
                        {
                            markup->element(y,x)->addPoint(i, j, input->element(i,j));
                            markup->element(i,j) = markup->element(y,x);
                            addingPending = false;
                        }
                        else
                        {
                            markup->element(i,j)->markMerge(markup->element(y,x));
                        }
                    }
                }

                if (addingPending)
                {
                    SegmentType *newSegment = new SegmentType();
                    newSegment->addPoint(i, j, input->element(i,j));
                    markup->element(i,j) = newSegment;
                    segments->push_back(newSegment);
                }
            }
        }

        /*Cycle finished. Now we will need to merge segments and delete segments that are not of the need */
        typename vector<SegmentType *>::iterator it;
        for (it = segments->begin(); it < segments->end(); it++)
        {
            if ( (*it) != NULL)
                (*it)->dadify();
        }

        for(int i = 0; i < input->h; i++)
        {
            for(int j = 0; j < input->w; j++)
            {
                if (markup->element(i,j) == NULL)
                    continue;
                markup->element(i,j) = markup->element(i,j)->getTopFather();
            }
        }

        for (it = segments->begin(); it < segments->end(); it++)
        {
            if ( !(*it)->isTop())
            {
                delete (*it);
                (*it) = NULL;
            }
        }

        /*Delete NULL entries in the vector and shorten the vector*/
        segments->erase(
                remove_if(
                        segments->begin(),
                        segments->end(),
                        bind2nd(equal_to<void *>(), (void *)NULL)
                ),
                segments->end());
        return new SegmentationResult(segments, markup);
    }


    virtual ~Segmentator() {}
};


class CornerSegment : public BaseSegment<CornerSegment>
{
public:
    /*TODO: These members are should be moved to a sort of payload*/
    int size;
    Vector2dd sum;

    CornerSegment()
    {
        size = 0;
        sum = Vector2dd(0.0);
    }

    void dadify()
    {
        BaseSegment<CornerSegment>::dadify();
        if (father != NULL && father != this)
        {
            father->size += this->size;
            father->sum += this->sum;
        }
    }

    Vector2dd getMean()
    {
        return this->sum / (double)(this->size);
    }

    void addPoint(int i, int j, const uint16_t & /*tile*/)
    {
        this->size++;
        this->sum += Vector2dd(j, i);
    }

};

class CornerSegmentator : public Segmentator<CornerSegmentator, CornerSegment>
{

public:
    int threshold;

    CornerSegmentator(int _threshold) :
        threshold(_threshold)
    {}


    static int xZoneSize()
    {
        return 1;
    }

    static int yZoneSize()
    {
        return 1;
    }


    bool canStartSegment(int /*i*/, int /*j*/, const uint16_t &element)
    {
        return element > threshold;
    }

};


} //namespace corecvs

#endif /* SEGMENTATOR_H_ */

