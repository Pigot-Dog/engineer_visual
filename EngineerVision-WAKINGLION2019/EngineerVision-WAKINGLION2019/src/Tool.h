#ifndef TOOL_H
#define TOOL_H

#include "Settings.h"
#include "Tool.h"

#include "opencv2/opencv.hpp"
#include <iostream>
#include <string>
#include <chrono>

using namespace std;
using namespace std::chrono;

static inline cv::Point calcPoint(cv::Point2f center,
                                  double R,
                                  double angle)
{
    return center+
            cv::Point2f((float)cos(angle),
                        (float)-sin(angle))*(float)R;
}

class Tool
{
public:
    Tool();

    static bool grabImg(cv::Mat &img,char order,long interval)
    {
        cv::Mat img_show=img.clone();
        //1 s
        //=1000       ms = 10e3 ms
        //=1000000    us = 10e6 us
        //=1000000000 ns = 10e9 ns
        static long start;
        static int img_filename;
        if(cv::getTickCount()-start>=interval)
        {
            start=cv::getTickCount();

            std::ostringstream s;
            s<<SAVE_PIC_DIR<<++img_filename<<".png";
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
            s<<SAVE_PIC_DIR<<++img_filename<<".png";
            cv::imwrite(s.str(),img);
            img_show=-img_show;
        }
            break;
        default:
            break;
        }
#ifdef DEBUG_MODE
        return showResult(img.clone(),img_filename);
#else
        return true;
#endif
    }

    static bool showResult(cv::Mat img_show,int img_filename)
    {
        std::ostringstream s;
        s<<img_filename;
        cv::putText(img_show,s.str(),cv::Point(60,30),
                    CV_FONT_HERSHEY_COMPLEX_SMALL,
                    1.3,cv::Scalar(0,255,0),1);
        cv::namedWindow("save_pic");
        cv::imshow("save_pic",img_show);
        return true;
    }

    static float addByWeight(std::vector<float> s,
                            std::vector<float> w)
    {
        float sum=0;
        for(int i=0;i<s.size();i++)
        {
            sum+=s[i]*w[i];
        }
        return sum;
    }
};

class Timer
{
public:

    Timer():_name("Time elapsed:")
    {
        restart();
    }

    explicit Timer(const std::string &name):_name(name + ":")
    {
        restart();
    }

    /**
    * 启动计时
    */
    inline void restart()
    {
        _start_time = std::chrono::steady_clock::now();
    }

    /**
    * 结束计时
    * @return 返回ms数
    */
    inline double elapsed(bool restart = false)
    {
        _end_time = std::chrono::steady_clock::now();
        std::chrono::duration<double> diff = _end_time-_start_time;
        if(restart)
            this->restart();
        return diff.count()*1000;
    }

    /**
     * 打印时间并重启计时器
     * @param tip 提示
     */
    void rlog(const std::string &tip)
    {
        log(true,tip,true,false);
    }

    /**
    * 打印时间
    * @param reset 输出之后是否重启计时器，true重启，false不重启
    * @param unit_ms true是ms，false是s
    * @param tip 输出提示
    * @param kill 输出之后是否退出程序，true退出，false不退出
    */
    void log(bool reset = false, const std::string &tip = "",
             bool unit_ms = true, bool kill = false)
    {
        if (unit_ms) {
            if (tip.length() > 0)
                std::cout << tip+":" << elapsed() << "ms" << std::endl;
            else
                std::cout << _name << elapsed() << "ms" << std::endl;
        } else {
            if (tip.length() > 0)
                std::cout << tip+":" << elapsed() / 1000.0 << "s" << std::endl;
            else
                std::cout << _name << elapsed() / 1000.0 << "s" << std::endl;
        }

        if (reset)
            this->restart();

        if (kill)
            exit(5);
    }

private:
    std::chrono::steady_clock::time_point _start_time;
    std::chrono::steady_clock::time_point _end_time;
    std::string _name;
}; // Timer

#endif // TOOL_H
