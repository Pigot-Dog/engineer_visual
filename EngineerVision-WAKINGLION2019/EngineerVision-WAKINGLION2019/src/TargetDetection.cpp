#include "TargetDetection.h"

#define WIN_NAME_TARGET "target_size"

TargetDetection::TargetDetection()
{

}

TargetDetection::TargetDetection(TargetSizeParam &target_param_size,
                                 Debug &debug,
                                 std::vector<cv::Point> contours_target)
{
    tsize=&target_param_size;
    this->debug=&debug;
    this->contours_target.assign(contours_target.begin(),contours_target.end());
    //this->contours_target=contours_target;
}


TargetDetection::~TargetDetection()
{
}

bool TargetDetection::targetDetectionProc(cv::Mat src,
                                          std::vector<std::vector<cv::Point>> &target_contours,
                                          std::vector<cv::RotatedRect> &rot_rect_list,
                                          std::vector<cv::Rect> &rect_list,
                                          std::vector<struct TargetSize> &target_size_list,
                                          std::vector<int> &contours_idx,
                                          std::vector<Index> &vin)
{
    if(target_contours.size())
        target_contours.clear();

    if(rot_rect_list.size())
        rot_rect_list.clear();

    if(rect_list.size())
        rect_list.clear();

    if(target_size_list.size())
        target_size_list.clear();

    if(contours_idx.size())
        contours_idx.clear();

    if(vin.size())
        vin.clear();

    //std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(src,target_contours,hierarchy,cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

    //-----------------------------------【传统方法过滤】--------------------------------------------
    // brief：使用传统方法过滤大部分明显不是目标的区域，为后续的分类减小干扰和运算量
    //------------------------------------------------------------------------------------------------
    filterByAttrubute(target_contours,rot_rect_list,rect_list,target_size_list,contours_idx);

    //-----------------------------------【利用拓扑关系过滤】--------------------------------------------
    // brief：
    //------------------------------------------------------------------------------------------------
    filterByRelationship(target_contours,hierarchy,
                         contours_idx,
                         vin);

#ifdef DEBUG_MODE
    cv::imshow("3.tgt_det",ret);
#endif

    return target_contours.size();
}

bool TargetDetection::targetDetectionProc(cv::Mat src)
{
    //std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(src,contours,hierarchy,cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

    //-----------------------------------【传统方法过滤】--------------------------------------------
    // brief：使用传统方法过滤大部分明显不是目标的区域，为后续的分类减小干扰和运算量
    //------------------------------------------------------------------------------------------------
    filterByAttrubute(contours,rot_rect_list,rect_list,target_size_list,contours_idx);

    //-----------------------------------【利用拓扑关系过滤】--------------------------------------------
    // brief：
    //------------------------------------------------------------------------------------------------
    filterByRelationship(contours,hierarchy,
                         contours_idx,
                         vin);

#ifdef DEBUG_MODE
    cv::imshow("3.tgt_det",ret);
#endif

    return contours.size();
}

