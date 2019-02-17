#ifndef ZIPFILES_H
#define ZIPFILES_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QDebug>
#include <QStringList>

#include <string>
#include <vector>

#include "progressdialogworker.h"
#include "staticfunctions_files.h"
#include "process.h"

namespace ZIP
{
    Process* zipFiles(QVector<QString> filePathsToZip,
                      QString zipFileBasePath,
                      QString tarZipFilePath);
    Process* zipFiles(const std::vector<std::string>& filePathsToZip,
                      const std::string& zipFileBasePath,
                      const std::string& tarZipFilePath);
    Process* zipFiles(const std::vector<std::string>& filePathsToZip,
                      const std::string& tarZipFilePath);
    Process* zipFiles(QVector<QString> filePathsToZip,
                      QString tarZipFilePath);
    QString evalZipBasePath(const QVector<QString>& filePathsToZip);


    Process* unZipFile(const QString& zipFilePath, const QString& tarUnzipPath = QString(""));
    Process* unZipFile(const std::string& zipFilePath, const std::string& tarUnzipPath = "");

//---------------------------------------------------------

    class ZipHelper : public QObject
    {
        Q_OBJECT
    public:
        explicit ZipHelper(QObject* parent = nullptr);

        explicit ZipHelper(const ZipHelper& h);
        explicit ZipHelper();

        virtual ~ZipHelper() override;
    signals:
        void finished();
        void cancelled();
    public slots:
        void cancel();
        virtual void run() = 0;
        void zipProcessedFinished();
    protected:
        void connectAndExecuteProcess();

        bool m_cancelled;

        Process* m_process;
    };

//---------------------------------------------------------

    class ZipFilesHelper : public ZipHelper
    {
        Q_OBJECT
    public:
        explicit ZipFilesHelper(std::string absTarZipFilePath,
                           std::vector<std::string> absSrcPaths,
                           QObject* parent = nullptr);

        explicit ZipFilesHelper(const ZipFilesHelper& h);
        explicit ZipFilesHelper();

        virtual ~ZipFilesHelper() override;
    public slots:
        virtual void run() override;
    private:
        std::string m_absTarZipFilePath;
        std::vector<std::string> m_absSrcPaths;
    };

//---------------------------------------------------------

    class UnZipHelper : public ZipHelper
    {
        Q_OBJECT
    public:
        explicit UnZipHelper(std::string absSrcZipFilePath,
                             std::string absTarZipFilePath,
                             QObject* parent = nullptr);

        explicit UnZipHelper(const UnZipHelper& h);
        explicit UnZipHelper();

        virtual ~UnZipHelper() override;
    public slots:
        virtual void run() override;
    private:
        std::string absSrcZipFilePath;
        std::string absTarZipFilePath;
    };

//---------------------------------------------------------

    class ZipFileBase : public ProgressDialogWorker
    {
        Q_OBJECT
    public:
        explicit ZipFileBase();
        explicit ZipFileBase(const ZipFileBase& zf);

        virtual ~ZipFileBase() override;

    public slots:
        void helperFinished();
        void finish() override;
    protected:
        void connectHelper();
        void disconnectHelper();


        ZipHelper* m_helper = nullptr;
    };

//---------------------------------------------------------

    class ZipFiles: public ZipFileBase
    {
        Q_OBJECT
    public:
        explicit ZipFiles(const std::vector<std::string>& pathsToZip,
                          const std::string& tarPath = "");
        explicit ZipFiles();
        explicit ZipFiles(const ZipFiles& zf);

        virtual ~ZipFiles() override;
    public slots:
        void run() override;
    private:
        void zipFiles();

        std::vector<std::string> m_pathsToZip;
        std::string m_tarPath;
    };

//---------------------------------------------------------

    class UnZipFile: public ZipFileBase
    {
        Q_OBJECT
    public:
        explicit UnZipFile(const std::string& zipFilePath,
                           const std::string& unZipTarPath = "");
        explicit UnZipFile();
        explicit UnZipFile(const UnZipFile& ufz);

        virtual ~UnZipFile() override;
    public slots:
        void run() override;
    private:
        void unZipFile();
        QString getTargetUnZipDirPath();

        std::string m_zipFilePath;
        std::string m_unZipTarDirPath;
    };
}


#endif // ZIPFILES_H
