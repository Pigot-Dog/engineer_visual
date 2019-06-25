#include "Preprocessing.h"

#define WIN_NAME_HSV "hsv"
#define WIN_NAME_ADT "adaptive"

//Preprocessing类
Preprocessing::Preprocessing()
{
    element_size = 5;
}


Preprocessing::~Preprocessing()
{
    element1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 5));
    element2 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 7));
}

void Preprocessing::setStructElement()
{
    cv::createTrackbar("element_size", WIN_NAME_HSV, &element_size, 50, on_setStructElement, &element);
    //on_setStructElement(element_size,0);
}

void on_setStructElement(int element_size, void* element)
{
    *(cv::Mat*)element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(element_size, element_size));
}

//AdaptiveThreshold类
AdaptiveThreshold::AdaptiveThreshold()
{
    adt.param1 = 103;
    adt.param2 = 255;
}

AdaptiveThreshold::AdaptiveThreshold(struct PreprocessParam &preprocess_param)
{
    this->preprocess_param=&preprocess_param;
}


AdaptiveThreshold::~AdaptiveThreshold()
{
}

void AdaptiveThreshold::adaptiveThresholdProc(cv::Mat src, cv::Mat &dst,
                                              int method, int morphology,
                                              int iteration, int filter)
{
    switch (method)
    {
    case 0://hsv
    {
        setHsv();
        hsvThresholdProc(src, dst);
        break;
    }
    case 1://Canny
        cvtColor(src, src, cv::COLOR_BGR2GRAY);
        cv::blur(src, src, cv::Size(3, 3));
        cv::Canny(src, dst, adt.param1, adt.param2, 3);
        break;
    case 2://局部自适应
        cvtColor(src, src, cv::COLOR_BGR2GRAY);
        cv::adaptiveThreshold(src, dst, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY,
            preprocess_param->gray_thre_min * 2 + 1,
            preprocess_param->gray_thre_max);//参数要改
        //cv::imshow("bin", dst);
        //cv::waitKey();
        break;
    case 3://大津算法
        cvtColor(src, src, cv::COLOR_BGR2GRAY);
        cv::threshold(src, dst, adt.param1, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
        //cv::imshow("bin",dst);
        //cv::waitKey();
        break;
    case 5:
    {
        std::vector<cv::Mat> rgbChannels(3);
        cv::split(src, rgbChannels);

        cv::imshow("r", rgbChannels[2]);
        cv::imshow("g", rgbChannels[1]);
        cv::imshow("b", rgbChannels[0]);

        cv::Mat gray = rgbChannels[1] - rgbChannels[2];//绿色通道 - 红色通道
        cv::threshold(gray, dst, adt.param1, 255, cv::THRESH_BINARY);
        break;
    }
    case 6:
    {
        cv::Mat gray_b(src.size(), CV_8UC1, cv::Scalar(0));
        cv::Mat gray_g(src.size(), CV_8UC1, cv::Scalar(0));
        cv::Mat gray_r(src.size(), CV_8UC1, cv::Scalar(0));

        int srcRows = src.cols*src.channels();
        for (int i = 0; i < srcRows; i++)
        {
            uchar *p_src = src.data + srcRows*i;
            uchar *p_gray_b = gray_b.data + gray_b.cols*i;
            uchar *p_gray_g = gray_g.data + gray_g.cols*i;
            uchar *p_gray_r = gray_r.data + gray_r.cols*i;
            for (int j = 0; j < src.cols; j++)
            {
                int cols = src.channels() * j;
                *(p_gray_b + j) = *(p_src + cols);
                *(p_gray_g + j) = *(p_src + cols);
                *(p_gray_r + j) = *(p_src + cols);
            }
        }
    }
    break;
    case 7:
    {
        gray_b = cv::Mat::zeros(src.size(), CV_8UC1);
        gray_g = cv::Mat::zeros(src.size(), CV_8UC1);
        gray_r = cv::Mat::zeros(src.size(), CV_8UC1);

        uchar *p_src = src.data;
        uchar *p_gray_b = gray_b.data;
        uchar *p_gray_g = gray_g.data;
        uchar *p_gray_r = gray_r.data;
        uchar *p_src_end = src.data + src.rows*src.cols*src.channels();
        for (; p_src != p_src_end; p_src++, p_gray_b++, p_gray_g++, p_gray_r++)
        {
            if (*p_src > preprocess_param->gray_thre_min)
                *p_gray_b = 255;

            *p_gray_g = *(++p_src);

            if (*(++p_src) > preprocess_param->gray_thre_min)
                *p_gray_r = 255;
        }
        if (1)
        {
            dst = gray_b;
        }//
    }
    break;
    case 8:
    {
        cv::Mat gray_b(src.size(), CV_8UC1, cv::Scalar(0));
        cv::Mat gray_g(src.size(), CV_8UC1, cv::Scalar(0));
        cv::Mat gray_r(src.size(), CV_8UC1, cv::Scalar(0));

        uchar *p_src = src.data;
        uchar *p_gray_b = gray_b.data;
        uchar *p_gray_g = gray_g.data;
        uchar *p_gray_r = gray_r.data;
        uchar *p_src_end = src.data + src.rows*src.cols*src.channels();
        for (; p_src != p_src_end; p_src++, p_gray_b++, p_gray_g++, p_gray_r++)
        {
            uchar b = *p_src;
            *p_gray_g = *(++p_src);
            uchar r = *(++p_src);

            if (b > preprocess_param->gray_thre_min)
                *p_gray_b = 255;
            if (b - r > 220 && b >= *p_gray_g)
                *p_gray_b = 255;

//            if (r > preprocess_param->gray_thre_min)
//                *p_gray_r = 255;
//            if (b - r > 30 && b >= *p_gray_g)
//                *p_gray_r = 255;
        }
        if (1)
        {
            dst = gray_b;
        }
    }
    break;
    default://全局
        //std::cout<<adt.param1<<std::endl;
        cvtColor(src, src, cv::COLOR_BGR2GRAY);
        cv::inRange(src, cv::Scalar(preprocess_param->gray_thre_min),
            cv::Scalar(preprocess_param->gray_thre_max), dst);
        cv::imshow("bin", dst);
        break;
    }

    switch (morphology)
    {
        setStructElement();
    case 1://开操作
        cv::morphologyEx(dst, dst, cv::MORPH_OPEN, element, cv::Point(-1,-1), iteration);
        break;
    case 2://闭操作
        cv::morphologyEx(dst, dst, cv::MORPH_CLOSE, element, cv::Point(-1, -1), iteration);
        break;
    }

    switch (filter)
    {
    case 1://开操作、闭操作

        break;
    case 2://中值滤波
        cv::medianBlur(dst, dst, 3);
        break;
    }
}


void AdaptiveThreshold::setAdaptive()
{
    cv::namedWindow(WIN_NAME_ADT);
    cv::createTrackbar("blockSize", WIN_NAME_ADT, &adt.param1, 255);
    cv::createTrackbar("constValue", WIN_NAME_ADT, &adt.param2, 10000);
}

void AdaptiveThreshold::hsvThresholdProc(cv::Mat src, cv::Mat &dst)
{
    cv::Mat bgr_img, hsv_img, bin_img, bin2_img;
    src.convertTo(bgr_img, CV_32FC3, 1.0 / 255, 0);
    cv::cvtColor(bgr_img, hsv_img, cv::COLOR_BGR2HSV);
    inRange(hsv_img,
            cv::Scalar(hsv.hmin, hsv.smin / 255.0, hsv.vmin / 255.0),
            cv::Scalar(hsv.hmax, hsv.smax / 255.0, hsv.vmax / 255.0), bin_img);
    inRange(hsv_img,
            cv::Scalar(hsv.hmin2, hsv.smin / 255.0, hsv.vmin / 255.0),
            cv::Scalar(hsv.hmax2, hsv.smax / 255.0, hsv.vmax / 255.0), bin2_img);
    dst = bin_img + bin2_img;

    //setStructElement();
    //cv::morphologyEx(bin_img, bin_img, cv::MORPH_OPEN, element);
    //cv::morphologyEx(bin_img, dst,cv::MORPH_CLOSE,element);
}

void AdaptiveThreshold::initHsv()
{
    hsv.hmin = 0;
    hsv.hmax = 21;
    hsv.hmin2 = 355;
    hsv.hmax2 = 360;

    hsv.smin = 152;
    hsv.smax = 240;

    hsv.vmin = 18;
    hsv.vmax = 104;
}

void AdaptiveThreshold::setHsv()
{
    cv::namedWindow(WIN_NAME_HSV);
    cv::createTrackbar("hmin", WIN_NAME_HSV, &hsv.hmin, 360);
    cv::createTrackbar("hmax", WIN_NAME_HSV, &hsv.hmax, 360);
    cv::createTrackbar("hmin2", WIN_NAME_HSV, &hsv.hmin2, 360);
    cv::createTrackbar("hmax2", WIN_NAME_HSV, &hsv.hmax2, 360);

    cv::createTrackbar("smin", WIN_NAME_HSV, &hsv.smin, 255);
    cv::createTrackbar("smax", WIN_NAME_HSV, &hsv.smax, 255);

    cv::createTrackbar("vmin", WIN_NAME_HSV, &hsv.vmin, 255);
    cv::createTrackbar("vmax", WIN_NAME_HSV, &hsv.vmax, 255);
}

void AdaptiveThreshold::mulAndIterBlueTre(cv::Mat src,cv::Mat &bin,
                                          struct PreprocessInfo &preprocess_info)
{
    gray_b=cv::Mat::zeros(src.size(),CV_8UC1);
    gray_g=cv::Mat::zeros(src.size(),CV_8UC1);

    int srcRows=src.cols*src.channels();
    for(int i=0;i<src.rows;i++)
    {
        uchar *p_src=src.data+srcRows*i;
        uchar *p_gray_b=gray_b.data+gray_b.cols*i;
        uchar *p_gray_g=gray_g.data+gray_g.cols*i;

        int jj=0;
        for(int j=0;j<srcRows;j+=3)
        {
            *(p_gray_b+jj)=*(p_src+j)*0.7+*(p_src+j+1)*0.2+*(p_src+j+2)*0.1;//*(*(p_src+j+1))*1.0/255;
            *(p_gray_g+jj)=*(p_src+j+1);
            jj++;
        }
    }

    //最大灰度
    uchar gray_max=gray_b.data[0];
    for(int i=0;i<gray_b.rows;i++)
    {
        uchar *p=gray_b.data+i*gray_b.cols;
        for(int j=0;j<gray_b.cols;j++)
        {
            if(*(p+j)>gray_max)
                gray_max=*(p+j);
        }
    }

    int &thre=preprocess_info.thre;
    thre=gray_max*preprocess_param->gray_max_w/10. +
            avgGrayThreshold(gray_b)*preprocess_param->gray_avg_w*2;
    thre=math_max(preprocess_param->gray_thre_min,thre);
    thre=math_min(preprocess_param->gray_thre_max,thre);
    cv::threshold(gray_b, bin, thre, 255, 0);
    bin_b=bin.clone();
}

void AdaptiveThreshold::mulAndIterRedTre(cv::Mat src,cv::Mat &bin,
                                         struct PreprocessInfo &preprocess_info)
{
    gray_g=cv::Mat::zeros(src.size(),CV_8UC1);
    gray_r=cv::Mat::zeros(src.size(),CV_8UC1);

    int srcRows=src.cols*src.channels();
    for(int i=0;i<src.rows;i++)
    {
        uchar *p_src=src.data+srcRows*i;
        uchar *p_gray_g=gray_g.data+gray_g.cols*i;
        uchar *p_gray_r=gray_r.data+gray_r.cols*i;

        int jj=0;
        for(int j=0;j<srcRows;j+=3)
        {
            *(p_gray_g+jj)=*(p_src+j+1);
            *(p_gray_r+jj)=*(p_src+j)*0.1+*(p_src+j+1)*0.2+*(p_src+j+2)*0.7;
            jj++;
        }
    }

    //最大灰度
    uchar gray_max=gray_r.data[0];
    for(int i=0;i<gray_r.rows;i++)
    {
        uchar *p=gray_r.data+i*gray_r.cols;
        for(int j=0;j<gray_r.cols;j++)
        {
            if(*(p+j)>gray_max)
                gray_max=*(p+j);
        }
    }

    int &thre=preprocess_info.thre;
    thre=gray_max*preprocess_param->gray_max_w/10. +
            avgGrayThreshold(gray_r)*preprocess_param->gray_avg_w*2;
    thre=math_max(preprocess_param->gray_thre_min,thre);
    thre=math_min(preprocess_param->gray_thre_max,thre);

    cv::threshold(gray_r, bin, thre, 255, 0);
    bin_r=bin.clone();
}

int AdaptiveThreshold::iterationThreshold(cv::Mat src)
{
    int width = src.cols;
    int height = src.rows;
    int hisData[256] = { 0 };
    //统计直方图
    for (int j = 0; j < height; j++)
    {
        uchar* data = src.ptr<uchar>(j);
        for (int i = 0; i < width; i++)
            hisData[data[i]]++;
    }

    //加权求和
    int T0 = 0;
    for (int i = 0; i < 256; i++)
    {
        T0 += i * hisData[i];
    }
    T0 /= width * height; //平均灰度值

    int T1 = 0, T2 = 0;
    int num1 = 0, num2 = 0;
    int T = 0;
    while (1) //全黑会卡住
    {
        for (int i = 0; i < T0 + 1; i++)
        {
            T1 += i * hisData[i];
            num1 += hisData[i];
        }
        if (num1 == 0)
            continue;
        for (int i = T0 + 1; i < 256; i++)
        {
            T2 += i * hisData[i];
            num2 += hisData[i];
        }
        if (num2 == 0)
            continue;

        T = (T1 / num1 + T2 / num2) / 2;

        if (T == T0)
            break;
        else
            T0 = T;
    }

    return T;
}

int AdaptiveThreshold::avgGrayThreshold(cv::Mat src)
{
    long sum_gray=0;
    for(int j=0;j<src.rows;j++)
    {
        uchar *data=src.ptr<uchar>(j);
        for(int i=0;i<src.cols;i++)
        {
            sum_gray+=data[i];
        }
    }
    return sum_gray*1.0/(src.cols*src.rows);
}

void AdaptiveThreshold::filter(cv::Mat &dst)
{
    cv::erode(dst, dst, element1);
    cv::dilate(dst, dst, element2);

//    setStructElement();
//    cv::morphologyEx(dst, dst, cv::MORPH_OPEN, element);
//    cv::morphologyEx(dst, dst,cv::MORPH_CLOSE,element);
    cv::medianBlur(dst, dst, 3);
}
