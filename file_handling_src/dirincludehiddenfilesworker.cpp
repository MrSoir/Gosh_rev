#include "dirincludehiddenfilesworker.h"

DirIncludeHiddenFilesWorker::DirIncludeHiddenFilesWorker(FileInfoBD* root_dir,
                                                         bool include,
                                                         QThread* threadToMoveObjectsTo)
    : DirManagerWorker(nullptr),
      m_root_dir(root_dir),
      m_include(include),
      m_threadToMoveObjectsTo(threadToMoveObjectsTo)
{
    connect(this, &DirIncludeHiddenFilesWorker::runTask, this, &DirIncludeHiddenFilesWorker::run);
}

DirIncludeHiddenFilesWorker::DirIncludeHiddenFilesWorker()
    : DirManagerWorker(nullptr),
      m_root_dir(nullptr),
      m_include(false),
      m_threadToMoveObjectsTo(nullptr)
{
    connect(this, &DirIncludeHiddenFilesWorker::runTask, this, &DirIncludeHiddenFilesWorker::run);
}

DirIncludeHiddenFilesWorker::DirIncludeHiddenFilesWorker(const DirIncludeHiddenFilesWorker &wrkr)
    : DirManagerWorker(wrkr.parent()),
      m_root_dir(wrkr.m_root_dir),
      m_include(wrkr.m_include),
      m_threadToMoveObjectsTo(wrkr.m_threadToMoveObjectsTo)
{
    connect(this, &DirIncludeHiddenFilesWorker::runTask, this, &DirIncludeHiddenFilesWorker::run);
}

DirIncludeHiddenFilesWorker &DirIncludeHiddenFilesWorker::operator=(const DirIncludeHiddenFilesWorker &wrkr)
{
    DirManagerWorker::operator=(wrkr);
    this->setParent(wrkr.parent());
    m_root_dir = wrkr.m_root_dir;
    m_include = wrkr.m_include;
    m_threadToMoveObjectsTo = wrkr.m_threadToMoveObjectsTo;

    connect(this, &DirIncludeHiddenFilesWorker::runTask, this, &DirIncludeHiddenFilesWorker::run);

    return *this;
}
DirIncludeHiddenFilesWorker::~DirIncludeHiddenFilesWorker()
{
}

bool DirIncludeHiddenFilesWorker::blockOtherThreads() const
{
    return true;
}

bool DirIncludeHiddenFilesWorker::revalidateDirStructureAfterWorkerHasFinished() const
{
    return true;
}

void DirIncludeHiddenFilesWorker::run()
{
    if(m_root_dir)
    {
        if(m_include)
        {
            m_root_dir->showHiddenFiles_rec();
        }else{
            m_root_dir->hideHiddenFiles_rec();
        }

        if(m_threadToMoveObjectsTo)
        {
            m_root_dir->moveToThread_rec(m_threadToMoveObjectsTo);
        }
    }else{
        qDebug() << "DirIncludeHiddenFilesWorker::run -> m_root_dir == nullptr -> Default-Constructor!";
    }

    emit finished(revalidateDirStructureAfterWorkerHasFinished());
}

void DirIncludeHiddenFilesWorker::workBeforeLaunchThread()
{
    if(m_root_dir && m_thread)
        m_root_dir->moveToThread_rec(m_thread);
}

