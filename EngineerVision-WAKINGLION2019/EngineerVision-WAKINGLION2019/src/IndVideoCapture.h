//-----------------------------------【视频处理类】--------------------------------------------
// brief：重写的视频处理类，有调整分辨率/FPS/曝光度等功能
//------------------------------------------------------------------------------------------------

#ifndef INDVIDEOCAPTURE_H
#define INDVIDEOCAPTURE_H

#include "Settings.h"
#include "VideoCaptureFactory.h"
#include "RMVideoCapture.hpp"
#include "MVVideoCapture.h"

#include "opencv2/opencv.hpp"
#include <thread> //thread类、this_thread命名空间、内部类id
#include <mutex>
#include <condition_variable>

#define kItemRepositorySize 3 // Item buffer size.

struct ItemRepository
{
    cv::Mat item_buffer[kItemRepositorySize]; // 产品缓冲区, 配合 read_position 和 write_position 模型环形队列.
    size_t read_position;                     // 消费者读取产品位置.
    size_t write_position;                    // 生产者写入产品位置.
    std::mutex mtx;                           // 互斥量,保护产品缓冲区
    std::condition_variable repo_not_full;    // 条件变量, 指示产品缓冲区不为满.
    std::condition_variable repo_not_empty;   // 条件变量, 指示产品缓冲区不为空.
};

class IndVideoCapture:
        public RMVideoCapture
        //, public VideoCaptureFactory
{
public:
    IndVideoCapture():
        RMVideoCapture()
    {
        img_repository.read_position=0;
        img_repository.write_position=0;
    }

    void collecting();

    void ProduceItem();
    void ConsumeItem(cv::Mat &img);

    void ProduceItem(ItemRepository *ir);
    void ConsumeItem(ItemRepository *ir, cv::Mat &img);

    void init()
    {
        //枚举所有相机，返回相机句柄

        //初始化所有相机

        //得到相机信息
    }

    /**
     * @brief  打开摄像头
     * @param  第一个参数
     * @return 返回值
     * @autor  梁尧森，傅子权，练志深
     * @date   2018-
     */
    bool open(const char *filename,
              int width,
              int height,
              int exp_t);

    /**
     * @brief  设置曝光度
     * @param  第一个参数
     * @return 返回值
     * @autor  梁尧森，傅子权，练志深
     * @date   2018-
     */
    void setExposure();

    /**
     * @brief  设置分辨率
     * @param  第一个参数
     * @return 返回值
     * @autor  梁尧森，傅子权，练志深
     * @date   2018-
     */
    void setFormat();

//    void testProcessing()
//    {
//        int th=190;
//        cv::namedWindow("二值化测试");
//        cv::createTrackbar("二值化阈值","二值化测试",
//                           &th,255);

//        //采集帧率与帧数参数
//        long t1 = 0;
//        int frame_num = 0;
//        long t2 = 0;

//        while(1)
//        {
//            t1 = cv::getTickCount();
//            //1.视频流
//            cv::Mat src,gray,bin;
//            cap >> src;
//            cv::cvtColor(src,gray,CV_BGR2GRAY);
//            cv::threshold(gray,bin,th,255,0);

//#ifdef DEBUG_MODE
//            imshow("src", src);
//            imshow("bin", bin);
//#endif

//            t2=cv::getTickCount();
//            std::cout << "FPS: " << cv::getTickFrequency()/(t2 - t1) << " frame No.:" << frame_num << std::endl;
//            printf("\033[2J");
//            frame_num++;

//            cv::waitKey(1);
//        }
//    }

    //void testMVProc()
    //{
    //    printf("MVCamera Init\n");
    //    MVVideoCapture::Init(0);
    //    printf("MVCamera Play\n");
    //    MVVideoCapture::Play();
    //    //printf("MVCamera SetWBMode\n");
    //    //MVVideoCapture::SetWBMode(true);
    //    printf("MVCamera SetLargeResolution\n");
    //    MVVideoCapture::SetLargeResolution(true);
    //    printf("MVCamera SetExposureTime\n");
    //    MVVideoCapture::SetExposureTime(false, 5000);
    //    double read_exp_time = MVVideoCapture::GetExposureTime();
    //    printf("read_exp_time: %lf\n", read_exp_time);
    //    //printf("Set Gain\n");
    //    //MVVideoCapture::SetGain(1);
    //    bool auto_wb;
    //    printf("MVCamera GetWBMode\n");
    //    MVVideoCapture::GetWBMode(auto_wb);
    //    //printf("MVCamera SetOnceWB\n");
    //    //MVVideoCapture::SetOnceWB();

    //    cv::VideoWriter videoWriter("test.avi", CV_FOURCC('M', 'J', 'P', 'G'), 100, cv::Size(752, 480));

    //    int i = 0;
    //    double exp_time = 10000;
    //    bool   auto_exp = false;
    //    bool   large_resolution = false;
    //    cv::Mat frame;
    //    while (i < 1e10) {

    //    TIME_BEGIN()
    //    MVVideoCapture::GetFrame(frame);
    //    TIME_END("MVCamera::Read")

    //    if (frame.empty()) {
    //      cv::waitKey(10);
    //      continue;
    //    }

    //    videoWriter.write(frame);
    //    cv::imshow("read", frame);
    //    char cmd = cv::waitKey(1);

    //    if (cmd == 'a') {
    //      auto_exp = !auto_exp;
    //      MVVideoCapture::SetExposureTime(auto_exp, exp_time);
    //      double read_exp_time = MVVideoCapture::GetExposureTime();
    //      printf("read_exp_time: %lf\n", read_exp_time);
    //    }

    //    if (cmd == 'w') {
    //      exp_time += 10000;
    //      MVVideoCapture::SetExposureTime(auto_exp, exp_time);
    //      double read_exp_time = MVVideoCapture::GetExposureTime();
    //      printf("read_exp_time: %lf\n", read_exp_time);
    //    }

    //    if (cmd == 's') {
    //      if (exp_time-10000 > 0)
    //        exp_time -= 10000;
    //      MVVideoCapture::SetExposureTime(auto_exp, exp_time);
    //      double read_exp_time = MVVideoCapture::GetExposureTime();
    //      printf("read_exp_time: %lf\n", read_exp_time);
    //    }

    //    if (cmd == 'b') {
    //      large_resolution = !large_resolution;
    //      MVVideoCapture::SetLargeResolution(large_resolution);
    //    }

    //    if (cmd == 'q') {
    //      break;
    //    }

    //    i++;
    //    }
    //    videoWriter.release();
    //    MVVideoCapture::Stop();
    //    MVVideoCapture::Uninit();
    //}

private:
    struct ItemRepository img_repository;
};

#endif // INDVIDEOCAPTURE_H
