#include "Gui.h"

std::vector<cv::Scalar> color_list={cv::Scalar(0,0,255),cv::Scalar(0,128,255),cv::Scalar(0,255,255),
                              cv::Scalar(0,255,128),cv::Scalar(0,255,0),cv::Scalar(128,255,0),
                              cv::Scalar(255,255,0),cv::Scalar(255,128,0),cv::Scalar(255,0,0),cv::Scalar(255,255,255)};

int is_changeOffset;
int offset_v_tmp;
int offset_t_tmp;
int xold;
int yold;

extern ItemRepository img_repository;

Gui::Gui()
{
}

Gui::Gui(cv::Mat img)
{
    ret_img=img.clone();
}

Gui::Gui(cv::Point2f center,double digits_size,int status,cv::Mat img)
{
    this->ret_img=img.clone();
    this->center=center;
    this->digits_size=digits_size;
    this->status=status;

    cv::circle(this->ret_img,cv::Point(center),3,cv::Scalar(0,255,0),1);
    cv::line(this->ret_img,cv::Point(0,center.y),cv::Point(img.cols,center.y),cv::Scalar(0,127,0),1);
    cv::line(this->ret_img,cv::Point(center.x,0),cv::Point(center.x,img.rows),cv::Scalar(0,127,0),1);
}


Gui::~Gui()
{
}

void Gui::processing(MainSettings *setting,IndVideoCapture *cap)
{


    while(1)
    {
        //cap->ConsumeItem(&img_repository,src);

        //阈值
        //std::cout<<thre<<std::endl;
    }
}

void Gui::drawingPointProc(std::vector<std::vector<cv::Point2i>>contours, cv::Scalar color, int i)
{
    //点
    for (size_t i = 0; i < contours.size(); i++)
    {
        for (int j = 0; j < contours[i].size(); j++)
        {
            cv::circle(ret_img, contours[i][j], 7, color, 2);
        }
    }
}

void Gui::drawingProc(std::vector<std::vector<cv::Point2i>>contours, std::vector<cv::Rect> rect, cv::Scalar color,int i)
{
    //边缘
    drawContours(ret_img, contours, i, color, 2);
    
    if (i >= 0)
    {
        //矩形
        cv::rectangle(ret_img, rect[i], color, 1);
        
        char strText[10];
        std::sprintf(strText, "%d %d", rect[i].width, rect[i].height);
        cv::putText(ret_img, strText, rect[i].br(), cv::FONT_HERSHEY_PLAIN, 0.5, color, 1);
        
        //点
        //for (int j = 0; j < contours[i].size(); j++)
        //{
        //    cv::circle(ret_img, contours[i][j], 1, color, 1);
        //}
    }
}

void Gui::setShowImg(cv::Mat img)
{
    src =img.clone();
}

