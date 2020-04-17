#include "ColorDetector.h"

ColorDetector::ColorDetector()
{

}

ColorDetector::ColorDetector(cv::Scalar color_center,
                             double color_dist)
{
    this->color_center=color_center;
    this->max_dist=color_dist;
}

ColorDetector::ColorDetector(uchar b,uchar g,uchar r,
	double color_dist)
{

}

cv::Mat ColorDetector::process(cv::Mat src)
{
    cv::Mat img;
    if(!src.isContinuous())
    {
        img=src.clone();
    }
    else
    {
        img=src;
    }

    this->ret=cv::Mat::zeros(img.size(),CV_8UC1);

    int src_rows=img.rows*img.channels();
    for(int i=0;i<img.cols;i++)
    {
        uchar *p_src=img.data+i*src_rows;
        uchar *p_ret=ret.data+i*this->ret.rows;
        int jj=0;
        for(int j=0;jj<ret.rows;j+=img.channels(),jj++)
        {
            if(getDist2TgtColor(*(p_src+j),
                                *(p_src+j+1),
                                *(p_src+j+2))<=this->max_dist)
            {
                *(p_ret+jj)=255;
            }
        }
    }

    return ret;
}

double ColorDetector::getDist2TgtColor(uchar b,uchar g,uchar r)
{
	return (abs(color_center[0] - b) +
		abs(color_center[1] - g) +
		abs(color_center[2] - r)) / 765.0;
}

double ColorDetector::getDist2TgtColorByWeight(uchar b, uchar g, uchar r)
{
	return (color_center[0] * abs(color_center[0] - b) +
		color_center[1] * abs(color_center[1] - g) +
		color_center[2] * abs(color_center[2] - r)) / (765.0 + 
			color_center[0] * 
			color_center[1] * 
			color_center[2]);
}
