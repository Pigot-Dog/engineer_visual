#ifndef Gui_H
#define Gui_H

#include "Settings.h"
#include "IndVideoCapture.h"

#include "opencv2/opencv.hpp"
#include <iostream>

//-----------------------------------【调试界面模块】--------------------------------------------
// brief：可视化界面
//------------------------------------------------------------------------------------------------
class Gui
{
public:
    Gui();
    Gui(cv::Mat img);
    Gui(cv::Point2f center,double digits_size,int status=0,cv::Mat img=cv::Mat());
    ~Gui();

    /**
     * @brief  gui调试线程
     * @param  第一个参数
     * @return 返回值
     * @autor  梁尧森
     * @date   2018-
     */
    void processing(MainSettings *setting,IndVideoCapture *cap);

    /**
     * @brief  画出边缘点
     * @param  第一个参数
     * @return 返回值
     * @autor  梁尧森
     * @date   2018-
     */
    void drawingPointProc(std::vector<std::vector<cv::Point2i>>contours, cv::Scalar color, int i);

    /**
     * @brief  画出边缘列表及其编号
     * @param  第一个参数
     * @return 返回值
     * @autor  梁尧森
     * @date   2018-
     */
    void drawingProc(std::vector<std::vector<cv::Point2i>>contours, std::vector<cv::Rect>, cv::Scalar color,int i);

    /**
     * @brief  更新画布
     * @param  第一个参数
     * @return 返回值
     * @autor  梁尧森
     * @date   2018-
     */
    void setShowImg(cv::Mat img);

    /**
     * @brief  实时绘制可视化信息
     * @param  画布
     * @param  装甲板四个顶点
     * @param  姿态角的三个坐标轴的单位向量，只传入顶点（1,0,0），（0,1,0），（0,0,1）
     * @param  pitch角
     * @param  yaw角
     * @param  距离
     * @return 返回值
     * @autor  梁尧森
     * @date   2018-
     */
    void drawingInfo(cv::Mat &img,
                     std::vector<cv::Point2f> armor_rect,
                     std::vector<cv::Point2f> attitude_angle,
                     double pitch,
                     double yaw,
                     double dist);

    void showDebug(cv::Mat &img,float data,cv::Point point,cv::Scalar color);
private:
    cv::Mat src;
    cv::Mat ret_img;

    cv::Point2f center;
    double digits_size;
    int status;
};

typedef struct ProOffset
{
    float proportion_v;//值比例
    float proportion_t;//时间比例

    int offset_v;//值偏移
    int offset_t;//时间偏移

    ProOffset()
    {

    }
}ProOffset_t;

//-----------------------------------【数据波形】--------------------------------------------
// brief：方便查看实时系统的历史数据，对数据的变化有一个直观的理解
// 完成功能：
// 1、完成dataCollectProc方法，实现多个数据同时在同一张图中显示
// 完成dataCollectInvProc方法，实现多个数据同时在同一张图中显示，dataCollectInvProc方法可分别调用
// 2、完善平移时画布坐标线的显示
// 3、实现水平与垂直方向的缩放功能
// 垂直方向超出范围自动缩放
// 4、实现使用鼠标平移画布
//------------------------------------------------------------------------------------------------
class DataCollect
{
public:
    DataCollect(int canvas_w,int canvas_h,int ndataSize=-1,int ndataRange=-1);

    /**
     * @brief  重载1：调用私有方法dataCollect，通过多次调用，传入多个数据，在一个窗口绘制该组数据的波形
     * @param  winname 窗口名
     * @return
     * @autor  梁尧森
     * @date   2019.3.31
     */
    void dataCollectProc(const std::string winname);

    /**
     * @brief  重载2：调用私有方法dataCollect，通过多次调用，传入多个数据，在一个窗口绘制该组数据的波形
     * @param  data    显示的数据向量
     * @param  color   对应的颜色向量
     * @param  winname 窗口名
     * @return
     * @autor  张泾锋
     * @date   2018-11-14
     */
    void dataCollectProc(std::vector<float> data,
                                      std::vector<cv::Scalar> color,
                                      std::string winname);

    /**
     * @brief  调用私有方法dataCollect，通过传入一组数据，在一个窗口绘制该组数据的波形
     * @param  data    显示的数据
     * @param  color   对应的颜色
     * @param  winname 窗口名
     * @return
     * @autor  张泾锋
     * @date   2018-11-14
     */
    void dataCollectInvProc(float data,cv::Scalar color,const std::string winname);

    /**
     * @brief  私有方法，实现核心的数据波形绘制操作
     * @param  data    显示的数据
     * @param  color   对应的颜色
     * @param  winname 窗口名
     * @return
     * @autor  梁尧森
     * @date   2018-11-14
     */
    void dataCollect(float data,cv::Scalar color,int data_q_index,const std::string winname);

    /**
     * @brief
     * @param  data    显示的数据
     * @param  color   对应的颜色
     * @return
     * @autor  梁尧森
     * @date   2018-11-14
     */
    void gui();
    void gui(const std::string winname);

    /**
     * @brief  设置画布
     * @return
     * @autor  梁尧森
     * @date   2018-11-14
     */
    void setCanvas();
    void setData(std::vector<float> data)
    {
        this->data=data;
    }

private:
    std::vector<float> data;
    float data_q[DATA_BUFF_ROW_SIZE_MAX][DATA_BUFF_COL_SIZE_MAX];//数据队列
    int data_q_h;
    int canvas_w;  //画布宽度
    int ndataSize; //显示数据个数
    int ndataRange;//显示数据范围
    float interval;//区间=画布宽度/显示数据个数
    int canvas_h;  //画布高度
    cv::Mat coll;  //画布

    ProOffset pro_offset;
};

#endif /* Gui_hpp */
