#include "dirreplacerootworker.h"

DirReplaceRootWorker::DirReplaceRootWorker(string root_path,
                                           FileInfoBD* current_rootDir)
    : DirManagerWorker(nullptr),
      m_new_root_path(root_path),
      m_current_rootDir(current_rootDir)
{
    connect(this, &DirReplaceRootWorker::runTask, this, &DirReplaceRootWorker::run);
}

DirReplaceRootWorker::~DirReplaceRootWorker()
{
}

bool DirReplaceRootWorker::blockOtherThreads() const
{
    return true;
}

bool DirReplaceRootWorker::revalidateDirStructureAfterWorkerHasFinished() const
{
    return true;
}

void DirReplaceRootWorker::run()
{
    bool isSubDir = STATIC_FUNCTIONS::isSubDirectory(m_new_root_path, m_current_rootDir->absPath());

    auto deleteFunc = [](FileInfoBD* fi){fi->closeAbsParent();}; // da 'delete fi;' nach hinten los gehn wuerde...

    if(isSubDir)
    {
        FileInfoBD* sub_dir = findSubDirInDir(m_current_rootDir, m_new_root_path);
        if(sub_dir)
        {
            *m_current_rootDir = *sub_dir;
        }else{
            FileInfoBD* new_root = new FileInfoBD(m_new_root_path);
            new_root->elapse();

            STATIC_FUNCTIONS::pointerResetter<FileInfoBD>(m_current_rootDir, new_root, deleteFunc);
        }
    }else{
        FileInfoBD* new_root = new FileInfoBD(m_new_root_path);
        new_root->elapse();

        STATIC_FUNCTIONS::pointerResetter<FileInfoBD>(m_current_rootDir, new_root, deleteFunc);
    }

    if( !m_current_rootDir->isElapsed() )
        m_current_rootDir->elapse();

    emit finished(revalidateDirStructureAfterWorkerHasFinished());
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
