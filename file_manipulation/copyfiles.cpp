#include "copyfiles.h"

COPY_MOVE::CopyFilesHelper::CopyFilesHelper(const std::string& absSrcPath,
                                            const std::string& absTarDir,
                                            bool deleteAfterCopying,
                                            QObject* parent)
    : QObject(parent),
      m_absSrcPaths(std::vector<std::string>()),
      m_absTarDir(absTarDir),
      m_entriesToCopy(0),
      m_deleteAfterCopying(deleteAfterCopying),
      m_cancelled(false)
{
    m_absSrcPaths.push_back(absSrcPath);
    m_entriesToCopy = STATIC_FUNCTIONS::evaluateFileCount(m_absSrcPaths);

    connect(this, &CopyFilesHelper::finished,  this, &CopyFilesHelper::deleteLater);
//    connect(this, &CopyFilesHelper::cancelled, this, &CopyFilesHelper::deleteLater);
}
COPY_MOVE::CopyFilesHelper::CopyFilesHelper(const std::vector<std::string>& absSrcPaths,
                                            const std::string& absTarDir,
                                            bool deleteAfterCopying,
                                            QObject* parent)
    : QObject(parent),
      m_absSrcPaths(absSrcPaths),
      m_absTarDir(absTarDir),
      m_entriesToCopy(STATIC_FUNCTIONS::evaluateFileCount(m_absSrcPaths)),
      m_deleteAfterCopying(deleteAfterCopying),
      m_cancelled(false)
{
    connect(this, &CopyFilesHelper::finished,  this, &CopyFilesHelper::deleteLater);
//    connect(this, &CopyFilesHelper::cancelled, this, &CopyFilesHelper::deleteLater);
}

COPY_MOVE::CopyFilesHelper::CopyFilesHelper(const COPY_MOVE::CopyFilesHelper &h)
    : QObject(h.parent()),
      m_absSrcPaths(h.m_absSrcPaths),
      m_absTarDir(h.m_absTarDir),
      m_entriesToCopy(h.m_entriesToCopy),
      m_deleteAfterCopying(h.m_deleteAfterCopying),
      m_cancelled(h.m_cancelled)
{
    connect(this, &CopyFilesHelper::finished,  this, &CopyFilesHelper::deleteLater);
//    connect(this, &CopyFilesHelper::cancelled, this, &CopyFilesHelper::deleteLater);
}

COPY_MOVE::CopyFilesHelper::CopyFilesHelper()
    : QObject(nullptr),
      m_absSrcPaths(std::vector<std::string>()),
      m_absTarDir(""),
      m_entriesToCopy(0),
      m_deleteAfterCopying(false),
      m_cancelled(false)
{
    connect(this, &CopyFilesHelper::finished,  this, &CopyFilesHelper::deleteLater);
//    connect(this, &CopyFilesHelper::cancelled, this, &CopyFilesHelper::deleteLater);
}

COPY_MOVE::CopyFilesHelper::~CopyFilesHelper()
{
    qDebug() << "~CopyFilesHelper";
}

bool COPY_MOVE::CopyFilesHelper::copyFile(const QString &absSourcePath, const QString &absTarPath)
{
    return STATIC_FUNCTIONS::copyFile(absSourcePath, absTarPath);
}

bool COPY_MOVE::CopyFilesHelper::makeDir(const QString &tarPath)
{
    return STATIC_FUNCTIONS::makeDir(tarPath);
}

void COPY_MOVE::CopyFilesHelper::cancel()
{
    m_cancelled = true;
    emit cancelled();
}

