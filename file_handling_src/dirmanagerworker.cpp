#include "dirmanagerworker.h"

DirManagerWorker::DirManagerWorker(QObject *parent)
    : QObject(parent),
      m_cancelled(false),
      m_thread(new QThread())
{
    m_thread->setObjectName(QString("DIR_MANAGER_WORKER_THREAD-%1").arg(STATIC_FUNCTIONS::genRandomNumberString()));
}

DirManagerWorker::~DirManagerWorker()
{
    qDebug() << "~DirManagerWorker";
}

QThread *DirManagerWorker::getThread()
{
    return m_thread;
}

void DirManagerWorker::cancel()
{
    m_cancelled = true;
    emit cancelled();
}

void DirManagerWorker::start()
{
    qDebug() << " DirManagerWorker::start():"
                "\n   this.thread:  " << this->thread()->objectName()
             << "\n   activeThread: " << QThread::currentThread()->objectName()
             << "\n   m_thread:     " << m_thread->objectName();

    workBeforeLaunchThread();

    this->moveToThread(m_thread);

    connect(this, &DirManagerWorker::finished, m_thread, &QThread::quit);
    connect(m_thread, &QThread::finished, m_thread, &QThread::deleteLater);
    connect(this, &DirManagerWorker::finished, this, &DirManagerWorker::deleteLater);

    m_thread->start();

    emit runTask();
}

void DirManagerWorker::workBeforeLaunchThread()
{
    // in general - do nothin!
}
