#include "direlapseworker.h"

DirElapseWorker::DirElapseWorker(std::vector<FileInfoBD*> dirsToElapse,
                                 bool recursive,
                                 bool collapse,
                                 QThread* threadToMoveObjectsTo)
    : DirManagerWorker(nullptr),
      m_dirsToElapse(dirsToElapse),
      m_recursive(recursive),
      m_collapse(collapse),
      m_useSubThreads(!collapse),
      m_runningThreads(0),
      m_threadToMoveObjectsTo(threadToMoveObjectsTo)
{
    connectSignals();
    STATIC_FUNCTIONS::removeSubDirsIfParentDirIsInContainer(m_dirsToElapse);
}

DirElapseWorker::DirElapseWorker(FileInfoBD *dirToElapse,
                                 bool recursive,
                                 bool collapse,
                                 QThread* threadToMoveObjectsTo)
    : DirManagerWorker(nullptr),
      m_dirsToElapse(std::vector<FileInfoBD*>()),
      m_recursive(recursive),
      m_collapse(collapse),
      m_useSubThreads(!collapse),
      m_runningThreads(0),
      m_threadToMoveObjectsTo(threadToMoveObjectsTo)
{
    m_dirsToElapse.push_back(dirToElapse);
    connectSignals();
}

DirElapseWorker::DirElapseWorker(const DirElapseWorker &de)
    : DirManagerWorker(de.parent()),
      m_dirsToElapse(de.m_dirsToElapse),
      m_recursive(de.m_recursive),
      m_collapse(de.m_collapse),
      m_useSubThreads(de.m_useSubThreads),
      m_runningThreads(de.m_runningThreads),
      m_threadToMoveObjectsTo(de.m_threadToMoveObjectsTo)
{
    connectSignals();
    STATIC_FUNCTIONS::removeSubDirsIfParentDirIsInContainer(m_dirsToElapse);
}

DirElapseWorker &DirElapseWorker::operator=(const DirElapseWorker &de)
{
    DirManagerWorker::operator=(de);
    this->setParent(de.parent());
    m_dirsToElapse = de.m_dirsToElapse;
    m_recursive = de.m_recursive;
    m_collapse = de.m_collapse;
    m_useSubThreads = de.m_useSubThreads;
    m_runningThreads = de.m_runningThreads;
    m_threadToMoveObjectsTo = de.m_threadToMoveObjectsTo;

    connectSignals();
    STATIC_FUNCTIONS::removeSubDirsIfParentDirIsInContainer(m_dirsToElapse);

    return *this;
}

DirElapseWorker::~DirElapseWorker()
{
    qDebug() << "~DirElapseWorker";
}

bool DirElapseWorker::blockOtherThreads() const
{
    return false;
}

bool DirElapseWorker::revalidateDirStructureAfterWorkerHasFinished() const
{
    return true;
}

void DirElapseWorker::run()
{
    for(auto* dir: m_dirsToElapse)
    {
        if(m_collapse)
        {
            collapseHelper(dir);
        }else{
            ++m_runningThreads;

            QThread* thread = new QThread();            
            thread->setObjectName(QString("DIR_ELAPSE_WORKER_THREAD-%1").arg(STATIC_FUNCTIONS::genRandomNumberString()));

            DirElapseWorkerHelper* helper = new DirElapseWorkerHelper(dir,
                                                                      m_recursive,
                                                                      this->thread());
            dir->moveToThread_rec(thread);
            helper->moveToThread(thread);

            connect(helper, &DirElapseWorkerHelper::finished, this, &DirElapseWorker::workerFinished, Qt::QueuedConnection);
            connect(this, &DirElapseWorker::killHelpers, helper, &DirElapseWorkerHelper::cancel, Qt::DirectConnection);

            connect(helper, &DirElapseWorkerHelper::finished, thread, &QThread::quit);
            connect(thread, &QThread::finished, thread, &QThread::deleteLater);
            connect(thread, &QThread::started, helper, &DirElapseWorkerHelper::run);

            thread->start();

            if(m_cancelled)
                break;
        }
    }
    if( !m_useSubThreads )
    {
        moveDirsToCallerThread();
        emit finished(revalidateDirStructureAfterWorkerHasFinished());
    }
}

