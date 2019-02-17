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
    class CopyFilesHelper : public QObject
    {
        Q_OBJECT
    public:
        explicit CopyFilesHelper(const std::string& absSrcPath, const std::string& absTarDir, bool deleteAfterCopying, QObject* parent = nullptr);
        explicit CopyFilesHelper(const std::vector<std::string>& absSrcPaths, const std::string& absTarDir, bool deleteAfterCopying, QObject* parent = nullptr);

        explicit CopyFilesHelper(const CopyFilesHelper& h);
        explicit CopyFilesHelper();

        virtual ~CopyFilesHelper() override;

        static bool copyFile(const QString& absSourcePath, const QString& absTarPath);
        static bool makeDir(const QString& tarPath);
    signals:
        void incrementProgress();
        void addProgressUpperBound(unsigned long long);
        void finished();
        void cancelled();
    public slots:
        void cancel();
        void run();
        void receiveUserSelectedString(QString s);
    private:
        bool deleteFile(SOURCE_TARGET::FileTree* tree);

        std::vector<std::string> m_absSrcPaths;
        std::string m_absTarDir;
        unsigned long long m_entriesToCopy;
        bool m_deleteAfterCopying;
        bool m_cancelled;
    };

    //---------------------------------------------------------------

    class CopyFiles: public ProgressDialogWorker
    {
        Q_OBJECT
    public:
        explicit CopyFiles(std::vector<std::string> pathsToCopy,
                  std::string targetPath,
                  bool deleteAfterCopying = false);
        explicit CopyFiles();
        explicit CopyFiles(const CopyFiles& cp);

        virtual ~CopyFiles() override;

    public slots:
        void run() override;
    private:
        void copyFiles();

        std::vector<std::string> m_paths;
        std::string m_tarPath;

        bool m_deleteAfterCopying;
    };

//    ---------------------------------------------

    class DuplicateFiles: public ProgressDialogWorker
    {
        Q_OBJECT
    public:
        explicit DuplicateFiles(std::vector<std::string> pathsToDplct);
        explicit DuplicateFiles();
        explicit DuplicateFiles(const DuplicateFiles& df);

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
        explicit MoveFiles(std::vector<std::string> pathsToCopy,
                           std::string targetPath);
        explicit MoveFiles();
        explicit MoveFiles(const MoveFiles& mf);

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
        explicit DeleteFiles(std::vector<std::string> pathsToCopy);
        explicit DeleteFiles();
        explicit DeleteFiles(const DeleteFiles& df);

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
