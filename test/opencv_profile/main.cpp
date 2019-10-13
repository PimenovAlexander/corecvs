#include "gtest/gtest.h"

#include <iostream>

#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "core/utils/preciseTimer.h"


const static unsigned POLLUTING_INPUTS = 10;
const static unsigned LIMIT = 50;

const static int  TEST_H_SIZE = 3000;
const static int  TEST_W_SIZE = 4000;

using namespace std;
using namespace cv;
using namespace corecvs;




TEST(OpenCVProfile, convoleDouble)
{
    /// Declare variables
      Mat src[POLLUTING_INPUTS];
      Mat dst[POLLUTING_INPUTS];

      for (int i = 0; i < POLLUTING_INPUTS; i++)
      {
          src[i] = Mat(TEST_H_SIZE, TEST_W_SIZE, CV_64F );
          dst[i] = Mat(TEST_H_SIZE, TEST_W_SIZE, CV_64F );

          for (int k = 0; k < TEST_H_SIZE; k++)
              for (int l = 0; l < TEST_W_SIZE; l++)
              {
                  src[i].at<double>(k, l) = (double)(k * 54536351 + l * 8769843);
              }
      }

      Mat kernel;
      Point anchor( -1, -1 );

      int kernelSize = 11;
      kernel = Mat::ones( kernelSize, kernelSize, CV_64F )/ (double)(kernelSize*kernelSize);

      /* flops */
      double flop   = 2.0 * (double)TEST_H_SIZE * TEST_W_SIZE * kernelSize * kernelSize;
      double gflops = flop / 1000000.0 / 1000.0;

      printf("Profiling %15s Approach [%dx%d](%d runs)", "OpenCV", kernelSize, kernelSize, LIMIT);

      PreciseTimer start = PreciseTimer::currentTime();
      for (uint i = 0; i < LIMIT; i++)
      {
        filter2D(src[i % POLLUTING_INPUTS], dst[i % POLLUTING_INPUTS], -1 , kernel, anchor, 0, BORDER_DEFAULT );
      }

      // cout << "Usecs: " <<  start.usecsToNow() << endl;
      uint64_t delay = start.usecsToNow();
      uint64_t odelay = delay / LIMIT;
      uint64_t sodelay = odelay;

      double runss = 1000000.0 / ((double)delay / LIMIT);
      double gflopss = runss * gflops;

      SYNC_PRINT(("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us %3.2lf%% | % 7.3lf Gflops/s |\n",
                  delay, delay / 1000, delay / LIMIT, odelay * 100.0 / sodelay, gflopss));
}

TEST(OpenCVProfile, convoleFloat)
{
    /// Declare variables
      Mat src[POLLUTING_INPUTS];
      Mat dst[POLLUTING_INPUTS];

      for (int i = 0; i < POLLUTING_INPUTS; i++)
      {
          src[i] = Mat(TEST_H_SIZE, TEST_W_SIZE, CV_32F );
          dst[i] = Mat(TEST_H_SIZE, TEST_W_SIZE, CV_32F );

          for (int k = 0; k < TEST_H_SIZE; k++)
              for (int l = 0; l < TEST_W_SIZE; l++)
              {
                  src[i].at<float>(k, l) = (float)(k * 54536351 + l * 8769843);
              }
      }

      Mat kernel;
      Point anchor( -1, -1 );

      int kernelSize = 11;
      kernel = Mat::ones( kernelSize, kernelSize, CV_32F )/ (double)(kernelSize*kernelSize);

      /* flops */
      double flop   = 2.0 * (double)TEST_H_SIZE * TEST_W_SIZE * kernelSize * kernelSize;
      double gflops = flop / 1000000.0 / 1000.0;

      printf("Profiling %15s Approach [%dx%d](%d runs)", "OpenCV", kernelSize, kernelSize, LIMIT);

      PreciseTimer start = PreciseTimer::currentTime();
      for (uint i = 0; i < LIMIT; i++)
      {
        filter2D(src[i % POLLUTING_INPUTS], dst[i % POLLUTING_INPUTS], -1 , kernel, anchor, 0, BORDER_DEFAULT );
      }

      //cout << "Usecs: " <<  start.usecsToNow() << endl;
      uint64_t delay = start.usecsToNow();
      uint64_t odelay = delay / LIMIT;
      uint64_t sodelay = odelay;

      double runss = 1000000.0 / ((double)delay / LIMIT);
      double gflopss = runss * gflops;

      SYNC_PRINT(("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us %3.2lf%% | % 7.3lf Gflops/s |\n",
                  delay, delay / 1000, delay / LIMIT, odelay * 100.0 / sodelay, gflopss));


}



int main(int argc, char **argv)
{

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

