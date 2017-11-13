#ifndef MORPHOLOGICAL_H_
#define MORPHOLOGICAL_H_
/**
 * \file morphological.h
 * \brief Add Comment Here
 *
 * This file holds the dirty and not finished morphological operations
 *
 * \date Aug 11, 2011
 * \author alexander
 */


#include "core/buffers/g12Buffer.h"
#include "core/buffers/g8Buffer.h"

namespace corecvs {


/**
 *
 *   This class holds morpological transformations
 *   \f{eqnarray*}
 *      B_z &=& {x + z | x \in B}, z \in E \\
 *      A^c &=& {x \in E | x \not \in A}
 *   \f}
 *
 *
 *   Erosion
 *   \f{eqnarray*}
 *      A \ominus B &=& \{z\in E | B_{z} \subseteq A\} \\
 *      A \ominus B &=& \bigcap_{b\in B} A_{-b}
 *   \f}
 *
 *   Dilation
 *   \f{eqnarray*}
 *      A \oplus B &=& \bigcup_{b\in B} A_b \\
 *      A \oplus B &=& B\oplus A = \bigcup_{a\in A} B_a
 *   \f}
 *
 *   Opening
 *   \f{eqnarray*}
 *      A \circ B = (A \ominus B) \oplus B
 *   \f}
 *
 *   Closing
 *   \f{eqnarray*}
 *      A \bullet B &=& (A \oplus B) \ominus B \\
 *      A \bullet B &=& (A^{c} \circ B^{s})^{c}
 *   \f}
 *
 *
 *
 **/
class Morphological
{
public:
    static G12Buffer *erode (G12Buffer *input, G12Buffer *element, int centerX, int centerY);
    static G12Buffer *dilate(G12Buffer *input, G12Buffer *element, int centerX, int centerY);

    static G8Buffer *erode (G8Buffer *input, G12Buffer *element, int centerX, int centerY);
    static G8Buffer *dilate(G8Buffer *input, G12Buffer *element, int centerX, int centerY);

};


template <typename Algebra>
class ErodeKernel
{
public:
    static const int inputNumber = 1;
    static const int outputNumber = 1;

    G12Buffer *element;
    int x;
    int y;

    ErodeKernel(G12Buffer *_element, int _y, int _x) :
        element(_element), x(_x), y(_y) {}

    inline int getCenterX(){ return x; }
    inline int getCenterY(){ return y; }
    inline int getSizeX()  { return element->w; }
    inline int getSizeY()  { return element->h; }

    typedef typename Algebra::InputType Type;

template <typename OtherAlgebra>
    ErodeKernel(const ErodeKernel<OtherAlgebra> & other)
    {
        this->x = other.x;
        this->y = other.y;
        this->element = other.element;
    }

    inline void process(Algebra &algebra) const
    {
        // TODO: Correct this please
        //Type result((uint16_t)0xFFFFU);
        Type result;
        Algebra::fillOnes(result);

        for (int i = 0; i < element->h; i++)
        {
            for (int j = 0; j < element->w; j++)
            {
                //Type condition((uint16_t)element->element(i,j));
                //result |= Algebra::selector(condition, algebra.getInput(dy,dx), Type((uint16_t)0xFFFF));

                if (!element->element(i,j))
                    continue;
                result &= algebra.getInput(i,j);
            }
        }
        algebra.putOutput(0, 0, result);
    }
};


template <typename Algebra>
class DilateKernel
{
public:
    static const int inputNumber = 1;
    static const int outputNumber = 1;

    G12Buffer *element;
    int x;
    int y;

    DilateKernel(G12Buffer *_element, int _y, int _x) :
        element(_element), x(_x), y(_y) {}

    inline int getCenterX(){ return x; }
    inline int getCenterY(){ return y; }
    inline int getSizeX(){ return element->w; }
    inline int getSizeY(){ return element->h; }

    typedef typename Algebra::InputType Type;

template <typename OtherAlgebra>
    DilateKernel(const DilateKernel<OtherAlgebra> & other)
    {
        this->x = other.x;
        this->y = other.y;
        this->element = other.element;
    }

    inline void process(Algebra &algebra) const
    {
        // TODO: Correct this please
        Type result((uint16_t)0x0);

        for (int i = 0; i < element->h; i++)
        {
            for (int j = 0; j < element->w; j++)
            {
                //Type condition((uint16_t)element->element(i,j));
                //result |= Algebra::selector(condition, algebra.getInput(dy,dx), Type((uint16_t)0xFF));

                if (!element->element(i,j))
                    continue;
                result |= algebra.getInput(i,j);
            }
        }
        algebra.putOutput(0,0,result);
    }
};


} //namespace corecvs
#endif /* MORPHOLOGICAL_H_ */

