#include "dircd_upworker.h"

DirCD_UpWorker::DirCD_UpWorker(FileInfoBD* current_rootDir, QThread *threadToMoveObjectsTo)
    : DirManagerWorker(nullptr),
      m_current_rootDir(current_rootDir),
      m_threadToMoveObjectsTo(threadToMoveObjectsTo)
{
    connect(this, &DirCD_UpWorker::runTask, this, &DirCD_UpWorker::run);
}

DirCD_UpWorker::DirCD_UpWorker(const DirCD_UpWorker &dc)
    : DirManagerWorker(dc.parent()),
      m_current_rootDir(dc.m_current_rootDir),
      m_threadToMoveObjectsTo(dc.m_threadToMoveObjectsTo)
{
    connect(this, &DirCD_UpWorker::runTask, this, &DirCD_UpWorker::run);
}

DirCD_UpWorker::DirCD_UpWorker()
    : DirManagerWorker(nullptr),
      m_current_rootDir(nullptr),
      m_threadToMoveObjectsTo(nullptr)
{
    connect(this, &DirCD_UpWorker::runTask, this, &DirCD_UpWorker::run);
}

DirCD_UpWorker &DirCD_UpWorker::operator=(const DirCD_UpWorker &dc)
{
    DirManagerWorker::operator=(dc);
    this->m_current_rootDir = dc.m_current_rootDir;
    this->m_threadToMoveObjectsTo = dc.m_threadToMoveObjectsTo;

    connect(this, &DirCD_UpWorker::runTask, this, &DirCD_UpWorker::run);

    return *this;
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
    if( !m_current_rootDir )
    {
        qDebug() << "\nDirCD_UpWorker::run - m_current_rootDir == nullptr -> DirCD_UpWorker-Default-Constructor!!!\n";
        emit finished(false);
        return;
    }


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
    if(m_current_rootDir)
        m_current_rootDir->moveAbsParentToThread(m_thread);
    else{
        qDebug() << "\nDirCD_UpWorker::workBeforeLaunchThread - m_current_rootDir == nullptr -> DirCD_UpWorker-Default-Constructor!!!\n";
    }
}
