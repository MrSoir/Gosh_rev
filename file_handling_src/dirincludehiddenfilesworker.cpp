#include "dirincludehiddenfilesworker.h"

DirIncludeHiddenFilesWorker::DirIncludeHiddenFilesWorker(FileInfoBD* root_dir,
                                                         bool include)
    : DirManagerWorker(nullptr),
      m_root_dir(root_dir),
      m_include(include)
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

    emit finished(revalidateDirStructureAfterWorkerHasFinished());
}

