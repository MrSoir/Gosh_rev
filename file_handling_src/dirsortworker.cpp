#include "dirsortworker.h"

DirSortWorker::DirSortWorker(Order order,
                             FileInfoBD* fi,
                             bool sort_recursive)
    : DirManagerWorker(nullptr),
      m_order(order),
      m_fis(std::vector<FileInfoBD*>()),
      m_sort_recursive(sort_recursive)
{
    m_fis.push_back(fi);

    connect(this, &DirSortWorker::runTask, this, &DirSortWorker::run);
}

DirSortWorker::DirSortWorker(Order order,
                             std::vector<FileInfoBD*> fis,
                             bool sort_recursive)
    : DirManagerWorker(nullptr),
      m_order(order),
      m_fis(fis),
      m_sort_recursive(sort_recursive)
{
    connect(this, &DirSortWorker::runTask, this, &DirSortWorker::run);
}

DirSortWorker::~DirSortWorker()
{
}

bool DirSortWorker::blockOtherThreads() const
{
    return true;
}

bool DirSortWorker::revalidateDirStructureAfterWorkerHasFinished() const
{
    return true;
}

void DirSortWorker::run()
{
    for(auto* fi: m_fis)
    {
        if(m_sort_recursive)
        {
            fi->sortFolder(m_order);
        }else{
            fi->sortFolder_rec(m_order);
        }
    }

    emit finished(revalidateDirStructureAfterWorkerHasFinished());
}
