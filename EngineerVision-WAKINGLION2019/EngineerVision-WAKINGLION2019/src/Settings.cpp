#include "Settings.h"

MainSettings::MainSettings(const char *param_name)
{
    //读取配置
    this->read_param_other(param_name);
}

void MainSettings::setMainParam(const char *win_name)
{
    cv::namedWindow(win_name);
    cv::createTrackbar("main_mode",win_name,&this->main_mode, 2);

    cv::createTrackbar("enemy_color",win_name,&this->enemy_color, 1);

    cv::createTrackbar("debug-b_show_src",win_name,&debug.b_show_src, 1);
    cv::createTrackbar("debug-b_show_bin",win_name,&debug.b_show_bin, 1);
    cv::createTrackbar("debug-b_show_target",win_name,&debug.b_show_target, 1);
    cv::createTrackbar("debug-b_show_armor",win_name,&debug.    b_show_armor, 1);
    cv::createTrackbar("debug-b_show_result",win_name,&debug.b_show_result, 1);
    cv::createTrackbar("debug-b_show_fps",win_name,&debug.b_show_fps, 1);
    cv::createTrackbar("debug-b_save_result",win_name,&debug.b_save_result, 1);
//    cv::createTrackbar("debug-n_save_result",win_name,&debug.n_save_result, -1);
//    cv::createTrackbar("debug-n_key_order",win_name,&debug.n_key_order, 255);
    cv::createTrackbar("debug-b_save_pic",win_name,&debug.b_save_pic, 1);
    cv::createTrackbar("debug-f_save_pic_inter",win_name,&debug.f_save_pic_inter, 10000);
    cv::createTrackbar("debug-expore_time",win_name,&debug.expore_time, 100000);
}

void MainSettings::read_param_other(const char *param_path)
{
    cv::FileStorage fs(param_path, cv::FileStorage::READ);

    fs["main_mode"] >> this->main_mode;

    fs["enemy_color"] >> this->enemy_color;

    fs["debug-b_show_src"] >> this->debug.b_show_src;
    fs["debug-b_show_bin"] >> this->debug.b_show_bin;
    fs["debug-b_show_target"] >> this->debug.b_show_target;
    fs["debug-b_show_armor"] >> this->debug.b_show_armor;
    fs["debug-b_show_result"] >> this->debug.b_show_result;
    fs["debug-b_show_fps"] >> this->debug.b_show_fps;
    fs["debug-b_save_result"] >> this->debug.b_save_result;
    fs["debug-n_save_result"] >> this->debug.n_save_result;
    //fs["debug-n_key_order"] >> this->debug.n_key_order;
    fs["debug-b_save_pic"] >> this->debug.b_save_pic;
    fs["debug-f_save_pic_inter"] >> this->debug.f_save_pic_inter;
    fs["debug-expore_time"] >> this->debug.expore_time;

    fs["cp-x"] >> this->cp.x;
    fs["cp-y"] >> this->cp.y;
    fs["cp-z"] >> this->cp.z;
    fs["cp-y_offset"] >> this->cp.y_offset;
    fs["cp-bullet_speed"] >> this->cp.bullet_speed;

//    fs["port_param-dev_name"] >> this->port_param.dev_name;


    std::cout << "Read other param finished!" << std::endl;
    fs.release();
}

