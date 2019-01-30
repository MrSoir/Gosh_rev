#include "copyfiles.h"


COPY_MOVE::CopyFiles::CopyFiles(std::vector<string> pathsToCopy,
                                std::string targetPath,
                                bool deleteAfterCopying)
    : ProgressDialogWorker(STATIC_FUNCTIONS::evaluateFileCount(pathsToCopy)),
      m_paths(pathsToCopy),
      m_tarPath(targetPath),
      m_deleteAfterCopying(deleteAfterCopying)
{    
}

COPY_MOVE::CopyFiles::~CopyFiles()
{
}

void COPY_MOVE::CopyFiles::copyFiles()
{
    std::vector<std::string> validFilePaths;
    for(const std::string& path: m_paths)
    {
        QString tarPath = QString::fromStdString(m_tarPath);
        QString qt_absPath = QString::fromStdString(path);

        QString absTarPath = PATH::genTargetPath(qt_absPath, tarPath);

        tarPath = STATIC_FUNCTIONS::askUserForNoneExistingFilePath(tarPath);

        if(!tarPath.isEmpty())
        {
            validFilePaths.push_back(tarPath.toStdString());
        }else{
            QMessageBox msgBox;
            msgBox.setText("'" + QFileInfo(absTarPath).fileName() + "' will not be copied!");
            msgBox.exec();
        }
    }

    if(validFilePaths.size() > 0)
    {
        auto caller = [=](QString absSourcePath, QString absTarPath, bool isDir, bool* cancel){
            if(cancelled()){
                *cancel = true;
                return false;
            }

            bool success;
            if(isDir)
                success = makeDir(absTarPath);
            else
                success = copyFile(absSourcePath, absTarPath);

            incrementProgress();

            return success;
        };
        auto trees = SOURCE_TARGET::iterateOverEntries(validFilePaths, m_tarPath, caller);
        while(trees.size() > 0)
        {
            auto* tree = trees[trees.size()-1];
            trees.pop_back();
            if(m_deleteAfterCopying)
                deleteFile(tree);
            if(tree)
                delete tree;
        }
    }

    finish();
}

bool COPY_MOVE::CopyFiles::copyFile(const QString &absSourcePath, const QString &absTarPath)
{
    bool success = QFile::copy(absSourcePath, absTarPath);
    return success;
}

bool COPY_MOVE::CopyFiles::makeDir(const QString &tarPath)
{
    return QDir().mkdir(tarPath);
}

void COPY_MOVE::CopyFiles::run()
{
    copyFiles();
}

bool COPY_MOVE::CopyFiles::deleteFile(SOURCE_TARGET::FileTree* tree)
{
    bool total_success = true;

    if(!tree)
        return true;

    // alle subFolder loeschen:
    for(const auto& sub_tree: tree->folds)
    {
        bool success = deleteFile(sub_tree);
        if(!success)
            total_success = false;

    }

    // alle files im folder loeschen:
    for(const auto& file: tree->files)
    {
        const auto& oldTarPath = std::get<0>(file);
        bool success = QFile(QString::fromStdString(oldTarPath)).remove();
        if(!success)
            total_success = false;
    }

    // zum schluss noch den folder loeschen, falls alle subFolder und files erfolgreich geloescht wurden:
    if(total_success)
    {
        bool success = QDir().rmdir(QString::fromStdString(tree->sourcePath));
        if(!success)
            total_success = false;
    }

    return total_success;
}

//---------------------------------------------------------------------------------------

COPY_MOVE::DuplicateFiles::DuplicateFiles(std::vector<string> pathsToDplct)
    : ProgressDialogWorker(STATIC_FUNCTIONS::evaluateFileCount(pathsToDplct)),
      m_paths(pathsToDplct)
{
}

COPY_MOVE::DuplicateFiles::~DuplicateFiles()
{
}

void COPY_MOVE::DuplicateFiles::run()
{
    duplicate();
}

void COPY_MOVE::DuplicateFiles::duplicate()
{
    for(const std::string& path: m_paths)
    {
        if(cancelled())
            break;

        duplicate_hlpr(QString::fromStdString(path));
    }

    finish();
}

