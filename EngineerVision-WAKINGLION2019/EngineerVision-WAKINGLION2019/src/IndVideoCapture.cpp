#include "IndVideoCapture.h"

void IndVideoCapture::collecting()
{
    while(1)
    {
        ProduceItem(&img_repository);
    }
}

void IndVideoCapture::ProduceItem()
{
    ProduceItem(&img_repository);
}

void IndVideoCapture::ConsumeItem(cv::Mat &img)
{
    ConsumeItem(&img_repository,img);
}

void IndVideoCapture::ProduceItem(ItemRepository *ir)
{
    std::unique_lock<std::mutex> lock(ir->mtx);
    // 缓冲区为满, 等待信号量
    while(((ir->write_position + 1) % kItemRepositorySize)
        == ir->read_position)
    {
        (ir->repo_not_full).wait(lock); // 生产者等待"产品库缓冲区不为满"这一条件发生.
    }

    read((ir->item_buffer)[ir->write_position]); // 写入产品.

    (ir->write_position)++; // 写入位置后移.

    if (ir->write_position == kItemRepositorySize) // 写入位置若是在队列最后则重新设置为初始位置.
        ir->write_position = 0;

    (ir->repo_not_empty).notify_all(); // 通知消费者产品库不为空.
    lock.unlock(); // 解锁.
}

void IndVideoCapture::ConsumeItem(ItemRepository *ir, cv::Mat &img_show)
{
    std::unique_lock<std::mutex> lock(ir->mtx);
    // 缓冲区为空, 等待信号量
    while(ir->write_position == ir->read_position)
    {
        (ir->repo_not_empty).wait(lock); // 消费者等待"产品库缓冲区不为空"这一条件发生.
    }

    img_show = (ir->item_buffer)[ir->read_position]; // 读取某一产品
//    cv::imshow("img_show",img_show);
//    cv::waitKey(10);

    (ir->read_position)++; // 读取位置后移

    if (ir->read_position >= kItemRepositorySize) // 读取位置若移到最后，则重新置位.
        ir->read_position = 0;

    (ir->repo_not_full).notify_all(); // 通知消费者产品库不为满.
    lock.unlock(); // 解锁.

    //return img_show; // 返回产品.
}

bool IndVideoCapture::open(const char *filename,
                           int width,
                           int height,
                           int exp_t)
{
    open_dev(filename);
    setVideoFormat(width, height, 1);
    setExposureTime(false,exp_t);
    startStream();
    info();
    return true;
}

void IndVideoCapture::setFormat()
{
    changeVideoFormat(1280, 720, 1);
    setExposureTime(0, 50);
    info();
}
