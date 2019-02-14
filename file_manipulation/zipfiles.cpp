#include "zipfiles.h"


Process* ZIP::zipFiles(const QVector<QString>& filePathsToZip, const QString& tarZipFilePath)
{
    QString relScriptPath("scripts/zip_files.py");
    QString absScriptPath = QFileInfo(relScriptPath).absoluteFilePath();

    QVector<QString> scriptArgs;

    scriptArgs.push_back(QString("-z"));
    for(const auto& path: filePathsToZip)
        scriptArgs.push_back(path);

    scriptArgs.push_back(QString("-t"));
    scriptArgs.push_back(tarZipFilePath);

    return STATIC_FUNCTIONS::execPythonScript(absScriptPath, scriptArgs, false);
}

Process* ZIP::zipFiles(const std::vector<string>& filePathsToZip, const string& tarZipFilePath)
{
    QVector<QString> q_filePathsToZip;
    for(const auto& path: filePathsToZip)
        q_filePathsToZip.push_back( QString::fromStdString(path) );

    return zipFiles(q_filePathsToZip, QString::fromStdString(tarZipFilePath));
}

//---------------------------------

Process* ZIP::unzipFile(const QString& zipFilePath, const QString& tarUnzipPath)
{
    QString relScriptPath("scripts/unzip_file.py");
    QString absScriptPath = QFileInfo(relScriptPath).absoluteFilePath();

    QVector<QString> scriptArgs;

    scriptArgs.push_back(QString("-z"));
    scriptArgs.push_back(zipFilePath);

    if( !tarUnzipPath.isEmpty() )
    {
        scriptArgs.push_back(QString("-t"));
        scriptArgs.push_back(tarUnzipPath);
    }

    return STATIC_FUNCTIONS::execPythonScript(absScriptPath, scriptArgs, false);
}
Process* ZIP::unzipFile(const std::string& zipFilePath, const std::string& tarUnzipPath)
{
    return unzipFile(QString::fromStdString(zipFilePath), QString::fromStdString(tarUnzipPath));
}

//---------------------------------------------------------

ZIP::ZipFiles::ZipFiles(std::vector<std::string> pathsToZip,
                        std::string tarPath)
    : ProgressDialogWorker(pathsToZip.size()),
      m_pathsToZip(pathsToZip),
      m_tarPath(tarPath),
      m_zipProcess(nullptr)
{
}

ZIP::ZipFiles::ZipFiles()
    : ProgressDialogWorker(),
      m_pathsToZip(std::vector<std::string>()),
      m_tarPath(""),
      m_zipProcess(nullptr)
{
}

ZIP::ZipFiles::ZipFiles(const ZIP::ZipFiles &zf)
    : ProgressDialogWorker(zf),
      m_pathsToZip(zf.m_pathsToZip),
      m_tarPath(zf.m_tarPath),
      m_zipProcess(zf.m_zipProcess)
{
}

ZIP::ZipFiles &ZIP::ZipFiles::operator=(const ZIP::ZipFiles &zf)
{
    ProgressDialogWorker::operator=(zf);
    m_pathsToZip = zf.m_pathsToZip;
    m_tarPath = zf.m_tarPath;
    m_zipProcess = zf.m_zipProcess;
    return *this;
}

ZIP::ZipFiles::~ZipFiles()
{
    if(m_zipProcess)
    {
        if(m_zipProcess->isOpen())
        {
            m_zipProcess->kill();
        }
        delete m_zipProcess;
    }
}

void ZIP::ZipFiles::run()
{
    zipFiles();
    finish();
}

void ZIP::ZipFiles::zipFiles()
{
    QVector<QString> validFilesToZip;

    for(const auto& path: m_pathsToZip)
    {
        QString q_path = QString::fromStdString(path);

        QFileInfo fi(q_path);

        if(fi.exists())
        {
            validFilesToZip.push_back(q_path);
        }else
        {
            qDebug() << "'" << fi.fileName() << "' does not exists - won't be zipped!";
        }
    }

    QString q_tarPath = QString::fromStdString(m_tarPath);
    QString validTarZipPath = STATIC_FUNCTIONS::askUserForNoneExistingFilePath(q_tarPath);

    if(validFilesToZip.size() > 0 && !validTarZipPath.isEmpty())
    {
        m_zipProcess = ZIP::zipFiles(validFilesToZip, validTarZipPath);
    }else{
        qDebug() << "ZIP::ZipFiles::run: validFilesToZip.size: " << validFilesToZip.size() << " validTarZipPath: '" << validTarZipPath << "'";
    }
}

//---------------------------------------------------------

ZIP::UnZipFile::UnZipFile(string zipFilePath, string unZipTarPath)
    : ProgressDialogWorker(1),
      m_zipFilePath(zipFilePath),
      m_unZipTarPath(unZipTarPath),
      m_zipProcess(nullptr)
{
}

ZIP::UnZipFile::UnZipFile()
    : ProgressDialogWorker(),
      m_zipFilePath(""),
      m_unZipTarPath(""),
      m_zipProcess(nullptr)
{
}

ZIP::UnZipFile::UnZipFile(const ZIP::UnZipFile &uzf)
    : ProgressDialogWorker(uzf),
      m_zipFilePath(uzf.m_zipFilePath),
      m_unZipTarPath(uzf.m_unZipTarPath),
      m_zipProcess(uzf.m_zipProcess)
{
}

ZIP::UnZipFile& ZIP::UnZipFile::operator=(const ZIP::UnZipFile &uzf)
{
    ProgressDialogWorker::operator=(uzf);
    m_zipFilePath = uzf.m_zipFilePath;
    m_unZipTarPath = uzf.m_unZipTarPath;
    m_zipProcess = uzf.m_zipProcess;
    return *this;
}

ZIP::UnZipFile::~UnZipFile()
{
    if(m_zipProcess)
    {
        if(m_zipProcess->isOpen())
        {
            m_zipProcess->kill();
        }
        delete m_zipProcess;
    }
}

void ZIP::UnZipFile::run()
{
    unZipFile();
    finish();
}

void ZIP::UnZipFile::unZipFile()
{
    QString q_zipFilePath = QString::fromStdString(m_zipFilePath);

    QString validUnZipTarDir = getTargetZipFilePath();

    QFileInfo fi(q_zipFilePath);
    if( fi.exists() && !validUnZipTarDir.isEmpty() )
    {
        m_zipProcess = ZIP::unzipFile(q_zipFilePath, validUnZipTarDir);
    }else{
        qDebug() << "ZIP::UnZipFile::run: zipFilePath does not exists! -> '" << fi.fileName() << "' |   validUnZipTarDir: " << validUnZipTarDir;
    }
}

QString ZIP::UnZipFile::getTargetZipFilePath()
{
    if(m_unZipTarPath.empty())
    {
        QString tarZipPath = PATH::join(PATH::getBasePath(m_zipFilePath), "zipped.zip");
        return StaticFunctions::getUniqueFileName(tarZipPath);
    }else{
        QString q_unZipTarPath = QString::fromStdString(m_unZipTarPath);
        return STATIC_FUNCTIONS::askUserForNoneExistingFilePath(q_unZipTarPath); // falls q_unZipTarPath nicht existiert, gibt askUserForNoneExistingFilePath q_unZipTarPath unveraendert zurueck!
    }
}

