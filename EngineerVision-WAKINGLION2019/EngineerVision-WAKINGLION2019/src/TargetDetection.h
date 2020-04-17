//-----------------------------------【目标检测】--------------------------------------------
// brief：提供检测的基本功能，如边缘检测，特征提取等
//------------------------------------------------------------------------------------------------

#ifndef TARGETDETECTION_H
#define TARGETDETECTION_H

#include "Settings.h"

#include "opencv2/opencv.hpp"
#include <iostream>
#include <math.h>

struct Index
{
    int a0;//父母框
    int a1;//儿子框
    int a2;//孙子框
};

class TargetDetection
{
public:
	TargetDetection();
	TargetDetection(TargetSizeParam &target_param_size,
		Debug &debug,
		std::vector<cv::Point> contours_target = std::vector<cv::Point>());
	~TargetDetection();

	bool targetDetectionProc(cv::Mat src,
		std::vector<std::vector<cv::Point>> &target_contours,
		std::vector<struct TargetSize> &target_size_list)
	{
#ifdef DEBUG_MODE
		float digits_size = 0.3;
		int y_dist = 10;
#endif
		contours.clear();
		std::vector<cv::Vec4i>hierarchys;
		cv::findContours(src, contours, hierarchys, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

		ret.create(src.rows, src.cols, CV_32FC3);

		rot_rect_list.clear();
		//ret_contours.clear();
		hierarchy.clear();
		//-----------------------------------【传统方法过滤】--------------------------------------------
		// brief：使用传统方法过滤大部分明显不是目标的区域，为后续基于深度学习的分类方法减小干扰和运算量
		//------------------------------------------------------------------------------------------------
		for (int i = 0; i < contours.size(); i++)
		{
			struct TargetSize target_size;

			//1.轮廓长度
			target_size.len = contours[i].size();
			//std::cout << target_size.len << std::endl;
			if (target_size.len<tsize->len_min || target_size.len>tsize->len_max)
			{
				//if (target_size.len > 200)
				//{
					//std::ostringstream s;
					//s << target_size.len;
					//cv::putText(ret, s.str(), cv::Point(500,600), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 0, 255), 1);
					//cv::drawContours(ret, contours, i, cv::Scalar(0, 0, 255));
					//std::cout << target_size.len << std::endl;
				//}
				continue;
			}
			//获取拟合椭圆的外包围矩形
			cv::RotatedRect rot_rect = adjustRRect(cv::fitEllipse(contours[i]));
			//cv::RotatedRect rot_rect = minAreaRect(contours[i]);

			//2.长宽比
			target_size.radio = rot_rect.size.height*1.0 / rot_rect.size.width;

			if (target_size.radio<tsize->radio_min * 0.01 || target_size.radio>tsize->radio_max * 0.01)
			{
#ifdef DEBUG_MODE
				std::ostringstream s;
				s << target_size.radio;
				cv::putText(ret, s.str(), cv::Point(rot_rect.center) + cv::Point(0, y_dist), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 0, 255), 1);
				cv::drawContours(this->ret, contours, i, cv::Scalar(0, 0, 255), 1);
#endif
				continue;
			}
#ifdef DEBUG_MODE
			else
			{
				std::ostringstream s;
				s << target_size.radio;
				cv::putText(ret, s.str(), cv::Point(rot_rect.center) + cv::Point(0, y_dist), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 255, 0), 1);
			}
#endif
			//3.面积比
			target_size.area = cv::contourArea(contours[i]);
			target_size.area_radio = target_size.area*1.0 / rot_rect.size.area();
			if (target_size.area_radio<tsize->area_radio_min * 0.01 || target_size.area_radio>tsize->area_radio_max * 0.01)
			{
#ifdef DEBUG_MODE
				std::ostringstream s;
				s << target_size.area_radio;
				cv::putText(ret, s.str(), cv::Point(rot_rect.center) + cv::Point(0, 2 * y_dist), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 0, 255), 1);
				cv::drawContours(this->ret, contours, i, cv::Scalar(0, 0, 255), 1);
#endif
				continue;
			}
#ifdef DEBUG_MODE
			else
			{
				std::ostringstream s;
				s << target_size.area_radio;
				cv::putText(ret, s.str(), cv::Point(rot_rect.center) + cv::Point(0, 2 * y_dist), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 255, 0), 1);
			}
