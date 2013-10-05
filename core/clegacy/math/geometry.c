/**
 * \file geometry.c
 * \brief implementation of some geometrical functions
 *
 *
 * \ingroup corefiles
 * \date Apr 20, 2009
 * \author Alexander Pimenov
 * \author part of the code comes form wikipedia
 */

#include <math.h>
#include <stdio.h>

#include "global.h"
#include "geometry.h"

/**
 * \brief This function implements the vector cross product.
 * It is computed as follows:
 *
 * \code
 *  [  i   j   k  ]
 *  [ a.x a.y a.z ]
 *  [ b.x b.y b.z ]
 * \endcode
 *
 * \param[in] a the first vector
 * \param[in] b the second vector
 *
 * \return vector a x b
 */
vector3Dd crossProduct(vector3Dd *a, vector3Dd *b)
{
        vector3Dd toReturn;
        toReturn.x = + a->y * b->z - a->z * b->y;
        toReturn.y = - a->x * b->z + a->z * b->x;
        toReturn.z = + a->x * b->y - a->y * b->x;
        return toReturn;
}

/**
 * \brief This function implements the vector dot product.
 * It is computed as follows:
 *
 * \f[
 *  a_x b_x + a_y b_y + a_z b_z
 * \f]
 *
 * \param[in] a the first vector
 * \param[in] b the second vector
 *
 * \return vector a . b
 */

double dotProduct(vector3Dd *a, vector3Dd *b)
{
        return a->x * b->x + a->y * b->y + a->z * b->z;
}

/**
 * \brief This function normalizes the 3d vector
 * It is computed as follows:
 *
 * \f[
 *  v_{result} = \frac v {|v|}
 * \f]
 *
 * \param[in, out] a the vector to normalize
 */
void vectorNormalize(vector3Dd *a)
{
        double normSquare = a->x * a->x + a->y * a->y  + a->z * a->z;
        if (normSquare == 0)
                return;
        double norm = sqrt(normSquare);
        a->x /= norm;
        a->y /= norm;
        a->z /= norm;
}

void vectorHomogeniousNormalize(vector3Dd *a)
{
        if (a->z == 0)
                return;
        a->x /= a->z;
        a->y /= a->z;
        a->z = 1;
}

/**
 * \brief This function prints the vector to stdout
 *
 * \param[in] v vector to print
 */
void vectorPrint(vector3Dd *v)
{
        printf("[%lg : %lg : %lg ]\n", v->x, v->y, v->z);
}

vector3Dd vector3DdCreate(double x, double y, double z)
{
        vector3Dd toReturn = {x,y,z};
        return toReturn;
}

vector2Dd vector2DdCreate(double x, double y)
{
        vector2Dd toReturn = {x, y};
        return toReturn;
}

void vector2DdIncrement(vector2Dd *a, vector2Dd *b)
{
        a->x += b->x;
        a->y += b->y;
}

double vector2DdLength(vector2Dd *a)
{
        return sqrt(a->x * a->x + a->y * a->y);
}

vector3Dd vector3DdAdd(vector3Dd a, vector3Dd b)
{
        vector3Dd toReturn = {a.x + b.x , a.y + b.y , a.z + b.z };
        return toReturn;
}

vector3Dd vectorMultiplyConst(vector3Dd a, double b)
{
        vector3Dd toReturn = {a.x * b, a.y * b, a.z * b};
        return toReturn;
}

void vector2Dswap (vector2Dd *p1, vector2Dd *p2)
{
        vector2Dd tmp;
        tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
}

TriangleIterator triangleIteratorCreate(vector2Dd p1, vector2Dd p2, vector2Dd p3)
{
        /* Sort points in ascending order by y*/
        if (p1.y > p2.y)
                vector2Dswap(&p1,&p2);
        if (p1.y > p3.y)
                vector2Dswap(&p1,&p3);
        if (p2.y > p3.y)
                vector2Dswap(&p2,&p3);

        TriangleIterator toReturn;

        toReturn.p1 = p1;
        toReturn.p2 = p2;
        toReturn.p3 = p3;
        toReturn.y = ceil(toReturn.p1.y) - 1;
        toReturn.lpos = 0;
        toReturn.rpos = 0;
        toReturn.x = 1;


        return toReturn;
}

int triangleIteratorNext(TriangleIterator *iterator, vector2Ds32 *pos)
{
        iterator->x++;
        while (1)
        {
                /* Go throw the horizontal line*/
                if (iterator->x <= iterator->rpos)
                {
                        pos->x = iterator->x;
                        pos->y = iterator->y;
                        return 1;
                }

                iterator->y++;
                /*We could be in the lower or on the upper part of the triangle*/
                if (iterator->y <= iterator->p2.y)
                {
                        if (iterator->p2.y == iterator->p1.y)
                        {
                                iterator->lpos = iterator->p1.x;
                                iterator->rpos = iterator->p2.x;
                        } else
                        {
                                double delta = iterator->y - iterator->p1.y;
                                iterator->lpos = iterator->p1.x + (iterator->p2.x - iterator->p1.x) / (iterator->p2.y - iterator->p1.y) * delta;
                                iterator->rpos = iterator->p1.x + (iterator->p3.x - iterator->p1.x) / (iterator->p3.y - iterator->p1.y) * delta;
                        }
                } else {
                        if (iterator->y > iterator->p3.y)
                        {
                                /* We finished */
                                return 0;
                        }

                        if (iterator->p2.y == iterator->p3.y)
                        {
                                iterator->lpos = iterator->p2.x;
                                iterator->rpos = iterator->p3.x;
                        } else
                        {
                                double delta = iterator->p3.y - iterator->y;
                                iterator->lpos = iterator->p3.x + (iterator->p2.x - iterator->p3.x) / (iterator->p3.y - iterator->p2.y) * delta;
                                iterator->rpos = iterator->p3.x + (iterator->p1.x - iterator->p3.x) / (iterator->p3.y - iterator->p1.y) * delta;
                        }
                }

                if (iterator->lpos > iterator->rpos)
                {
                        double tmp = iterator->lpos;
                        iterator->lpos = iterator->rpos;
                        iterator->rpos = tmp;
                }
                iterator->x = ceil(iterator->lpos);
        }

}



