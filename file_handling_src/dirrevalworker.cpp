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