void MainSettings::write_param_other(const char *param_path)
{
    cv::FileStorage fs(param_path, cv::FileStorage::WRITE);

    fs<<"main_mode"<<this->main_mode;

    fs<<"enemy_color"<<this->enemy_color;

    fs<<"debug-b_show_src"<<this->debug.b_show_src;
    fs<<"debug-b_show_bin"<<this->debug.b_show_bin;
    fs<<"debug-b_show_target"<<this->debug.b_show_target;
    fs<<"debug-b_show_armor"<<this->debug.b_show_armor;
    fs<<"debug-b_show_result"<<this->debug.b_show_result;
    fs<<"debug-b_show_fps"<<this->debug.b_show_fps;
    fs<<"debug-b_save_result"<<this->debug.b_save_result;
    fs<<"debug-n_save_result"<<this->debug.n_save_result;
//    fs<<"debug-n_key_order"<<this->debug.n_key_order;
    fs<<"debug-b_save_pic"<<this->debug.b_save_pic;
    fs<<"debug-f_save_pic_inter"<<this->debug.f_save_pic_inter;
    fs<<"debug-expore_time"<<this->debug.expore_time;

    fs<<"cp-x"<<this->cp.x;
    fs<<"cp-y"<<this->cp.y;
    fs<<"cp-z"<<this->cp.z;
    fs<<"cp-y_offset"<<this->cp.y_offset;
    fs<<"cp-bullet_speed"<<this->cp.bullet_speed;

    fs<<"port_param-dev_name"<<this->port_param.dev_name;
    fs<<"port_param-baud_rate"<<this->port_param.baud_rate;
    fs<<"port_param-databits"<<this->port_param.databits;
    fs<<"port_param-stopbits"<<this->port_param.stopbits;
    fs<<"port_param-parity"<<this->port_param.parity;

    std::cout<<"Sava other param finished!"<<std::endl;
    fs.release();
}


Settings::Settings()
{

}

void Settings::setPreprocessParam(const char *win_name)
{
    cv::namedWindow(win_name);
    cv::createTrackbar("gray_thre_min",win_name,&preprocess_param.gray_thre_min, 255);  //灰度图二值化最小阈值
    cv::createTrackbar("gray_thre_max",win_name,&preprocess_param.gray_thre_max, 255);  //灰度图二值化最大阈值

    cv::createTrackbar("gray_max_w",win_name,&preprocess_param.gray_max_w, 10);
    cv::createTrackbar("gray_avg_w",win_name,&preprocess_param.gray_avg_w, 10);
}

void Settings::setTgtSizeParam(const char *win_name,
                               struct TargetSizeParam &tgt_size_param)
{
    cv::namedWindow(win_name);

//    cv::createTrackbar("hough_circle_param1",win_name,&tgt_size_param.hough_circle_param1, 1000);
//    cv::createTrackbar("hough_circle_param2",win_name,&tgt_size_param.hough_circle_param2, 500);
//    cv::createTrackbar("hough_circle_minradius",win_name,&tgt_size_param.hough_circle_minradiu, 100);

    cv::createTrackbar("len_min",win_name,&tgt_size_param.len_min, 200);
    cv::createTrackbar("len_max",win_name,&tgt_size_param.len_max, 10000);
    cv::createTrackbar("radio_min",win_name,&tgt_size_param.radio_min, 20);
    cv::createTrackbar("radio_max",win_name,&tgt_size_param.radio_max, 20);
    cv::createTrackbar("area_min",win_name,&tgt_size_param.area_min, 10000);
    cv::createTrackbar("area_max",win_name,&tgt_size_param.area_max, 1000000);

    cv::createTrackbar("area_ratio_max",win_name,&tgt_size_param.area_ratio_max, 5000);
    cv::createTrackbar("len_ratio_max",win_name,&tgt_size_param.len_ratio_max, 5000);

    static int area_radio_min=tgt_size_param.area_radio_min*100000;
    cv::createTrackbar("area_radio_min",win_name,&area_radio_min, 100000);
    tgt_size_param.area_radio_min=area_radio_min/100000.0;

    static int area_radio_max=tgt_size_param.area_radio_max*100000;
    cv::createTrackbar("area_radio_max",win_name,&area_radio_max, 100000);
    tgt_size_param.area_radio_max=area_radio_max/100000.0;


    static int area_len_radio_min=tgt_size_param.area_len_radio_min*100000;
    cv::createTrackbar("area_len_radio_min",win_name,&area_len_radio_min, 100000);
    tgt_size_param.area_len_radio_min=area_len_radio_min/100000.0;

