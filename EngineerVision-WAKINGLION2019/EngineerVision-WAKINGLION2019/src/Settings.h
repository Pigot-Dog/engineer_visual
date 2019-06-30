//-----------------------------------【设置】--------------------------------------------
// brief：
// MainSettings 全局设置类，包括调试/串口等
// Settings 公共设置类，包括预处理与目标检测参数设置，被装甲板设置与神符设置继承
// ArmorSettings 装甲板设置类
// RuneSettings 神符设置类
//------------------------------------------------------------------------------------------------

#ifndef SETTINGS_H
#define SETTINGS_H

#include "Common.h"

#include "opencv2/opencv.hpp"
#include <iostream>
#include <thread> //thread类、this_thread命名空间、内部类id
#include <mutex>
#include <condition_variable>
#include "VisualPid.h"

//-----------------------------------【宏定义-全局】--------------------------------------------
// brief：
//------------------------------------------------------------------------------------------------
#define DEBUG_MODE     //调试模式
#define USE_DEBUG
//#define USE_DEBUG_CAMERA
#define SAVE_PIC   1   //0自动截图 1按键截图
//#define SHOW_DEBUG_IMG_CHOOSE  //筛选装甲板调试
#define USE_VIDEO  2   //0摄像头/视频 1图片 2摄像头多线程 3迈德威视摄像头
#define SHOW_RADIO 1   //显示比例
//#define USE_SHAPEGAN
#define USE_SERIAL 1    //使用串口
//#define USE_WIN
//#define USE_RECEIVE_STM32_INFO 1

//debug visual pid
//#define DEBUG_VISUAL_PID //debug pid
#define USE_VISUAL_PID

//-----------------------------------【宏定义-调试模块】--------------------------------------------
// brief：
//------------------------------------------------------------------------------------------------
#define VIDEO_PATH             "/dev/video0"
//#define VIDEO_PATH            "/home/robo-xiao/rm2019/sentry_src_139.avi"     //测试视频路径
#define PIC_PATH               "./test_pic/Rect.jpg"  //测试图片路径
#define WIN_NAME_TARGET        "目标检测调试"             //调试窗口命名
#define WIN_NAME_VISUALPID     "pid调试窗口名"
#define SHOW_WIDTH             640//752
#define SHOW_HEIGHT            480
#define DATA_BUFF_ROW_SIZE_MAX 5
#define DATA_BUFF_COL_SIZE_MAX 1200

#ifdef USE_WIN
#define PARAM_OTHER_PATH "E:/Rm2019/EngineerVision-WAKINGLION2019/EngineerVision-WAKINGLION2019/config_file/param_other.yml"  //全局配置文件路径
#define PARAM_CHEST_PATH "E:/Rm2019/EngineerVision-WAKINGLION2019/EngineerVision-WAKINGLION2019/config_file/param_chest.yml"

#define PARAM_CALIBRATION_752  "E:/Rm2019/EngineerVision-WAKINGLION2019/EngineerVision-WAKINGLION2019/calibration/Camera752.xml"  //相机参数

#define SAVE_VIDEO_DIR "E:/Rm2019/EngineerVision-WAKINGLION2019/EngineerVision-WAKINGLION2019/save_video/"
#define SAVE_PIC_DIR   "E:/Rm2019/EngineerVision-WAKINGLION2019/EngineerVision-WAKINGLION2019/save_pic/"

#define ARMOR_TEMPLATE_PATH "E:/Rm2019/EngineerVision-WAKINGLION2019/EngineerVision-WAKINGLION2019/template_path/"
#else
#define PARAM_OTHER_PATH "/home/robo/rm2019/engineer_visual/EngineerVision-WAKINGLION2019/EngineerVision-WAKINGLION2019/config_file/param_other.yml"  //全局配置文件路径
#define PARAM_CHEST_PATH "/home/robo/rm2019/engineer_visual/EngineerVision-WAKINGLION2019/EngineerVision-WAKINGLION2019/config_file/param_chest.yml"

