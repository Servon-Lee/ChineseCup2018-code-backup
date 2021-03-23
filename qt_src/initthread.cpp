#include "initthread.h"

initThread::initThread()
{

}

Detector *detector;

void initThread::run()
{
    qDebug()<<"initializing...";

    std::string cfg = "yolo-voc-hat.cfg";
    std::string weights = "yolo-voc-hat.weights";
    detector = new Detector(cfg, weights, 0);

}