    static int area_len_radio_max=tgt_size_param.area_len_radio_max*100000;
    cv::createTrackbar("area_len_radio_max",win_name,&area_len_radio_max, 100000);
    tgt_size_param.area_len_radio_max=area_len_radio_max/100000.0;

    cv::createTrackbar("corners_size_min",win_name,&tgt_size_param.corners_size_min, 10);
    cv::createTrackbar("slope_offset",win_name,&tgt_size_param.slope_offset, 90);
}

void Settings::setTgtSizeParam(const char *win_name)
{
    cv::namedWindow(win_name);

//    cv::createTrackbar("hough_circle_param1",win_name,&tgt_size_param.hough_circle_param1, 1000);
//    cv::createTrackbar("hough_circle_param2",win_name,&tgt_size_param.hough_circle_param2, 500);
//    cv::createTrackbar("hough_circle_minradius",win_name,&tgt_size_param.hough_circle_minradiu, 100);

    cv::createTrackbar("len_min",win_name,&tgt_size_param.len_min, 5000);
    cv::createTrackbar("len_max",win_name,&tgt_size_param.len_max, 10000);
    cv::createTrackbar("radio_min",win_name,&tgt_size_param.radio_min, 20);
    cv::createTrackbar("radio_max",win_name,&tgt_size_param.radio_max, 20);
    cv::createTrackbar("area_min",win_name,&tgt_size_param.area_min, 50000);
    cv::createTrackbar("area_max",win_name,&tgt_size_param.area_max, 100000);

    cv::createTrackbar("area_ratio_max",win_name,&tgt_size_param.area_ratio_max, 10000);
    cv::createTrackbar("len_ratio_max",win_name,&tgt_size_param.len_ratio_max, 5000);

    static int area_radio_min=tgt_size_param.area_radio_min*100000;
    cv::createTrackbar("area_radio_min",win_name,&area_radio_min, 100000);
    tgt_size_param.area_radio_min=area_radio_min/100000.0;

    static int area_radio_max=tgt_size_param.area_radio_max*100000;
    cv::createTrackbar("area_radio_max",win_name,&area_radio_max, 100000);
    tgt_size_param.area_radio_max=area_radio_max/100000.0;


    static int area_len_radio_min=tgt_size_param.area_len_radio_min*10.0;
    cv::createTrackbar("area_len_radio_min",win_name,&area_len_radio_min, 100);
    tgt_size_param.area_len_radio_min=area_len_radio_min/10.0;

    static int area_len_radio_max=tgt_size_param.area_len_radio_max*10;
    cv::createTrackbar("area_len_radio_max",win_name,&area_len_radio_max, 100);
    tgt_size_param.area_len_radio_max=area_len_radio_max/10.0;

    cv::createTrackbar("corners_size_min",win_name,&tgt_size_param.corners_size_min, 10);
    cv::createTrackbar("slope_offset",win_name,&tgt_size_param.slope_offset, 90);
    cv::createTrackbar("color_th",win_name,&tgt_size_param.color_th, 500);
}


ChestSettings::ChestSettings(const char *param):
    visual_pitch_pid(pidStruct.pitch_p/double(pidStruct.p_division),pidStruct.pitch_i/double(pidStruct.i_division),
                      pidStruct.pitch_d/double(pidStruct.d_division),pidStruct.pitch_division),//init pitch pid
    visual_yaw_pid(pidStruct.yaw_p/double(pidStruct.p_division),pidStruct.yaw_i/double(pidStruct.i_division),
                      pidStruct.yaw_d/double(pidStruct.d_division),pidStruct.yaw_division)//init yaw pid
{
    //读取装甲板配置
    this->read_param_armor(param);
    //读取模板
 //   cv::Mat tem = cv::imread(ARMOR_LEFT,0);
 //   std::vector<std::vector<cv::Point>>contours_list;
 //   cv::findContours(tem, contours_list,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_NONE);

	//chest_contours.assign(contours_list[0].begin(), contours_list[0].end());

    //template_path = EMPLATE_PATH;
}

