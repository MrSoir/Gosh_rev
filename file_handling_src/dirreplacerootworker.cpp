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
    // stupid, but easy and works stable:
//    FileInfoBD* fi = m_current_rootDir;
//    m_current_rootDir = new FileInfoBD(m_new_root_path);
//    m_current_rootDir->elapse();
//    fi->closeAbsParent();

//    m_current_rootDir->moveAbsParentToThread(m_threadToMoveObjectsTo);

//    emit replaceRoot(m_current_rootDir, false);
//    emit finished(false);


    //-----------------------------------------------------

    // the smart way - unstable:

    bool isSubDir = STATIC_FUNCTIONS::isSubDirectory(m_new_root_path, m_current_rootDir->absPath());

//    auto deleteFunc = [](FileInfoBD* fi){fi->closeAbsParent();}; // da 'delete fi;' nach hinten los gehn wuerde...

    FileInfoBD* new_root;
    bool deleteOldRoot = false;

    if(isSubDir)
    {
        FileInfoBD* sub_dir = findSubDirInDir(m_current_rootDir, m_new_root_path);
        if(sub_dir)
        {
            new_root = sub_dir;
//            *m_current_rootDir = *sub_dir;
        }else{
            /*FileInfoBD**/ new_root = new FileInfoBD(m_new_root_path);
            new_root->elapse();

            deleteOldRoot = true;

//            STATIC_FUNCTIONS::pointerResetter<FileInfoBD>(m_current_rootDir, new_root, deleteFunc);
        }
    }else{
        /*FileInfoBD**/ new_root = new FileInfoBD(m_new_root_path);
        new_root->elapse();

        deleteOldRoot = true;

//        STATIC_FUNCTIONS::pointerResetter<FileInfoBD>(m_current_rootDir, new_root, deleteFunc);
    }

    qDebug() << "--";
    if(m_threadToMoveObjectsTo)
        new_root->moveToThread(m_threadToMoveObjectsTo);
    qDebug() << "--";

    emit replaceRoot(new_root, deleteOldRoot);
    emit finished(false);
}

void DirReplaceRootWorker::workBeforeLaunchThread()
{

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