void Gui::drawingInfo(cv::Mat &img,
    std::vector<cv::Point2f> armor_rect,
    std::vector<cv::Point2f> attitude_angle,
    double pitch,
    double yaw,
    double dist)
{
    //img = this->ret_img.clone();
    cv::circle(img,cv::Point(center),3,cv::Scalar(0,255,0),1);
    cv::line(img,cv::Point(0,center.y),cv::Point(img.cols,center.y),cv::Scalar(0,127,0),1);
    cv::line(img,cv::Point(center.x,0),cv::Point(center.x,img.rows),cv::Scalar(0,127,0),1);

    //装甲板矩形角点排序
    std::sort(armor_rect.begin(),
        armor_rect.end(),
        [](cv::Point2f &p1, cv::Point2f &p2)->bool {return p1.y < p2.y; });
    std::sort(armor_rect.begin(),
        armor_rect.end(),
        [](cv::Point2f &p1, cv::Point2f &p2)->bool {return p1.x < p2.x; });

    cv::Point2f point_temp = armor_rect[2];
    armor_rect[2] = armor_rect[3];
    armor_rect[3] = point_temp;

    float x_sum = 0.0;
    float y_sum = 0.0;
    for (int i = 0; i < armor_rect.size(); i++)
    {
        if (status != 1)
        {
            //装甲板矩形
            //cv::line(img, cv::Point(armor_rect[i]), cv::Point(armor_rect[(i + 1) % 4]), cv::Scalar(0, 255, 0), 2);
            //装甲板角点
            cv::circle(img, cv::Point(armor_rect[i]), 3, cv::Scalar(0, 255, 0), 2);
        }
        x_sum += armor_rect[i].x;
        y_sum += armor_rect[i].y;
    }

    //装甲板中心点
    cv::Point2f armor_center = cv::Point2f(x_sum / 4.0, y_sum / 4.0);

    //姿态角三轴角点
    cv::circle(img, cv::Point(attitude_angle[0]), 2, cv::Scalar(255, 0, 0), 2);
    cv::circle(img, cv::Point(attitude_angle[1]), 2, cv::Scalar(0, 255, 0), 2);
    cv::circle(img, cv::Point(attitude_angle[2]), 2, cv::Scalar(0, 0, 255), 2);

    //姿态角三轴
    cv::line(img, cv::Point(armor_center), cv::Point(attitude_angle[0]), cv::Scalar(255, 0, 0), 1);
    cv::line(img, cv::Point(armor_center), cv::Point(attitude_angle[1]), cv::Scalar(0, 255, 0), 1);
    cv::line(img, cv::Point(armor_center), cv::Point(attitude_angle[2]), cv::Scalar(0, 0, 255), 1);

    //装甲板中心点与光轴中心连线
    cv::line(img, cv::Point(armor_center), cv::Point(center), cv::Scalar(255, 0, 255), 1);

    cv::Point2f dist_center = cv::Point2f((armor_center.x + center.x) / 2.0,
        (armor_center.y + center.y) / 2.0);

    //pitch角、yaw角、距离
    std::ostringstream text_pitch, text_yaw, text_dist;
    text_pitch << pitch;
    text_yaw << yaw;
    text_dist << dist;
    cv::putText(img, text_pitch.str(), cv::Point(center.x, armor_center.y), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 255, 0), 1);
    cv::putText(img, text_yaw.str(), cv::Point(armor_center.x, center.y), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 255, 0), 1);
    cv::putText(img, text_dist.str(), cv::Point(dist_center), cv::FONT_HERSHEY_COMPLEX, digits_size, cv::Scalar(0, 255, 0), 1);

#ifdef DEBUG_MODE
    cv::namedWindow("drawing");
    cv::imshow("5.drawing", img);
#endif
}

void Gui::showDebug(cv::Mat &img,float data,cv::Point point,cv::Scalar color)
{
    char str[30];
    sprintf(str,"%.3f",data);
    cv::putText(img,str,point,CV_FONT_HERSHEY_COMPLEX_SMALL,0.5,color,1);
}


DataCollect::DataCollect(int canvas_w,int canvas_h,int ndataSize,int ndataRange)
{

    //    for(int i=0;i<DATA_BUFF_SIZE_MAX;i++)
    //    {
    //        data_q[i]=0;
    //    }
    for (int i = 0; i < DATA_BUFF_ROW_SIZE_MAX; i++) {
        for(int j = 0;j < DATA_BUFF_COL_SIZE_MAX; j++)
        {
            data_q[i][j] = 0;
        }
    }

    data_q_h=0;

    this->canvas_w=canvas_w;//画布宽度
    if(ndataSize==-1)
    {
        this->ndataSize=canvas_w;//显示数据个数=画布宽度
    }else{
        this->ndataSize=ndataSize;//显示数据个数
    }

    if(ndataRange==-1){
        this->ndataRange=canvas_h;//显示数据范围=画布高度
    }else{
        this->ndataRange=ndataRange;//显示数据范围
    }
    interval=float(canvas_w)/canvas_w;//区间=画布宽度/显示数据个数
    this->canvas_h=canvas_h;//画布高度

    pro_offset.proportion_v=1;//值比例
    pro_offset.proportion_t=1;//时间比例
    pro_offset.offset_v=-90;//值偏移
    pro_offset.offset_t=0;//时间偏移

    //设置画布样式
    setCanvas();
    //利用滑动条调整显示比例与偏移
//    gui();
}

