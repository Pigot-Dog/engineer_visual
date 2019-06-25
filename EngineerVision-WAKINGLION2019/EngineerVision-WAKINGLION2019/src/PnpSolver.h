#ifndef PNPSOLVER_H
#define PNPSOLVER_H

#include "Settings.h"

#include <iostream>
#include "opencv2/opencv.hpp"

//-----------------------------------【矩形目标角度解析】--------------------------------------------
// brief：矩形目标角度解析，根据图片矩形四个角点坐标，算出旋转矩阵、位移向量
//------------------------------------------------------------------------------------------------
class RectPnPSolver 
{
public:
	/**
     * @brief  重载1.初始化类
     * @param  摄像机参数文件，包括内参矩阵、畸变系数等参数
     * @autor  梁尧森
	 * @date   2018.9.21
	 */
	RectPnPSolver(const char *camera_param_file_name)
	{
		cv::FileStorage fs(camera_param_file_name, cv::FileStorage::READ);
		if (!fs.isOpened())
		{
			std::cout << "Could not open the configuration file" << std::endl;
		}

		fs["Camera_Matrix"] >> cam_matrix;
		fs["Distortion_Coefficients"] >> distortion_coeff;

		fs["board_Width"] >> width_target;
		fs["board_Height"] >> height_target;

		//根据目标矩形的宽高设置三维坐标
		double half_x = width_target / 2.0;
		double half_y = height_target / 2.0;

		point3d.push_back(cv::Point3f(-half_x, -half_y, 0));
		point3d.push_back(cv::Point3f(half_x, -half_y, 0));
		point3d.push_back(cv::Point3f(half_x, half_y, 0));
		point3d.push_back(cv::Point3f(-half_x, half_y, 0));
	}

	/**
     * @brief  重载2.初始化类
     * @autor  梁尧森
	 * @date   2018.9.21
	 */
	RectPnPSolver(const cv::Mat &camera_matrix, const cv::Mat &dist_coeff, 
		double target_width = 21.6,
		double target_height = 5.4)
	{
		camera_matrix.copyTo(cam_matrix);
		dist_coeff.copyTo(distortion_coeff);

		width_target = target_width;
		height_target = target_height;

		//根据目标矩形的宽高设置三维坐标
		double half_x = width_target / 2.0;
		double half_y = height_target / 2.0;

		point3d.push_back(cv::Point3f(-half_x, -half_y, 0));
		point3d.push_back(cv::Point3f(half_x, -half_y, 0));
		point3d.push_back(cv::Point3f(half_x, half_y, 0));
		point3d.push_back(cv::Point3f(-half_x, half_y, 0));
	}

	/**
	 * @brief  设置相机参数
	 * @param  内参矩阵
	 * @param  畸变系数
	 * @return 返回值含义
     * @autor  梁尧森
	 * @date   2018.9.21
	 */
	void setCameraParam(const cv::Mat &camera_matrix, const cv::Mat &dist_coeff)
	{
		camera_matrix.copyTo(cam_matrix);
		dist_coeff.copyTo(distortion_coeff);
	}

	/**
	 * @brief  设置目标尺寸
	 * @param  目标矩形真实宽度
	 * @param  目标矩形真实高度
	 * @return 返回值含义
     * @autor  梁尧森
	 * @date   2018.9.21
	 */
	void setTargetSize(double width, double height)
	{
		width_target = width;
		height_target = height;

        //根据目标矩形的宽高设置三维坐标
        double half_x = width_target / 2.0;
        double half_y = height_target / 2.0;

        point3d.clear();
        point3d.push_back(cv::Point3f(-half_x, -half_y, 0));
        point3d.push_back(cv::Point3f(half_x, -half_y, 0));
        point3d.push_back(cv::Point3f(half_x, half_y, 0));
        point3d.push_back(cv::Point3f(-half_x, half_y, 0));
	}

	/**
	 * @brief solvePnP
	 * @param 
	 * @param 
	 * @param 
	 */
    void solvePnP4Points(const std::vector<cv::Point2f> &points2d, cv::Mat &rot, cv::Mat &trans);

	/**
	 * @brief  获取光轴中心
	 * @return 光轴中心
     * @autor  梁尧森
	 * @date   2018.9.21
	 */
	cv::Point2f getImageCenter()
	{
        return cv::Point2f(cam_matrix.at<double>(0, 2), cam_matrix.at<double>(1, 2));
	}
public:
	cv::Mat cam_matrix;        //内参矩阵
	cv::Mat distortion_coeff;  //畸变系数
	double width_target;       //目标矩形真实宽度
	double height_target;      //目标矩形真实高度
    std::vector<cv::Point3f> point3d; //目标三维坐标
};

//-----------------------------------【角度解算】--------------------------------------------
// brief：目标角度解析，根据位移向量算出真实的角度
//------------------------------------------------------------------------------------------------
class AngleSolver : public RectPnPSolver 
{
public:
	/**
	 * @brief  初始化类
	 * @param  摄像机参数文件，包括内参矩阵、畸变系数等参数
     * @param  比例系数
	 * @param  最小检测距离
	 * @param  最大检测距离
	 * @return 返回值含义
     * @autor  梁尧森
	 * @date   2018.9.21
	 */
	AngleSolver(const char *camera_param_file_name,
		double z_scale = 1.0,
        double min_dist = 10.0, double max_dist = 1000.0)
		:RectPnPSolver(camera_param_file_name)
	{
		scale_z = z_scale;
		min_distance = min_dist;
		max_distance = max_dist;

		rot_camera2ptz = cv::Mat::eye(3, 3, CV_64FC1);
		trans_camera2ptz = cv::Mat::zeros(3, 1, CV_64FC1);
		offset_y_barrel_ptz = 0;
	}

