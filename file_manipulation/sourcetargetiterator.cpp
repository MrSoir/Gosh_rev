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

SOURCE_TARGET::FileTree* SOURCE_TARGET::iterateOverDir_hlpr(const QString& sourceDir,
                                                                 const QString& tarDir,
                                                                 caller_func caller,
                                                                 bool preOrder)
{
    bool cancelled = false;
    return iterateOverDir_hlpr(sourceDir, tarDir, caller, &cancelled, preOrder);
}
SOURCE_TARGET::FileTree* SOURCE_TARGET::iterateOverDir_hlpr(const QString& sourceDir,
                                                            const QString& tarDir,
                                                            caller_func caller,
                                                            bool* cancelled,
                                                            bool preOrder)
{
    FileTree* tree = new FileTree();

    tree->sourcePath = sourceDir.toStdString();
    tree->tarPath = tarDir.toStdString();

    QString p_tarDir = tarDir;
    bool currentTreeSuccess = preOrder ? caller(sourceDir, &p_tarDir, true, QFileInfo(sourceDir).isHidden(), cancelled) : true;

    auto entries = ListFiles::getEntriesInDirectory(sourceDir).toVector();
    for(const auto& entry: entries)
    {
        if(*cancelled)
            break;

        bool isDir    = entry.isDir();
        bool isHidden = entry.isHidden();

        QString absSrcPath = entry.absoluteFilePath();

        QString absTarPath = PATH::genTargetPath(absSrcPath, p_tarDir);

        bool success = true;

        if(isDir)
        {
            FileTree* sub_tree = iterateOverDir_hlpr(absSrcPath, absTarPath, caller, cancelled, preOrder);
            tree->folds.push_back(sub_tree);
        }else if(entry.isFile())
        {
            success = caller(absSrcPath, &absTarPath, isDir, isHidden, cancelled);
            tree->files.push_back(std::tuple<std::string,std::string,bool>(absSrcPath.toStdString(), absTarPath.toStdString(), success));
        }

        if(!success)
            currentTreeSuccess = false;
    }

    if(!preOrder)
    {
        bool success = caller(sourceDir, &p_tarDir, true, QFileInfo(sourceDir).isHidden(), cancelled);
        if(!success)
            currentTreeSuccess = false;
    }

    tree->success = currentTreeSuccess;

    return tree;
}

//--------------------------------------------------

std::vector<SOURCE_TARGET::FileTree*> SOURCE_TARGET::iterateOverEntries(const QVector<QFileInfo>& entries,
                                                                        const QString& tarDir,
                                                                        caller_func caller,
                                                                        bool preOrder)
{
    std::vector<SOURCE_TARGET::FileTree*> trees;
    for(const QFileInfo& entry: entries)
    {
        QString basePath;
        if(entry.isDir())
        {
            QString absSrcPath = entry.absoluteFilePath();
            QString absTarPath = PATH::genTargetPath(absSrcPath, tarDir);
            trees.push_back(iterateOverDir_hlpr(absSrcPath, absTarPath, caller, preOrder));
        }else{
            QString absSourcePath = entry.absoluteFilePath();
            QString absTargetPath = PATH::genTargetPath(absSourcePath, tarDir);
            bool cancelled = false;
            FileTree* tree = new FileTree();
            tree->sourcePath = absSourcePath.toStdString();
            tree->tarPath    = absTargetPath.toStdString();
            tree->success    = caller(absSourcePath, &absTargetPath, entry.isDir(), entry.isHidden(), &cancelled);
            trees.push_back(tree);
        }
    }
    return trees;
}
SOURCE_TARGET::FileTree* SOURCE_TARGET::iterateOverEntries(QFileInfo entry,
                                                           const QString& tarPath,
                                                           caller_func caller,
                                                           bool preOrder)
{
    QVector<QFileInfo> v = {entry};
    return iterateOverEntries(v, tarPath, caller, preOrder)[0];
}

//--------------------------------------------------

std::vector<SOURCE_TARGET::FileTree*> SOURCE_TARGET::iterateOverEntries(const QVector<QString> absPaths,
                                                                        const QString& tarPath,
                                                                        caller_func caller,
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
                                                           caller_func caller,
                                                           bool preOrder)
{
    QVector<QFileInfo> v = {QFileInfo(absPath)};
    return iterateOverEntries(v, tarPath, caller, preOrder)[0];
}

//--------------------------------------------------

std::vector<SOURCE_TARGET::FileTree*> SOURCE_TARGET::iterateOverEntries(const std::vector<std::string> absPaths,
                                                                        const std::string& tarPath,
                                                                        caller_func caller,
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
                                                           caller_func caller,
                                                           bool preOrder)
{
    QVector<QFileInfo> v = {QFileInfo(QString::fromStdString(absPath))};
    return iterateOverEntries(v, QString::fromStdString(tarPath), caller, preOrder)[0];
}