#define PARAM_CALIBRATION_752  "/home/robo/rm2019/engineer_visual/EngineerVision-WAKINGLION2019/EngineerVision-WAKINGLION2019/calibration/Camera752.xml"  //相机参数

#define SAVE_VIDEO_DIR "/home/robo/rm2019/engineer_visual/EngineerVision-WAKINGLION2019/EngineerVision-WAKINGLION2019/save_video/"
#define SAVE_PIC_DIR   "/home/robo/rm2019/engineer_visual/EngineerVision-WAKINGLION2019/EngineerVision-WAKINGLION2019/save_pic/"
#endif
//-----------------------------------【宏定义-键盘指令】--------------------------------------------
// brief：
// 按v  开始、结束录视频
// 按p  截图
// 按s  保存参数
//------------------------------------------------------------------------------------------------
#define KEY_SAVE_VIDEO  76  //开始、结束录视频
#define KEY_SAVE_RESULT 70  //截图
#define KEY_SAVE_PARAM  115 //保存参数

struct VisualPIDStruct
{
    //pitch_pid
    int  pitch_p=294;
    int pitch_i=66;
    int pitch_d=0;
    int pitch_division=100;
    //yaw_pid
    int yaw_p=109;
    int yaw_i=21;
    int yaw_d=0;
    int yaw_division=100;
    //pid中p参数缩放倍数
    int p_division=100;
    //pid中i参数缩放倍数
    int i_division=100000;
    //pid中d参数缩放倍数
    int d_division=100000;
};

//-----------------------------------【配置-gui】--------------------------------------------
// brief：
//------------------------------------------------------------------------------------------------
struct Debug
{
    int b_show_src; //1.原图
    int b_show_bin; //2.二值图
    int b_show_target; //3.目标-灯条
    int b_show_armor; //4.装甲板
    int b_show_result; //5.结果
    int b_show_fps;
    int b_save_result;
    int  n_save_result;
    char n_key_order;
    int b_save_pic;
    int f_save_pic_inter;
    int expore_time;

    Debug()
    {
        b_show_src=false;
        b_show_bin=true;
        b_show_target=false; //3.目标-灯条
        b_show_armor=false; //4.装甲板
        b_show_result=true; //5.结果
        b_show_fps=true;
        b_save_result=false;
        n_save_result=0;
        n_key_order=-1;
        b_save_pic=true;
        f_save_pic_inter=1000;
        expore_time=3000;
    }
};

//-----------------------------------【配置-串口】--------------------------------------------
// brief：
//------------------------------------------------------------------------------------------------
struct DeviceParam
{
    const char *dev_name;  //设备名称
    int baud_rate;         //波特率
    int databits;          //数据位
    int stopbits;          //停止位
    char parity;            //校验位
    DeviceParam()
    {
        dev_name="/dev/ttyUSB0";
        baud_rate=115200;
        databits=8;
        stopbits=1;
        parity='n';
    }
};

//-----------------------------------【参数-敌方颜色】--------------------------------------------
// brief：敌方颜色：蓝/红
//------------------------------------------------------------------------------------------------
enum EnemyColor
{
    red,blue
};

//-----------------------------------【配置-模式】--------------------------------------------
// brief：弹药箱模式
//------------------------------------------------------------------------------------------------
enum MainMode
{
	chest_mode
};

struct CameraParam
{
    double x;
    double y;
    double z;
    double y_offset;
    double overlap_dist;
    double bullet_speed;
    CameraParam()
    {
        x=0.;
        y=5.;
        z=10.;
        y_offset=0.;
        overlap_dist=100000.;
        bullet_speed=26.;
    }
};

//-----------------------------------【类-主要设置】--------------------------------------------
// brief：
//------------------------------------------------------------------------------------------------
class MainSettings
{
public:
    MainSettings(const char *param);