void COPY_MOVE::CopyFilesHelper::run()
{
    emit addProgressUpperBound(m_entriesToCopy);

    std::vector<std::string> validSourceFilePaths;
    for(const std::string& absSrcPath: m_absSrcPaths)
    {
        QString q_absSrcPath = QString::fromStdString(absSrcPath);
        QFileInfo fi(q_absSrcPath);
        if(fi.exists())
        {
            validSourceFilePaths.push_back(absSrcPath);
        }
    }
    QString q_absTarDir = QString::fromStdString(m_absTarDir);
    bool executeCopying = true;
    if( !QFileInfo(q_absTarDir).exists() )
    {
        bool success = makeDir(q_absTarDir);
        executeCopying = success;
    }

    if(executeCopying)
    {
        if(validSourceFilePaths.size() > 0)
        {
            auto caller = [=](QString absSourcePath, QString* p_absTarPath, bool isDir, bool isHidden, bool* cancel){
                qDebug() << "copyFile: " << absSourcePath << "  to  " << *p_absTarPath << " cancelled: " << m_cancelled;

                if(m_cancelled){
                    *cancel = true;
                    return false;
                }

                QString absTarPath = *p_absTarPath;

//                thread()->msleep(0);
                thread()->sleep(2);

                Q_UNUSED(isHidden)

                QFileInfo fi(absTarPath);
                if(fi.exists())
                {
                    absTarPath = STATIC_FUNCTIONS::askUserForNoneExistingFilePath(absTarPath);
                    qDebug() << "absTarPath: " << absTarPath;
                    fi = QFileInfo(absTarPath);
                    if(absTarPath.isEmpty() || fi.exists())
                    {
                        return false;
                    }else{
                        *p_absTarPath = absTarPath;
                    }
                }

                bool success;
                if(isDir)
                {
                    success = makeDir(absTarPath);
                }else
                {
                    success = copyFile(absSourcePath, absTarPath);
                }

                emit incrementProgress();

                return success;
            };
            auto trees = SOURCE_TARGET::iterateOverEntries(validSourceFilePaths, m_absTarDir, caller);
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
    }else{
        qDebug() << "CopyFiles::run -> m_absTarDir does not exists and could not be created!: " << q_absTarDir;
    }

    emit finished();
}

void COPY_MOVE::CopyFilesHelper::receiveUserSelectedString(QString s)
{

}

bool COPY_MOVE::CopyFilesHelper::deleteFile(SOURCE_TARGET::FileTree *tree)
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


//---------------------------------------------------------------------------------


COPY_MOVE::CopyFiles::CopyFiles(std::vector<string> pathsToCopy,
                                std::string targetPath,
                                bool deleteAfterCopying)
    : ProgressDialogWorker(STATIC_FUNCTIONS::evaluateFileCount(pathsToCopy)),
      m_paths(pathsToCopy),
      m_tarPath(targetPath),
      m_deleteAfterCopying(deleteAfterCopying)
{    
}

COPY_MOVE::CopyFiles::CopyFiles()
    : ProgressDialogWorker(0),
      m_paths(std::vector<std::string>()),
      m_tarPath(""),
      m_deleteAfterCopying(false)
{
}

COPY_MOVE::CopyFiles::CopyFiles(const COPY_MOVE::CopyFiles &cp)
    : ProgressDialogWorker(cp),
      m_paths(cp.m_paths),
      m_tarPath(cp.m_tarPath),
      m_deleteAfterCopying(cp.m_deleteAfterCopying)
{
}

COPY_MOVE::CopyFiles::~CopyFiles()
{
    qDebug() << "~CopyFiles";
}

void COPY_MOVE::CopyFiles::copyFiles()
{
    CopyFilesHelper* helper = new CopyFilesHelper(m_paths, m_tarPath, m_deleteAfterCopying, this);
    connect(helper, &CopyFilesHelper::incrementProgress,     this, &CopyFiles::incrementProgress);
    connect(helper, &CopyFilesHelper::addProgressUpperBound, this, &CopyFiles::addProgressUpperBound);
    connect(helper, &CopyFilesHelper::finished, this, &CopyFiles::finish);
    connect(this, &CopyFiles::cancelSGNL, helper, &CopyFilesHelper::cancel, Qt::DirectConnection);
    helper->run();
}

void COPY_MOVE::CopyFiles::run()
{
    copyFiles();
}

//---------------------------------------------------------------------------------------

COPY_MOVE::DuplicateFiles::DuplicateFiles(std::vector<string> pathsToDplct)
    : ProgressDialogWorker(STATIC_FUNCTIONS::evaluateFileCount(pathsToDplct)),
      m_paths(pathsToDplct)
{
}

COPY_MOVE::DuplicateFiles::DuplicateFiles()
    : ProgressDialogWorker(0),
      m_paths(std::vector<std::string>())
{
}

COPY_MOVE::DuplicateFiles::DuplicateFiles(const DuplicateFiles &df)
    : ProgressDialogWorker(df),
      m_paths(df.m_paths)
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

    QString absTargetPath;
    if(fi.isDir())
    {
        absTargetPath = STATIC_FUNCTIONS::getUniqueFolderPath(absSourcePath);
    }else
    {
        absTargetPath = STATIC_FUNCTIONS::getUniqueFilePath(absSourcePath);
    }

    CopyFilesHelper* helper = new CopyFilesHelper(absSourcePath.toStdString(), absTargetPath.toStdString(), false);
    connect(helper, &CopyFilesHelper::incrementProgress,     this, &DuplicateFiles::incrementProgress);
    connect(helper, &CopyFilesHelper::addProgressUpperBound, this, &DuplicateFiles::addProgressUpperBound);
    connect(this, &DuplicateFiles::cancelSGNL, helper, &CopyFilesHelper::cancel, Qt::DirectConnection);
    helper->run();
}

