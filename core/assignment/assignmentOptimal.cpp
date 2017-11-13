/**
 * \file assignmentOptimal.cpp
 * \brief This file contains the implementations of fast assignment problem solution
 *
 * based in the Daniel Shved algorithm draft
 *
 * \ingroup cppcorefiles
 * \date Oct 17, 2010
 * \author alexander
 */

#include <stdlib.h>
#include <stdio.h>
#include <limits>

#include "core/utils/global.h"

#include "core/assignment/assignmentOptimal.h"
namespace corecvs {

/**
 *
 *  This assignment algorithm uses the highly optimized version proposed by
 *  Daniel Shved
 *
 *  at http://acm.mipt.ru/twiki/bin/view/Algorithms/HungarianAlgorithmCPP
 *
 *  First of all - this version uses the optimization that allows not to store the
 *  duplicate of the matrix.
 *
 *  All assignment manipulations are limited to adding or subtracting
 *  value to of from column or row in whole. In this version the subtraction
 *  values are stored in the v and u arrays.
 *
 *
 *  Secondary - this algorithm processes matrix row by row.
 *
 *  If naive approach for assigning element in the newly added row doesn't work,
 *  (the column with the smallest element is already used in the assignment) *
 *  the algorithm analyzes the upper rows (markedI holds the currently analyzed row)
 *  until it will find a new collision-free assignment.
 *
 *
 * */

template <typename BufferType, typename MappingType>
bool assignmentOptimal(
        const BufferType *A,
        MappingType* mapping)
{
   typedef typename BufferType::InternalElementType Type;

   int i, j, j1;
   int h = A->h;
   int w = A->w;

   Type   *v = new Type[w];
   Type   *u = new Type[h];

   int    *xy = new int[w];

   int   *links = new int[w];
   Type  *mins  = new Type[w];
   bool *visited  = new bool[w];

   for (i = 0; i < h; i++)
   {
       u[i] = Type(0);
   }

   for (j = 0; j < w; j++)
   {
       v[j] = Type(0);
       xy[j] = -1;
   }


   for(i = 0; i < h; i++) {

       /**
        * Staring new row analysis
        * While attempting to resolve collision we could build a chain of zeros
        * links will hold the connection between the rows
        * mins
        * visited - will hold the flags that the row is already visited
        *
        * */
       for (j = 0; j < w; j++)
       {
           links[j]   = -1;
           mins[j]    = std::numeric_limits<Type>::max();
           visited[j] = false;
       }

      /**
       * markedI holds the currently analyzed row.
       * Analysis starts with the just added row
       *
       * We will select  3 areas -
       * <ol>
       *   <li> unvisied rows
       *   <li> visied rows and visited columns - the visited block
       *   <li> visied rows and un visited columns - the search block
       * </ol>
       *
       *
       * */
      int markedI = i;
      int markedJ = -1;
      while(markedI != -1)
      {
          /**
           * Now we would update the information about the minimum in the un-visited columns
           * mins[j] will hold the the minimum value in the j-th column among all the
           * visited rows. Values in the visited columns do not matter.
           *
           * (j,markedI) will hold the position of the minimum in the search block
           *
           *
           * */
         j = -1;
         for(j1 = 0; j1 < w; j1 ++)
         {
            if(!visited[j1])
            {
                Type current = A->element(markedI,j1) - u[markedI] - v[j1];
                if(current < mins[j1]) {
                    /**
                     * We arrive here if the processed row in the j1 column holds the smallest element
                     * We update the mins array
                     * And create a horizontal link to the collision element
                     * */
                    mins[j1] = current;
                    links[j1] = markedJ;
                }
                if(j == -1 || mins[j1] < mins[j])
                   j = j1;
            }
         }


        /**
          * Now j hold the number of the unvisited column in which the minimum among the
          * visited rows is achieved.
          *
          * Thus delta will hold the minimum in the searchBlock.
          *
          * Next code will -
          *    In search block (visited rows, unvisited columns )
          *               decrease all the search block by delta - a new 0 will appear at (j, markedI)
          *
          *    In visited block (visited rows, visited columns)
          *               leave all unchanged (+ delta to columns, -delta to rows)
          *
          *    In the visited columns, unvisited rows
          *                  elements will increase. But there are no selected elements there
          *
          *    In the unvisited columns, unvisied rows
          *                nothing will happen
          *
          *
          * */
         Type delta = mins[j];
         for(j1 = 0; j1 < w; j1++)
         {
            if(visited[j1]) {
               u[xy[j1]] += delta;
               v[j1] -= delta;
            } else {
               mins[j1] -= delta;
            }
         }
         u[i] += delta;

         /**
          *  As said above (j, markedI) holds the new zero.  Now we have two options
          *  either xy[j] will hold -1 (which means no selected ("starred") zeros in the column j)
          *        then we will leave the cycle
          *  or there is another 0 in the row xy[j]
          *         then we make it current and continue to build links list.
          *        The new collision element is (j, xy[j])
          *
          * */
         visited[j] = 1;
         markedJ = j;
         markedI = xy[j];
      }

      /**
       *  We exited the collision processing cycle and now we have a linked list of collisions
       *
       *  Links holds the collision columns numbers  for "primed elements". It starts with the element in column j.
       *
       *  xy holds the map from columns to rows of the old assignment - the "stared elements"
       *
       *  We can now improve the list increasing it's size by new element and
       *  switching all the "stared" elements to "primed"
       *
       *  */
      for(; links[j] != -1; j = links[j])
         xy[j] = xy[links[j]];
      xy[j] = i;
   }

   // Returning the result
   for(j = 0; j < w; j++)
   {
      if(xy[j] != -1)
         mapping->element(xy[j],0) = j;
   }

   delete[] v;
   delete[] u;
   delete[] xy;
   delete[] links;
   delete[] mins;
   delete[] visited;

   return true;
}


void assignOptimal(AbstractBuffer<int, int> *input, AbstractBuffer<int, int> *output)
{
    assignmentOptimal<AbstractBuffer<int, int>, AbstractBuffer<int, int> >(input, output);
}

void assignOptimal(AbstractBuffer<uint16_t, int> *input, AbstractBuffer<int, int> *output)
{
    assignmentOptimal<AbstractBuffer<uint16_t, int>, AbstractBuffer<int, int> >(input, output);
}

void assignOptimal(AbstractBuffer<double, int> *input, AbstractBuffer<int, int> *output)
{
    assignmentOptimal<AbstractBuffer<double, int>, AbstractBuffer<int, int> >(input, output);
}

void assignOptimal(AbstractBuffer<float, int> *input, AbstractBuffer<int, int> *output)
{
    assignmentOptimal<AbstractBuffer<float, int>, AbstractBuffer<int, int> >(input, output);
}



} //namespace corecvs

