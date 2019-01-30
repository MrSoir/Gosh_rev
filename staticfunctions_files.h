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

#include <vector>
#include <string>
#include <unordered_set>
#include <functional>

#include "pathmanipulation.h"
#include "staticfunctions.h"
#include "process.h"

enum OS
{
    LINUX,
    WINDOWS,
    MACOS,
    UNKNOWN
};

namespace STATIC_FUNCTIONS
{
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

//    ----------------------------------------------------------------------------------

    QString getUniqueFilePath(const QString& absFilePath);
    QString getUniqueFolderPath(const QString& absFolderPath);

//    ----------------------------------------------------------------------------------

    OS LOCAL_OS(){
        QString os = QSysInfo::kernelType();
        if(os.contains("win"))
            return OS::WINDOWS;
        else if(os == "linux")
            return OS::LINUX;
        else if(os == "darwin")
            return OS::MACOS;
        else
            return OS::UNKNOWN;
    }

//    ----------------------------------------------------------------------------------

    void openTerminal(QString sourceDir);
}

#endif // SATATICFUNCTIONS_FILES_H
