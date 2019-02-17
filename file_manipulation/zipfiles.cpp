#include "zipfiles.h"


Process* ZIP::zipFiles(QVector<QString> filePathsToZip,
                       QString zipFileBasePath,
                       QString tarZipFilePath)
{
    QString relScriptPath("scripts/zip_files.dll");
    QString absScriptPath = QFileInfo(relScriptPath).absoluteFilePath();

    QVector<QString> scriptArgs;

    scriptArgs.append(tarZipFilePath);
    scriptArgs.append(zipFileBasePath);
    for(const auto& srcPath: filePathsToZip)
    {
        scriptArgs.append(srcPath);
    }

    return STATIC_FUNCTIONS::executeDotNetScript(absScriptPath, scriptArgs, false, false);
}

Process* ZIP::zipFiles(const std::vector<string>& filePathsToZip,
                       const std::string& zipFileBasePath,
                       const string& tarZipFilePath)
{
    QVector<QString> q_filePathsToZip;
    for(const auto& path: filePathsToZip)
        q_filePathsToZip.push_back( QString::fromStdString(path) );

    return zipFiles(q_filePathsToZip,
                    QString::fromStdString(zipFileBasePath),
                    QString::fromStdString(tarZipFilePath));
}

Process* ZIP::zipFiles(QVector<QString> filePathsToZip,
                       QString tarZipFilePath)
{
    QString zipBasePath = evalZipBasePath(filePathsToZip);
    return zipFiles(filePathsToZip, zipBasePath, tarZipFilePath);
}

Process* ZIP::zipFiles(const std::vector<string> &filePathsToZip,
                       const string &tarZipFilePath)
{

    QVector<QString> q_filePathsToZip;
    for(const auto& path: filePathsToZip)
        q_filePathsToZip.push_back( QString::fromStdString(path) );

    return zipFiles(q_filePathsToZip, QString::fromStdString(tarZipFilePath));
}

QString ZIP::evalZipBasePath(const QVector<QString>& filePathsToZip)
{
    if(filePathsToZip.size() == 0)
    {
        return QString("");
    }else if(filePathsToZip.size() == 1)
    {
        return filePathsToZip[0];
    }else{
        QString basePath = PATH::getBasePath(filePathsToZip[0]);
        for(int i = 1; i < filePathsToZip.size(); ++i)
        {
            basePath = PATH::getJointBasePath(basePath, filePathsToZip[i]);
        }
        return basePath;
    }
}



//---------------------------------

Process* ZIP::unZipFile(const QString& zipFilePath, const QString& tarUnzipPath)
{
    QString relScriptPath("scripts/unzip_file.dll");
    QString absScriptPath = QFileInfo(relScriptPath).absoluteFilePath();

    QVector<QString> scriptArgs;

    scriptArgs.append(zipFilePath);
    scriptArgs.append(tarUnzipPath);

    return STATIC_FUNCTIONS::executeDotNetScript(absScriptPath, scriptArgs, false, false);
}
Process* ZIP::unZipFile(const std::string& zipFilePath, const std::string& tarUnzipPath)
{
    return unZipFile(QString::fromStdString(zipFilePath), QString::fromStdString(tarUnzipPath));
}

//---------------------------------------------------------

ZIP::ZipHelper::ZipHelper(QObject *parent)
    : QObject(parent),
      m_cancelled(false),
      m_process(nullptr)
{
    connect(this, &ZipHelper::cancelled, this, &ZipHelper::deleteLater);
    connect(this, &ZipHelper::finished,  this, &ZipHelper::deleteLater);
}

ZIP::ZipHelper::ZipHelper(const ZIP::ZipHelper &h)
    : QObject(h.parent()),
      m_cancelled(h.m_cancelled),
      m_process(h.m_process)
{
    connect(this, &ZipHelper::cancelled, this, &ZipHelper::deleteLater);
    connect(this, &ZipHelper::finished,  this, &ZipHelper::deleteLater);
}

ZIP::ZipHelper::ZipHelper()
    : QObject(nullptr),
      m_cancelled(false),
      m_process(nullptr)
{
    connect(this, &ZipHelper::finished,  this, &ZipHelper::deleteLater);
}

ZIP::ZipHelper::~ZipHelper()
{
    qDebug() << "~ZipHelper";
}

