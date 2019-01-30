#ifndef SOURCETARGETITERATOR_H
#define SOURCETARGETITERATOR_H

#include <QString>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDir>
#include <QVector>
#include <QDebug>

#include <functional>
#include <string>
#include <vector>
#include <utility>
#include <tuple>

#include "file_handling_src/listfiles.h"
#include "pathmanipulation.h"
#include "staticfunctions.h"
#include "staticfunctions_files.h"

namespace SOURCE_TARGET
{

    class FileTree
    {
    public:
        FileTree();
        ~FileTree();
        std::string sourcePath;
        std::string tarPath;
        bool success;
        std::vector<std::tuple<std::string, std::string, bool>> files; // tuple: absSourcePath, absTargetPath, successfullyPerformedCaller
        std::vector<FileTree*> folds;
    };

    // bool preOrder: erstmal den caller aufrufen, danach weiter den tree ablaufen
    // bool postOrder = !preOrder: erst den tree weiter tiefer ablaufen, danach erst den caller aufrufen -> fuer delteFolderRecursive wichtig: von unten nach oben loeschen!

    FileTree* iterateOverEntries_hlpr(const QVector<QFileInfo>& entries,
                                      const QString& basePath,
                                      const QString& tarDir,
                                      std::function<bool(QString,QString,bool,bool*)> caller,
                                      bool preOrder = true);
    FileTree* iterateOverEntries_hlpr(const QVector<QFileInfo>& entries,
                                      const QString& basePath,
                                      const QString& tarDir,
                                      std::function<bool(QString,QString,bool,bool*)> caller,
                                      bool* cancelled,
                                      bool preOrder = true);

    //--------------------------------------------------

    std::vector<FileTree*> iterateOverEntries(const QVector<QFileInfo>& entries,
                                              const QString& tarDir,
                                              std::function<bool(QString,QString,bool,bool*)> caller,
                                              bool preOrder = true);
    FileTree* iterateOverEntries(QFileInfo entry,
                                 const QString& tarPath,
                                 std::function<bool(QString,QString,bool,bool*)> caller,
                                 bool preOrder = true);

    //--------------------------------------------------

    std::vector<FileTree*> iterateOverEntries(const QVector<QString> absPaths,
                                              const QString& tarPath,
                                              std::function<bool(QString,QString,bool,bool*)> caller,
                                              bool preOrder = true);
    FileTree* iterateOverEntries(QString absPath,
                                 const QString& tarPath,
                                 std::function<bool(QString,QString,bool,bool*)> caller,
                                 bool preOrder = true);

    //--------------------------------------------------

    std::vector<FileTree*> iterateOverEntries(const std::vector<std::string> absPaths,
                                              const std::string& tarPath,
                                              std::function<bool(QString,QString,bool,bool*)> caller,
                                              bool preOrder = true);
    FileTree* iterateOverEntries(std::string absPath,
                                 const std::string& tarPath,
                                 std::function<bool(QString,QString,bool,bool*)> caller,
                                 bool preOrder = true);
}

#endif // SOURCETARGETITERATOR_H
