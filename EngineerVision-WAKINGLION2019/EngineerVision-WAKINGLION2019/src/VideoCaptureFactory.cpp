#include "VideoCaptureFactory.h"

VideoCaptureFactory::VideoCaptureFactory(const char *dev_name,
                    int width,
                    int height,
                    int exposure,
                    int size_buffer)
{
    open_dev(dev_name);
    init(width,height,exposure);

    start=cv::getTickCount();
}

VideoCaptureFactory::VideoCaptureFactory(int dev_name,
                    int width,
                    int height,
                    int exposure,
                    int size_buffer)
{
    open_dev(dev_name);
    init(width,height,exposure);

    start=cv::getTickCount();
}

void VideoCaptureFactory::init(int width,
          int height,
          int exposure)
{
    capture.set(CV_CAP_PROP_FRAME_WIDTH, width);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, height);
    //capture.set(CV_CAP_PROP_EXPOSURE, exposure);
    //得到相机信息
    std::cout<<"宽："<<capture.get(CV_CAP_PROP_FRAME_WIDTH)<<std::endl;
    std::cout<<"高："<<capture.get(CV_CAP_PROP_FRAME_HEIGHT)<<std::endl;
    std::cout<<"曝光时间："<<capture.get(CV_CAP_PROP_EXPOSURE)<<std::endl;
}

bool VideoCaptureFactory::open_dev(const char *dev_name)
{
    capture.open(dev_name);
    if(!capture.isOpened())
    {
        std::cout<<"Camera open failed!"<<std::endl;
        return false;
    }
}

bool VideoCaptureFactory::open_dev(int dev_name)
{
    capture.open(dev_name);
    if(!capture.isOpened())
    {
        std::cout<<"Camera open failed!"<<std::endl;
        return false;
    }
}

void VideoCaptureFactory::setSaveFormat(const char *img_path,
                    const char *img_format,
                    long interval)
{
    this->img_path=img_path;
    this->img_format=img_format;
    this->interval=interval;
}

VideoCaptureFactory& VideoCaptureFactory::operator >> (cv::Mat &image)
{
    capture>>image;
    if(image.empty())
    {
        fprintf(stderr, "Can not load image\n");//如果读入图像失败，返回错误信息
    }
	return *this;
}

bool VideoCaptureFactory::grabImg(cv::Mat &img,char order)
{
    img_show=img.clone();
    //1 s
    //=1000       ms = 10e3 ms
    //=1000000    us = 10e6 us
    //=1000000000 ns = 10e9 ns
    if(cv::getTickCount()-start>=this->interval)
    {
        start=cv::getTickCount();

        std::ostringstream s;
        s<<this->img_path<<++img_filename<<this->img_format;
        cv::imwrite(s.str(),img);
        img_show=-img_show;
    }

    switch(order)
    {
    case 'q':
    case 'Q':
    case 27:
        return false;
    case 32:
    {
        std::ostringstream s;
        s<<this->img_path<<++img_filename<<this->img_format;
        cv::imwrite(s.str(),img);
        img_show=-img_show;
    }
        break;
    default:
        break;
    }
    return true;
}

void VideoCaptureFactory::showResult()
{
    std::ostringstream s;
    s<<img_filename;
    cv::putText(img_show,s.str(),cv::Point(60,30),
                CV_FONT_HERSHEY_COMPLEX_SMALL,
                1.3,cv::Scalar(0,255,0),1);
    cv::imshow("result",img_show);
}
