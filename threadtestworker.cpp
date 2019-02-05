#include "threadtestworker.h"

ThreadTestWorker::ThreadTestWorker(const std::string& str,
                                   QObject *parent)
    : QObject(parent),
      m_str(str)
{
    createThread();
}

void ThreadTestWorker::run()
{
    for(int i=0; i < 10; ++i)
    {
        qDebug() << "threaded: " << i;
        this->thread()->sleep(1);
    }
}

void ThreadTestWorker::threadFinished()
{
    qDebug() << "ThreadTestWorker::threadFinished";
}

void ThreadTestWorker::createThread()
{
    QThread* thread = new QThread();
    this->moveToThread(thread);

    connect(thread, &QThread::finished, this, &ThreadTestWorker::threadFinished);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(thread, &QThread::started, this, &ThreadTestWorker::run);

    thread->start();
}