#endif
			//4.面积
			if (target_size.area < tsize->area_min || target_size.area > tsize->area_max)
			{
#ifdef DEBUG_MODE
				std::ostringstream s;
				s << target_size.area;
				cv::putText(ret, s.str(), cv::Point(rot_rect.center) + cv::Point(0, 3 * y_dist), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 0, 255), 1);
				cv::drawContours(this->ret, contours, i, cv::Scalar(0, 0, 255), 1);
#endif
				continue;
			}
#ifdef DEBUG_MODE
			else
			{
				std::ostringstream s;
				s << target_size.area;
				cv::putText(ret, s.str(), cv::Point(rot_rect.center) + cv::Point(0, 3 * y_dist), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 255, 0), 1);
			}

			cv::drawContours(this->ret, contours, i, cv::Scalar(0, 255, 0), 2);

			//cv::Point2f rot_rect_point[4];
			//rot_rect.points(rot_rect_point);
			//for (int i = 0; i < 4; i++)
			//{
			//	cv::line(ret, rot_rect_point[i], rot_rect_point[(i + 1) % 4], cv::Scalar(255, 255, 255));
			//}
			//cv::ellipse(ret, rot_rect, cv::Scalar(0, 255, 255));
#endif
		//角度偏移
			target_size.slope_offset = rot_rect.angle;
			//if(rot_rect.angle)

			//hu矩
	//        double hu_dist = cv::matchShapes(contours[i],contours_target,CV_CONTOURS_MATCH_I3,0.0);
	//        if(hu_dist>m_hu_dist){
	//            std::cout<<hu_dist<<" ";
	//            //continue;
	//        }
			cv::Moments mts = cv::moments(contours[i], true);
			double hu[7];
			cv::HuMoments(mts, hu);

			//float log_hu=log(abs(hu[0]));
			int hu_num = 3;



#ifdef DEBUG_MODE

#endif

			if ((hu[hu_num]) < m_hu_dist_min) {//&&abs(hu[hu_num])>m_hu_dist_max
				//std::cout<<"ref"<<(hu[hu_num])<<" "<<m_hu_dist_min<<std::endl;
				//continue;
			}
			else {
				//std::cout<<"---"<<(hu[hu_num])<<" ";
			}

			cv::Rect rect = cv::boundingRect(contours[i]);

			rot_rect_list.push_back(rot_rect);
			target_contours.push_back(contours[i]);
			target_size_list.push_back(target_size);
			hierarchy.push_back(hierarchys[i]);
		}

#ifdef DEBUG_MODE
		cv::imshow("3.tgt_det", ret);
        cv::waitKey();
