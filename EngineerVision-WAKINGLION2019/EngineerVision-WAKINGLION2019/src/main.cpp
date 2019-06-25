//#include "ShapeGan.h"  //轮廓图生成
#include "Settings.h"
#include "PackData.h"
#include "PnpSolver.h"
#include "ChestDetection.h"
#include "Gui.h"
#include "IndVideoCapture.h"
#include "Common.h"
#include "Kalman.h"

#include "opencv2/opencv.hpp"
#include <iostream>
#include <thread>
#include "math.h"

using namespace cv;
using namespace std;

#include "opencv2/ml.hpp"
using namespace cv::ml;

//主线程函数
void mainProcessing(MainSettings *main_setting,
                    ChestSettings *chest_setting,
                    //Gui gui,
                    IndVideoCapture *cap0,
                    PackData *pack_data,UnpackData *unpack_data,Serial *serial);

int main()
{
    //Kalman test
//    Kalman KF;
//    KF.processing();

    //-----------------------------------【title】--------------------------------------------
    // brief：读取配置
    //------------------------------------------------------------------------------------------------
    MainSettings main_setting(PARAM_OTHER_PATH);
	ChestSettings chest_setting(PARAM_CHEST_PATH);
    PackData pack_data;
    UnpackData unpack_data;
    UnpackData *p_unpack_data=&unpack_data;
    Serial serial(main_setting.port_param);
//    Gui gui(cv::Point(320,240),0.3);

    IndVideoCapture cap0;
#if (USE_VIDEO==2)
    //初始化摄像头
    cap0.open(VIDEO_PATH,
              SHOW_WIDTH / SHOW_RADIO,
              SHOW_HEIGHT / SHOW_RADIO,
              20);
    //摄像头test（帧率和曝光时间的问题未解决）
//    cap0.testMVProc();
#endif

    //-----------------------------------【线程】--------------------------------------------
    // brief：
    // 线程1：打包：将控制信息发送到主控板
    // 线程2：解包：接收主控板的信息（如控制模式，敌方颜色等）
    // 线程3：采集图片
    // 线程4：主线程
    // 线程5：设置参数
    // 线程6：gui调试：未完成
    //------------------------------------------------------------------------------------------------
#ifdef USE_SERIAL
    //打开串口
    serial.openDevice();

    // 线程1：打包：将控制信息发送到主控板（如云台角度等）
    // 弃用，改为单线程发送
    // std::thread process_pack(&PackData::processing,pack_data,serial);

    // 线程2：解包：接收主控板的信息（如控制模式（装甲/神符），敌方颜色等）
    std::thread process_unpack(&UnpackData::processing,p_unpack_data,&serial);//unpack_data
#endif

#if (USE_VIDEO==2)
    // 线程3：采集图片
    std::thread process_iv_cap(&IndVideoCapture::collecting,&cap0);
#endif

    // 线程4：主线程
    std::thread process_main(&mainProcessing,
                             &main_setting,
                             &chest_setting,
                             //gui,
                             &cap0,
                             &pack_data,&unpack_data,&serial);

    //-----------------------------------【join】--------------------------------------------
    // brief：
    //------------------------------------------------------------------------------------------------
#if (USE_VIDEO==2)
    process_iv_cap.join();
#endif

    process_main.join();

#ifdef USE_SERIAL
    //process_pack.join();
    process_unpack.join();
#endif

#ifdef DEBUG_MODE
    //process_setting.join();
    //process_gui.join();
#endif

#ifdef USE_SHAPEGAN
    //生成轮廓图初始化
    std::vector<ContoursShape> multi_shape_prod;
    std::vector<Shape> multi_shape = {Circle, Circle, Rectangle, Rectangle};
    Property property=len;
    Property vertical_property=angle;
    std::vector<float> multi_len = {1.0, 2.0, 1.0, 2.0};
    ShapeGan shape_gan(multi_shape_prod, multi_shape, property, multi_len); //轮廓图生成类

    DataCollect dc(1000,800,-1,-1);
#endif
    return 0;
}

