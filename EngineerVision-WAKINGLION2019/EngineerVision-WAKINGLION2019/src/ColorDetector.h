#ifndef COLORDETECTOR_H
#define COLORDETECTOR_H

#include "opencv2/opencv.hpp"
#include <iostream>
#include <math.h>

class ColorDetector
{
public:
    ColorDetector();

    /**
     * @brief  重载1
     * @param  颜色中心
     * @param  距离参数
     * @autor  梁尧森
     * @date   2019.3.31
     */
    ColorDetector(cv::Scalar color_center,
		double color_dist);

    /**
     * @brief  重载2
     * @param  颜色中心
     * @param  距离参数
     * @autor  梁尧森
     * @date   2019.3.31
     */
    ColorDetector(uchar b,uchar g,uchar r,
		double color_dist);

    /**
     * @brief  处理
     * @param  原图
     * @return 二值图
     * @autor  梁尧森
     * @date   2019.3.31
     */
    cv::Mat process(cv::Mat src);

    void setColorCenterAndDist(cv::Scalar color_center,
                        double color_dist)
    {
        this->color_center=color_center;
        this->max_dist=color_dist;
    }
private:
    /**
     * @brief
     * @param  颜色
     * @return 归一化的欧式距离
     * @autor  梁尧森
     * @date   2019.3.31
     */
	double getDist2TgtColor(uchar b,uchar g,uchar r);
	double getDist2TgtColorByWeight(uchar b, uchar g, uchar r);
private:
    cv::Mat ret;
    cv::Scalar color_center;
	double max_dist;
};

#endif // COLORDETECTOR_H
