//-----------------------------------【title】--------------------------------------------
// brief：
//------------------------------------------------------------------------------------------------

#ifndef PREPROCESSING_H
#define PREPROCESSING_H

#include "Settings.h"
#include "Common.h"

#include "opencv2/opencv.hpp"

struct Hsv
{
    int hmin;
    int hmax;
    int hmin2;
    int hmax2;
    
    int smin;
    int smax;
    
    int vmin;
    int vmax;
};

struct Adaptive
{
    int param1;
    int param2;
};

//-----------------------------------【预处理】--------------------------------------------
// brief：对从相机获取的图片进行预先的处理，如二值化、滤波等，方便后续信息的提取，如目标的位置
//------------------------------------------------------------------------------------------------
class Preprocessing
{
public:
    Preprocessing();
    ~Preprocessing();
    void setStructElement();
public:
    cv::Mat element;
    cv::Mat element1;
    cv::Mat element2;
    int element_size;
};

//-----------------------------------【二值化】--------------------------------------------
// brief：能根据光照自动调整阈值二值化方法
//------------------------------------------------------------------------------------------------
class AdaptiveThreshold :Preprocessing
{
public:
    AdaptiveThreshold();
    AdaptiveThreshold(struct PreprocessParam &preprocess);
    ~AdaptiveThreshold();

    /**
     * @brief  局部二值化
     * @param  源图
     * @return
     * @autor  sen
     * @date   2018.11.7
     */
    void adaptiveThresholdProc(cv::Mat src, cv::Mat &dst,
                               int method = -1, int morphology = -1,
                               int iteration = 1, int filter = -1);
    void setAdaptive();

    /**
     * @brief  获取通道灰度图
     * @return 返回值
     * @autor  梁尧森
     * @date   2019-1-13
     */
    cv::Mat getGrayB()
    {
        return gray_b;
    }
    cv::Mat getGrayG()
    {
        return gray_g;
    }
    cv::Mat getGrayR()
    {
        return gray_r;
    }

    /**
     * @brief  获取通道二值图
     * @return 返回值
     * @autor  梁尧森
     * @date   2019-1-13
     */
    cv::Mat getBinB()
    {
        return bin_b;
    }
    cv::Mat getBinG()
    {
        return bin_g;
    }
    cv::Mat getBinR()
    {
        return bin_r;
    }

    //-----------------------------------【多种阈值方法】--------------------------------------------
    // brief：
    //------------------------------------------------------------------------------------------------
    /**
     * @brief  基于hsv二值化
     * @param  源图
     * @return
     * @autor  sen
     * @date   2018.11.6
     */
    void hsvThresholdProc(cv::Mat src, cv::Mat &dst);
    void initHsv();
    void setHsv();

    /**
     * @brief  正片叠底+迭代法测试（蓝色）
     * @param  第一个参数
     * @return 返回值
     * @autor  梁尧森
     * @date   2018-
     */
    void mulAndIterBlueTre(cv::Mat src,cv::Mat &bin,
                           struct PreprocessInfo &preprocess_info);

    /**
     * @brief  正片叠底+迭代法测试（红色）
     * @param  第一个参数
     * @return 返回值
     * @autor  梁尧森
     * @date   2018-
     */
    void mulAndIterRedTre(cv::Mat src,cv::Mat &bin,
                          struct PreprocessInfo &preprocess_info);

    /**
     * @brief  迭代阈值分割
     * @param  第一个参数含义
     * @return 返回值含义
     * @autor  sen
     * @date   2018.9.21
     */
    int iterationThreshold(cv::Mat src);

    /**
     * @brief  平均灰度法
     * @param  第一个参数
     * @return 返回值
     * @autor  梁尧森
     * @date   2018-
     */
    int avgGrayThreshold(cv::Mat src);

    void filter(cv::Mat &dst);

    cv::Mat subC(cv::Mat src,int c)
    {
        cv::Mat a_channels[3];
        cv::split(src,a_channels);
        if(c==0) //red
            return a_channels[0]-a_channels[2];
        else     //blue
            return a_channels[2]-a_channels[0];
    }

private:
    Adaptive adt;
    Hsv hsv;

    struct PreprocessParam *preprocess_param;

    cv::Mat gray_b;
    cv::Mat gray_g;
    cv::Mat gray_r;

    cv::Mat bin_b;
    cv::Mat bin_g;
    cv::Mat bin_r;
};

void on_setStructElement(int, void*);

#endif
