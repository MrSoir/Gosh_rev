#include "dirsortworker.h"

DirSortWorker::DirSortWorker(Order order,
                             FileInfoBD* fi,
                             bool sort_recursive,
                             QThread* threadToMoveObjectsTo)
    : DirManagerWorker(nullptr),
      m_order(order),
      m_fis(std::vector<FileInfoBD*>()),
      m_sort_recursive(sort_recursive),
      m_threadToMoveObjectsTo(threadToMoveObjectsTo)
{
    m_fis.push_back(fi);

    connect(this, &DirSortWorker::runTask, this, &DirSortWorker::run);
}

DirSortWorker::DirSortWorker(Order order,
                             std::vector<FileInfoBD*> fis,
                             bool sort_recursive,
                             QThread* threadToMoveObjectsTo)
    : DirManagerWorker(nullptr),
      m_order(order),
      m_fis(fis),
      m_sort_recursive(sort_recursive),
      m_threadToMoveObjectsTo(threadToMoveObjectsTo)
{
    connect(this, &DirSortWorker::runTask, this, &DirSortWorker::run);
}

DirSortWorker::DirSortWorker()
    : DirManagerWorker(nullptr),
      m_order(Order()),
      m_fis(std::vector<FileInfoBD*>()),
      m_sort_recursive(false),
      m_threadToMoveObjectsTo(nullptr)
{
    connect(this, &DirSortWorker::runTask, this, &DirSortWorker::run);
}

DirSortWorker::DirSortWorker(const DirSortWorker &w)
    : DirManagerWorker(w.parent()),
      m_order(w.m_order),
      m_fis(w.m_fis),
      m_sort_recursive(w.m_sort_recursive),
      m_threadToMoveObjectsTo(w.m_threadToMoveObjectsTo)
{
    connect(this, &DirSortWorker::runTask, this, &DirSortWorker::run);
}

DirSortWorker &DirSortWorker::operator=(const DirSortWorker &w)
{
    DirManagerWorker::operator=(w);
    this->setParent(w.parent());
    m_order = w.m_order;
    m_fis = w.m_fis;
    m_sort_recursive = w.m_sort_recursive;
    m_threadToMoveObjectsTo = w.m_threadToMoveObjectsTo;

    connect(this, &DirSortWorker::runTask, this, &DirSortWorker::run);

    return *this;
}

DirSortWorker::~DirSortWorker()
{
    qDebug() << "~DirSortWorker";
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

    if(m_threadToMoveObjectsTo)
    {
        for(auto* dir: m_fis)
        {
            dir->moveToThread_rec(m_threadToMoveObjectsTo);
        }
    }

    emit finished(revalidateDirStructureAfterWorkerHasFinished());
}

void DirSortWorker::workBeforeLaunchThread()
{
    for(auto* dir: m_fis)
    {
        dir->moveToThread_rec(m_thread);
    }
}