bool TargetDetection::targetDetectionProc(cv::Mat src,
                                          std::vector<std::vector<cv::Point>> &target_contours,
                                          std::vector<cv::RotatedRect> &rot_rect_list,
                                          std::vector<cv::Rect> &rect_list,
                                          std::vector<struct TargetSize> &target_size_list,
                                          const std::vector<double> &hu_list)
{
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(src,contours,hierarchy,cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

    //-----------------------------------【传统方法过滤】--------------------------------------------
    // brief：使用传统方法过滤大部分明显不是目标的区域，为后续的分类减小干扰和运算量
    //------------------------------------------------------------------------------------------------
    filterByAttrubute(contours,target_contours,rot_rect_list,rect_list,target_size_list,hu_list);

#ifdef DEBUG_MODE
    if(debug->b_show_target)
        cv::imshow("3.tgt_det",ret);
    else
    {
        if(-1!=cv::getWindowProperty("3.tgt_det",1))
            cv::destroyWindow("3.tgt_det");
    }
#endif

    return target_contours.size();
}

void TargetDetection::filterByAttrubute(std::vector<std::vector<cv::Point>> contours,
                       std::vector<cv::RotatedRect> &rot_rect_list,
                       std::vector<cv::Rect> &rect_list,
                       std::vector<struct TargetSize> &target_size_list,
                       std::vector<int> &contours_idx)
{
    if(rot_rect_list.size())
        rot_rect_list.clear();

    if(rect_list.size())
        rect_list.clear();

    if(target_size_list.size())
        target_size_list.clear();

    if(contours_idx.size())
        contours_idx.clear();

#ifdef DEBUG_MODE
    float digits_size=0.3;
    int y_dist=20;
#endif
    for(int i=0;i<contours.size();i++)
    {
        struct TargetSize target_size;

        //获取拟合椭圆的外包围矩形
        cv::RotatedRect rot_rect = adjustRRect(cv::minAreaRect(contours[i])); // fitEllipse
        //cv::RotatedRect rot_rect = minAreaRect(contours[i]);
        cv::Rect rect=cv::boundingRect(contours[i]);

        rot_rect_list.push_back(rot_rect);
        rect_list.push_back(rect);

        //1.轮廓长度
        target_size.len=contours[i].size();
        //2.长宽比
        target_size.radio = rot_rect.size.height*1.0/rot_rect.size.width;
        //3.面积比
        target_size.area = cv::contourArea(contours[i]);
        target_size.area_radio=target_size.area*1.0/rot_rect.size.area();
        //5.角度偏移
        target_size.slope_offset=rot_rect.angle;

        target_size_list.push_back(target_size);

        //1.轮廓长度
        if(target_size.len<tsize->len_min||target_size.len>tsize->len_max)continue;

        //2.长宽比
        if(target_size.radio<tsize->radio_min||target_size.radio>tsize->radio_max)
        {
#ifdef DEBUG_MODE
            std::ostringstream s;
            s<<target_size.radio;
            cv::putText(ret, s.str(), cv::Point(rot_rect.center)+cv::Point(0, y_dist), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 0, 255), 1);
            cv::drawContours(this->ret,contours,i,cv::Scalar(0,0,255),1);
            continue;
#endif
        }
#ifdef DEBUG_MODE
        else
        {
            std::ostringstream s;
            s<<target_size.radio;
            cv::putText(ret, s.str(), cv::Point(rot_rect.center)+cv::Point(0, y_dist), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 255, 0), 1);
        }
#endif
        //3.面积比
        if(target_size.area_radio<tsize->area_radio_min||target_size.area_radio>tsize->area_radio_max)
        {
#ifdef DEBUG_MODE
            std::ostringstream s;
            s<<target_size.area_radio;
            cv::putText(ret, s.str(), cv::Point(rot_rect.center)+cv::Point(0, 2*y_dist), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 0, 255), 1);
            cv::drawContours(this->ret,contours,i,cv::Scalar(0,0,255),1);
            continue;
#endif
        }
#ifdef DEBUG_MODE
        else
        {
            std::ostringstream s;
            s<<target_size.area_radio;
            cv::putText(ret, s.str(), cv::Point(rot_rect.center)+cv::Point(0, 2*y_dist), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 255, 0), 1);
        }
#endif
        //4.面积
        if(target_size.area < tsize->area_min || target_size.area > tsize->area_max)
        {
#ifdef DEBUG_MODE
            std::ostringstream s;
            s<<target_size.area;
            cv::putText(ret, s.str(), cv::Point(rot_rect.center)+cv::Point(0, 3*y_dist), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 0, 255), 1);
            cv::drawContours(this->ret,contours,i,cv::Scalar(0,0,255),1);
            continue;
#endif
        }
#ifdef DEBUG_MODE
        else
        {
            std::ostringstream s;
            s<<target_size.area;
            cv::putText(ret, s.str(), cv::Point(rot_rect.center)+cv::Point(0, 3*y_dist), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 255, 0), 1);
        }

        cv::drawContours(this->ret,contours,i,cv::Scalar(0,255,0),1);
#endif
        //5.角度偏移
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
        int hu_num=3;

#ifdef DEBUG_MODE
        cv::Point2f vertex[4];
        rot_rect.points(vertex);
        for (int i = 0; i < 4; i++)
        {
            cv::line(this->ret, vertex[i], vertex[(i + 1) % 4], cv::Scalar(0, 128, 0), 1);
        }
#endif

        if((hu[hu_num])<m_hu_dist_min){//&&abs(hu[hu_num])>m_hu_dist_max
            //std::cout<<"ref"<<(hu[hu_num])<<" "<<m_hu_dist_min<<std::endl;
            //continue;
        }else{
            //std::cout<<"---"<<(hu[hu_num])<<" ";
        }

        //保存通过筛选轮廓的索引
        contours_idx.push_back(i);
    }
}