void DirElapseWorker::moveDirsToCallerThread()
{
    if( !m_threadToMoveObjectsTo )
        return;

    for(auto* dir: m_dirsToElapse)
    {
        dir->moveToThread_rec(m_threadToMoveObjectsTo);
    }
}

void DirElapseWorker::workerFinished(FileInfoBD* dir)
{
    Q_UNUSED(dir)
    --m_runningThreads;

    if(m_runningThreads <= 0)
    {
        moveDirsToCallerThread();
        emit finished(revalidateDirStructureAfterWorkerHasFinished());
    }
}

void DirElapseWorker::workBeforeLaunchThread()
{
    for(auto* dir: m_dirsToElapse)
    {
        dir->moveToThread_rec(m_thread);
    }
}

void DirElapseWorker::connectSignals()
{
    connect(this, &DirElapseWorker::cancelled, this, &DirElapseWorker::killHelpers, Qt::QueuedConnection);
    connect(this, &DirElapseWorker::runTask, this, &DirElapseWorker::run);
}

void DirElapseWorker::collapseHelper(FileInfoBD *dir)
{
    dir->collapse();

    if(m_cancelled)
        return;

    if(m_recursive)
    {
        for(auto* sub_dir: dir->getSubFolders())
        {
            if(m_cancelled)
                return;

            collapseHelper(sub_dir);
        }
    }
}


//----------------------------------------------------------------------------------------



DirElapseWorkerHelper::DirElapseWorkerHelper(FileInfoBD* dir,
                                             bool recursive,
                                             QThread* threadToMoveObjectsTo)
    : QObject(nullptr),
      m_dir(dir),
      m_recursive(recursive),
      m_cancelled(false),
      m_threadToMoveObjectsTo(threadToMoveObjectsTo)
{
    connect(this, &DirElapseWorkerHelper::finished, this, &DirElapseWorkerHelper::deleteLater);
}

DirElapseWorkerHelper::DirElapseWorkerHelper()
    : QObject(nullptr),
      m_dir(nullptr),
      m_recursive(false),
      m_cancelled(false),
      m_threadToMoveObjectsTo(nullptr)
{
    connect(this, &DirElapseWorkerHelper::finished, this, &DirElapseWorkerHelper::deleteLater);
}

DirElapseWorkerHelper &DirElapseWorkerHelper::operator=(const DirElapseWorkerHelper &hlpr)
{
    m_dir = hlpr.m_dir;
    m_recursive = hlpr.m_recursive;
    m_cancelled = hlpr.m_cancelled;
    m_threadToMoveObjectsTo = hlpr.m_threadToMoveObjectsTo;

    connect(this, &DirElapseWorkerHelper::finished, this, &DirElapseWorkerHelper::deleteLater);

    return *this;
}

DirElapseWorkerHelper::~DirElapseWorkerHelper()
{
//    qDebug() << "~DirElapseWorkerHelper";
}

void DirElapseWorkerHelper::run()
{
    elapseHelper(m_dir);

    if(m_threadToMoveObjectsTo)
        m_dir->moveToThread_rec(m_threadToMoveObjectsTo);

    emit finished(m_dir);
}

void DirElapseWorkerHelper::cancel()
{
    m_cancelled = true;
}

void DirElapseWorkerHelper::elapseHelper(FileInfoBD* dir)
{
    if(m_cancelled)
        return;

    dir->elapse();

    if(m_cancelled)
        return;

    if(m_recursive)
    {
        for(auto* sub_fold: dir->getSubFolders())
        {
            elapseHelper(sub_fold);
            if(m_cancelled)
                break;
        }
    }
}
