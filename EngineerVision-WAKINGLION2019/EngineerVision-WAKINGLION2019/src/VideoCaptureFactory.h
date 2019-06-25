//-----------------------------------【视频处理类】--------------------------------------------
// brief：重写的视频处理类，有调整分辨率/FPS/曝光度等功能
//------------------------------------------------------------------------------------------------

#ifndef VIDEOCAPTUREFACTORY_H
#define VIDEOCAPTUREFACTORY_H

//#include "Settings.h"

#include "opencv2/opencv.hpp"
#include <iostream>
#include <string>

class VideoCaptureFactory
{
public:
    /**
     * @brief  简介
     * @param  第一个参数
     * @return 返回值
     * @autor  梁尧森
     * @date   2019.3.31
     */
    VideoCaptureFactory(const char *dev_name,
                        int width,
                        int height,
                        int exposure,
                        int size_buffer=1);

    VideoCaptureFactory(int dev_name,
                        int width,
                        int height,
                        int exposure,
                        int size_buffer=1);

    /**
     * @brief  简介
     * @param  第一个参数
     * @return 返回值
     * @autor  梁尧森
     * @date   2019.3.31
     */
    void init(int width,
              int height,
              int exposure);

    /**
     * @brief  简介
     * @param  第一个参数
     * @return 返回值
     * @autor  梁尧森
     * @date   2019.3.31
     */
    bool open_dev(const char *dev_name);

    bool open_dev(int dev_name);

    bool setVideoFormat(int width, int height, bool mjpg)
    {

    }

    bool changeVideoFormat(int width, int height, bool mjpg)
    {

    }

    bool setExposureTime(bool auto_exp, int t)
    {

    }

    bool startStream()
    {

    }

    void info()
    {

    }


    void setSaveFormat(const char *img_path,
                        const char *img_format,
                        long interval);

    /**
     * @brief  简介
     * @param  第一个参数
     * @return 返回值
     * @autor  梁尧森
     * @date   2019.3.31
     */
    VideoCaptureFactory& operator >> (cv::Mat &image);

    bool grabImg(cv::Mat &img,char order);

    void showResult();
private:
    cv::VideoCapture capture;
    const char *img_path;
    const char *img_format;
    long interval;
    long start;

    int img_filename=0;

    cv::Mat img_show;
};

#endif // VIDEOCAPTUREFACTORY_H