    void setMainParam(const char *win_name);

    void setCameraParam(const char *win_name)
    {
        cv::namedWindow(win_name);
//        static int x=cp.x*100.;
//        cv::createTrackbar("cp-x",win_name,&x, 1000);
//        cp.x=x/100.;

//        static int y=cp.y*100.;
//        cv::createTrackbar("cp-y",win_name,&y, 1000);
//        cp.y=y/100.;

//        static int z=cp.z*100.;
//        cv::createTrackbar("cp-z",win_name,&z, 3000);
//        cp.z=z/100.;

//        static int y_offset=cp.y_offset*100.;
//        cv::createTrackbar("cp-y_offset",win_name,&y_offset, 1000);
//        cp.y_offset=y_offset/100.;

//        static int overlap_dist=cp.overlap_dist*1.;
//        cv::createTrackbar("cp-overlap_dist",win_name,&overlap_dist, 1000000);
//        cp.overlap_dist=overlap_dist/1.;

//        static int bullet_speed=cp.bullet_speed*100.;
//        cv::createTrackbar("cp-bullet_speed",win_name,&bullet_speed, 3000);
//        cp.bullet_speed=bullet_speed/100.;
    }

    /**
     * @brief  读取其它配置
     * @param  第一个参数
     * @return 返回值
     * @autor  梁尧森
     * @date   2019.3.7
     */
    void read_param_other(const char *param_path);

    /**
     * @brief  保存配置mainProcessing(MainSettings *main_setting,
                    ChestSettings
     * @param  第一个参数
     * @return 返回值
     * @autor  梁尧森
     * @date   2019.3.7
     */
    void write_param_other(const char *param_path);
public:
    int main_mode=chest_mode;
    int enemy_color=red;
    Debug debug;
    DeviceParam port_param;

    CameraParam cp;
};


//-----------------------------------【信息-目标检测】--------------------------------------------
// brief：
//------------------------------------------------------------------------------------------------
//预处理信息
struct PreprocessInfo
{
    int thre;
};

//目标尺寸
struct TargetSize
{
    int len;              //1.轮廓长度
    float radio;          //2.长宽比
    int area;             //3.面积
    float area_radio;     //4.面积比
    float area_len_radio; //5.面积比长度
    int corners;        //6.多边形逼近
    int slope_offset;     //7.角度偏移
};

struct QuadrilateralPos
{
    cv::Point2f p[4];  //四边形目标顶点在屏幕投影坐标
};

//-----------------------------------【参数-预处理】--------------------------------------------
// brief：
//------------------------------------------------------------------------------------------------
struct PreprocessParam
{
    int gray_thre_min;            //灰度图二值化最小阈值
    int gray_thre_max;            //灰度图二值化最大阈值
    int gray_max_w;
    int gray_avg_w;

    PreprocessParam()
    {
        gray_thre_min=35;
        gray_thre_max=70;

        gray_max_w=6;
        gray_avg_w=4;
    }
};

//-----------------------------------【参数-目标尺寸】--------------------------------------------
// brief：
//------------------------------------------------------------------------------------------------
struct TargetSizeParam
{
    int len_min;   //轮廓长度最小值
    int len_max;   //轮廓长度最大值
    int radio_min; //长宽比最小值
    int radio_max; //长宽比最大值
    int area_min;  //最小面积
    int area_max;  //最大面积

    int area_ratio_max;
    int len_ratio_max;

    float area_radio_min;   //矩形度最小值
    float area_radio_max;   //矩形度最大值
    float area_len_radio_min;
    float area_len_radio_max;
    int corners_size_min;
    int slope_offset;  //角度偏移
    int color_th;

