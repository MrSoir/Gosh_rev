#ifndef COPYFILES_H
#define COPYFILES_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDir>
#include <QString>
#include <QLineEdit>
#include <QInputDialog>
#include <QMessageBox>

#include <vector>
#include <string>
#include <unordered_set>
#include <functional>

#include "progressdialogworker.h"
#include "staticfunctions_files.h"
#include "sourcetargetiterator.h"

namespace COPY_MOVE
{

    class CopyFiles: public ProgressDialogWorker
    {
        Q_OBJECT
    public:
        CopyFiles(std::vector<std::string> pathsToCopy,
                  std::string targetPath,
                  bool deleteAfterCopying = false);
        virtual ~CopyFiles() override;

        static bool copyFile(const QString& absSourcePath, const QString& absTarPath);
        static bool makeDir(const QString& tarPath);
    public slots:
        void run() override;
    private:
        void copyFiles();

        bool deleteFile(SOURCE_TARGET::FileTree* tree);

        std::vector<std::string> m_paths;
        std::string m_tarPath;

        bool m_deleteAfterCopying;
    };

//    ---------------------------------------------

    class DuplicateFiles: public ProgressDialogWorker
    {
        Q_OBJECT
    public:
        DuplicateFiles(std::vector<std::string> pathsToDplct);
        virtual ~DuplicateFiles() override;

    public slots:
        void run() override;
    private:
        void duplicate();
        void duplicate_hlpr(const QString& absSourcePath);

        std::vector<std::string> m_paths;
    };

//    ---------------------------------------------

    class MoveFiles: public ProgressDialogWorker
    {
        Q_OBJECT
    public:
        MoveFiles(std::vector<std::string> pathsToCopy,
                  std::string targetPath);
        virtual ~MoveFiles() override;

        static bool moveEntry(const QString& absSourcePath, const QString& absTarPath);
    public slots:
        void run() override;
    private:
        void moveFiles();

        std::vector<std::string> m_paths;
        std::string m_tarPath;
    };

//    ---------------------------------------------

    class DeleteFiles: public ProgressDialogWorker
    {
        Q_OBJECT
    public:
        DeleteFiles(std::vector<std::string> pathsToCopy);
        virtual ~DeleteFiles() override;

        static bool removeDir(const QString& path);
        static bool removeFile(const QString& path);
    public slots:
        void run() override;
    private:
        void deleteFiles();

        std::vector<std::string> m_paths;
    };
}

#endif // COPYFILES_H
