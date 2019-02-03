#ifndef SATATICFUNCTIONS_FILES_H
#define SATATICFUNCTIONS_FILES_H

#include <QString>
#include <QVector>
#include <QFileInfo>
#include <QFileInfoList>
#include <QStringList>
#include <QDebug>
#include <QObject>
#include <QSysInfo>
#include <QMimeDatabase>
#include <QMimeData>
#include <QMimeType>

#include <vector>
#include <string>
#include <unordered_set>
#include <functional>
#include <stdlib.h> // rand

#include "pathmanipulation.h"
#include "staticfunctions.h"
#include "process.h"

class FileInfoBD;

//----------------------------------------------------------------------------------


enum OS
{
    LINUX,
    WINDOWS,
    MACOS,
    UNKNOWN
};


//----------------------------------------------------------------------------------


namespace STATIC_FUNCTIONS
{
    static QMimeDatabase MIME_DATA_BASE;

//----------------------------------------------------------------------------------

    QString getResourceDir();
    QString getTempResourcesDir();

//----------------------------------------------------------------------------------

    unsigned long long evaluateFileCount(const std::vector<std::string>& paths);

    // askUserForNoneExistingFilePath -> name ist irrefuehrend: path wird gegeben -> falls path existiert, wird lediglich vom user ein anderer fileName gefragt.
    // Directory des absFilePath bleibt erhalten!
    // falls ein Directory als path uebergeben wird, wird vom user ein directory-name verlangt, der noch nicht existiert, d.h.: basePath(path) + userDirName
    QString askUserForNoneExistingFilePath(const QString& path, std::function<bool(const QString& fileName)> fileNameValidator = [](const QString& fileName){Q_UNUSED(fileName) return true;});

    QString genStringGetterDialog(const QString &headline,
                                  const QString &message,
                                  const QString& initInput,
                                  std::function<bool (const QString &)> stringValidator = [](const QString& s){Q_UNUSED(s)return true;});

    Process* execPythonScript(const QString& scriptPath, const QVector<QString>& args, bool waitForFinished = true);
    Process* execPythonScript(const std::string& scriptPath, const std::vector<std::string>& args, bool waitForFinished = true);

    Process* execCommand(const QString& program, const QVector<QString>& args, bool waitForFinished = true);
    Process* execCommand(const std::string& program, const std::vector<std::string>& args, bool waitForFinished = true);

//----------------------------------------------------------------------------------

    QPixmap getPixmapFromPDF(const QString& path, QSize imageSize);
    QPixmap getPixmapFromPDF(const std::string& path, QSize imageSize);

//----------------------------------------------------------------------------------

    QString getUniqueFilePath(const QString& absFilePath);
    QString getUniqueFilePathInDir(const QString& dir, const QString& fileExtension);
    QString getUniqueRandomFilePathInDir(const QString& dir, const QString& fileExtension);
    QString getUniqueFolderPath(const QString& absFolderPath);
    QString getUniqueFolderPathInDir(const QString& dir);
    QString getUniqueRandomFolderPathInDir(const QString& dir);

    //-----

    QString getUniqueFilePathInDir_hlpr(const QString& dir, const QString& fileBaseName, const QString& fileExtension);
    QString getUniqueFolderPathInDir_hlpr(const QString& dir, const QString& dirName);

    QString genRandomFileBaseName();

//----------------------------------------------------------------------------------

    OS LOCAL_OS();

//----------------------------------------------------------------------------------

    void openTerminal(QString sourceDir);

//----------------------------------------------------------------------------------

    bool isSubDirectory(const std::string& potentialSubFile, const std::string& potentialParentFolder);
    bool isSubDirectory(const QString& potentialSubFile, const QString& potentialParentFolder);
    bool isSubDirectory(const QFileInfo& potentialSubFile, const QFileInfo& potentialParentFolder);

//----------------------------------------------------------------------------------

    void removeSubDirsIfParentDirIsInContainer(std::vector<FileInfoBD*>& dirs_to_validate);
    bool removeSubDirsIfParentDirIsInContainer(std::vector<std::string>& paths_to_validate);

//----------------------------------------------------------------------------------

    // pointerResetter setzt den zeiger von src_pntr nach tar_pntr und loescht danach das element von src_pntr -> der aufrufer muss schlussendlich nur src_pntr manuell loeschen, nicht tar_pntr!
    template <class T>
    void pointerResetter(T* src_pntr, T* tar_pntr, std::function<void(T*)> delete_func = [](T* t){delete t;})
    {
        T* oa = new T(*src_pntr);

        *src_pntr = *tar_pntr;

        delete_func(oa);
    }

//----------------------------------------------------------------------------------

    bool isSupportedImage(const std::string& path);
    bool isSupportedImage(const QString& path);
    bool isSupportedImage(const QFileInfo& fi);

//----------------------------------------------------------------------------------

    bool isPDF(const std::string& path);
    bool isPDF(const QString& path);
    bool isPDF(const QFileInfo& fi);

}

#endif // SATATICFUNCTIONS_FILES_H
