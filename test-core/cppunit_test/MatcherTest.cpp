#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#undef __STDC_FORMAT_MACROS
#include <iostream>

#include <limits>

#include "core/utils/global.h"

#include "core/math/mathUtils.h"
#include "featureVectorBuffer.h"
#include "core/buffers/g12Buffer.h"
#include "core/buffers/bufferFactory.h"
#include "featuringParameters.h"
#include "featureVectorBuffer.h"
#include "cortageListSorter.h"
#include "cortageListMatcher.h"
#include "featureProducer.h"
#include "core/utils/preciseTimer.h"
#include "cortageStripeMatcher.h"
#include "MatcherTest.h"

using namespace std;

void MatcherTest::profileListMatching( void )
{
    static const unsigned LIMIT = 300;
    RectCortegeList *list1[LIMIT];
    RectCortegeList *list2[LIMIT];
    FlowBuffer *results[LIMIT];


    LinearSignatureParameters sParams;
      sParams.epsilonHighLimit = 10;
      sParams.epsilonLowLimit  = 1;
      sParams.sdevHighLimit    = 151;
      sParams.sdevLowLimit     = 2;


    G12Buffer *input1 = BufferFactory::getInstance()->loadG12Bitmap("data/pair/image0001_c0.pgm");
    G12Buffer *input2 = BufferFactory::getInstance()->loadG12Bitmap("data/pair/image0001_c1.pgm");

    RectCortegeList *example1 = SignatureProducer::getSignaturesFastest(input1, sParams);
    RectCortegeList *example2 = SignatureProducer::getSignaturesFastest(input2, sParams);

        PreciseTimer start;
        uint64_t delayOld;
        RectCortegeList *example1Sorted = example1->clone();
        RectCortegeList *example2Sorted = example2->clone();

        CortegeListSorter::sort(example1Sorted);
        CortegeListSorter::sort(example2Sorted);
        CortegeListMatcher matcher(7);
//         CortegeTileMatcher matcher1;

        for (unsigned i = 0; i < LIMIT; i++)
        {
            list1[i] = example1Sorted->clone();
            list2[i] = example2Sorted->clone();
        }

        start = PreciseTimer::currentTime();
        for (unsigned i = 0; i < LIMIT; i++)
        {
            results[i] = matcher.doMatch(list1[i], list2[i]);
        }
        delayOld = start.usecsToNow();
        /*CPPUNIT_NS::stdCOut() << "List Matching reesults: " 
                              << delayOld << "us " << delayOld/LIMIT << "us " 
                              << (uint64_t)fround(delayOld / 1000.0 / LIMIT) "ms" << endl;*/
        printf("List Matching results: %8" PRIu64 "us %8" PRIu64 "us  %8" PRIu64 "ms \n",
                delayOld,
                delayOld / LIMIT,
                (uint64_t)fround(delayOld / 1000.0 / LIMIT)
              );
        
        CPPUNIT_ASSERT (true);

        for (unsigned i = 0; i < LIMIT; i++)
        {
            delete results[i];
            delete list1[i];
            delete list2[i];
        }

        /*================================*/

        delete example1Sorted;
        delete example2Sorted;
 }

void MatcherTest::testTileMatching( void )
{
    LinearSignatureParameters sParams;
    sParams.epsilonHighLimit = 10;
    sParams.epsilonLowLimit  = 1;
    sParams.sdevHighLimit    = 151;
    sParams.sdevLowLimit     = 2;

    G12Buffer *input1 = BufferFactory::getInstance()->loadG12Bitmap("data/pair/image0001_c0.pgm");
    G12Buffer *input2 = BufferFactory::getInstance()->loadG12Bitmap("data/pair/image0001_c1.pgm");

    RectCortegeList *example1 = SignatureProducer::getSignaturesFastest(input1, sParams);
    RectCortegeList *example2 = SignatureProducer::getSignaturesFastest(input2, sParams);
    unsigned tileH = 3;
    CortegeListSorter::sortStripes(example1, tileH);
    CortegeListSorter::sortStripes(example2, tileH);

    CortegeStripeMatcher matcher1;
    CortegeListMatcher matcher(7);
    FlowBuffer *result1 = matcher1.doStereoStripeMatch(example1, example2, tileH);
    FlowBuffer *result2 = NULL;//matcher .doStereoTileMatch(example1, example2, tileH);
    
    CPPUNIT_ASSERT (true);

    /*if (result1->isEqualTrace(result2))
    {
        printf("Results are equal\n");
    } else {
        printf("Results are not equal\n");

        printf("Buffer densities are: new:%d  old:%d\n", result1->density(), result2->density());
    }
    fflush(stdout);*/
    delete input1;
    delete input2;
    delete example1;
    delete example2;
    delete result1;
    delete result2;
}