void ZIP::ZipHelper::cancel()
{
    m_cancelled = true;
    emit cancelled();
}

void ZIP::ZipHelper::connectAndExecuteProcess()
{
    if(m_process)
    {
        connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &ZipHelper::zipProcessedFinished);
        connect(this, &ZipHelper::cancelled, m_process, &Process::cancel);
        m_process->start_Delayed();
    }else{
        qDebug() << "ZipHelper::connectAndExecuteProcess -> m_process is nullptr!!!";
        emit finished();
    }
}

void ZIP::ZipHelper::zipProcessedFinished()
{
    emit finished();
}

//---------------------------------------------

ZIP::ZipFilesHelper::ZipFilesHelper(string absTarZipFilePath,
                                    std::vector<string> absSrcPaths,
                                    QObject *parent)
    : ZipHelper(parent),
      m_absTarZipFilePath(absTarZipFilePath),
      m_absSrcPaths(absSrcPaths)
{
}

ZIP::ZipFilesHelper::ZipFilesHelper(const ZipFilesHelper &h)
    : ZipHelper(h.parent()),
      m_absTarZipFilePath(h.m_absTarZipFilePath),
      m_absSrcPaths(h.m_absSrcPaths)
{
}

ZIP::ZipFilesHelper::ZipFilesHelper()
    : ZipHelper(nullptr),
      m_absTarZipFilePath(""),
      m_absSrcPaths(std::vector<std::string>())
{
}

ZIP::ZipFilesHelper::~ZipFilesHelper()
{
}

void ZIP::ZipFilesHelper::run()
{
    m_process = zipFiles(m_absSrcPaths,
                         m_absTarZipFilePath);
    connectAndExecuteProcess();
}

//---------------------------------------------

ZIP::UnZipHelper::UnZipHelper(string absSrcZipFilePath,
                              string absTarZipFilePath,
                              QObject *parent)
    : ZipHelper(parent),
      absSrcZipFilePath(absSrcZipFilePath),
      absTarZipFilePath(absTarZipFilePath)
{
}

ZIP::UnZipHelper::UnZipHelper(const ZIP::UnZipHelper &h)
    : ZipHelper(h.parent()),
      absSrcZipFilePath(h.absSrcZipFilePath),
      absTarZipFilePath(h.absTarZipFilePath)
{
}

ZIP::UnZipHelper::UnZipHelper()
    : ZipHelper(nullptr),
      absSrcZipFilePath(""),
      absTarZipFilePath("")
{
}

ZIP::UnZipHelper::~UnZipHelper()
{
}

void ZIP::UnZipHelper::run()
{
    m_process = unZipFile(absSrcZipFilePath,
                          absTarZipFilePath);
    connectAndExecuteProcess();
}

//---------------------------------------------------------

ZIP::ZipFiles::ZipFiles(const std::vector<std::string>& pathsToZip,
                        const std::string& tarPath)
    : ZipFileBase(),
      m_pathsToZip(pathsToZip),
      m_tarPath(tarPath)
{
}

ZIP::ZipFiles::ZipFiles()
    : ZipFileBase(),
      m_pathsToZip(std::vector<std::string>()),
      m_tarPath("")
{
}

ZIP::ZipFiles::ZipFiles(const ZIP::ZipFiles &zf)
    : ZipFileBase(zf),
      m_pathsToZip(zf.m_pathsToZip),
      m_tarPath(zf.m_tarPath)
{
}

ZIP::ZipFiles::~ZipFiles()
{
    qDebug() << "~ZipFiles";
}

void ZIP::ZipFiles::run()
{
    this->zipFiles();
}

void ZIP::ZipFiles::zipFiles()
{
    std::vector<std::string> validFilesToZip;

    for(const auto& path: m_pathsToZip)
    {
        QString q_path = QString::fromStdString(path);

        QFileInfo fi(q_path);

        if(fi.exists() && !q_path.isEmpty())
        {
            validFilesToZip.push_back(path);
        }else
        {
            qDebug() << "'" << fi.fileName() << "' does not exists - won't be zipped!";
        }
    }

    QString q_tarPath = QString::fromStdString(m_tarPath);
    QString validTarZipPath = STATIC_FUNCTIONS::askUserForNoneExistingFilePath(q_tarPath);

    if(validFilesToZip.size() > 0 && !validTarZipPath.isEmpty())
    {
        m_helper = new ZipFilesHelper(validTarZipPath.toStdString(),
                                      validFilesToZip);
        connectHelper();
        m_helper->run();
    }else{
        qDebug() << "ZIP::ZipFiles::run: validFilesToZip.size: " << validFilesToZip.size() << " validTarZipPath: '" << validTarZipPath << "'";
        finish();
    }
}

