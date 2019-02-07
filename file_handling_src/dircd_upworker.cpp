#include "dircd_upworker.h"

DirCD_UpWorker::DirCD_UpWorker(FileInfoBD* current_rootDir, QThread *threadToMoveObjectsTo)
    : DirManagerWorker(nullptr),
      m_current_rootDir(current_rootDir),
      m_threadToMoveObjectsTo(threadToMoveObjectsTo)
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
        FileInfoBD* new_root = m_current_rootDir->getParentDir();
        bool deleteOldRoot = false;

        if(m_threadToMoveObjectsTo)
        {
            new_root->moveAbsParentToThread(m_threadToMoveObjectsTo);
        }

        emit replaceRoot(new_root, deleteOldRoot);
        emit finished(false);
        return;
    }else{
        std::string absPath = PATH::getBasePath(m_current_rootDir->absPath()).toStdString();

        QFileInfo fi(QString::fromStdString(absPath));

        if(!absPath.empty() && absPath != m_current_rootDir->absPath() &&
                fi.exists() && fi.isDir())
        {

            FileInfoBD* new_root = new FileInfoBD(absPath);
            new_root->elapse();

            bool deleteOldRoot = true;

            if(m_threadToMoveObjectsTo)
            {
                new_root->moveAbsParentToThread(m_threadToMoveObjectsTo);
                m_current_rootDir->moveAbsParentToThread(m_threadToMoveObjectsTo);
            }

            emit replaceRoot(new_root, deleteOldRoot);
            emit finished(false);
            return;
        }
    }

    emit finished(false);
}

void DirCD_UpWorker::workBeforeLaunchThread()
{
    m_current_rootDir->moveAbsParentToThread(m_thread);
}