#endif

		ret.release();
		return target_contours.size();
	}

	/**
	 * @brief  检测出符合面积大小，长宽比，hu特征的轮廓。获取索引值（重载一）
	 * @param  源图-二值图
	 * @param  目标轮廓列表引用
	 * @return 是否有目标
	 * @autor  梁尧森
	 * @date   2019.3.29
	 */
	bool targetDetectionProc(cv::Mat src,
		std::vector<std::vector<cv::Point>> &target_contours,
		std::vector<cv::RotatedRect> &rot_rect_list,
		std::vector<cv::Rect> &rect_list,
		std::vector<struct TargetSize> &target_size_list,
		std::vector<int> &contours_idx,
		std::vector<Index> &vin);

	/**
	 * @brief  检测出符合面积大小，长宽比，hu特征的轮廓。获取索引值（重载二）
	 * @param  源图-二值图
	 * @param  目标轮廓列表引用
	 * @return 是否有目标
	 * @autor  梁尧森
	 * @date   2019.3.29
	 */
	bool targetDetectionProc(cv::Mat src);

	/**
	 * @brief  检测出符合面积大小，长宽比，hu特征的轮廓。返回新的列表（重载三）
	 * @param  源图-二值图
	 * @param  目标轮廓列表引用
	 * @return 是否有目标
	 * @autor  梁尧森
	 * @date   2019.3.29
	 */
	bool targetDetectionProc(cv::Mat src,
		std::vector<std::vector<cv::Point>> &target_contours,
		std::vector<cv::RotatedRect> &rot_rect_list,
		std::vector<cv::Rect> &rect_list,
		std::vector<struct TargetSize> &target_size_list,
		const std::vector<double> &hu_list = std::vector<double>());

	/**
	 * @brief  霍夫圆变换提取最大的圆（重载四）
	 * @param  源图-二值图
 	 * @param  霍夫圆容器
	 * @return 是否有目标
	 * @autor  马兴沛
	 * @date   2019.6.30
	 */
    bool targetDetectionProc(cv::Mat ret,cv::Mat bin,
                             std::vector<cv::Point2f>&need_centers,
                             std::vector<float>&need_radius);

	/**
	 * @brief  获取索引值（重载一）
	 * @param  轮廓
	 * @param  外接最大旋转矩形
	 * @param  外接最大矩形
	 * @param  轮廓属性
	 * @param  筛选后索引
	 * @return 空
	 * @autor  梁尧森
	 * @date   2019.3.27
	 */
	void filterByAttrubute(std::vector<std::vector<cv::Point>> contours,
		std::vector<cv::RotatedRect> &rot_rect_list,
		std::vector<cv::Rect> &rect_list,
		std::vector<struct TargetSize> &target_size_list,
		std::vector<int> &contours_idx);

	/**
	 * @brief  返回新的列表（重载二）
	 * @param  轮廓
	 * @param  筛选轮廓
	 * @param  外接最大旋转矩形
	 * @param  外接最大矩形
	 * @param  轮廓属性
	 * @return 返回值
	 * @autor  梁尧森
	 * @date   2019.3.27
	 */
	void filterByAttrubute(std::vector<std::vector<cv::Point>> contours,
		std::vector<std::vector<cv::Point>> &target_contours,
		std::vector<cv::RotatedRect> &rot_rect_list,
		std::vector<cv::Rect> &rect_list,
		std::vector<struct TargetSize> &target_size_list,
		const std::vector<double> &hu_list = std::vector<double>());

	/**
	 * @brief  处理索引中的列表，获取索引值（重载一）
	 * @param  第一个参数
	 * @return 返回值
	 * @autor  梁尧森
	 * @date   2019.3.27
	 */
	void filterByRelationship(std::vector<std::vector<cv::Point>> contours,
		std::vector<cv::Vec4i> hierarchy,
		std::vector<int> contours_idx,
		std::vector<Index> &vin);

	/**
	 * @brief  获取索引值（重载二）
	 * @param  第一个参数
	 * @return 返回值
	 * @autor  梁尧森
	 * @date   2019.3.27
	 */
	void filterByRelationship(std::vector<std::vector<cv::Point>>contours,
		std::vector<cv::Vec4i>hierarchy,
		std::vector<Index> &vin);

	/**
	 * @brief  返回新的列表（重载三）
	 * @param  第一个参数
	 * @return 返回值
	 * @autor  梁尧森
	 * @date   2019.3.27
	 */
	void filterByRelationship(std::vector<std::vector<cv::Point>>contours,
		std::vector<cv::Vec4i>hierarchy,
		std::vector<std::vector<cv::Point>> &contours_a0,
		std::vector<std::vector<cv::Point>> &contours_a1);

	/**
	 * @brief  简介
	 * @param  第一个参数
	 * @return 返回值
	 * @autor  梁尧森
	 * @date   2019.3.27
	 */
	cv::RotatedRect adjustRRect(const cv::RotatedRect &rect);

	/**
	 * @brief  简介
	 * @param  第一个参数
	 * @return 返回值
	 * @autor  梁尧森
	 * @date   2019.3.27
	 */
	void getRect4Points(std::vector<cv::Point>contours,
		std::vector<cv::Point2f> &rect4points);

	/**
	 * @brief  获取点到直线的距离
	 * @param  第一个参数
	 * @return 返回值
	 * @autor  梁尧森
	 * @date   2018-
	 */
	float getDistP2L(cv::Point pointP, cv::Point pointA, cv::Point pointB);

	/**
	 * @brief  对角点进行排序，因为之前检测出的轮廓是带序号的
	 * @param  第一个参数
	 * @return 返回值
	 * @autor  梁尧森
	 * @date   2018-
	 */
	int listConnor(int i1, int i2, int i3);

	/**
	 * @brief  获取两点距离
	 * @param  第一个参数
	 * @return 返回值
	 * @autor  梁尧森
	 * @date   2018-
	 */
	float getDistance(cv::Point pointO, cv::Point pointA);

	/**
	 * @brief  海伦公式用三边长求三角形面积，两三角形面积和为四边形面积
	 * @param  四边形4个角点
	 * @return 四边形面积
	 * @autor  梁尧森，曾庭威
	 * @date   2019.3.6
	 */
	float calcAreaFrom4Points(std::vector<cv::Point2f> rect4points);

	void setDrawingRetImg(cv::Mat ret)
	{
		this->ret = ret.clone();
	}

	/**
	 * @brief  两图片相与求公共区域面积
	 * @param  第一个参数
	 * @return 返回值
	 * @autor  曾庭威
	 * @date   2018-3.14
	 */
	double imgAlignment(cv::Mat bin, cv::Mat quadrilateral);

	/**
	 * @brief  计算出轮廓的面积
	 * @param  第一个参数
	 * @return 返回值
	 * @autor  曾庭威
	 * @date   2018-3.14
	 */
	double calculateArea(cv::Mat img_alignment);

	/**
	 * @brief  简介
	 * @param  不规则图形二值图
	 * @param  预测的四边形角点
	 * @param  轮廓的外接矩形
	 * @return 重合部分的面积
	 * @autor  梁尧森，曾庭威
	 * @date   2019.3.15
	 */
	double calcIou(cv::Mat bin, std::vector<cv::Point2f> rect4points, cv::Rect rect);

	void perspectiveTran(cv::Mat roi, cv::Mat &dst, cv::Point2f rect4points[4])
	{
		//由定位角校正图片
		//找到角度最大的点
		/*double ca[2];
		double cb[2];

		ca[0] = rect4points[1].x - rect4points[0].x;
		ca[1] = rect4points[1].y - rect4points[0].y;
		cb[0] = rect4points[2].x - rect4points[0].x;
		cb[1] = rect4points[2].y - rect4points[0].y;
		double angle1 = 180 / 3.1415*acos((ca[0] * cb[0] + ca[1] * cb[1]) / (sqrt(ca[0] * ca[0] + ca[1] * ca[1])*sqrt(cb[0] * cb[0] + cb[1] * cb[1])));
		double ccw1;
		if (ca[0] * cb[1] - ca[1] * cb[0] > 0) ccw1 = 0;
		else ccw1 = 1;

		ca[0] = rect4points[0].x - rect4points[1].x;
		ca[1] = rect4points[0].y - rect4points[1].y;
		cb[0] = rect4points[2].x - rect4points[1].x;
		cb[1] = rect4points[2].y - rect4points[1].y;
		double angle2 = 180 / 3.1415*acos((ca[0] * cb[0] + ca[1] * cb[1]) / (sqrt(ca[0] * ca[0] + ca[1] * ca[1])*sqrt(cb[0] * cb[0] + cb[1] * cb[1])));
		double ccw2;
		if (ca[0] * cb[1] - ca[1] * cb[0] > 0) ccw2 = 0;
		else ccw2 = 1;

		ca[0] = rect4points[1].x - rect4points[2].x;
		ca[1] = rect4points[1].y - rect4points[2].y;
		cb[0] = rect4points[0].x - rect4points[2].x;
		cb[1] = rect4points[0].y - rect4points[2].y;
		double angle3 = 180 / 3.1415*acos((ca[0] * cb[0] + ca[1] * cb[1]) / (sqrt(ca[0] * ca[0] + ca[1] * ca[1])*sqrt(cb[0] * cb[0] + cb[1] * cb[1])));
		double ccw3;
		if (ca[0] * cb[1] - ca[1] * cb[0] > 0) ccw3 = 0;
		else ccw3 = 1;

		cv::Point2f poly[4];
		if (angle3 > angle2 && angle3 > angle1)
		{
			if (ccw3)
			{
				poly[1] = rect4points[1];
				poly[3] = rect4points[0];
			}
			else
			{
				poly[1] = rect4points[0];
				poly[3] = rect4points[1];
			}
			poly[0] = rect4points[2];
			cv::Point temp(rect4points[0].x + rect4points[1].x - rect4points[2].x, rect4points[0].y + rect4points[1].y - rect4points[2].y);
			poly[2] = temp;
		}
		else if (angle2 > angle1 && angle2 > angle3)
		{
			if (ccw2)
			{
				poly[1] = rect4points[0];
				poly[3] = rect4points[2];
			}
			else
			{
				poly[1] = rect4points[2];
				poly[3] = rect4points[0];
			}
			poly[0] = rect4points[1];
			cv::Point temp(rect4points[0].x + rect4points[2].x - rect4points[1].x, rect4points[0].y + rect4points[2].y - rect4points[1].y);
			poly[2] = temp;
		}
		else if (angle1 > angle2 && angle1 > angle3)
		{
			if (ccw1)
			{
				poly[1] = rect4points[1];
				poly[3] = rect4points[2];
			}
			else
			{
				poly[1] = rect4points[2];
				poly[3] = rect4points[1];
			}
			poly[0] = rect4points[0];
			cv::Point temp(rect4points[1].x + rect4points[2].x - rect4points[0].x, rect4points[1].y + rect4points[2].y - rect4points[0].y);
			poly[2] = temp;
		}*/

		cv::Point2f trans[4];
		trans[0] = cv::Point2f(0, 0);
		trans[1] = cv::Point2f(0, roi.cols);
		trans[3] = cv::Point2f(roi.rows, roi.cols);
		trans[2] = cv::Point2f(roi.rows, 0);

		//获取透视投影变换矩阵
		std::vector<cv::Point2f> dstTri;
		cv::Mat warpper_mat(3, 3, CV_32FC1);
		warpper_mat = cv::getPerspectiveTransform(rect4points, trans);

		//计算变换结果
		cv::warpPerspective(roi, dst, warpper_mat, cv::Size(roi.rows, roi.cols));
	}

public:
    void setTargetSize();
    std::vector<std::vector<cv::Point2i>> getContours();
    std::vector<cv::Vec4i> getTargetHierarchy();
    std::vector<cv::RotatedRect> getTargetRotRect();
    std::vector<cv::Rect> getTragetRect();
    std::vector<struct TargetSize> getTargetSize();
    std::vector<int> getTargetIdx();
    std::vector<Index> getTargetRelationShip();
protected:
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i>              hierarchy;
    std::vector<cv::RotatedRect>        rot_rect_list;
    std::vector<cv::Rect>               rect_list;
    std::vector<struct TargetSize>      target_size_list;
    std::vector<int>                    contours_idx;
    std::vector<Index>                  vin;

    std::vector<cv::Point2f>            centers;
    std::vector<float>                  radius;
private:
    cv::Mat ret;

    TargetSizeParam *tsize;
    struct Debug *debug;
    double m_hu_dist_min;
    double m_hu_dist_max;
    std::vector<cv::Point> contours_target;
};

#endif
