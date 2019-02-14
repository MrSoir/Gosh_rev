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
    Process* zipFiles(const QVector<QString>& filePathsToZip, const QString& tarZipFilePath);
    Process* zipFiles(const std::vector<std::string>& filePathsToZip, const std::string& tarZipFilePath);

    Process* unzipFile(const QString& zipFilePath, const QString& tarUnzipPath = QString(""));
    Process* unzipFile(const std::string& zipFilePath, const std::string& tarUnzipPath = "");

//---------------------------------------------------------

    class ZipFiles: public ProgressDialogWorker
    {
        Q_OBJECT
    public:
        explicit ZipFiles(std::vector<std::string> pathsToZip,
                          std::string tarPath);
        explicit ZipFiles();
        explicit ZipFiles(const ZipFiles& zf);

        ZipFiles& operator=(const ZipFiles& zf);

        virtual ~ZipFiles() override;

    public slots:
        void run() override;
    private:
        void zipFiles();

        std::vector<std::string> m_pathsToZip;
        std::string m_tarPath;
        Process* m_zipProcess;
    };

 //---------------------------------------------------------

    class UnZipFile: public ProgressDialogWorker
    {
        Q_OBJECT
    public:
        explicit UnZipFile(std::string zipFilePath,
                           std::string unZipTarPath = "");
        explicit UnZipFile();
        explicit UnZipFile(const UnZipFile& ufz);

        UnZipFile& operator=(const UnZipFile& uzf);

        virtual ~UnZipFile() override;

    public slots:
        void run() override;
    private:
        void unZipFile();
        QString getTargetZipFilePath();

        std::string m_zipFilePath;
        std::string m_unZipTarPath;
        Process* m_zipProcess;
    };
}


#endif // ZIPFILES_H