void ChestSettings::setChestParam()
{
    setPreprocessParam(WIN_NAME_TARGET);
    setTgtSizeParam(WIN_NAME_TARGET);
}

void ChestSettings::read_param_armor(const char *param_path)
{
    cv::FileStorage fs(param_path, cv::FileStorage::READ);

    fs["armor_height"] >> armor_height;
    fs["armor_width"] >> armor_width;

    fs["gray_thre_min"] >> preprocess_param.gray_thre_min;
    fs["gray_thre_max"] >> preprocess_param.gray_thre_max;
    fs["gray_max_w"] >> preprocess_param.gray_max_w;
    fs["gray_avg_w"] >> preprocess_param.gray_avg_w;

//    fs["hough_circle_param1"] >> tgt_size_param.hough_circle_param1;
//    fs["hough_circle_param2"] >> tgt_size_param.hough_circle_param2;
//    fs["hough_circle_minradius"] >> tgt_size_param.hough_circle_minradiu;

    fs["len_min"] >> tgt_size_param.len_min;
    fs["len_max"] >> tgt_size_param.len_max;
    fs["light_radio_min"] >> tgt_size_param.radio_min;
    fs["light_radio_max"] >> tgt_size_param.radio_max;
    fs["light_area_min"] >> tgt_size_param.area_min;
    fs["light_area_max"] >> tgt_size_param.area_max;

    fs["area_ratio_max"] >> tgt_size_param.area_ratio_max;
    fs["len_ratio_max"] >> tgt_size_param.len_ratio_max;

    fs["area_radio_min"] >> tgt_size_param.area_radio_min;
    fs["area_radio_max"] >> tgt_size_param.area_radio_max;
    fs["area_len_radio_min"] >> tgt_size_param.area_len_radio_min;
    fs["area_len_radio_max"] >> tgt_size_param.area_len_radio_max;
    fs["corners_size_min"] >> tgt_size_param.corners_size_min;
    fs["slope_offset"] >> tgt_size_param.slope_offset;
    fs["color_th"] >> tgt_size_param.color_th;

    fs["hu_dist_min"] >> param.hu_dist_min;
    fs["hu_dist_max"] >> param.hu_dist_max;

    fs["avg_board_gray_threshold"] >> param.avg_board_gray_threshold;
    fs["avg_board_grad_threshold"] >> param.avg_board_grad_threshold;
    fs["grad_threshold"] >> param.grad_threshold;

    fs["delta_height_max"] >> param.delta_height_max;
    fs["delta_angle_max"] >> param.delta_angle_max;
    fs["delta_w_radio_min"] >> param.delta_w_radio_min;
    fs["delta_w_radio_max"] >> param.delta_w_radio_max;
    fs["delta_h_max"] >> param.delta_h_max;
    fs["delta_size_max"] >> param.delta_size_max;

    fs["pidStruct-pitch_p"] >> pidStruct.pitch_p;
    fs["pidStruct-pitch_i"] >> pidStruct.pitch_i;
    fs["pidStruct-pitch_d"] >> pidStruct.pitch_d;
    fs["pidStruct-pitch_division"] >> pidStruct.pitch_division;
    fs["pidStruct-yaw_p"] >> pidStruct.yaw_p;
    fs["pidStruct-yaw_i"] >> pidStruct.yaw_i;
    fs["pidStruct-yaw_d"] >> pidStruct.yaw_d;
    fs["pidStruct-yaw_division"] >> pidStruct.yaw_division;

    std::cout << "Read armor param finished!" << std::endl;
    fs.release();
}