void COPY_MOVE::DuplicateFiles::duplicate_hlpr(const QString& absSourcePath)
{
    QFileInfo fi(absSourcePath);

    if(!fi.exists())
        return;

    else if(fi.isDir())
    {
        QString uniqueTargetPath = STATIC_FUNCTIONS::getUniqueFolderPath(absSourcePath);;
        auto caller = [=](QString absSourcePath, QString absTarPath, bool isDir, bool* cancel){
            if(cancelled()){
                *cancel = true;
                return false;
            }

            bool success;
            if(isDir)
                success = CopyFiles::makeDir(absTarPath);
            else
                success = CopyFiles::copyFile(absSourcePath, absTarPath);

            incrementProgress();

            return success;
        };
        auto trees = SOURCE_TARGET::iterateOverEntries(absSourcePath, uniqueTargetPath, caller);
        Q_UNUSED(trees);
    }else{
        QString unique_file_path = STATIC_FUNCTIONS::getUniqueFilePath(absSourcePath);
        CopyFiles::copyFile(absSourcePath, unique_file_path);

        incrementProgress();
    }
}

//---------------------------------------------------------------------------------------

COPY_MOVE::MoveFiles::MoveFiles(std::vector<string> pathsToCopy, string targetPath)
    : ProgressDialogWorker(STATIC_FUNCTIONS::evaluateFileCount(pathsToCopy)),
      m_paths(pathsToCopy),
      m_tarPath(targetPath)
{
}

COPY_MOVE::MoveFiles::~MoveFiles()
{
}

bool COPY_MOVE::MoveFiles::moveEntry(const QString &absSourcePath, const QString &absTarPath)
{
    bool success = QDir().rename(absSourcePath, absTarPath);
    return success;
}

void COPY_MOVE::MoveFiles::run()
{
    moveFiles();
}

void COPY_MOVE::MoveFiles::moveFiles()
{
    std::vector<std::string> validFilePaths;
    for(const std::string& path: m_paths)
    {
        QString tarPath = QString::fromStdString(m_tarPath);
        QString qt_absPath = QString::fromStdString(path);

        QString absTarPath = PATH::genTargetPath(qt_absPath, tarPath);

        tarPath = STATIC_FUNCTIONS::askUserForNoneExistingFilePath(tarPath);

        if(!tarPath.isEmpty())
        {
            validFilePaths.push_back(tarPath.toStdString());
        }else{
            QMessageBox msgBox;
            msgBox.setText("'" + QFileInfo(absTarPath).fileName() + "' will not be moved!");
            msgBox.exec();
        }
    }

    if(validFilePaths.size() > 0)
    {
        auto caller = [=](QString absSourcePath, QString absTarPath, bool isDir, bool* cancel){
            if(cancelled()){
                *cancel = true;
                return false;
            }

            bool success = moveEntry(absSourcePath, absTarPath);
            if(!success)
            {
                if(isDir)
                {
                    success = CopyFiles::makeDir(absTarPath);
                }else{
                    success = CopyFiles::copyFile(absSourcePath, absTarPath);
                }
            }

            incrementProgress();

            return success;
        };
        auto trees = SOURCE_TARGET::iterateOverEntries(validFilePaths, m_tarPath, caller);
        while(trees.size() > 0)
        {
            auto* tree = trees[trees.size()-1];
            trees.pop_back();
            if(tree)
                delete tree;
        }
    }

    finish();
}


//---------------------------------------------------------------------------------------

COPY_MOVE::DeleteFiles::DeleteFiles(std::vector<string> pathsToDelete)
    : ProgressDialogWorker(STATIC_FUNCTIONS::evaluateFileCount(pathsToDelete)),
      m_paths(pathsToDelete)
{
}

COPY_MOVE::DeleteFiles::~DeleteFiles()
{
}

bool COPY_MOVE::DeleteFiles::removeFile(const QString& path)
{
    QFile file(path);
    return file.remove();
}
bool COPY_MOVE::DeleteFiles::removeDir(const QString& path)
{
    QString basePath = PATH::getBasePath(path);
    QFileInfo fi(path);
    return QDir(basePath).rmdir(fi.fileName());
}

void COPY_MOVE::DeleteFiles::run()
{
    deleteFiles();
}

void COPY_MOVE::DeleteFiles::deleteFiles()
{
    for(const auto& path: m_paths)
    {
        QString q_path = QString::fromStdString(path);

        auto caller = [=](QString absSourcePath, QString absTarPath, bool isDir, bool* cancel){
            if(cancelled()){
                *cancel = true;
                return false;
            }

            Q_UNUSED(absTarPath);

            bool success;
            if(isDir)
            {
                success = removeDir(absSourcePath);
            }else{
                success = removeFile(absSourcePath);
            }

            incrementProgress();

            return success;
        };
        SOURCE_TARGET::iterateOverEntries(path, "", caller, false);
    }
}