void TargetDetection::filterByAttrubute(std::vector<std::vector<cv::Point>> contours,
                       std::vector<std::vector<cv::Point>> &target_contours,
                       std::vector<cv::RotatedRect> &rot_rect_list,
                       std::vector<cv::Rect> &rect_list,
                       std::vector<struct TargetSize> &target_size_list,
                       const std::vector<double> &hu_list)
{
    if(target_contours.size())
        target_contours.clear();

    if(rot_rect_list.size())
        rot_rect_list.clear();

    if(rect_list.size())
        rect_list.clear();

    if(target_size_list.size())
        target_size_list.clear();

//	if (hu_list.size())
//		hu_list.clear();
#ifdef DEBUG_MODE
    float digits_size=0.3;  //检测信息的 打印的字体大小
    float digits_size_p=0.6;//参数的    打印的字体大小

    int x_min_p=10;
    int x_max_p=50;
    int y_min_p=10;
    int y_dist_p=30;
    int y_dist=20;
#endif
    for(int i=0;i<contours.size();i++)
    {
        struct TargetSize target_size;

        //1.轮廓长度
        //target_size.len=contours[i].size();
        target_size.len=cv::arcLength(contours[i],true);
        if(target_size.len<tsize->len_min||target_size.len>tsize->len_max)continue;
        //获取拟合椭圆的外包围矩形
        cv::RotatedRect rot_rect = adjustRRect(cv::minAreaRect(contours[i]));
        //cv::RotatedRect rot_rect = minAreaRect(contours[i]);
        //cv::RotatedRect rot_rect = fitEllipse(contours[i]);
        //2.长宽比
        target_size.radio = rot_rect.size.height*1.0/rot_rect.size.width;
        if(target_size.radio<1.45||target_size.radio>tsize->radio_max)
        {
#ifdef DEBUG_MODE
            std::ostringstream s,ss,sb;
            s<<target_size.radio;
            ss<<tsize->radio_min;
            sb<<tsize->radio_max;
            cv::putText(ret, s.str(), cv::Point(rot_rect.center)+cv::Point(0, 2*y_dist), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 0, 255), 1);
            cv::putText(ret, ss.str(), cv::Point(x_min_p,y_min_p)+cv::Point(0, 2*y_dist_p), cv::FONT_HERSHEY_COMPLEX, digits_size_p, cv::Scalar(255, 0, 255), 1);
            cv::putText(ret, sb.str(), cv::Point(x_max_p,y_min_p)+cv::Point(0, 2*y_dist_p), cv::FONT_HERSHEY_COMPLEX, digits_size_p, cv::Scalar(255, 0, 255), 1);
            cv::drawContours(this->ret,contours,i,cv::Scalar(0,0,255),1);
            continue;
#endif
        }
#ifdef DEBUG_MODE
        else
        {
            std::ostringstream s;
            s<<target_size.radio;
            cv::putText(ret, s.str(), cv::Point(rot_rect.center)+cv::Point(0, 2*y_dist), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 255, 0), 1);
        }
#endif
        //3.面积
        target_size.area = cv::contourArea(contours[i]);
        if(target_size.area < tsize->area_min || target_size.area > tsize->area_max)
        {
#ifdef DEBUG_MODE
            std::ostringstream s,ss,sb;
            s<<target_size.area;
            ss<<tsize->area_min;
            sb<<tsize->area_max;
            cv::putText(ret, s.str(), cv::Point(rot_rect.center)+cv::Point(0, 3*y_dist), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 0, 255), 1);
            cv::putText(ret, ss.str(), cv::Point(x_min_p,y_min_p)+cv::Point(0, 3*y_dist_p), cv::FONT_HERSHEY_COMPLEX, digits_size_p, cv::Scalar(255, 0, 255), 1);
            cv::putText(ret, sb.str(), cv::Point(x_max_p,y_min_p)+cv::Point(0, 3*y_dist_p), cv::FONT_HERSHEY_COMPLEX, digits_size_p, cv::Scalar(255, 0, 255), 1);
            cv::drawContours(this->ret,contours,i,cv::Scalar(0,0,255),1);
            continue;
#endif
        }
#ifdef DEBUG_MODE
        else
        {
            std::ostringstream s;
            s<<target_size.area;
            cv::putText(ret, s.str(), cv::Point(rot_rect.center)+cv::Point(0, 3*y_dist), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 255, 0), 1);
        }

        cv::drawContours(this->ret,contours,i,cv::Scalar(0,255,0),1);
#endif
        //4.矩形度
        target_size.area_radio=target_size.area*1.0/rot_rect.size.area();
        if(target_size.area_radio<tsize->area_radio_min||
                target_size.area_radio>tsize->area_radio_max)
        {
#ifdef DEBUG_MODE
            std::ostringstream s,ss,sb;
            s<<target_size.area_radio;
            ss<<tsize->area_radio_min;
            sb<<tsize->area_radio_max;
            cv::putText(ret, s.str(), cv::Point(rot_rect.center)+cv::Point(0, 4*y_dist), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 0, 255), 1);
            cv::putText(ret, ss.str(), cv::Point(x_min_p,y_min_p)+cv::Point(0, 4*y_dist_p), cv::FONT_HERSHEY_COMPLEX, digits_size_p, cv::Scalar(255, 0, 255), 1);
            cv::putText(ret, sb.str(), cv::Point(x_max_p,y_min_p)+cv::Point(0, 4*y_dist_p), cv::FONT_HERSHEY_COMPLEX, digits_size_p, cv::Scalar(255, 0, 255), 1);
            cv::drawContours(this->ret,contours,i,cv::Scalar(0,0,255),1);
            continue;
#endif
        }
#ifdef DEBUG_MODE
        else
        {
            std::ostringstream s;
            s<<target_size.area_radio;
            cv::putText(ret, s.str(), cv::Point(rot_rect.center)+cv::Point(0, 4*y_dist), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 255, 0), 1);
        }