    int hough_circle_param1;
    int hough_circle_param2;
    int hough_circle_minradiu; //最小半径
    TargetSizeParam()
    {
        len_min=50;
        len_max=10000;
        radio_min=1;
        radio_max=2;
        area_min=5000;
        area_max=300000;
        area_ratio_max = 4277;
        area_radio_min=0.5;
        area_radio_max=1.2f;
        area_len_radio_min=0.5;
        area_len_radio_max=10.f;
        corners_size_min=7;
        slope_offset=15;
        color_th=60;

        hough_circle_param1=10;
        hough_circle_param2=10;
        hough_circle_minradiu=10;
    }
};

//-----------------------------------【类-设置】--------------------------------------------
// brief：
//------------------------------------------------------------------------------------------------
class Settings
{
public:
    Settings();

    /**
     * @brief  简介
     * @param  第一个参数
     * @return 返回值
     * @autor  梁尧森
     * @date   2018-
     */
    void setPreprocessParam(const char *win_name);

    /**
     * @brief  设置目标检测参数线程（重载一）
     * @return 无
     * @autor  梁尧森
     * @date   2019.3.16
     */
    void setTgtSizeParam(const char *win_name,
                         struct TargetSizeParam &tgt_size_param);

    /**
     * @brief  设置目标检测参数线程（重载二）
     * @return 无
     * @autor  梁尧森
     * @date   2019.3.16
     */
    void setTgtSizeParam(const char *win_name);
public:
    struct PreprocessParam preprocess_param;
    struct TargetSizeParam tgt_size_param;
};

//-----------------------------------【信息-弹药箱检测】--------------------------------------------
// brief：
//------------------------------------------------------------------------------------------------

struct LightInfo
{
    int idx; //索引

    std::vector<cv::Point>contours;       //轮廓
	struct TargetSize size;       //轮廓尺寸信息
	cv::RotatedRect rrect;
	cv::Rect        rect;
	cv::Mat roi;
};

//-----------------------------------【参数-弹药箱】--------------------------------------------
// brief：
//
//------------------------------------------------------------------------------------------------
struct ChestParam {
    double hu_dist_min;           //hu矩距离最小值
    double hu_dist_max;           //hu矩距离最大值

    int avg_board_gray_threshold;
    int avg_board_grad_threshold;
    int grad_threshold;

    float delta_height_max;
    int delta_angle_max;
    float delta_w_radio_min;
    float delta_w_radio_max;
    float delta_h_max;
    float delta_size_max;

	ChestParam(){
        hu_dist_min=0.00001;
        hu_dist_max=0.00010;

        avg_board_gray_threshold=12;
        avg_board_grad_threshold=90;
        grad_threshold=25;

        delta_height_max=0.5;
        delta_angle_max=45;
        delta_w_radio_min=0.3f;
        delta_w_radio_max=2.0;
        delta_h_max=2.0;
        delta_size_max=0.5;
    }
};

//-----------------------------------【类-弹药箱设置】--------------------------------------------
// brief：
//------------------------------------------------------------------------------------------------
class ChestSettings :public Settings
{
public:
	ChestSettings(const char *param);

    /**
     * @brief  设置弹药箱参数线程
     * @return 返回值
     * @autor  马兴沛
     * @date
     */
    void setChestParam();

    /**
     * @brief  读取弹药箱配置
     * @param  第一个参数
     * @return 返回值
     * @autor  马兴沛
     * @date
     */
    void read_param_armor(const char *param_path);

    /**
     * @brief  保存弹药箱配置
     * @param  第一个参数
     * @return 返回值
     * @autor  马兴沛
     * @date
     */
    void write_param_armor(const char *param_path);

    /**
        * @brief  change visual pid
        * @autor  weiTao
        * @date   2019.4.25
    */
    void debug_visual_pid();
public:
    double armor_height;
    double armor_width;

    ChestParam param;
    std::vector<cv::Point>chest_contours;
	const char *template_path;

    //pid参数结构体
    VisualPIDStruct pidStruct;
    //pitch轴角度pid解算控制类
    VisualPid visual_pitch_pid;
    //yaw轴角度pid解算控制类
    VisualPid visual_yaw_pid;
};

#endif
