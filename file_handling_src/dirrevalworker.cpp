#include "dirrevalworker.h"

DirRevalWorker::DirRevalWorker(FileInfoBD* fi,
                               QThread* threadToMoveObjectsTo)
    : DirManagerWorker(nullptr),
      m_fis(std::vector<FileInfoBD*>()),
      m_threadToMoveObjectsTo(threadToMoveObjectsTo)
{
    m_fis.push_back(fi);
    connect(this, &DirRevalWorker::runTask, this, &DirRevalWorker::run);
}

DirRevalWorker::DirRevalWorker(std::vector<FileInfoBD *> fis,
                               QThread* threadToMoveObjectsTo)
    : DirManagerWorker(nullptr),
      m_fis(fis),
      m_threadToMoveObjectsTo(threadToMoveObjectsTo)
{
    connect(this, &DirRevalWorker::runTask, this, &DirRevalWorker::run);
}

DirRevalWorker::DirRevalWorker()
    : DirManagerWorker(nullptr),
      m_fis(std::vector<FileInfoBD*>()),
      m_threadToMoveObjectsTo(nullptr)
{
    connect(this, &DirRevalWorker::runTask, this, &DirRevalWorker::run);
}

DirRevalWorker::DirRevalWorker(const DirRevalWorker &w)
    : DirManagerWorker(w.parent()),
      m_fis(w.m_fis),
      m_threadToMoveObjectsTo(w.m_threadToMoveObjectsTo)
{
    connect(this, &DirRevalWorker::runTask, this, &DirRevalWorker::run);
}

DirRevalWorker &DirRevalWorker::operator=(const DirRevalWorker &w)
{
    DirManagerWorker::operator=(w);

    this->setParent(w.parent());
    m_fis = w.m_fis;
    m_threadToMoveObjectsTo = w.m_threadToMoveObjectsTo;

    connect(this, &DirRevalWorker::runTask, this, &DirRevalWorker::run);

    return *this;
}

DirRevalWorker::~DirRevalWorker()
{
}

bool DirRevalWorker::blockOtherThreads() const
{
    return true;
}

bool DirRevalWorker::revalidateDirStructureAfterWorkerHasFinished() const
{
    return true;
}

void DirRevalWorker::run()
{
    for(auto* fi: m_fis)
    {
        fi->revalidate();
    }

    if(m_threadToMoveObjectsTo)
    {
        for(auto* dir: m_fis)
        {
            dir->moveToThread_rec(m_threadToMoveObjectsTo);
        }
    }

    emit finished(revalidateDirStructureAfterWorkerHasFinished());
}

void DirRevalWorker::workBeforeLaunchThread()
{
    for(auto* dir: m_fis)
    {
        dir->moveToThread_rec(m_thread);
    }
}