#endif
        //5.面积比长度（似圆度）
        target_size.area_len_radio=target_size.area*1.0/(target_size.len);//*target_size.len);
        if(target_size.area_len_radio < tsize->area_len_radio_min ||
                target_size.area_len_radio> tsize->area_len_radio_max)
        {
#ifdef DEBUG_MODE
            std::ostringstream s,ss,sb;
            s<<target_size.area/target_size.len;
            ss<<tsize->area_len_radio_min;
            sb<<tsize->area_len_radio_max;
            cv::putText(ret, s.str(), cv::Point(rot_rect.center)+cv::Point(0, 5*y_dist), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 0, 255), 1);
            cv::putText(ret, ss.str(), cv::Point(x_min_p,y_min_p)+cv::Point(0, 5*y_dist_p), cv::FONT_HERSHEY_COMPLEX, digits_size_p, cv::Scalar(255, 0, 255), 1);
            cv::putText(ret, sb.str(), cv::Point(x_max_p,y_min_p)+cv::Point(0, 5*y_dist_p), cv::FONT_HERSHEY_COMPLEX, digits_size_p, cv::Scalar(255, 0, 255), 1);
            cv::drawContours(this->ret,contours,i,cv::Scalar(0,0,255),1);
            continue;
#endif
        }
#ifdef DEBUG_MODE
        else
        {
            std::ostringstream s;
            s<<target_size.area/target_size.len;
            cv::putText(ret, s.str(), cv::Point(rot_rect.center)+cv::Point(0, 5*y_dist), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 255, 0), 1);
        }

        cv::drawContours(this->ret,contours,i,cv::Scalar(0,255,0),1);
#endif
//        //6.多边形逼近
//        std::vector<cv::Point>approx;
//        cv::approxPolyDP(contours[i],approx,0.01*target_size.len,true);
//        target_size.corners=approx.size();

//        if(target_size.corners < tsize->corners_size_min)
//        {
//#ifdef DEBUG_MODE
//            std::ostringstream s,ss,sb;
//            s<<target_size.corners;
//            ss<<tsize->corners_size_min;
////            sb<<tsize->area_max;
//            cv::putText(ret, s.str(), cv::Point(rot_rect.center)+cv::Point(0, 6*y_dist), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 0, 255), 1);
//            cv::putText(ret, ss.str(), cv::Point(x_min_p,y_min_p)+cv::Point(0, 6*y_dist_p), cv::FONT_HERSHEY_COMPLEX, digits_size_p, cv::Scalar(255, 0, 255), 1);
////            cv::putText(ret, sb.str(), cv::Point(x_max_p,y_min_p)+cv::Point(0, 6*y_dist_p), cv::FONT_HERSHEY_COMPLEX, digits_size_p, cv::Scalar(255, 0, 255), 1);
//            cv::drawContours(this->ret,contours,i,cv::Scalar(0,0,255),1);
//            continue;
//#endif
//        }
//#ifdef DEBUG_MODE
//        else
//        {
//            std::ostringstream s;
//            s<<target_size.corners;
//            cv::putText(ret, s.str(), cv::Point(rot_rect.center)+cv::Point(0, 6*y_dist), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 255, 0), 1);
//        }

//        cv::drawContours(this->ret,contours,i,cv::Scalar(0,255,0),1);
//#endif

        //角度偏移
        target_size.slope_offset=rot_rect.angle;
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
        int hu_num=3;

#ifdef DEBUG_MODE
        cv::Point2f vertex[4];
        rot_rect.points(vertex);
        for (int i = 0; i < 4; i++)
        {
            cv::line(this->ret, vertex[i], vertex[(i + 1) % 4], cv::Scalar(0, 128, 0), 1);
        }
#endif

        if((hu[hu_num])<m_hu_dist_min){//&&abs(hu[hu_num])>m_hu_dist_max
            //std::cout<<"ref"<<(hu[hu_num])<<" "<<m_hu_dist_min<<std::endl;
            //continue;
        }else{
            //std::cout<<"---"<<(hu[hu_num])<<" ";
        }

        target_contours.push_back(contours[i]);
        rot_rect_list.push_back(rot_rect);
        rect_list.push_back(cv::boundingRect(contours[i]));
        target_size_list.push_back(target_size);
//        hu_list.push_back(hu[0]);
    }
}