void DataCollect::dataCollectProc(const std::string winname)
{
    while(1)
    {
        //黑色的背景
        coll=cv::Mat::zeros(canvas_h,canvas_w,CV_8UC3);
//        gui();
        gui(winname);

        setCanvas();

        for (int i = 0; i < data.size(); i++) {
            if (i < DATA_BUFF_ROW_SIZE_MAX) {
                dataCollect(data[i], color_list[i], i, winname);
            }
            else
            {
                std::cout<<"Error: The number of data vectors exceeding the maximum!";
                return;
            }
        }
        data_q_h++;
        imshow(winname,coll);
    }

}

void DataCollect::dataCollectProc(std::vector<float> data,
                                  std::vector<cv::Scalar> color,
                                  std::string winname)
{
    //黑色的背景
    coll=cv::Mat::zeros(canvas_h,canvas_w,CV_8UC3);

    setCanvas();
    gui(winname);

    for (int i = 0; i < data.size(); i++)
    {
        if (i < DATA_BUFF_ROW_SIZE_MAX)
        {
            dataCollect(data[i], color[i], i, winname);
        }
        else
        {
            std::cout<<"Error: The number of data vectors exceeding the maximum!"<<std::endl;
            return;
        }
    }
    data_q_h++;
    imshow(winname,coll);
}

void DataCollect::dataCollectInvProc(float data,cv::Scalar color,const std::string winname)
{
    //黑色的背景
    coll=cv::Mat::zeros(canvas_h,canvas_w,CV_8UC3);

    dataCollect(data, color, 0, winname);
    data_q_h++;
    imshow(winname,coll);
}

void DataCollect::dataCollect(float data,cv::Scalar color,int data_q_index,const std::string winname)
{
    //data_q_h是当前数据写入的索引（数组下标）
    //data_q_h超出范围后归0
    //data_q 就是一个用数组实现的循环队列
    data_q_h=data_q_h%ndataSize;

    data_q[data_q_index][data_q_h]=data;
    //缩放到合适的范围
    data_q[data_q_index][data_q_h]*=(canvas_h/ndataRange);

    char fdata[10]="";
    sprintf(fdata,"%.f",data);
    putText(coll, fdata, cv::Point(canvas_w-35+pro_offset.offset_t, canvas_h-data+pro_offset.offset_v), 1,1,color,1,8);

    //将 data_q 循环队列中的所有数据（点）连接起来，画出
    int p_data;
    for(int j = 0; j < ndataSize - 1; j++)
    {
        p_data=data_q_h-j+canvas_w;
        //画线
        //第2个参数：前一个点（后一个点的数据位置）Point(x,y) x是时间轴，从前各往后 y是数据的大小
        //第3个参数：当前点（当前的数据位置）
        line(coll, cv::Point(interval * (ndataSize - j - 1) + pro_offset.offset_t,
                         canvas_h - data_q[data_q_index][(p_data - 1) % canvas_w] * pro_offset.proportion_v + pro_offset.offset_v),
             cv::Point(interval * (ndataSize - j) + pro_offset.offset_t,
                   canvas_h - data_q[data_q_index][p_data % canvas_w] * pro_offset.proportion_v + pro_offset.offset_v),
             color, 1, 8, 0);
    }
    //    data_q_h++;
}

