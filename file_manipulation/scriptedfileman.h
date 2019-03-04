#ifndef SCRIPTEDFILEMAN_H
#define SCRIPTEDFILEMAN_H


#include <QObject>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <QString>

#include "process.h"
#include "staticfunctions_files.h"

class SCRIPTED_FILE_MAN
{
public:
    static void CopyFiles(const std::vector<std::string>& absSrcPaths,
                          const std::string& srcDir);

    static void DuplicateFiles(const std::vector<std::string>& absSrcPaths);

    static void MoveFiles(const std::vector<std::string>& absSrcPaths,
                          const std::string& tarDir);

    static void DeleteFiles(const std::vector<std::string>& absSrcPaths);

    static void ZipFiles(const std::vector<std::string>& filesToZip,
                         const std::string& tarZipFilePath);

    static void UnZipArchive(const std::string& absArchivePath,
                             const std::string& tarExtractionDir="");

    static void CreateFolder(const std::string& tarDir);
    static void CreateFile(const std::string& tarDir);
    static void RenameEntry(const std::string& tarEntryPath);
    static void OpenFilesWith(const std::vector<std::string>& tarFilePath);

private:
    static void callScriptProgram(std::string scriptName, QVector<QString> args);
};

#endif // SCRIPTEDFILEMAN_H
