#include "dirmanagerqueue.h"

DirManagerQueue::DirManagerQueue(QObject *parent)
    : QObject(parent),
      m_workersRunning(0),
      m_workersToRun(std::vector<DirManagerWorker*>()),
      m_closed(false)
{
}

DirManagerQueue::~DirManagerQueue()
{
    qDebug() << "~DirManagerQueue";
}

void DirManagerQueue::addWorker(DirManagerWorker* worker)
{
    if(m_closed)
    {
        worker->deleteLater();
        return;
    }else{
        m_workersToRun.push_back(worker);

        startWorkersInQueue();
    }
}

void DirManagerQueue::workerFinished(bool revalidateDirStructureAfterWorkerHasFinished)
{
    --m_workersRunning;

    if(m_closed)
    {
        if(m_workersRunning <= 0)
            cleanUp();
    }else{
        if(revalidateDirStructureAfterWorkerHasFinished)
        {
            emit revalidateDirStructure();
        }else{
            startWorkersInQueue();
        }
    }
}

void DirManagerQueue::dirStructureRevalidated()
{
    startWorkersInQueue();
}

void DirManagerQueue::cancelWorkers()
{
    std::vector<DirManagerWorker*> workers(m_workersToRun.begin(), m_workersToRun.end());

    emit killWorkers();

    m_workersToRun.clear();

    while(workers.size())
    {
        auto* worker = workers[0];
        workers.erase(workers.begin());
        delete worker;
    }
}

void DirManagerQueue::close()
{
    m_closed = true;

    emit killWorkers();

    if(m_workersRunning <= 0)
    {
        cleanUp();
    }
}

void DirManagerQueue::startWorkersInQueue()
{
    if(m_workersRunning < 0)
        m_workersRunning = 0;

    if(m_closed)
        return;

    while(m_workersToRun.size() > 0)
    {
        if(m_closed)
            return;

       auto* worker = m_workersToRun.front();

       if(worker->blockOtherThreads())
       {
           if(m_workersRunning == 0)
           {
               m_workersToRun.erase(m_workersToRun.begin());
               launchWorker(worker);
               return;
           }else{
               return;
           }
       }else{
           m_workersToRun.erase(m_workersToRun.begin());
           launchWorker(worker);
       }
    }
}

void DirManagerQueue::launchWorker(DirManagerWorker* worker)
{
    if(m_closed)
    {
        if(worker)
            worker->deleteLater();
        return;
    }

    if(worker)
    {
        ++m_workersRunning;
        connect(worker, &DirManagerWorker::finished, this, &DirManagerQueue::workerFinished, Qt::QueuedConnection);
        connect(this, &DirManagerQueue::killWorkers, worker, &DirManagerWorker::cancel, Qt::QueuedConnection);
        worker->start();
    }
}

void DirManagerQueue::cleanUp()
{
    m_workersRunning = 0;
    emit killWorkers();
    while(m_workersToRun.size() > 0)
    {
        auto* worker = m_workersToRun.front();
        m_workersToRun.erase(m_workersToRun.begin());
        delete worker;
//        worker->cancel();
//        worker->deleteLater();
    }
    emit noMoreWorkersRunning();

    this->deleteLater();
}