//---------------------------------------------------------------------------------------

COPY_MOVE::MoveFiles::MoveFiles(std::vector<std::string> pathsToMove,
                                std::string targetPath)
    : ProgressDialogWorker(pathsToMove.size()), // pathsToCopy.size() und nicht STATIC_FUNCTIONS::evaluateFileCount(pathsToMove), da bei move nicht tief in den ordner iteriert werden muss!
      m_paths(pathsToMove),
      m_tarPath(targetPath)
{
}

COPY_MOVE::MoveFiles::MoveFiles()
    : ProgressDialogWorker(0),
      m_paths(std::vector<std::string>()),
      m_tarPath("")
{
}

COPY_MOVE::MoveFiles::MoveFiles(const COPY_MOVE::MoveFiles &mf)
    : ProgressDialogWorker(mf),
      m_paths(mf.m_paths),
      m_tarPath(mf.m_tarPath)
{
}

COPY_MOVE::MoveFiles::~MoveFiles()
{
    qDebug() << "~MoveFiles";
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
    for(const std::string& absSrcPath: m_paths)
    {
        if(QFileInfo(QString::fromStdString(absSrcPath)).exists())
        {
            validFilePaths.push_back(absSrcPath);
        }else{
            qDebug() << "'" << QString::fromStdString(absSrcPath)  << "' does not exist!";
//            QMessageBox msgBox;
//            msgBox.setText("'" + QString::fromStdString(absSrcPath) + "' does not exist!");
//            msgBox.exec();
        }
    }

    if(validFilePaths.size() > 0)
    {
        for(const auto& absSourcePath: validFilePaths)
        {
            QString absTarDir = QString::fromStdString(m_tarPath);

            QString q_absSourcePath = QString::fromStdString(absSourcePath);

            QString absTarPath = PATH::genTargetPath(q_absSourcePath, absTarDir);

            // checken, ob absTarPath nicht bereits existiert. falls doch, muss der User einen neuen, nicht-existierenden path auswaehlen
            // falls das fehlschlaegt, darf/kann nicht die move-operation durchgefuehrt werden -> executeMoving == false!:
            bool executeMoving = true;
            QFileInfo fi(absTarPath);
            if(fi.exists())
            {
                absTarPath = STATIC_FUNCTIONS::askUserForNoneExistingFilePath(absTarPath);
                fi = QFileInfo(absTarPath);
                if(absTarPath.isEmpty() || fi.exists())
                {
                    executeMoving = false;
                }
            }

            if(executeMoving)
            {
                bool success = moveEntry(q_absSourcePath, absTarPath);
                if(!success)
                {
                    qDebug() << "moveEntry FAILED! - trying to copy!";
                    // if moveEntry didn't work (different partition or other error), then simply copy->past & delete src after copying (slower than moveing, but identical result):
                    CopyFilesHelper* helper = new CopyFilesHelper(q_absSourcePath.toStdString(), absTarPath.toStdString(), true);
                    connect(helper, &CopyFilesHelper::incrementProgress,     this, &MoveFiles::incrementProgress);
                    connect(helper, &CopyFilesHelper::addProgressUpperBound, this, &MoveFiles::addProgressUpperBound);
                    connect(this, &MoveFiles::cancelSGNL, helper, &CopyFilesHelper::cancel, Qt::DirectConnection);
                    helper->run();
                }else{
                    qDebug() << "moveEntry SUCCESSFUL!";
                    incrementProgress();
                }
            }else
            {
                incrementProgress();
            }
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

COPY_MOVE::DeleteFiles::DeleteFiles()
    : ProgressDialogWorker(0),
      m_paths(std::vector<std::string>())
{
}

COPY_MOVE::DeleteFiles::DeleteFiles(const COPY_MOVE::DeleteFiles &df)
    : ProgressDialogWorker(df),
      m_paths(df.m_paths)
{
}

COPY_MOVE::DeleteFiles::~DeleteFiles()
{
    qDebug() << "~DeleteFiles";
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
    finish();
}

void COPY_MOVE::DeleteFiles::deleteFiles()
{
    for(const auto& path: m_paths)
    {
        QString q_path = QString::fromStdString(path);
        qDebug() << "deleting: " << q_path;
        if(QFile(q_path).exists())
        {
            auto caller = [=](QString absSourcePath, QString* absTarPath, bool isDir, bool isHidden, bool* cancel){
                if(cancelled()){
                    *cancel = true;
                    return false;
                }

                Q_UNUSED(isHidden)

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
}