void TargetDetection::filterByRelationship(std::vector<std::vector<cv::Point>> contours,
                          std::vector<cv::Vec4i> hierarchy,
                          std::vector<int> contours_idx,
                          std::vector<Index> &vin)
{
    if(vin.size())
        vin.clear();

    for(int i=0;i<contours_idx.size();i++)
    {
        int tempindex;
        Index in;

        //利用拓扑关系查找轮廓
        if(hierarchy[contours_idx[i]][2] == -1)
        {
            continue;
        }
        else
        {
            tempindex = hierarchy[contours_idx[i]][2];//子轮廓的索引
            //记录搜索到的有两个子轮廓的轮廓并把他们的编号存储

            double area[2] = { cv::contourArea(contours[contours_idx[i]]), cv::contourArea(contours[tempindex]) };
            //if (area[0] < 900)continue;//30*30

            if (area[1] / area[0] < 0.54 || area[1] / area[0] > 0.72)//0.64
            {
                //continue;
            }

            double lenth[2] = { cv::arcLength(contours[contours_idx[i]], 1) ,cv::arcLength(contours[tempindex], 1) };
            if (lenth[1] / lenth[0] < 0.6 || lenth[1] / lenth[0] > 1.0)
            {
                //continue;
            }

            //cv::drawContours(drawing_parents, contours, i, CV_RGB(255, 255, 255), CV_FILLED, 8);
            //cv::drawContours(drawing_child, contours, tempindex, CV_RGB(255, 255, 255), CV_FILLED, 8);
            in.a0 = contours_idx[i];//父母框
            in.a1 = tempindex;//儿子框！！！
            vin.push_back(in);
            //gui.drawingProc(contours, rect, cv::Scalar(255, 0, 0), in.a0);
            //gui.drawingProc(contours, rect, cv::Scalar(0, 0, 255), in.a1);
        }
    }
}

void TargetDetection::filterByRelationship(std::vector<std::vector<cv::Point>>contours,
                          std::vector<cv::Vec4i>hierarchy,
                          std::vector<Index> &vin)
{
    if(vin.size())
        vin.clear();

    for(int i=0;i<contours.size();i++)
    {
        int tempindex;
        Index in;

        //利用拓扑关系查找轮廓
        if(hierarchy[i][2] == -1)
        {
            continue;
        }
        else
        {
            tempindex = hierarchy[i][2];//子轮廓的索引
            //记录搜索到的有两个子轮廓的轮廓并把他们的编号存储

            double area[2] = { cv::contourArea(contours[i]), cv::contourArea(contours[tempindex]) };
            //if (area[0] < 900)continue;//30*30

            if (area[1] / area[0] < 0.54 || area[1] / area[0] > 0.72)//0.64
            {
                //continue;
            }

            double lenth[2] = { cv::arcLength(contours[i], 1) ,cv::arcLength(contours[tempindex], 1) };
            if (lenth[1] / lenth[0] < 0.6 || lenth[1] / lenth[0] > 1.0)
            {
                //continue;
            }

            //cv::drawContours(drawing_parents, contours, i, CV_RGB(255, 255, 255), CV_FILLED, 8);
            //cv::drawContours(drawing_child, contours, tempindex, CV_RGB(255, 255, 255), CV_FILLED, 8);
            in.a0 = i;//父母框
            in.a1 = tempindex;//儿子框！！！
            vin.push_back(in);
            //gui.drawingProc(contours, rect, cv::Scalar(255, 0, 0), in.a0);
            //gui.drawingProc(contours, rect, cv::Scalar(0, 0, 255), in.a1);
        }
    }
}

void TargetDetection::filterByRelationship(std::vector<std::vector<cv::Point>>contours,
                          std::vector<cv::Vec4i>hierarchy,
                          std::vector<std::vector<cv::Point>> &contours_a0,
                          std::vector<std::vector<cv::Point>> &contours_a1)
{
    if(contours_a0.size())
        contours_a0.clear();
    if(contours_a1.size())
        contours_a1.clear();

    for(int i=0;i<contours.size();i++)
    {
        int tempindex;

        //利用拓扑关系查找轮廓
        if(hierarchy[i][2] == -1)
        {
            continue;
        }
        else
        {
            tempindex = hierarchy[i][2];//子轮廓的索引
            //记录搜索到的有两个子轮廓的轮廓并把他们的编号存储

            double area[2] = { cv::contourArea(contours[i]), cv::contourArea(contours[tempindex]) };
            //if (area[0] < 900)continue;//30*30

            if (area[1] / area[0] < 0.54 || area[1] / area[0] > 0.72)//0.64
            {
                //continue;
            }

            double lenth[2] = { cv::arcLength(contours[i], 1) ,cv::arcLength(contours[tempindex], 1) };
            if (lenth[1] / lenth[0] < 0.6 || lenth[1] / lenth[0] > 1.0)
            {
                //continue;
            }

            //cv::drawContours(drawing_parents, contours, i, CV_RGB(255, 255, 255), CV_FILLED, 8);
            //cv::drawContours(drawing_child, contours, tempindex, CV_RGB(255, 255, 255), CV_FILLED, 8);
            contours_a0.push_back(contours[i]);//父母框
            contours_a1.push_back(contours[tempindex]);//儿子框！！！
            //gui.drawingProc(contours, rect, cv::Scalar(255, 0, 0), in.a0);
            //gui.drawingProc(contours, rect, cv::Scalar(0, 0, 255), in.a1);
        }
    }
}

