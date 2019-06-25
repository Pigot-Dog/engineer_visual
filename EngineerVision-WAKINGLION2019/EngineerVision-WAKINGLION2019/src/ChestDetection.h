//-----------------------------------����ҩ���⡿--------------------------------------------
// brief����ҩ���⣬�����ź�
//------------------------------------------------------------------------------------------------

#ifndef CHESTDETECTION_H
#define CHESTDETECTION_H

#include "Preprocessing.h"   //Ԥ����
#include "TargetDetection.h" //Ŀ����
//#include "DigitDetector.h"
#include "Settings.h"        //����
#include "Tool.h"

//-----------------------------------����ҩ���⡿--------------------------------------------
// brief���̳У�Ŀ�����࣬�Զ�����Ӧ��ֵ�ָ���
// ����������������������Ĺ��캯��
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
	 * @brief  �������е�ҩ��
	 * @param  ԭͼ
	 * @param  ���ҵ�ҩ��λ��
	 * @return 
	 * @autor  ��Ңɭ
	 * @date   2019.3.31
	 */
	bool detectorProc(cv::Mat src, double &pos, bool &is_small);

	/**
	 * @brief  ���ò���
	 * @autor  ��Ңɭ
	 * @date   2019.4.20
	 */
	ChestSettings *setParam();
private:
		//-----------------------------------��title��--------------------------------------------
		// brief��
		//------------------------------------------------------------------------------------------------
	cv::RotatedRect boundingRRect(const cv::RotatedRect &left,
		const cv::RotatedRect &right);



	/**
	 * @brief  ������֮�����
	 * @param  ��
	 * @return �������
	 * @autor  ��Ңɭ
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
	ChestSettings *chest_setting; // װ�װ����ָ��
	MainSettings *main_setting;

	bool _is_lost;
	bool _is_small_armor;
	cv::RotatedRect _res_last;
	cv::Mat _src;

	cv::Mat _ret0;
	cv::Mat _ret;
};

#endif