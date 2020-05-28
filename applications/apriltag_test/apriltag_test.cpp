//
// Created by jakhremchik on 14.11.2019.
//

#include "apriltag_test.h"

using namespace cv;
using namespace std;


/// NOTICE! Just some trash over here. Do not take it seriously!
/// \return
int main () {
    VideoCapture cap(0);

    vector<corecvs::PatternDetectorResult> patts;

    auto *a_detector = new ApriltagDetector();

    if(!cap.isOpened()){
        cout << "Error opening video stream or file" << endl;
        return -1;
    }

    while(true){
        Mat frame;
        cap >> frame;

        if (frame.empty())
          break;

        patts.clear();
        a_detector->setInputImageCV(frame);
        a_detector->operator()();
        a_detector->getParameters();
        a_detector->getOutput(patts);
        for (const auto &patt : patts) {
            cv::line(frame, cv::Point(patt.mPosition.mX, patt.mPosition.mY),
                    cv::Point(patt.mOrtX.mX, patt.mOrtX.mY), cv::Scalar(0, 100, 0), 3);
            cv::line(frame, cv::Point(patt.mPosition.mX, patt.mPosition.mY),
                     cv::Point(patt.mOrtY.mX, patt.mOrtY.mY), cv::Scalar(0, 100, 0), 3);
            cv::line(frame, cv::Point(patt.mUnityPoint.mX, patt.mUnityPoint.mY),
                     cv::Point(patt.mOrtX.mX, patt.mOrtX.mY), cv::Scalar(0, 100, 0), 3);
            cv::line(frame, cv::Point(patt.mUnityPoint.mX, patt.mUnityPoint.mY),
                     cv::Point(patt.mOrtY.mX, patt.mOrtY.mY), cv::Scalar(0, 100, 0), 3);

            cv::putText(frame, std::to_string(patt.id()) ,
                    cv::Point((patt.mOrtX.mX + patt.mOrtY.mX)/2, (patt.mOrtX.mY + patt.mOrtY.mY)/2 ), 2, 4.0, cv::Scalar(0, 255, 0), 4);
        }
//
        imshow( "Frame", frame );


        // Press  ESC on keyboard to exit
        char c=(char)waitKey(25);
        if(c==27)
          break;
  }
}