//-----------------------------------【弹药箱检测】--------------------------------------------
// brief：弹药箱检测，发送信号
//------------------------------------------------------------------------------------------------

#ifndef CHESTDETECTION_H
#define CHESTDETECTION_H

#include "Preprocessing.h"   //预处理
#include "TargetDetection.h" //目标检测
//#include "DigitDetector.h"
#include "Settings.h"        //设置
#include "Tool.h"

//-----------------------------------【弹药箱检测】--------------------------------------------
// brief：继承：目标检测类，自定自适应阈值分割类
// 并将参数传至其两个父类的构造函数
//------------------------------------------------------------------------------------------------
class ChestDetection :public TargetDetection, public AdaptiveThreshold
{
public:
	ChestDetection(ChestSettings *chest_setting, MainSettings *main_setting,
		std::vector<cv::Point>contours) :
		TargetDetection(chest_setting->tgt_size_param, main_setting->debug, contours),
		AdaptiveThreshold(chest_setting->preprocess_param)
	{
		this->chest_setting = chest_setting;
		this->main_setting = main_setting;
	}

	/**
	 * @brief  检测出所有弹药箱
	 * @param  原图
	 * @param  最右弹药箱位置
	 * @return 
	 * @autor  梁尧森
	 * @date   2019.3.31
	 */
	bool detectorProc(cv::Mat src, double &pos, bool &is_small);

	/**
	 * @brief  设置参数
	 * @autor  梁尧森
	 * @date   2019.4.20
	 */
	ChestSettings *setParam();
private:
		//-----------------------------------【title】--------------------------------------------
		// brief：
		//------------------------------------------------------------------------------------------------
	cv::RotatedRect boundingRRect(const cv::RotatedRect &left,
		const cv::RotatedRect &right);



	/**
	 * @brief  求两点之间距离
	 * @param  点
	 * @return 两点距离
	 * @autor  梁尧森
	 * @date   2019.4.20
	 */
	float getDistance(CvPoint pointO, CvPoint pointA);

	bool makeRectSafe(cv::Rect & rect, cv::Size size);

	bool broadenRect(cv::Rect & rect,
		int width_added,
		int height_added,
		cv::Size size);

	float juli(cv::Point aa, cv::Point bb);
	double colorRatioR(cv::Mat pic1, cv::Mat pic2);
	double colorRatioB(cv::Mat pic1, cv::Mat pic2);
	double colorRatioHSV(cv::Mat pic1, cv::Mat pic2);
	double colorRatioHSV_S(cv::Mat pic1, cv::Mat pic2);
	cv::Mat pretreat(cv::Mat picture);
	double hor_angle(cv::Point p1, cv::Point p2);
	int auto_shot(cv::Mat frame);
private:
	ChestSettings *chest_setting; // 装甲板参数指针
	MainSettings *main_setting;

	bool _is_lost;
	bool _is_small_armor;
	cv::RotatedRect _res_last;
	cv::Mat _src;

	cv::Mat _ret0;
	cv::Mat _ret;
};

#endif