cv::RotatedRect TargetDetection::adjustRRect(const cv::RotatedRect & rect)
{
    const cv::Size2f &s = rect.size;
    if (s.width < s.height){
        return rect;
    }
    return cv::RotatedRect(rect.center, cv::Size2f(s.height, s.width), rect.angle + 90.0);
}

void TargetDetection::getRect4Points(std::vector<cv::Point>contours,
                                     std::vector<cv::Point2f> &rect4points)
{
    //int s = contours.size();

    //for (int i = 0; i < 4; i++)
    //{
    //    cv::Point2f temp;
    //    temp.x = contours[s * i / 4].x;
    //    temp.y = contours[s * i / 4].y;
    //    rect4points.push_back(temp);
    //}

    float distance = 0, distanceMax = 0;
    cv::Point connorPoint[4], point_add;
    std::vector<cv::Point> connor4_add(3);  //先找到的三个角点
    int conP_i1, conP_i2, conP_i3, conP_i_add;
    int j = 0, flag = 0;

    for (int i = 0; i < contours.size(); i++)
    {//找第一个角点
        distance = getDistance(contours[i], contours[0]);
        if (distance > distanceMax)
        {
            distanceMax = distance;
            connorPoint[0] = contours[i]; //第一个角点
            conP_i1 = i;
        }
    }
    distance = 0;
    distanceMax = 0;
    for (int i = 0; i < contours.size(); i++)
    {//找第二个角点
        distance = getDistance(contours[i], connorPoint[0]);
        if (distance > distanceMax)
        {
            distanceMax = distance;
            connorPoint[1] = contours[i]; //第二个角点
            conP_i2 = i;
        }
    }
    distance = 0;
    distanceMax = 0;
    for (int i = 0; i < contours.size(); i++)
    {//找第三个角点
        distance = getDistance(contours[i], connorPoint[0]) + getDistance(contours[i], connorPoint[1]);
        if (distance > distanceMax)
        {
            distanceMax = distance;
            connorPoint[2] = contours[i]; //第三个角点
            conP_i3 = i;
        }
    }
    flag = listConnor(conP_i1, conP_i2, conP_i3);//对三个角点由大到小排序
    switch (flag)
    {//对三个角点排序
    case 0:break;
    case 123:break;
    case 132:point_add = connorPoint[1]; connorPoint[1] = connorPoint[2]; connorPoint[2] = point_add; break;//2,3交换
    case 213:point_add = connorPoint[0]; connorPoint[0] = connorPoint[1]; connorPoint[1] = point_add; break;//1,2交换
    case 231:point_add = connorPoint[0]; connorPoint[0] = connorPoint[1]; connorPoint[1] = point_add;
        point_add = connorPoint[1]; connorPoint[1] = connorPoint[2]; connorPoint[2] = point_add; break;//1,2交换+2,3交换
    case 321:point_add = connorPoint[2]; connorPoint[2] = connorPoint[0]; connorPoint[0] = point_add; break;//1,3交换
    case 312:point_add = connorPoint[2]; connorPoint[2] = connorPoint[0]; connorPoint[0] = point_add;
        point_add = connorPoint[1]; connorPoint[1] = connorPoint[2]; connorPoint[2] = point_add; break;//1,3交换+2,3交换
    }
    switch (flag)
        {//对三个角点排序
        case 0:break;
        case 123:break;
        case 132:conP_i_add = conP_i2; conP_i2 = conP_i3; conP_i3 = conP_i_add; break;//2,3交换
        case 213:conP_i_add = conP_i1; conP_i1 = conP_i2; conP_i2 = conP_i_add; break;//1,2交换
        case 231:conP_i_add = conP_i1; conP_i1 = conP_i2; conP_i2 = conP_i_add;
            conP_i_add = conP_i2; conP_i2 = conP_i3; conP_i3 = conP_i_add; break;//1,2交换+2,3交换
        case 321:conP_i_add = conP_i3; conP_i3 = conP_i1; conP_i1 = conP_i_add; break;//1,3交换
        case 312:conP_i_add = conP_i3; conP_i3 = conP_i1; conP_i1 = conP_i_add;
            conP_i_add = conP_i2; conP_i2 = conP_i3; conP_i3 = conP_i_add; break;//1,3交换+2,3交换
        }
    distance = 0;
    distanceMax = 0;
    for (int i = conP_i3; i < conP_i2; i++)
    {//相隔两角点之间找到怀疑是4角点的点
        distance = getDistance(contours[i], connorPoint[2]) + getDistance(contours[i], connorPoint[1]);
        if (distance > distanceMax)
        {
            distanceMax = distance;
            connor4_add[0] = contours[i];
        }
    }
    distance = 0;
    distanceMax = 0;
    for (int i = conP_i2; i < conP_i1; i++)
    {//相隔两角点之间找到怀疑是4角点的点
        distance = getDistance(contours[i], connorPoint[0]) + getDistance(contours[i], connorPoint[1]);
        if (distance > distanceMax)
        {
            distanceMax = distance;
            connor4_add[1] = contours[i];
        }
    }
    distance = 0;
    distanceMax = 0;
    for (int i = conP_i1; i < contours.size() + conP_i3; i++)
    {//相隔两角点之间找到怀疑是4角点的点
        if (i < contours.size())
        {
            distance = getDistance(contours[i], connorPoint[0]) + getDistance(contours[i], connorPoint[2]);
            if (distance > distanceMax)
            {
                distanceMax = distance;
                connor4_add[2] = contours[i];
            }
        }
        else
        {
            distance = getDistance(contours[i - contours.size()], connorPoint[0]) + getDistance(contours[i - contours.size()], connorPoint[2]);
            if (distance > distanceMax)
            {
                distanceMax = distance;
                connor4_add[2] = contours[i - contours.size()];
            }
        }
    }

    float d[3];
    d[0] = getDistP2L(connor4_add[0], connorPoint[2], connorPoint[1]);
    d[1] = getDistP2L(connor4_add[1], connorPoint[1], connorPoint[0]);
    d[2] = getDistP2L(connor4_add[2], connorPoint[0], connorPoint[2]);

    float max_dist = d[0];
    int max_dist_idx = 0;
    for (int j = 1; j < 3; j++)
    {
        if (d[j] > max_dist)
        {
            max_dist = d[j];
            max_dist_idx = j;
        }
    }

    connorPoint[3] = connor4_add[max_dist_idx];

//    cv::circle(drawing, contours[0], 15, cv::Scalar(0, 255, 0), 1, LINE_AA);

//    cv::circle(drawing, connorPoint1, 3, cv::Scalar(255, 0, 0), FILLED, LINE_AA);
//    cv::circle(drawing, connorPoint2, 3, cv::Scalar(0, 255, 0), FILLED, LINE_AA);
//    cv::circle(drawing, connorPoint3, 3, cv::Scalar(0, 0, 255), FILLED, LINE_AA);
//    cv::circle(drawing, connorPoint4, 10, cv::Scalar(255, 0, 255), 1, LINE_AA);

    for (int i = 3; i > 0; i--)
    {
        for (int j = 0; j < i; j++)
        {
            if (connorPoint[j].x > connorPoint[j + 1].x)
            {
                cv::Point con_temp = connorPoint[j];
                connorPoint[j] = connorPoint[j + 1];
                connorPoint[j + 1] = con_temp;
            }
        }
    }
    for (int j = 0; j < 4; j+=2)
    {
        if (connorPoint[j].y > connorPoint[j + 1].y)
        {
            cv::Point con_temp = connorPoint[j];
            connorPoint[j] = connorPoint[j + 1];
            connorPoint[j + 1] = con_temp;
        }
    }

    rect4points.push_back(connorPoint[0]);
    rect4points.push_back(connorPoint[1]);
    rect4points.push_back(connorPoint[2]);
    rect4points.push_back(connorPoint[3]);


//    rect4points[0] = connorPoint1;
//    rect4points[1] = connorPoint2;
//    rect4points[2] = connorPoint3;
//    rect4points[3] = connorPoint4;

}

