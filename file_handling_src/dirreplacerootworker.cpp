#include "dirreplacerootworker.h"

DirReplaceRootWorker::DirReplaceRootWorker(string root_path,
                                           FileInfoBD* current_rootDir,
                                           QThread* threadToMoveObjectsTo)
    : DirManagerWorker(nullptr),
      m_new_root_path(root_path),
      m_current_rootDir(current_rootDir),
      m_threadToMoveObjectsTo(threadToMoveObjectsTo)
{
    connect(this, &DirReplaceRootWorker::runTask, this, &DirReplaceRootWorker::run);
}

DirReplaceRootWorker::DirReplaceRootWorker(const DirReplaceRootWorker &w)
    : DirManagerWorker(w.parent()),
      m_new_root_path(w.m_new_root_path),
      m_current_rootDir(w.m_current_rootDir),
      m_threadToMoveObjectsTo(w.m_threadToMoveObjectsTo)
{
    connect(this, &DirReplaceRootWorker::runTask, this, &DirReplaceRootWorker::run);
}

DirReplaceRootWorker::DirReplaceRootWorker()
    : DirManagerWorker(nullptr),
      m_new_root_path(""),
      m_current_rootDir(nullptr),
      m_threadToMoveObjectsTo(nullptr)
{
    connect(this, &DirReplaceRootWorker::runTask, this, &DirReplaceRootWorker::run);
}

DirReplaceRootWorker &DirReplaceRootWorker::operator=(const DirReplaceRootWorker &w)
{
    DirManagerWorker::operator=(w);
    this->setParent(w.parent());
    m_new_root_path = w.m_new_root_path;
    m_current_rootDir = w.m_current_rootDir;
    m_threadToMoveObjectsTo = w.m_threadToMoveObjectsTo;

    connect(this, &DirReplaceRootWorker::runTask, this, &DirReplaceRootWorker::run);

    return *this;
}

DirReplaceRootWorker::~DirReplaceRootWorker()
{
    qDebug() << "~DirReplaceRootWorker";
}

bool DirReplaceRootWorker::blockOtherThreads() const
{
    return true;
}

bool DirReplaceRootWorker::revalidateDirStructureAfterWorkerHasFinished() const
{
    return false;
}

void DirReplaceRootWorker::run()
{
    if(m_current_rootDir)
    {
        // the stupid way - but easy and stable:
//        FileInfoBD* fi = m_current_rootDir;
//        m_current_rootDir = new FileInfoBD(m_new_root_path);
//        m_current_rootDir->elapse();
//        fi->closeAbsParent();

//        m_current_rootDir->moveAbsParentToThread(m_threadToMoveObjectsTo);

//        emit replaceRoot(m_current_rootDir, false);
//        emit finished(false);


        //-----------------------------------------------------

//        // the smart way - unstable:

        bool isSubDir = STATIC_FUNCTIONS::isSubDirectory(m_new_root_path, m_current_rootDir->absPath());

        FileInfoBD* new_root;
        bool deleteOldRoot = false;

        if(isSubDir)
        {
            FileInfoBD* sub_dir = findSubDirInDir(m_current_rootDir, m_new_root_path);
            if(sub_dir)
            {
                new_root = sub_dir;
            }else{
                new_root = new FileInfoBD(m_new_root_path);

                deleteOldRoot = true;
            }
        }else{
            bool isParentDir = STATIC_FUNCTIONS::isSubDirectory(m_current_rootDir->absPath(), m_new_root_path);
            if(isParentDir)
            {
                FileInfoBD* par_dir = findParentFromDir(m_current_rootDir, m_new_root_path);
                if(par_dir)
                {
                    new_root = par_dir;
                }else{
                    new_root = new FileInfoBD(m_new_root_path);

                    deleteOldRoot = true;
                }
            }else{
                new_root = new FileInfoBD(m_new_root_path);

                deleteOldRoot = true;
            }
        }

        new_root->elapse();

        if(m_threadToMoveObjectsTo)
            new_root->moveAbsParentToThread(m_threadToMoveObjectsTo);

        emit replaceRoot(new_root, deleteOldRoot);
    }else{
        // m_current_rootDir == nullptr -> eg. DirManager on initialization -> DirManager::m_root == nullptr!:
        FileInfoBD* new_root = new FileInfoBD(m_new_root_path);
        new_root->elapse();
        if(m_threadToMoveObjectsTo)
            new_root->moveToThread_rec(m_threadToMoveObjectsTo);
        emit replaceRoot(new_root, false);
    }
    emit finished(false);
}

void DirReplaceRootWorker::workBeforeLaunchThread()
{
    if(m_current_rootDir && m_thread)
        m_current_rootDir->moveAbsParentToThread(m_thread);
}

FileInfoBD* DirReplaceRootWorker::findSubDirInDir(FileInfoBD *dir, std::string path)
{
    if(dir->absPath() == path)
    {
        return dir;
    }
    for(auto* sub_dir: dir->getSubFolders())
    {
        auto* potential_match = findSubDirInDir(sub_dir, path);
        if(potential_match)
            return potential_match;
    }
    return nullptr;
}

FileInfoBD* DirReplaceRootWorker::findParentFromDir(FileInfoBD *dir, std::string path)
{
    if(dir->absPath() == path)
    {
        return dir;
    }

    bool isParentDir = STATIC_FUNCTIONS::isSubDirectory(dir->absPath(), path);

    if(isParentDir && dir->getParentDir())
    {
        return findParentFromDir(dir->getParentDir(), path);
    }else
        return nullptr;
}
