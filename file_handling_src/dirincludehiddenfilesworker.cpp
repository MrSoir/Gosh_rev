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

    emit finished(revalidateDirStructureAfterWorkerHasFinished());
}

void DirIncludeHiddenFilesWorker::workBeforeLaunchThread()
{
    m_root_dir->moveToThread_rec(m_thread);
}