void DataCollect::gui()
{
    float ra=100.0;

    static int proportion_v_subhalf_red=ra;
    static int proportion_t_subhalf_red=ra;

    static int proportion_v_subhalf_exp=1;
    static int proportion_t_subhalf_exp=1;
//    static int offset_v_subhalf=canvas_h*ra;//  14130
//    static int offset_t_subhalf=canvas_w*ra;

    //设置比例
    cv::namedWindow("gui",cv::WINDOW_AUTOSIZE);
//    cv::createTrackbar("proportion_v_subhalf_red","gui",&proportion_v_subhalf_red,ra,NULL);
//    cv::createTrackbar("proportion_t_subhalf_red","gui",&proportion_t_subhalf_red,ra,NULL);

//    cv::createTrackbar("proportion_v_subhalf_exp","gui",&proportion_v_subhalf_exp,ra,NULL);
//    cv::createTrackbar("proportion_t_subhalf_exp","gui",&proportion_t_subhalf_exp,ra,NULL);

    pro_offset.proportion_v=proportion_v_subhalf_exp*proportion_v_subhalf_red/ra;
    pro_offset.proportion_t=proportion_t_subhalf_exp*ra*proportion_t_subhalf_red/ra;

//    //设置偏移
//    cv::createTrackbar("offset_v+h/2","gui",&offset_v_subhalf,canvas_h*ra*2,NULL);
//    cv::createTrackbar("offset_t+w/2","gui",&offset_t_subhalf,canvas_w*ra*2,NULL);
//    pro_offset.offset_v=offset_v_subhalf-canvas_h*ra;
//    pro_offset.offset_t=offset_t_subhalf-canvas_w*ra;
}

//--------------------------------【on_MouseHandle( )函数】-----------------------------
//		描述：鼠标回调函数，根据不同的鼠标事件进行不同的操作
//-----------------------------------------------------------------------------------------------
void on_MouseHandle(int event, int x, int y, int flags, void* param)
{
    ProOffset_t *offset=(struct ProOffset *)param;

    switch (event)
    {
        //鼠标移动消息
    case cv::EVENT_MOUSEMOVE:
    {
        if (is_changeOffset)//如果是否进行绘制的标识符为真，则记录下长和宽到RECT型变量中
        {
            //设置偏移
            offset->offset_v=offset_v_tmp+y-yold;
            offset->offset_t=offset_t_tmp+x-xold;
        }
    }
    break;

    //左键按下消息
    case cv::EVENT_LBUTTONDOWN:
    {
        is_changeOffset = true;
        xold=x;
        yold=y;
    }
    break;

    //左键抬起消息
    case cv::EVENT_LBUTTONUP:
    {
        is_changeOffset = false;//置标识符为false
        offset_v_tmp=offset->offset_v;
        offset_t_tmp=offset->offset_t;
    }
    break;
    }
}

void DataCollect::gui(const std::string winname)
{
    cv::setMouseCallback(winname,on_MouseHandle,(void*)&pro_offset);

    //设置比例
//    pro_offset.proportion_v=proportion_v_subhalf/100.0;
//    pro_offset.proportion_t=proportion_t_subhalf/100.0;
}

void DataCollect::setCanvas()
{
    int color_gray=100;

    for(int y=0;y<canvas_h;y+=64)
    {
        //横线
        line(coll,cv::Point(pro_offset.offset_t,y+pro_offset.offset_v),
             cv::Point(canvas_w+pro_offset.offset_t,y+pro_offset.offset_v),
             cv::Scalar(color_gray,color_gray,color_gray),1,8,0);
        //横坐标
        char row_coor[10];
        sprintf(row_coor,"%d",y);
        putText(coll,row_coor,
                cv::Point(canvas_w-50+pro_offset.offset_t,canvas_h-y+pro_offset.offset_v),
                1,1,cv::Scalar(color_gray,color_gray,color_gray),1,8);
    }

    for(int x=0;x<canvas_w;x+=50)
    {
        //纵线
        line(coll,cv::Point(x+pro_offset.offset_t,pro_offset.offset_v),
             cv::Point(x+pro_offset.offset_t,canvas_h+pro_offset.offset_v),
             cv::Scalar(color_gray,color_gray,color_gray),1,8,0);
        //纵坐标
        char col_coor[10];
        sprintf(col_coor,"%d",x);
        putText(coll,col_coor,
                cv::Point(canvas_w-x+pro_offset.offset_t,canvas_h-50+pro_offset.offset_v),
                1,1,cv::Scalar(color_gray,color_gray,color_gray),1,8);
    }
}
