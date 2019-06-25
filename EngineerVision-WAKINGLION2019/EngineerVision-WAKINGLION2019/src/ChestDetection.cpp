#include "ChestDetection.h"
#include "Gui.h"

Gui debug_tool;

bool ChestDetection::detectorProc(cv::Mat src, double &pos, bool &is_small)
{
	//-----------------------------------【流程】--------------------------------------------
	// brief：
	// 1、预处理找出二值图
	// 2、检测并筛选出轮廓
	//------------------------------------------------------------------------------------------------

	//-----------------------------------【预处理】--------------------------------------------
	// brief：
	// 1. 正片叠底+迭代法
	// 2. 绿通道图
	//------------------------------------------------------------------------------------------------
	cv::Mat bin;
	cv::Mat bin_test;
	struct PreprocessInfo preprocess_info;

    adaptiveThresholdProc(src,bin);

	//    filter(bin);

		//对比测试
	//    adaptiveThresholdProc(src,bin_test,-1,-1);
	//    cv::Mat ch[3];
	//    cv::split(src,ch);

		//-----------------------------------【目标检测】--------------------------------------------
		// brief：
		// 1. 初筛
		// 2. 保存轮廓等信息
		//------------------------------------------------------------------------------------------------
#ifdef DEBUG_MODE
	setDrawingRetImg(src);  //设置画布，用于绘制检测结果
	_src = src.clone();
#endif
	std::vector<std::vector<cv::Point>> light_contours;
	std::vector<cv::RotatedRect> rot_rect_list;
	std::vector<cv::Rect> rect_list;
	std::vector<struct TargetSize> target_size_list;
	std::vector<double> hu_list;
	if (!targetDetectionProc(bin,//////////////////////////
		light_contours,
		rot_rect_list,
		rect_list,
		target_size_list,
		hu_list));

	double pos_max=-1;
	for (int i = 0; i < rect_list.size(); i++)
	{
		double center_x = (rect_list[0].x + rect_list[0].width) / 2.0;
		if (rect_list[i].x > pos_max)
		{
			pos_max = center_x;
		}
	}

	if (rect_list.size() != 0)
	{
		pos = pos_max;
	}
	return pos_max>0?true:false;
}

ChestSettings *ChestDetection::setParam()
{
	return chest_setting;
}

cv::RotatedRect ChestDetection::boundingRRect(const cv::RotatedRect &left,
	const cv::RotatedRect &right)
{
	const cv::Point &pl = left.center, &pr = right.center;
	cv::Point2f center = (pl + pr) / 2.0;
	cv::Size2f wh_l = left.size;
	cv::Size2f wh_r = right.size;
	float width = POINT_DIST(pl, pr) - (wh_l.width + wh_r.width) / 2.0;
	float height = std::max(wh_l.height, wh_r.height);
	//float height = (wh_l.height + wh_r.height) / 2.0;
	float angle = std::atan2(right.center.y - left.center.y, right.center.x - left.center.x);
	return cv::RotatedRect(center, cv::Size2f(width, height), angle * 180 / CV_PI);
}

bool ChestDetection::makeRectSafe(cv::Rect & rect, cv::Size size)
{
	if (rect.x < 0)
		rect.x = 0;
	if (rect.x + rect.width > size.width)
		rect.width = size.width - rect.x;
	if (rect.y < 0)
		rect.y = 0;
	if (rect.y + rect.height > size.height)
		rect.height = size.height - rect.y;
	if (rect.width <= 0 || rect.height <= 0)
		return false;
	return true;
}

bool ChestDetection::broadenRect(cv::Rect & rect,
	int width_added,
	int height_added,
	cv::Size size)
{
	rect.x -= width_added;
	rect.width += width_added * 2;
	rect.y -= height_added;
	rect.height += height_added * 2;
	return makeRectSafe(rect, size);
}
