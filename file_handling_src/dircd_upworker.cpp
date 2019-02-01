#include "dircd_upworker.h"

DirCD_UpWorker::DirCD_UpWorker(FileInfoBD* current_rootDir)
    : DirManagerWorker(nullptr),
      m_current_rootDir(current_rootDir)
{
    connect(this, &DirCD_UpWorker::runTask, this, &DirCD_UpWorker::run);
}

DirCD_UpWorker::~DirCD_UpWorker()
{
}

bool DirCD_UpWorker::blockOtherThreads() const
{
    return true;
}

bool DirCD_UpWorker::revalidateDirStructureAfterWorkerHasFinished() const
{
    return true;
}

void DirCD_UpWorker::run()
{
    if(m_current_rootDir->getParentDir())
    {
        *m_current_rootDir = m_current_rootDir->getParentDir();
    }else{
        std::string absPath = PATH::getBasePath(m_current_rootDir->absPath()).toStdString();
        QFileInfo fi(QString::fromStdString(absPath));
        if(!absPath.empty() && absPath != m_current_rootDir->absPath() &&
                fi.exists() && fi.isDir())
        {
            auto deletFunc = [](FileInfoBD* fi){
                fi->closeAbsParent();
            };

            FileInfoBD* new_root = new FileInfoBD(absPath);
            new_root->elapse();

            STATIC_FUNCTIONS::pointerResetter<FileInfoBD>(m_current_rootDir, new_root, deletFunc);
        }
    }

    emit finished(revalidateDirStructureAfterWorkerHasFinished());
}
