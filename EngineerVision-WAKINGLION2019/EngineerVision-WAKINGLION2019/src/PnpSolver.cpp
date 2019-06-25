
#include "PnpSolver.h"

#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

//-----------------------------------【矩形目标角度解析】--------------------------------------------
// brief：矩形目标角度解析，根据图片矩形四个角点坐标，算出旋转矩阵、位移向量
//------------------------------------------------------------------------------------------------
void RectPnPSolver::solvePnP4Points(const std::vector<cv::Point2f> &points2d, 
	cv::Mat &rot, 
    cv::Mat &trans)
{
	cv::Mat rvec;
    cv::solvePnP(point3d,
                 points2d, cam_matrix, distortion_coeff, rvec, trans);
	Rodrigues(rvec, rot);
}


//-----------------------------------【角度解算】--------------------------------------------
// brief：目标角度解析，根据位移向量算出真实的角度
//------------------------------------------------------------------------------------------------
bool AngleSolver::getAngle(const cv::RotatedRect &rect,
	double bullet_speed,
    double current_ptz_angle,
	const cv::Point2f &offset)
{
	vector<Point2f> target2d;
	//求物体坐标系相对于相机坐标系，求向量矩阵
	getTarget2dPoinstion(rect, target2d, offset);
	
    getAngle(target2d, bullet_speed, current_ptz_angle,offset);
	return true;
}

bool AngleSolver::getAngle(std::vector<cv::Point2f> target2d,
	double bullet_speed, 
    double current_ptz_angle,
	const cv::Point2f &offset)
{
	//根据检测出的目标在图像中的二维坐标，算出旋转矩阵与位移向量
    solvePnP4Points(target2d, this->rot, position_in_camera);
	//
	position_in_camera.at<double>(2, 0) = scale_z * position_in_camera.at<double>(2, 0);
	//超出检测范围
	if (position_in_camera.at<double>(2, 0) < min_distance ||
		position_in_camera.at<double>(2, 0) > max_distance)
	{
		cout << position_in_camera.at<double>(2, 0) <<
			"out of range: [" << min_distance << ", " << max_distance << "]\n";
		return false;
	}
	// 相机坐标转换到PTZ坐标
	tranformationCamera2PTZ(position_in_camera, position_in_ptz);
	// 根据目标在PTZ坐标中的位置，计算偏移角度，使枪管瞄准目标
	adjustPTZ2Barrel(position_in_ptz,
        yaw_ref,pit_ref,
		bullet_speed,
		current_ptz_angle);
	return true;
}

void AngleSolver::tranformationCamera2PTZ(const cv::Mat &pos, cv::Mat &transed_pos) 
{
	transed_pos = rot_camera2ptz * pos - trans_camera2ptz;
}

void AngleSolver::adjustPTZ2Barrel(const cv::Mat &pos_in_ptz,
	double &angle_x, double &angle_y,
	double bullet_speed,
	double current_ptz_angle)
{
	const double *_xyz = (const double *)pos_in_ptz.data;
	double down_t = 0.0;
	if (bullet_speed > 10e-3)
		down_t = _xyz[2] / 100.0 / bullet_speed;
	double offset_gravity = 0.5 * 9.8 * down_t * down_t * 100;
	double xyz[3] = { _xyz[0], _xyz[1] - offset_gravity, _xyz[2] };
	double alpha = 0.0, theta = 0.0;

	alpha = asin(offset_y_barrel_ptz / sqrt(xyz[1] * xyz[1] + xyz[2] * xyz[2]));
	if (xyz[1] < 0) 
	{
		theta = atan(-xyz[1] / xyz[2]);
		angle_y = - alpha - theta;
	}
	else if (xyz[1] < offset_y_barrel_ptz) 
	{
		theta = atan(xyz[1] / xyz[2]);
		angle_y = -alpha + theta;
	}
	else 
	{
		theta = atan(xyz[1] / xyz[2]);
		angle_y = -alpha + theta;
	}
	angle_x = atan2(xyz[0], xyz[2]);
	
	angle_x = angle_x * 180.0 / PI;
	angle_y = angle_y * 180.0 / PI;
}

void AngleSolver::getTarget2dPoinstion(const cv::RotatedRect &rect, 
	vector<Point2f> &target2d, 
	const cv::Point2f &offset)
{
	Point2f vertices[4];
	rect.points(vertices);
	Point2f lu, ld, ru, rd;
	sort(vertices, vertices + 4, [](const Point2f & p1, const Point2f & p2) { return p1.x < p2.x; });
	if (vertices[0].y < vertices[1].y) 
	{
		lu = vertices[0];
		ld = vertices[1];
	}
	else 
	{
		lu = vertices[1];
		ld = vertices[0];
	}
	if (vertices[2].y < vertices[3].y) 
	{
		ru = vertices[2];
		rd = vertices[3];
	}
	else 
	{
		ru = vertices[3];
		rd = vertices[2];
	}

	target2d.clear();
	target2d.push_back(lu + offset);
	target2d.push_back(ru + offset);
	target2d.push_back(rd + offset);
	target2d.push_back(ld + offset);
}
