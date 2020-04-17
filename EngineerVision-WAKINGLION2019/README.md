# 目录
[TOC]

# 摘要

# 开发环境
平台：ubuntu16.04
<br>框架：opencv3.4.1  tensorflow
<br>语言：c++  python
<br>IDE：qt/vscode
<br>项目管理平台：gitee
<br>数学基础：微积分，线代，概率论
<br>通信：串口
<br>[其它研究方向：SLAM PCL Halcon ROS](https://mp.weixin.qq.com/s/hkhWZjWMATwWf9TojTJxTQ)

# 开发环境搭建
**平台：**
<br>[ubuntu16.04](http://www.ubuntu.org.cn/download/desktop)

**框架：**
<br>[opencv3.4.1](https://github.com/opencv/opencv/tree/3.4.1)
<br>[opencv3.4.1](https://zhuanlan.zhihu.com/mengcius-opencvbase)
			 
**语言：**
<br>c++ 
<br>http://www.cplusplus.com/doc/tutorial/（英文）
<br>python
<br>https://stackoverflow.com/（英文）

**IDE**
<br>vscode
<br>https://code.visualstudio.com/ 
<br>https://code.visualstudio.com/docs/languages/cpp（英文）

**项目管理平台：**
<br>git
<br>https://git-scm.com/（英文）
<br>http://rogerdudler.github.io/git-guide/index.zh.html（中文）

**数学基础：**
<br>线代
<br>https://m.bilibili.com/video/av6731067.html

**其它研究方向：**
<br>SLAM PCL Halcon ROS
<br>（https://mp.weixin.qq.com/s/hkhWZjWMATwWf9TojTJxTQ）

# 框架
**简介**

**代码文件说明**

|文件|类|解释|详细说明|
|----|-------|----|--------|
|Preprocessing|Preprocessing|预处理|	对从相机获取的图片进行预先的处理，如二值化、滤波等，方便后续信息的提取，如目标的位置||
||AdaptiveThreshold|二值化|能根据光照自动调整阈值二值化方法||
|PackData|PackData|打包|将控制数据的结构图转成字符型数据，以便串口发送||
||UnpackData|解包|接收主控板的信号，包括控制模式（比如人工操作/开启自动辅助瞄准/神符击打，由操作手控制），敌方的颜色||
|Protocol|协议|串口相关协议，包括crc8和crc16校验||
|Serial||串口|打开串口，配置串口，发送数据||
|Tool||工具类|工具类，包括查重，命名，算距离等||
|AngleSolver||角度解算|目标角度解析，根据图片的坐标算出真实的角度||
|ChestDetector|ChestDetector|弹药箱检测||
|main||管理多线程|用于获取图像、处理图像检测、信号发关接收||
|IndVideoCapture|视频处理类|重写的视频处理类，有调整分辨率/FPS/曝光度等功能（暂时先收集资料）||
|Settings|设置|载入配置文件，具体参数见下方“程序调试-配置参数”|
|Gui|Gui|调试界面模块|可视化界面||
||DataCollection|数据波形|方便查看实时系统的历史数据，对数据的变化有一个直观的理解||
|TargetDetection|目标检测|提供检测的基本功能，如边缘检测，特征提取等||

- 逻辑图
```
graph TD
    start[开始] --> is_auto_mode{带文本的菱形}
    is_auto_mode -- YES --> A
    is_auto_mode -- NO --> start
    A --> B
    B --> C
    C --> start
```

```flow
st=>start: Start:>https://www.zybuluo.com
io=>inputoutput: verification
op=>operation: Your Operation
cond=>condition: Yes or No?
sub=>subroutine: Your Subroutine
e=>end

st->io->op->cond
cond(yes)->e
cond(no)->sub->io
```

**自动夹取弹药箱**
- 简介：检测出3维位置，控制机器人底盘，发送夹取信号

# 命名规范
一个合理的命名规范能大大提升源代码的可读性和软件的可维护性

**命名风格**

||风格|例子|
|-|---|----|
|文件名|大驼峰|AngleSolver.hpp|
|类名|大驼峰|class AngleSolver{}|
|方法（函数）|小驼峰|`void setTargetSize(double width, double height);`<br>`void setCameraParam(const cv::Mat & camera_matrix, const cv::Mat &dist_coeff);`<br>方法规范如下表：方法常用前缀|
|变量|小写+下划线|`cv::Mat cam_matrix;`<br>`cv::Mat distortion_coeff;`<br>`double width_target;`<br>`double height_target;`|
|宏|大写+下划线|`#define TRUNC_ABS(a) ((a) > 0 ? (a) : 0);`|

**方法常用前缀**

|方法|说明|
|----|----|
|initXX()|初始化相关方法,使用init为前缀标识，如初始化布局initView()|
|isXX()|checkXX()方法返回值为boolean型的请使用is或check为前缀标识|
|getXX()|返回某个值的方法，使用get为前缀标识|
|processXX()<br>xXProc()|对数据进行处理的方法，使用process为前缀标识，或使用Proc为后缀标识|
|displayXX()|弹出提示框和提示信息，使用display为前缀标识|
|saveXX()|与保存数据相关的，使用save为前缀标识|
|resetXX()|对数据重组的，使用reset前缀标识|
|clearXX()|清除数据相关的|
|removeXXX()|清除数据相关的|
|drawingXXX()|绘制数据或效果相关的，使用drawing前缀标识|

**局部变量前缀规范**

|前缀|说明|例子|意义|
|----|----|----|----|
|i|整型|int i_ miss_detection_cnt;|使用整型变量，用于检测丢失的计数|
|f|浮点数|float f_pitch;|pitch角|
|b|布尔|bool b_is_lost;|丢失目标标志|
|ch|字符|char ch_ data;	|
|str|字符串|char *str_ config_file;|配置文件名称，为字符串|
|p|指针|char *p_data;|打包的数据|
|a|数组|||
|inst|类的实例|||
注：对于某些用途简单明了的局部变量，也可以使用简化的方式，如：i, j, k, x, y, z ....

**作用域前缀规范**

|前缀|说明|例子|
|g_ |全局变量（global）|g_prd_iex|
|无|局部变量	||
|m_|类的成员变量（member）||

宗旨：尽可能用最少的字符而又能完整地表达标识符的含义**（除非该缩写是约定俗成的，否则不要用缩写）**，好的命名应直观可拼读，可望文知意，用词准确，常用例子如下：
- 英文缩写原则：
1. 较短的单词可通过去掉“元音”形成缩写
2. 较长的单词可取单词的头几个字母形成缩写
3. 此外还有一些约定成俗的英文单词缩写

|变量名|意义|缩写|全称|
|------|----|----|----|
|src_img|源图片/未经处理的图片|src_img|sourse_image|
|pc_recv_mesg|pc端接收的数据|recv_mesg|receive_message|
|hand_writing_idx|手写字的索引|idx|index|
|led_num_flag|数码管数字标志|num|number|
|avg_gray|平均灰度值|avg|average|
|buf_size|串口缓冲区大小|buf|buffer|
|gimbal_ctrl_t|云台控制数据的结构体|ctrl|control|
|err_info|错误信息|err_info|error_infomation|
|doc|略|doc|document|
|del||delete|delete|
|esc||esc|escape|
|inc||inc|increment|
|init||init|initial|
|len||len|length|
|lib||lib|library|
|pwd||pwd|password|
|pos||pos|position|
|srv||srv|server|
|tmp||tmp|temp|
|wnd||wnd|window|

# 注释规范

|||
|------|--------|
|代码块|统一规范<br>`//-----------------------------------【标题】----------------<br>//  描述：<br>//----------------------------------------------------------------- 
|方法|统一规范<br>`/**` <br> `* @brief 这里书写该方法的简介，包括其作用等`<br> `* @param 第一个参数说明<br> * @param第二个参数说明`<br> `* @return 返回值说明`<br> `* @author  参与开发人员`<br> `* @date   2018.11.7`<br> `*/`
|宏定义|例： #define SHOW_IMAGE //如果注释掉则不显示图片，有利于加快运行速度|
|成员变量|如果变量名不能很好的显示其含义或有特殊用途，就需要注释|
|其它|不容易理解的逻辑部分|

# 程序调试
使用**宏定义**开启或关闭调试功能，例：
```
#define SHOW_IMAGE  //如果注释掉则不显示图片，有利于加快运行速度
#define COUT_LOG    //如果注释则不显示终端打印的数据
```

**GUI界面**
<br>例：上一年小符调试界面
![上一年小符调试界面](https://gitee.com/rm_ai_vision/InfantryVision-WAKINGLION2019/blob/dev/test_pic/rune_gui.png)

**数据波形**
<br>方便查看实时系统的历史数据，对数据的变化有一个直观的理解

**配置参数**
<br>配置参数使用param_config.xml统一保存管理，使用Setting.hpp进行读取，为了方便调试，需要做一个按键自动保存参数的功能