	/**
	 * @brief  初始化类
	 * @param  内参矩阵
	 * @param  畸变参数
	 * @param  目标宽度
	 * @param  目标高度
	 * @param  
	 * @param  最小检测距离
	 * @param  最大检测距离
	 * @return 返回值含义
     * @autor  梁尧森
	 * @date   2018.9.21
	 */
	AngleSolver(const cv::Mat &camera_matrix, const cv::Mat &dist_coeff,
		double target_width = 0, double target_height = 0, 
		double z_scale = 1.0,
		double min_dist = 50.0, double max_dist = 600.0)
		:RectPnPSolver(camera_matrix, dist_coeff, target_width, target_height)
	{
		scale_z = z_scale;
		min_distance = min_dist;
		max_distance = max_dist;

		rot_camera2ptz = cv::Mat::eye(3, 3, CV_64FC1);
		trans_camera2ptz = cv::Mat::zeros(3, 1, CV_64FC1);
		offset_y_barrel_ptz = 0;
	}

	/**
     * @brief  设置相机安装参数
     * @param  第一个参数含义
     * @autor  梁尧森
	 * @date   2018.9.21
	 */
	void setRelationPoseCameraPTZ(const double ptz_camera_x,
		const double ptz_camera_y,
		const double ptz_camera_z,
		double y_offset_barrel_ptz,
		const double overlap_dist = 100000.0
		)
	{
//        double dist=overlap_dist;
//        double theta = asin(abs(y_offset_barrel_ptz)/dist)+
//                asin(abs(ptz_camera_y+overlap_dist)/dist);
        double theta = -atan((ptz_camera_y + y_offset_barrel_ptz) / overlap_dist);
        std::cout<<"theta= "<<theta<<std::endl;
		double r_data[] = { 1,0,0,
		0,cos(theta),-sin(theta),
		0, sin(theta), cos(theta) };
		
		cv::Mat rot_camera_ptz(3, 3, CV_64FC1, r_data);
		rot_camera_ptz.copyTo(rot_camera2ptz);

		// 云台相对于相机的偏移向量和平移向量
		double t_data[] = { 0, ptz_camera_y, ptz_camera_z };
		cv::Mat trans_camera_ptz(3, 1, CV_64FC1, t_data);
		trans_camera_ptz.copyTo(trans_camera2ptz);

		offset_y_barrel_ptz = y_offset_barrel_ptz;
	}

	/**
	 * @brief  解析角度
	 * @return 
     * @autor  梁尧森
	 * @date   2018.9.21
	 */
	bool getAngle(const cv::RotatedRect &rect,
		double bullet_speed,
        double current_ptz_angle,
        const cv::Point2f &offset=cv::Point2f());

	/**
	 * @brief  解析角度
     * @return
     * @autor  梁尧森
	 * @date   2018.9.21
	 */
	bool getAngle(std::vector<cv::Point2f> target2d,
		double bullet_speed,
        double current_ptz_angle=0,
        const cv::Point2f &offset=cv::Point2f());

	/**
     * @brief
	 * @param  第一个参数含义
     * @return 返回角度、距离、姿态角等信息
     * @autor  梁尧森
	 * @date   2018.9.21
	 */
	double getPitRef()
	{
		return pit_ref;
	}

	double getYawRef()
	{
		return yaw_ref;
	}

	float getTgtDist()
    {
        return position_in_camera.at<double>(2, 0);
	}

	float getEulerX()
	{
		return position_in_camera.at<double>(0, 0);
	}

	float getEulerY()
	{
		return position_in_camera.at<double>(1, 0);
	}

	float getEulerZ()
	{
		return position_in_camera.at<double>(2, 0);
	}
private:
	/**
	 * @brief  相机坐标转换到PTZ坐标
	 * @param  目标在相机坐标下的位置
     * @param  目标在PTZ坐标下的位置
     * @autor  梁尧森
	 * @date   2018.9.21
	 */
	void tranformationCamera2PTZ(const cv::Mat &pos, cv::Mat &transed_pos);

	/**
	 * @brief  根据目标在PTZ坐标中的位置，计算偏移角度，使枪管瞄准目标
     * @param  第一个参数含义
     * @autor  梁尧森
	 * @date   2018.9.21
	 */
	void adjustPTZ2Barrel(const cv::Mat &pos_in_ptz,
		double &angle_x, double &angle_y,
		double bullet_speed,
		double current_ptz_angle);

	/**
     * @brief  获取矩形四个角点的坐标
     * @autor  梁尧森
	 * @date   2018.9.21
	 */
	void getTarget2dPoinstion(const cv::RotatedRect &rect,
		std::vector<cv::Point2f> &target2d,
		const cv::Point2f &offset);
public:
	//检测出的信息
	cv::Mat rot;                //旋转矩阵
	cv::Mat position_in_camera; //偏移向量
	cv::Mat position_in_ptz;    //目标在PTZ坐标中的位置
	double  pit_ref;      //云台偏转角（俯仰角）
	double  yaw_ref;      //云台偏转角（偏航角）
private:
	//参数设置
	cv::Mat rot_camera2ptz;     //旋转矩阵（相机坐标转换到PTZ坐标）
	cv::Mat trans_camera2ptz;   //偏移向量（相机坐标转换到PTZ坐标）
	double offset_y_barrel_ptz;  //云台中心与枪管的偏移量
	double min_distance;  //最小检测距离
	double max_distance;  //最大检测距离
    double scale_z;       //放缩比例
};

#endif
