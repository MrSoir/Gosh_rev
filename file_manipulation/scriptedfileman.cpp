#include "scriptedfileman.h"


void SCRIPTED_FILE_MAN::CopyFiles(const std::vector<string> &absSrcPaths, const string &tarDir)
{
    QVector<QString> args;

    args.push_back("-t");
    args.push_back(QString::fromStdString(tarDir));
    args.push_back("-s");
    for(const auto& srcPath: absSrcPaths)
    {
        args.push_back(QString::fromStdString(srcPath));
    }
    callScriptProgram("copy_files", args);
}

void SCRIPTED_FILE_MAN::DuplicateFiles(const std::vector<string> &absSrcPaths)
{
    QVector<QString> args;

    args.push_back("-s");
    for(const auto& srcPath: absSrcPaths)
    {
        args.push_back(QString::fromStdString(srcPath));
    }
    callScriptProgram("duplicate_files", args);
}

void SCRIPTED_FILE_MAN::MoveFiles(const std::vector<string> &absSrcPaths, const string &tarDir)
{
    QVector<QString> args;

    args.push_back("-t");
    args.push_back(QString::fromStdString(tarDir));
    args.push_back("-s");
    for(const auto& srcPath: absSrcPaths)
    {
        args.push_back(QString::fromStdString(srcPath));
    }
    callScriptProgram("move_files", args);
}

void SCRIPTED_FILE_MAN::DeleteFiles(const std::vector<string> &absSrcPaths)
{
    QVector<QString> args;

    args.push_back("-s");
    for(const auto& srcPath: absSrcPaths)
    {
        args.push_back(QString::fromStdString(srcPath));
    }
    callScriptProgram("delete_files", args);
}

void SCRIPTED_FILE_MAN::ZipFiles(const std::vector<string> &filesToZip,
                                 const string &tarZipFilePath)
{
    QVector<QString> args;

    args.push_back("-t");
    args.push_back(QString::fromStdString(tarZipFilePath));
    args.push_back(("-s"));
    for(const auto& srcPath: filesToZip)
    {
        args.push_back(QString::fromStdString(srcPath));
    }

    callScriptProgram("zip_files", args);
}

void SCRIPTED_FILE_MAN::UnZipArchive(const string &absArchivePath,
                                     const string &tarExtractionDir)
{
    QVector<QString> args;

    args.push_back("-s");
    args.push_back(QString::fromStdString(absArchivePath));
    if( !tarExtractionDir.empty() )
    {
        args.push_back("-t");
        args.push_back(QString::fromStdString(tarExtractionDir));
    }
    callScriptProgram("unzip_file", args);
}

//--------------------------------------

void SCRIPTED_FILE_MAN::CreateFolder(const string &tarDir)
{
    QVector<QString> args;

    args.push_back("-t");
    args.push_back(QString::fromStdString(tarDir));
    callScriptProgram("create_folder", args);
}

void SCRIPTED_FILE_MAN::CreateFile(const string &tarDir)
{
    QVector<QString> args;

    args.push_back("-t");
    args.push_back(QString::fromStdString(tarDir));
    callScriptProgram("create_file", args);
}

void SCRIPTED_FILE_MAN::RenameEntry(const string &tarEntryPath)
{
    QVector<QString> args;

    args.push_back("-t");
    args.push_back(QString::fromStdString(tarEntryPath));
    callScriptProgram("rename_file", args);
}

void SCRIPTED_FILE_MAN::OpenFilesWith(const std::vector<std::string>& tarFilePaths)
{
    QVector<QString> args;

    args.push_back("-t");
    for(const auto& path: tarFilePaths)
    {
        args.push_back(QString::fromStdString(path));
    }
    callScriptProgram("open_with", args);
}

//--------------------------------------

void SCRIPTED_FILE_MAN::callScriptProgram(std::string scriptName, QVector<QString> args)
    {
    #ifdef __linux__
    auto scriptPath =  QString("scripts%1%2%3").arg(QDir::separator())
                                                .arg(QString::fromStdString(scriptName))
                                                .arg(".py");
    STATIC_FUNCTIONS::execPythonScript(scriptPath, args, true, true);
//    STATIC_FUNCTIONS::execCommand(scriptPath, args, false, true);
    #elif _WIN32
    auto program = QString("scripts%1%2.exe").arg(QDir::separator())
                                             .arg(QString::fromStdString(scriptName));
    STATIC_FUNCTIONS::execCommand(program, args, false, true);
    #else
    #endif
}
