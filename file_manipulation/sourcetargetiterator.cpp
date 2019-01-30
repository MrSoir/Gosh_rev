#include "sourcetargetiterator.h"

SOURCE_TARGET::FileTree::FileTree()
    : sourcePath(""),
      tarPath(""),
      success(true),
      files(std::vector<std::tuple<std::string, std::string, bool>>()),
      folds(std::vector<FileTree*>())
{
}

SOURCE_TARGET::FileTree::~FileTree()
{
    while(folds.size() > 0)
    {
        FileTree* sub_tree = folds[folds.size()-1];
        folds.pop_back();
        delete sub_tree;
    }
}


SOURCE_TARGET::FileTree* SOURCE_TARGET::iterateOverEntries_hlpr(const QVector<QFileInfo>& entries,
                                                                const QString& basePath,
                                                                const QString& tarDir,
                                                                std::function<bool(QString,QString,bool,bool*)> caller,
                                                                bool preOrder)
{
    bool cancelled = false;
    return iterateOverEntries_hlpr(entries, basePath, tarDir, caller, &cancelled, preOrder);
}
SOURCE_TARGET::FileTree* SOURCE_TARGET::iterateOverEntries_hlpr(const QVector<QFileInfo>& entries,
                                                                const QString& basePath,
                                                                const QString& tarDir,
                                                                std::function<bool(QString,QString,bool,bool*)> caller,
                                                                bool* cancelled,
                                                                bool preOrder)
{
    FileTree* tree = new FileTree();
    bool currentTreeSuccess = true;

    for(const QFileInfo& entry: entries)
    {
        QString absPath = entry.absoluteFilePath();
        QString absTarPath = PATH::genTargetPath(absPath, basePath, tarDir);

        tree->sourcePath = StaticFunctions::getDir(absPath).toStdString();    // etw. unelegant...
        tree->tarPath    = StaticFunctions::getDir(absTarPath).toStdString(); // etw. unelegant...

        if(*cancelled)
            break;

        bool success = true;

        if(preOrder)
        {
            success = caller(absPath, absTarPath, entry.isDir(), cancelled);
        }

        if(*cancelled)
            break;

        if(entry.isDir())
        {
            FileTree* sub_tree = iterateOverEntries_hlpr(ListFiles::getEntriesInDirectory(entry.absoluteFilePath()).toVector(), basePath, tarDir, caller,cancelled, preOrder);
            tree->folds.push_back(sub_tree);
        }else{
            tree->files.push_back(std::tuple<std::string,std::string,bool>(absPath.toStdString(), absTarPath.toStdString(), success));
        }

        if(!preOrder)
        {
            success = caller(absPath, absTarPath, entry.isDir(), cancelled);
        }


        if(!success)
            currentTreeSuccess = false;

    }

    tree->success = currentTreeSuccess;

    return tree;
}

//--------------------------------------------------

std::vector<SOURCE_TARGET::FileTree*> SOURCE_TARGET::iterateOverEntries(const QVector<QFileInfo>& entries,
                                                                        const QString& tarDir,
                                                                        std::function<bool(QString,QString,bool,bool*)> caller,
                                                                        bool preOrder)
{
    std::vector<SOURCE_TARGET::FileTree*> trees;
    for(const QFileInfo& entry: entries)
    {
        QString basePath;
        if(entry.isDir())
        {
            QString basePath = entry.absoluteFilePath();
            const auto& sub_entries = ListFiles::getEntriesInDirectory(entry).toVector();
            trees.push_back(iterateOverEntries_hlpr(sub_entries, basePath, tarDir, caller, preOrder));
        }else{
            basePath = PATH::getBasePath(entry);
            QVector<QFileInfo> v = {entry};
            trees.push_back(iterateOverEntries_hlpr(v, basePath, tarDir, caller, preOrder));
        }
    }
    return trees;
}
SOURCE_TARGET::FileTree* SOURCE_TARGET::iterateOverEntries(QFileInfo entry,
                                                           const QString& tarPath,
                                                           std::function<bool(QString,QString,bool,bool*)> caller,
                                                           bool preOrder)
{
    QVector<QFileInfo> v = {entry};
    return iterateOverEntries(v, tarPath, caller, preOrder)[0];
}

//--------------------------------------------------

std::vector<SOURCE_TARGET::FileTree*> SOURCE_TARGET::iterateOverEntries(const QVector<QString> absPaths,
                                                                        const QString& tarPath,
                                                                        std::function<bool(QString,QString,bool,bool*)> caller,
                                                                        bool preOrder)
{
    QVector<QFileInfo> v;
    for(const auto& path: absPaths)
    {
        v.push_back(QFileInfo(path));
    }
    return iterateOverEntries(v, tarPath, caller, preOrder);
}
SOURCE_TARGET::FileTree* SOURCE_TARGET::iterateOverEntries(QString absPath,
                                                           const QString& tarPath,
                                                           std::function<bool(QString,QString,bool,bool*)> caller,
                                                           bool preOrder)
{
    QVector<QFileInfo> v = {QFileInfo(absPath)};
    return iterateOverEntries(v, tarPath, caller, preOrder)[0];
}

//--------------------------------------------------

std::vector<SOURCE_TARGET::FileTree*> SOURCE_TARGET::iterateOverEntries(const std::vector<std::string> absPaths,
                                                                        const std::string& tarPath,
                                                                        std::function<bool(QString,QString,bool,bool*)> caller,
                                                                        bool preOrder)
{
    QVector<QFileInfo> v;
    for(const auto& path: absPaths)
    {
        v.push_back(QFileInfo(QString::fromStdString(path)));
    }
    return iterateOverEntries(v, QString::fromStdString(tarPath), caller, preOrder);
}
SOURCE_TARGET::FileTree* SOURCE_TARGET::iterateOverEntries(std::string absPath,
                                                           const std::string& tarPath,
                                                           std::function<bool(QString,QString,bool,bool*)> caller,
                                                           bool preOrder)
{
    QVector<QFileInfo> v = {QFileInfo(QString::fromStdString(absPath))};
    return iterateOverEntries(v, QString::fromStdString(tarPath), caller, preOrder)[0];
}

