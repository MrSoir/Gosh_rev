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
#include <fstream>
#include <sstream>
#include <string>

#include "pathmanipulation.h"
#include "staticfunctions.h"
#include "process.h"
#include "dialogs/stringgetterdialog.h"

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
    extern QThread* MAIN_THREAD;

    static QMimeDatabase MIME_DATA_BASE;

//----------------------------------------------------------------------------------

    std::unordered_set<std::string> evalSupportedImageFileTypes();

    static std::unordered_set<std::string> SUPPORTED_IMAGE_FORMATS = STATIC_FUNCTIONS::evalSupportedImageFileTypes();

//    QPixmap loadPreviewIcon(const std::string& file_path,
//                            const QSize& size);
//    QPixmap loadPreviewIcon(const QString& file_path,
//                            const QSize& size);

//----------------------------------------------------------------------------------

    void setIconToWidget(QWidget* widget);
    void setStyleSheet(QWidget* widget);

//----------------------------------------------------------------------------------

    QString getResourceDir();
    QString getTempResourcesDir();

//----------------------------------------------------------------------------------

    bool createNewFile(QString absTarFilePath);
    bool createNewFile(const std::string& absTarFilePath);
    //---------
    bool createNewFolder(QString absTarDirPath);
    bool createNewFolder(const std::string& absTarDirPath);
    //---------
    bool makeDir(const QString& tarPath);
    bool makeDir(const std::string& tarPath);

    bool copyFile(const QString& absSourcePath, const QString& absTarPath);
    bool copyFile(const std::string& absSourcePath, const std::string& absTarPath);

//----------------------------------------------------------------------------------

    unsigned long long evaluateFileCount(const std::vector<std::string>& paths);

    // askUserForNoneExistingFilePath -> name ist irrefuehrend: path wird gegeben -> falls path existiert, wird lediglich vom user ein anderer fileName gefragt.
    // Directory des absFilePath bleibt erhalten!
    // falls ein Directory als path uebergeben wird, wird vom user ein directory-name verlangt, der noch nicht existiert, d.h.: basePath(path) + userDirName
    QString askUserForNoneExistingFilePath(const QString& path,
                                           std::function<bool(const QString& fileName)> fileNameValidator = [](const QString& fileName){return !fileName.contains(QDir::separator());},
                                           bool askForDir = false);
    QString askUserForNoneExistingDirPath(const QString& path, std::function<bool(const QString& fileName)> dirNameValidator = [](const QString& fileName){ return !fileName.contains('.') && !fileName.contains(QDir::separator());});

    QString genStringGetterDialog(const QString &headline,
                                  const QString &message,
                                  const QString& initInput,
                                  std::function<bool (const QString &)> stringValidator = [](const QString& s){Q_UNUSED(s)return true;});

//----------------------------------------------------------------------------------

    Process* execPythonScript(const QString& scriptPath,
                              const QVector<QString>& args,
                              bool waitForFinished = true,
                              bool execute = true);
    Process* execPythonScript(const std::string& scriptPath,
                              const std::vector<std::string>& args,
                              bool waitForFinished = true,
                              bool execute = true);

    //---------------------

    Process* executeDotNetScript(const QString& scriptPath,
                                 const QVector<QString>& args,
                                 bool waitForFinished = true,
                                 bool execute = true);
    Process* executeDotNetScript(const std::string& scriptPath,
                                 const std::vector<std::string>& args,
                                 bool waitForFinished = true,
                                 bool execute = true);

    //---------------------

    Process* execCommand(const QString& program,
                         const QVector<QString>& args,
                         bool waitForFinished = true,
                         bool execute = true);
    Process* execCommand(const std::string& program,
                         const std::vector<std::string>& args,
                         bool waitForFinished = true,
                         bool execute = true);

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
    QString genRandomNumberString(int cnt = 5);

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
//        T* oa = new T(*src_pntr);
        T* oa = src_pntr;
        *oa = *src_pntr;

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