void MatcherTest::profileTileMatching( void )
{
    static const unsigned LIMIT = 300;
    RectCortegeList *list1[LIMIT];
    RectCortegeList *list2[LIMIT];
    FlowBuffer *results[LIMIT];


    LinearSignatureParameters sParams;
      sParams.epsilonHighLimit = 10;
      sParams.epsilonLowLimit  = 1;
      sParams.sdevHighLimit    = 151;
      sParams.sdevLowLimit     = 2;


    G12Buffer *input1 = BufferFactory::getInstance()->loadG12Bitmap("data/pair/image0001_c0.pgm");
    G12Buffer *input2 = BufferFactory::getInstance()->loadG12Bitmap("data/pair/image0001_c1.pgm");

    RectCortegeList *example1 = SignatureProducer::getSignaturesFastest(input1, sParams);
    RectCortegeList *example2 = SignatureProducer::getSignaturesFastest(input2, sParams);

    for (unsigned tileH = 1; tileH < 10; tileH += 3)
    {
        PreciseTimer start;
        uint64_t delayOld;
        uint64_t delayNew;
        RectCortegeList *example1Sorted = example1->clone();
        RectCortegeList *example2Sorted = example2->clone();

        CortegeListSorter::sortStripes(example1Sorted, tileH);
        CortegeListSorter::sortStripes(example2Sorted, tileH);
        CortegeListMatcher matcher(7);
        CortegeStripeMatcher matcher1;

        for (unsigned i = 0; i < LIMIT; i++)
        {
            list1[i] = example1Sorted->clone();
            list2[i] = example2Sorted->clone();
        }

        start = PreciseTimer::currentTime();
        for (unsigned i = 0; i < LIMIT; i++)
        {
            results[i] = matcher.doStereoStripeMatch(list1[i], list2[i], tileH);
        }
        delayOld = start.usecsToNow();
        /*CPPUNIT_NS::stdCOut() << "Tile Matching results: "
                              << "Old style: "
                              << delayOld << "us " << delayOld/LIMIT << "us "
                              << (uint64_t)fround(delayOld / 1000.0 / LIMIT) "ms " 
                              << endl << tileH << endl;*/
        printf("Tile Matching results:\nOld style %3d: %8" PRIu64 "us %8" PRIu64 "us  %8" PRIu64 "ms \n", tileH,
                delayOld,
                delayOld / LIMIT,
                (uint64_t)fround(delayOld / 1000.0 / LIMIT)
              );

        for (unsigned i = 0; i < LIMIT; i++)
        {
            delete results[i];
            delete list1[i];
            delete list2[i];
        }

        /*================================*/

        for (unsigned i = 0; i < LIMIT; i++)
        {
            list1[i] = example1Sorted->clone();
            list2[i] = example2Sorted->clone();
        }

        start = PreciseTimer::currentTime();
        for (unsigned i = 0; i < LIMIT; i++)
        {
            results[i] = matcher1.doStereoStripeMatch(list1[i], list2[i], tileH);
        }
        delayNew = start.usecsToNow();
        /*CPPUNIT_NS::stdCOut() << "New style: " 
                              << delayNew << "us " << delayNew/LIMIT << "us "
                              << (uint64_t)fround(delayNew / 1000.0 / LIMIT) "ms " 
                              << delayNew * 100.0 / delayOld << endl << tileH << endl;*/
        printf("New style %3d: %8" PRIu64 "us %8" PRIu64 "us  %8" PRIu64 "ms %5.2lf%%\n", tileH,
                delayNew,
                delayNew / LIMIT,
                (uint64_t)fround(delayNew / 1000.0 / LIMIT),
                delayNew * 100.0 / delayOld
              );
        
        CPPUNIT_ASSERT (true);

        for (unsigned i = 0; i < LIMIT; i++)
        {
            delete results[i];
            delete list1[i];
            delete list2[i];
        }



        delete example1Sorted;
        delete example2Sorted;
    }
}

 