void ChestSettings::write_param_armor(const char *param_path)
{
    cv::FileStorage fs(param_path, cv::FileStorage::WRITE);

    fs << "armor_height" << armor_height;
    fs << "armor_width" << armor_width;

    fs << "gray_thre_min" << preprocess_param.gray_thre_min;
    fs << "gray_thre_max" << preprocess_param.gray_thre_max;
    fs << "gray_max_w" << preprocess_param.gray_max_w;
    fs << "gray_avg_w" << preprocess_param.gray_avg_w;

//    fs << "hough_circle_param1" << tgt_size_param.hough_circle_param1;
//    fs << "hough_circle_param2" << tgt_size_param.hough_circle_param2;
//    fs << "hough_circle_minradius" << tgt_size_param.hough_circle_minradiu;

    fs << "len_min" << tgt_size_param.len_min;
    fs << "len_max" << tgt_size_param.len_max;
    fs << "light_radio_min" << tgt_size_param.radio_min;
    fs << "light_radio_max" << tgt_size_param.radio_max;
    fs << "light_area_min" << tgt_size_param.area_min;
    fs << "light_area_max" << tgt_size_param.area_max;

    fs << "area_ratio_max" << tgt_size_param.area_ratio_max;
    fs << "len_ratio_max" << tgt_size_param.len_ratio_max;

    fs << "area_radio_min" << tgt_size_param.area_radio_min;
    fs << "area_radio_max" << tgt_size_param.area_radio_max;
    fs << "area_len_radio_min" << tgt_size_param.area_len_radio_min;
    fs << "area_len_radio_max" << tgt_size_param.area_len_radio_max;
    fs << "corners_size_min" << tgt_size_param.corners_size_min;
    fs << "slope_offset" << tgt_size_param.slope_offset;
    fs << "color_th" << tgt_size_param.color_th;

    fs << "hu_dist_min" << param.hu_dist_min;
    fs << "hu_dist_max" << param.hu_dist_max;

    fs << "avg_board_gray_threshold" << param.avg_board_gray_threshold;
    fs << "avg_board_grad_threshold" << param.avg_board_grad_threshold;
    fs << "grad_threshold" << param.grad_threshold;

    fs << "delta_height_max" << param.delta_height_max;
    fs << "delta_angle_max" << param.delta_angle_max;
    fs << "delta_w_radio_min" << param.delta_w_radio_min;
    fs << "delta_w_radio_max" << param.delta_w_radio_max;
    fs << "delta_h_max" << param.delta_h_max;
    fs << "delta_size_max" << param.delta_size_max;

    fs << "pidStruct-pitch_p" << pidStruct.pitch_p;
    fs << "pidStruct-pitch_i" << pidStruct.pitch_i;
    fs << "pidStruct-pitch_d" << pidStruct.pitch_d;
    fs << "pidStruct-pitch_division" << pidStruct.pitch_division;
    fs << "pidStruct-yaw_p" << pidStruct.yaw_p;
    fs << "pidStruct-yaw_i" << pidStruct.yaw_i;
    fs << "pidStruct-yaw_d" << pidStruct.yaw_d;
    fs << "pidStruct-yaw_division" << pidStruct.yaw_division;

    std::cout << "Save armor param finished!" << std::endl;
    fs.release();
}

void ChestSettings::debug_visual_pid()
{
    cv::namedWindow(WIN_NAME_VISUALPID);

    //debug pitch
    cv::createTrackbar("pitch_p",WIN_NAME_VISUALPID,&pidStruct.pitch_p, 800);
    cv::createTrackbar("pitch_i",WIN_NAME_VISUALPID,&pidStruct.pitch_i, 5000);
    cv::createTrackbar("pitch_d",WIN_NAME_VISUALPID,&pidStruct.pitch_d, 8000);
    cv::createTrackbar("pitch_division",WIN_NAME_VISUALPID,&pidStruct.pitch_division, 200);

    //debug yaw
    cv::createTrackbar("yaw_p",WIN_NAME_VISUALPID,&pidStruct.yaw_p, 800);
    cv::createTrackbar("yaw_i",WIN_NAME_VISUALPID,&pidStruct.yaw_i, 5000);
    cv::createTrackbar("yaw_d",WIN_NAME_VISUALPID,&pidStruct.yaw_d, 8000);
    cv::createTrackbar("yaw_division",WIN_NAME_VISUALPID,&pidStruct.yaw_division, 200);
}