void mainProcessing(MainSettings *main_setting,
                    ChestSettings *chest_setting,
                    //Gui gui,
                    IndVideoCapture *cap0,
                    PackData *pack_data,UnpackData *unpack_data,Serial *serial)
{
    //Kalman KF;
    //-----------------------------------【类初始化】--------------------------------------------
    // brief：
    //------------------------------------------------------------------------------------------------
    //装甲检测（装甲设置类，主要功能设置类，轮廓）
    ChestDetection chest(chest_setting,main_setting, chest_setting->chest_contours);
    //角度解算
    AngleSolver angle_slover(PARAM_CALIBRATION_752,
                             3,
                             10,
                             3000);

    angle_slover.setTargetSize(21.6,5.4);

    //相机安装参数
    //(x,y,z)  相机相对云台中心偏移量
    //y_offset 云台中心与枪管的偏移量

    angle_slover.setRelationPoseCameraPTZ(main_setting->cp.x,
                                          main_setting->cp.y,
                                          main_setting->cp.z,
                                          main_setting->cp.y_offset,
                                          main_setting->cp.overlap_dist
                                          );

    //Gui单线程。多线程方式未完成
    Gui gui(angle_slover.getImageCenter(),0.3);
#ifdef DEBUG_MODE
    main_setting->setMainParam("主要设置");

    //波形绘制
    DataCollect dc(800,180);
    std::vector<cv::Scalar> color_list={cv::Scalar(0,0,255),   //红 pitch角
                                        cv::Scalar(255,0,255),   //绿 pitch角滤波后
                                        cv::Scalar(0,255,0), //品 yaw角
                                        cv::Scalar(255,255,0)};//青 yaw角滤波后
    std::vector<float> angle_data={50,-50,1,-1};
#endif

//打开视觉pid调试窗口
#ifdef DEBUG_VISUAL_PID
    chest_setting->debug_visual_pid();
#endif
    //-----------------------------------【保存视频】--------------------------------------------
    // brief：从配置文件中获取当前序号，序号自增
    // 是否保存视频在配置文件param_other中设置
    //------------------------------------------------------------------------------------------------
    cv::VideoWriter vw_ret, vw_src;
    if (main_setting->debug.b_save_result == true)
    {
        std::ostringstream s_ret,s_src;

        int n=main_setting->debug.n_save_result;
        main_setting->debug.n_save_result++;
        main_setting->write_param_other(PARAM_OTHER_PATH);

        s_ret<<SAVE_VIDEO_DIR<<"sentry_"<<"ret_"<<n<<".avi";
        s_src<<SAVE_VIDEO_DIR<<"sentry_"<<"src_"<<n<<".avi";

        vw_ret.open(s_ret.str(),
                    cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
                    30,
                    cv::Size(SHOW_WIDTH / SHOW_RADIO, SHOW_HEIGHT / SHOW_RADIO),
                    true);
        vw_src.open(s_src.str(),
                    cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
                    30,
                    cv::Size(SHOW_WIDTH / SHOW_RADIO, SHOW_HEIGHT / SHOW_RADIO),
                    true);
        if (vw_ret.isOpened() == false || vw_src.isOpened() == false)
        {
            std::cout << "Can't open *.avi file" << std::endl;
            return;
        }
    }

    //采集帧率与帧数参数
    long t1 = 0;
    int frame_num = 0;
    long t2 = 0;

    //-----------------------------------【打开视频或图片】--------------------------------------------
    // brief：打开测试视频或图片模式
    // 宏定义
    // 0摄像头/视频 1图片 2摄像头多线程
    //------------------------------------------------------------------------------------------------
#if (USE_VIDEO==0)
    //从摄像头/视频中读取
    VideoCaptureFactory cap(VIDEO_PATH,640,480,50);
//    if(!cap.open);
//    {
//        std::cout<<"摄像头打开失败"<<std::endl;
//        return;
//    }

#elif (USE_VIDEO==1)
    //从图片中读取
    cv::Mat src = cv::imread(PIC_PATH, 1);
    cv::Mat src_show;
    resize(src, src_show, cv::Size(src.cols / SHOW_RADIO, src.rows / SHOW_RADIO));
    cv::imshow("1.src", src_show);
#elif (USE_VIDEO==2)

#elif (USE_VIDEO==3)
    printf("MVVideoCapture Init\n");
    if(-1==MVVideoCapture::Init())return;
    printf("MVVideoCapture Play\n");
    MVVideoCapture::Play();
    MVVideoCapture::SetExposureTime(false, main_setting->debug.expore_time);
    MVVideoCapture::SetLargeResolution(true);
    printf("MVVideoCapture Finished\n");
#endif

    //-----------------------------------【开始循环】--------------------------------------------
    // brief：
    //------------------------------------------------------------------------------------------------
    while(1)
    {
#ifdef DEBUG_MODE
#ifdef USE_DEBUG
    // 线程5：设置参数
    if(main_setting->main_mode==armor_mode)
    {
        armor_setting->setArmorParam();
    }
    else if(main_setting->main_mode==rune_mode)
    {
        rune_setting->setRuneParam();
    }
    //线程6：gui调试：未完成
    //std::thread process_gui(&Gui::processing,gui,&setting,&cap0);
#endif
#ifdef USE_DEBUG_CAMERA
    main_setting->setCameraParam("camera");
    angle_slover.setRelationPoseCameraPTZ(main_setting->cp.x,
                                          main_setting->cp.y,
                                          main_setting->cp.z,
                                          main_setting->cp.y_offset,
                                          main_setting->cp.overlap_dist
                                          );
#endif
#endif
        t1 = cv::getTickCount();

        cv::Mat src;
#if (USE_VIDEO==0)
        //1.视频流
        cap >> src;
#elif (USE_VIDEO==2)
        //2.从消费队列中获取一帧
        cap0->ConsumeItem(src);
#elif (USE_VIDEO==3)
        TIME_BEGIN();
        MVVideoCapture::GetFrame(src);
        TIME_END("MVVideoCapture::Read");

        if(src.empty())
        {
            printf("Image empty !\n");
//            usleep(10000);
            continue;
        }

        if(main_setting->debug.b_save_pic)
            Tool::grabImg(src,main_setting->debug.n_key_order,
                          main_setting->debug.f_save_pic_inter*cv::getTickFrequency()/1000.0);
#endif
        cv::Mat ret_img;
        if(main_setting->debug.b_save_result)
        {
            ret_img=src.clone();
        }

#ifdef DEBUG_MODE
        if(!ret_img.data)
        {
            ret_img=src.clone();
        }

        if(main_setting->debug.b_show_src)
        {
            cv::Mat src_show;
            resize(src, src_show, cv::Size(src.cols / SHOW_RADIO, src.rows / SHOW_RADIO));
            cv::imshow("1.src", src_show);
        }
        else
        {
            if(-1!=cv::getWindowProperty("1.src",1))
                cv::destroyWindow("1.src");
        }
#endif

        if(main_setting->main_mode==chest_mode)
        {
            double pos;
            static int lost_flag;
            bool is_small;
            bool is_find= chest.detectorProc(src, pos,is_small);

            if(is_find)
            {
//                lost_flag=0;


                if(1)//if(0)//
                {
#ifdef DEBUG_MODE
                
#endif				
					double pos_ref = pos - angle_slover.getImageCenter().x;

                    //过滤1：卡尔曼
//                    cv::Mat state={};
//                    float *p_state=state.ptr<float>(0);
//                    KF.process();

                    //过滤2：
                    static long filter_num=-1;

                    static double pos_send;

                    static double pos_last;

                    double pit_ref_diff=std::abs(pos_ref - pos_last);

                    static double angle_ref_diff_th=0.5f;
                    if(pit_ref_diff>angle_ref_diff_th)
                    {
                        filter_num=0;
                    } else {
                        filter_num++;
                    }
                    if(filter_num>3)
                    {
						pos_send = pos_last;
                    }
					pos_last = pos_ref;

#ifdef USE_SERIAL
#ifdef DEBUG_VISUAL_PID
    chest_setting->visual_pitch_pid.setPIDAndDivision(chest_setting->pidStruct.pitch_p/double(chest_setting->pidStruct.p_division), chest_setting->pidStruct.pitch_i/double(chest_setting->pidStruct.i_division),
													chest_setting->pidStruct.pitch_d/double(chest_setting->pidStruct.d_division), chest_setting->pidStruct.pitch_division);
#endif

#ifdef USE_VISUAL_PID
                    double posRef= chest_setting->visual_pitch_pid.calAngle(pos_send,2);
                    printf("posRef=%f\n", posRef);
                    pack_data->setPc2StmMesg()->chassis_control_data.x_spd = posRef;
#else
                    pack_data->setPc2StmMesg()->chassis_control_data.x_spd = pos_send;
#endif

                    //pack_data->setPc2StmMesg()->chassis_control_data.time=t1;
                    pack_data->setPc2StmMesg()->chassis_control_data.ctrl_mode=1;

                    //pack_data->setPc2StmMesg()->chassis_control_data.visual_valid=1;
#endif

#ifdef DEBUG_MODE
                    //5.2.绘制波形
                    angle_data.clear();
                    angle_data={float(pos_send),
                               50,
                               float(posRef),
                               50,
                               };
#endif
                }
            }
            else{
                lost_flag++;
#ifdef USE_VISUAL_PID
				chest_setting->visual_pitch_pid.adaptiveCleanControlTimes();
				chest_setting->visual_yaw_pid.adaptiveCleanControlTimes();
#endif
#ifdef USE_SERIAL
                //pack_data->setPc2StmMesg()->chassis_control_data.time=t1;
                pack_data->setPc2StmMesg()->chassis_control_data.ctrl_mode=0;
                pack_data->setPc2StmMesg()->chassis_control_data.x_spd =0;
                //pack_data->setPc2StmMesg()->chassis_control_data.visual_valid=0;
                if(lost_flag<50)
                {
                    //start enter bug
                    pack_data->setPc2StmMesg()->gimbal_control_data.ctrl_mode=8;
                    pack_data->setPc2StmMesg()->gimbal_control_data.visual_valid=1;
                }
#endif

#ifdef DEBUG_MODE
                //5.2.绘制波形
                angle_data.clear();
                angle_data={0,
                            0,
                            0,0};
#endif
            }

#ifdef DEBUG_MODE
            //5.2.绘制波形
            dc.dataCollectProc(angle_data,color_list,"5.pos");
#endif

#ifdef USE_SERIAL
            //6.串口发送
            pack_data->process(serial);
#endif
        }

        if(main_setting->debug.b_show_fps)
        {
            t2=cv::getTickCount();
            std::cout << "FPS: " << cv::getTickFrequency()/(t2 - t1) << " frame No.:" << frame_num << std::endl;

            std::ostringstream s;
            if(main_setting->enemy_color==0)
                s<<"red";
            else
                s<<"blue";
            std::cout<<"enemy_color: "<<s.str()<<std::endl;
            printf("\033[2J");
            frame_num++;
        }
#ifdef DEBUG_MODE
        //按键s保存参数
        main_setting->debug.n_key_order=cv::waitKey(1);
        if(main_setting->debug.n_key_order=='q')break;

        if(main_setting->debug.n_key_order==KEY_SAVE_PARAM)
        {
            main_setting->write_param_other(PARAM_OTHER_PATH);
            if(main_setting->main_mode==chest_mode)
            {
				chest_setting->write_param_armor(PARAM_CHEST_PATH);
            }
        }
#else
        cv::waitKey(1);
#endif
        if (main_setting->debug.b_save_result == true)
        {
            vw_ret << ret_img;
            vw_src << src;
            if(main_setting->debug.n_key_order == KEY_SAVE_RESULT)
            {
                vw_ret.release();
                vw_src.release();
            }
        }
    }
#if (USE_VIDEO==3)
    MVVideoCapture::Stop();
    MVVideoCapture::Uninit();
#endif
}