float TargetDetection::getDistP2L(cv::Point pointP, cv::Point pointA, cv::Point pointB)
{
    //求直线方程
    int A = 0, B = 0, C = 0;
    A = pointA.y - pointB.y;
    B = pointB.x - pointA.x;
    C = pointA.x*pointB.y - pointA.y*pointB.x;
    //代入点到直线距离公式
    float distance = 0;
    distance = ((float)abs(A*pointP.x + B * pointP.y + C)) / ((float)sqrtf(A*A + B * B));
    return distance;
}

int TargetDetection::listConnor(int i1, int i2, int i3)
{
    int flag = 0;

    if (i1 >= i2 && i2 >= i3)
        flag = 123;
    else if (i1 >= i3 && i3 >= i2)
        flag = 132;
    else if (i2 >= i1 && i1 >= i3)
        flag = 213;
    else if (i2 >= i3 && i3 >= i1)
        flag = 231;
    else if (i3 >= i2 && i2 >= i1)
        flag = 321;
    else if (i3 >= i1 && i1 >= i2)
        flag = 312;
    return flag;
}

float TargetDetection::getDistance(cv::Point pointO, cv::Point pointA)
{
    float distance;
    distance = powf((pointO.x - pointA.x), 2) + powf((pointO.y - pointA.y), 2);
    distance = sqrtf(distance);

    return distance;
}

