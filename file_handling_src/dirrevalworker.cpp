#include "dirrevalworker.h"

DirRevalWorker::DirRevalWorker(FileInfoBD* fi)
    : DirManagerWorker(nullptr),
      m_fis(std::vector<FileInfoBD*>())
{
    m_fis.push_back(fi);
    connect(this, &DirRevalWorker::runTask, this, &DirRevalWorker::run);
}

DirRevalWorker::DirRevalWorker(std::vector<FileInfoBD *> fis)
    : DirManagerWorker(nullptr),
      m_fis(fis)
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
    emit finished(revalidateDirStructureAfterWorkerHasFinished());
}
