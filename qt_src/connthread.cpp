#include "connthread.h"

ConnThread::ConnThread()
{

}

bool connStatus;

void ConnThread::run()
{
    qDebug()<<"conn initializing...";

    connStatus = createConnection();

}