float TargetDetection::calcAreaFrom4Points(std::vector<cv::Point2f> rect4points)
{
    float d01,d12,
            d02,
            d03,d23;
    d01=getDistance(rect4points[0],rect4points[1]);
    d12=getDistance(rect4points[1],rect4points[2]);
    d02=getDistance(rect4points[0],rect4points[2]);
    d03=getDistance(rect4points[0],rect4points[3]);
    d23=getDistance(rect4points[2],rect4points[3]);

    float p1 = (d01 + d02 + d12) / 2;
    float a1 = sqrt(p1*(p1 - d01)*(p1 - d12)*(p1 - d02));

    float p2 = (d03 + d23 + d02) / 2;
    float a2 = sqrt(p2*(p2 - d03)*(p2 - d23)*(p2 - d02));

    return (a1 + a2);
}

double TargetDetection::imgAlignment(cv::Mat bin, cv::Mat quadrilateral)
{
    cv::Mat img_alignment = cv::Mat::zeros(bin.size(), CV_8UC1);
    for (int i = 0; i < bin.rows; ++i)
    {
        uchar * b = bin.ptr<uchar>(i);
        uchar * q = quadrilateral.ptr<uchar>(i);
        uchar * a = img_alignment.ptr<uchar>(i);

        //两图片均为黑色区域判定为相交区域，赋值为255，否则为0
        for (int j = 0; j < bin.cols; ++j)
        {
            if ((b[j] == 255) && (q[j] == 255))
            {
                a[j] = 255;
            }
            else { a[j] = 0; }
        }
    }
    double area = calculateArea(img_alignment);
#ifdef DEBUG_MODE
    imshow("4.两图片的相与图", img_alignment);
#endif
    return area;
}

double TargetDetection::calculateArea(cv::Mat img_alignment)
{
    double area;
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(img_alignment, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
    for (int i = 0; i < contours.size(); i++)
    {
        area = cv::contourArea(contours[i]);
    }
#ifdef DEBUG_MODE
    printf("公共区域面积为：%.2f\n", area);
#endif
    return area;
}

double TargetDetection::calcIou(cv::Mat bin, std::vector<cv::Point2f> rect4points, cv::Rect rect)
{
    std::sort(rect4points.begin(),
              rect4points.end(),
              [](cv::Point2f &p1,cv::Point2f &p2)->bool {return p1.y < p2.y;});
    int u=rect4points[0].y;
    int b=rect4points[3].y;
    //double height=b-u;

    std::sort(rect4points.begin(),
        rect4points.end(),
        [](cv::Point2f &p1, cv::Point2f &p2)->bool {return p1.x < p2.x; });
    int l=rect4points[0].x;
    int r=rect4points[3].x;
    //double width=r-l;

    cv::Point2f point_temp = rect4points[2];
    rect4points[2] = rect4points[3];
    rect4points[3] = point_temp;

    cv::Point lu=cv::Point(std::min(l,rect.x),std::min(u,rect.y));
    cv::Point br=cv::Point(std::max(r,rect.x+rect.width),std::max(b,rect.y+rect.height));

    double area_iou;
    cv::Mat rect_roi = cv::Mat(bin, cv::Rect(lu,br));      //从原图中分割出轮廓的最大外接矩形图
    cv::Mat quadrilateral = cv::Mat::zeros(bin.size(), CV_8UC1); //四边形二值图的绘板

    //将传参rect4points转换成二维数组形式
    cv::Point root_points[1][4];
    for (int i = 0; i < 4; i++)
    {
        root_points[0][i] = cv::Point(rect4points[i].x, rect4points[i].y);
    }
    const cv::Point* ppt[1] = { root_points[0] };
    int npt[] = { 4 };
    cv::fillPoly(quadrilateral, ppt, npt, 1, cv::Scalar(255));

    cv::Mat quadrilateral_roi=quadrilateral(cv::Rect(lu,br));

#ifdef DEBUG_MODE
    cv::imshow("4.根据rect4points角点画出的四边形", quadrilateral);
    cv::imshow("4.bin二值图", bin);

    cv::imshow("4.根据rect4points角点画出的四边形ROI", quadrilateral_roi);
    cv::imshow("4.bin二值图ROI", rect_roi);
#endif

    area_iou = imgAlignment(rect_roi, quadrilateral_roi);

    return area_iou;
}

void TargetDetection::setTargetSize()
{

}

std::vector<std::vector<cv::Point2i>> TargetDetection::getContours()
{
    return this->contours;
}

std::vector<cv::Vec4i> TargetDetection::getTargetHierarchy()
{
    return this->hierarchy;
}

std::vector<cv::RotatedRect> TargetDetection::getTargetRotRect()
{
    return this->rot_rect_list;
}

std::vector<cv::Rect> TargetDetection::getTragetRect()
{
    return this->rect_list;
}

std::vector<struct TargetSize> TargetDetection::getTargetSize()
{
    return this->target_size_list;
}

std::vector<int> TargetDetection::getTargetIdx()
{
    return this->contours_idx;
}

std::vector<Index> TargetDetection::getTargetRelationShip()
{
    return this->vin;
}