//---------------------------------------------------------

ZIP::UnZipFile::UnZipFile(const std::string& zipFilePath,
                          const std::string& unZipTarPath)
    : ZipFileBase(),
      m_zipFilePath(zipFilePath),
      m_unZipTarDirPath(unZipTarPath)
{
}

ZIP::UnZipFile::UnZipFile()
    : ZipFileBase(),
      m_zipFilePath(""),
      m_unZipTarDirPath("")
{
}

ZIP::UnZipFile::UnZipFile(const ZIP::UnZipFile &uzf)
    : ZipFileBase(uzf),
      m_zipFilePath(uzf.m_zipFilePath),
      m_unZipTarDirPath(uzf.m_unZipTarDirPath)
{
}


ZIP::UnZipFile::~UnZipFile()
{
}

void ZIP::UnZipFile::run()
{
    this->unZipFile();
}

void ZIP::UnZipFile::unZipFile()
{
    QString q_zipFilePath = QString::fromStdString(m_zipFilePath);

    QString validUnZipTarPath = getTargetUnZipDirPath();

    QFileInfo fi(q_zipFilePath);
    if( fi.exists() && !validUnZipTarPath.isEmpty() )
    {
        m_helper = new UnZipHelper(m_zipFilePath, validUnZipTarPath.toStdString());
        connectHelper();
    }else{
        qDebug() << "ZIP::UnZipFile::run: zipFilePath does not exists! -> '" << fi.fileName() << "' |   validUnZipTarDir: " << validUnZipTarPath;
        finish();
    }
}

QString ZIP::UnZipFile::getTargetUnZipDirPath()
{
    if(m_unZipTarDirPath.empty())
    {
        auto baseFileName = PATH::getFileNameWithoutExtension(QString::fromStdString(m_zipFilePath));
        QString tarZipPath = PATH::join(PATH::getBasePath(m_zipFilePath), baseFileName);
        return STATIC_FUNCTIONS::askUserForNoneExistingDirPath(tarZipPath);
    }else{
        QString q_unZipTarPath = QString::fromStdString(m_unZipTarDirPath);
        return STATIC_FUNCTIONS::askUserForNoneExistingDirPath(q_unZipTarPath);
    }
}

//-------------------------------------------------------------------------------------------------

ZIP::ZipFileBase::ZipFileBase()
    : ProgressDialogWorker(),
      m_helper(nullptr)
{
}

ZIP::ZipFileBase::ZipFileBase(const ZIP::ZipFileBase &zf)
    : ProgressDialogWorker(),
      m_helper(zf.m_helper)
{
}

ZIP::ZipFileBase::~ZipFileBase()
{
}

void ZIP::ZipFileBase::helperFinished()
{
    this->finish();
}

void ZIP::ZipFileBase::finish()
{
    disconnectHelper();
    ProgressDialogWorker::finish();
}

void ZIP::ZipFileBase::connectHelper()
{
    if(m_helper)
        return;
    connect(this, &ZipFileBase::cancelSGNL, m_helper, &UnZipHelper::cancel, Qt::DirectConnection);
    connect(this, &ZipFileBase::cancelSGNL, this, &ZipFileBase::finish);
    connect(m_helper, &ZipFilesHelper::finished, this, &ZipFileBase::helperFinished);
}

void ZIP::ZipFileBase::disconnectHelper()
{
    if(m_helper)
        return;
    disconnect(this, &ZipFileBase::cancelSGNL, m_helper, &UnZipHelper::cancel);
    disconnect(this, &ZipFileBase::cancelSGNL, this, &ZipFileBase::finish);
    disconnect(m_helper, &ZipFilesHelper::finished, this, &ZipFileBase::helperFinished);
}
