#ifndef KALMAN_H
#define KALMAN_H

#include "Gui.h"
#include "Tool.h"

#include "opencv2/opencv.hpp"

class Kalman
{
public:
    Kalman():
        KF(2, 1, 0)
    {
        KF.transitionMatrix=(cv::Mat_<float>(2,2)<<
                             1,1,0,1);
        cv::setIdentity(KF.measurementMatrix);
        cv::setIdentity(KF.processNoiseCov,cv::Scalar::all(1e-5));
        cv::setIdentity(KF.measurementNoiseCov,cv::Scalar::all(1e-1));
        cv::setIdentity(KF.errorCovPost,cv::Scalar::all(1));

        randn(KF.statePost,cv::Scalar::all(0),cv::Scalar::all(0.1));
    }

    void processing()
    {
        cv::Mat state(2,1,CV_32F);
        cv::Mat prediction;
        cv::randn(state,cv::Scalar::all(0),cv::Scalar::all(0.1));

        std::vector<float>data_list;
        std::vector<cv::Scalar> color_list;
        color_list.push_back(cv::Scalar(0,255,255));
        color_list.push_back(cv::Scalar(0,0,255));
        color_list.push_back(cv::Scalar(0,255,0));
        color_list.push_back(cv::Scalar(0,0,128));
        color_list.push_back(cv::Scalar(0,128,0));
        color_list.push_back(cv::Scalar(255,255,255));

        DataCollect dc(800,600,-1,-1);
        cv::Mat img(500, 500, CV_8UC3);
        cv::Point2f center(img.cols*0.5f, img.rows*0.5f);
        float R = img.cols/3.f;

        while(1)
        {
            cv::Mat measurement;
            process(state,prediction,measurement);

            double stateAngle = state.at<float>(0);
            cv::Point statePt = calcPoint(center, R, stateAngle);

            double predictAngle = prediction.at<float>(0);
            cv::Point predictPt = calcPoint(center, R, predictAngle);

            double measAngle = measurement.at<float>(0);
            cv::Point measPt = calcPoint(center, R, measAngle);

            cv::line( img, center, statePt, cv::Scalar(0,255,255), 1, CV_AA, 0 );
            cv::line( img, center, measPt, cv::Scalar(0,0,255), 1, CV_AA, 0 );
            cv::line( img, center, predictPt, cv::Scalar(0,255,0), 1, CV_AA, 0 );

            data_list.clear();
            static int radio=100;
            data_list.push_back(stateAngle*radio);//黄
            data_list.push_back(measAngle*radio);//红
            data_list.push_back(predictAngle*radio);//绿
            data_list.push_back((measAngle-stateAngle)*radio);//红
            data_list.push_back((predictAngle-stateAngle)*radio);//绿
            dc.dataCollectProc(data_list,color_list,"0");

            imshow( "Kalman", img );
            char code = (char)cv::waitKey(100);
        }
    }

    void process(cv::Mat state,
                 cv::Mat &prediction,
                 cv::Mat &measurement)
    {
        prediction=KF.predict();

        measurement = cv::Mat::zeros(1, 1, CV_32F);
        cv::randn( measurement,
               cv::Scalar::all(0),
               cv::Scalar::all(KF.measurementNoiseCov.at<float>(0)));
        measurement += KF.measurementMatrix*state;

        if(cv::theRNG().uniform(0,4) != 0)
            KF.correct(measurement);

        cv::Mat processNoise(2, 1, CV_32F);
        cv::randn( processNoise,
               cv::Scalar(0),
               cv::Scalar::all(sqrt(KF.processNoiseCov.at<float>(0, 0))));
        state = KF.transitionMatrix*state + processNoise;
    }
private:
    cv::KalmanFilter KF;
};

#endif // KALMAN_H
