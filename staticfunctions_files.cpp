#include "staticfunctions_files.h"


QThread* STATIC_FUNCTIONS::MAIN_THREAD = nullptr;

unsigned long long STATIC_FUNCTIONS::evaluateFileCount(const std::vector<string> &paths)
{
    unsigned long long cntr = 0;
    for(const auto& path: paths)
    {
        QFileInfo info(QString::fromStdString(path));
        cntr += static_cast<unsigned long long>(StaticFunctions::evaluateContentCount(info));
    }
    return cntr;
}

//--------------------------------------------------------------------------------------------------

// askUserForNoneExistingFilePath -> name ist irrefuehrend: path wird gegeben -> falls path existiert, wird lediglich vom user ein anderer fileName gefragt.
// Directory des absFilePath bleibt erhalten!
// falls ein Directory als path uebergeben wird, wird vom user ein directory-name verlangt, der noch nicht existiert, d.h.: basePath(path) + userDirName
QString STATIC_FUNCTIONS::askUserForNoneExistingFilePath(const QString &path,
                                                         std::function<bool (const QString&)> fileNameValidator,
                                                         bool askForDir)
{
    QFileInfo fi(path);

    QString tarPath = path;
    QString tarBasePath = PATH::getBasePath(tarPath);

    if(fi.exists())
    {
        auto filePathValidator = [=](const QString& fileName){
            QString newAbsPath = tarBasePath + QDir::separator() + fileName;
            if( QFileInfo(newAbsPath).exists() )
                return false;

            return fileNameValidator(fileName);
        };

        QString newFileName = "";
        QString s = askForDir ? "Directory" : "File";
        StringGetterDialogCreator dialogCreator;
        dialogCreator.moveToThread(QApplication::instance()->thread());

        StringGetterDialog* dialog = dialogCreator.createDialog(s + " does already exist!",
                                                                 QString("please select a %1 name").arg(s.toLower()),
                                                                 "'" + fi.fileName() + " does already exists or contains invalid characters - please choose another " + s.toLower() + " name!",
                                                                 fi.fileName(),
                                                                 filePathValidator);
        if(!dialog)
            return QString("");

        QObject::connect(dialog, &StringGetterDialog::stringSelected, [&](const QString& s){
            newFileName = s;
        });

        QMetaObject::invokeMethod(dialog, "show", Qt::QueuedConnection);

//        dialog->exec();

        qDebug() << "selected string: " << newFileName;

        if(newFileName.isEmpty())
            return QString("");

        return PATH::getBasePath(tarPath) + QDir::separator() + newFileName;

//        int cntr = 0, LOOP_MAX = 2;

//        while(fi.exists())
//        {
//            QString s = askForDir ? "Directory" : "File";
//            QString newFileName = genStringGetterDialog( s + " does already exist!",
//                                                         "'" + fi.fileName() + " does already exists or contains invalid characters - please choose another " + s.toLower() + " name:",
//                                                         fi.fileName(),
//                                                         filePathValidator);
//            tarPath = PATH::getBasePath(tarPath) + QDir::separator() + newFileName;

//            fi = QFileInfo(tarPath);

//            if(++cntr >= LOOP_MAX)
//                break;
//        }
//        return cntr < LOOP_MAX ? tarPath : "";
    }else{
        return path;
    }
}
QString STATIC_FUNCTIONS::askUserForNoneExistingDirPath(const QString& path,
                                                        std::function<bool(const QString& fileName)> dirNameValidator)
{
    return askUserForNoneExistingFilePath(path, dirNameValidator, true);
}

QString STATIC_FUNCTIONS::genStringGetterDialog(const QString &headline,
                                                const QString &message,
                                                const QString &initInput,
                                                std::function<bool (const QString &)> stringValidator)
{
    QString retVal = "";

    bool ok;

    QString adapted_message = message;

    QString text;
    do{
        text = QInputDialog::getText(nullptr,
                                     headline,
                                     adapted_message,
                                     QLineEdit::Normal,
                                     initInput, &ok);

        adapted_message = "invalid selection: '" + text + "': " + message;
    }while(!stringValidator(text));

    if (ok)
        retVal = text;

    return retVal;
}

//---------------------

std::string getPythonPath()
{
    QString python_path_info = QString("resources%1python_path.txt").arg(QDir::separator());
    std::ifstream infile(python_path_info.toStdString());
    if(infile)
    {
        std::string line;
        std::getline(infile, line);
        infile.close();
        return line;
    }else{
        return "";
    }
}

Process* STATIC_FUNCTIONS::execPythonScript(const QString &scriptPath,
                                            const QVector<QString> &args,
                                            bool waitForFinished,
                                            bool execute)
{
    QVector<QString> params;

    params.push_back( scriptPath );
    for(const auto& arg: args)
        params.push_back( arg );

    auto python_path = getPythonPath();
    if(python_path.empty())
    {
        python_path = "python3";
    }

    return execCommand(QString::fromStdString(python_path),
                       params,
                       waitForFinished,
                       execute);
}

Process* STATIC_FUNCTIONS::execPythonScript(const string &scriptPath,
                                            const std::vector<string> &args,
                                            bool waitForFinished,
                                            bool execute)
{
    QVector<QString> q_args;
    for(const auto& arg: args)
        q_args.push_back(QString::fromStdString(arg));

    return execPythonScript(QString::fromStdString(scriptPath),
                            q_args,
                            waitForFinished,
                            execute);
}

//---------------------

Process* STATIC_FUNCTIONS::executeDotNetScript(const QString& scriptPath,
                                               const QVector<QString>& args,
                                               bool waitForFinished,
                                               bool execute)
{
    QVector<QString> params;

    params.push_back( scriptPath );
    for(const auto& arg: args)
        params.push_back( arg );

    return execCommand(QString("scripts%1dotnet").arg(QDir::separator()),
                       params,
                       waitForFinished,
                       execute);
}

Process* STATIC_FUNCTIONS::executeDotNetScript(const string &scriptPath,
                                               const std::vector<string> &args,
                                               bool waitForFinished,
                                               bool execute)
{
    QVector<QString> q_args;
    for(const auto& arg: args)
        q_args.push_back(QString::fromStdString(arg));

    return executeDotNetScript(QString::fromStdString(scriptPath),
                               q_args,
                               waitForFinished,
                               execute);
}
//---------------------

Process* STATIC_FUNCTIONS::execCommand(const QString &program,
                                       const QVector<QString> &args,
                                       bool waitForFinished,
                                       bool execute)
{
    QStringList params;

    for(const auto& arg: args)
        params << arg;

    QString s(program);
    for(const auto& arg: args)
        s = s.append("  " + arg);
    qDebug() << s;


    if(waitForFinished)
    {
        Process* p = new Process();
        p->start(program, params);
        p->waitForFinished(-1);
        qDebug() << p->readAll();
        qDebug() << p->readAllStandardError();
        qDebug() << p->errorString();
        return nullptr;
    }else
    {
        Process* p = new Process(program, params);
        if(execute)
        {
            p->start_Delayed();
        }
        return p;
    }
}
Process* STATIC_FUNCTIONS::execCommand(const std::string& program,
                                       const std::vector<std::string>& args,
                                       bool waitForFinished,
                                       bool execute)
{
    QVector<QString> q_args;
    for(const auto& arg: args)
        q_args.push_back(QString::fromStdString(arg));

    return execCommand(QString::fromStdString(program),
                       q_args, waitForFinished,
                       execute);
}

//--------------------------------------------------------------------------------------------------

QPixmap STATIC_FUNCTIONS::getPixmapFromPDF(const QString& path, QSize imageSize)
{
    //    convert -density 150 input.pdf -quality 90 output.png
    QString tmpDirPth = getTempResourcesDir();
    QString tmpTarFilePath = getUniqueRandomFilePathInDir(tmpDirPth, ".png");

    qDebug() << "tmpDirPth: " << tmpDirPth;
    qDebug() << "tmpTarFilePath: " << tmpTarFilePath;

    QString program("convert");

    QVector<QString> args;
    args.push_back("-density");
    args.push_back("10");
    args.push_back(path);
    args.push_back("-quality");
    args.push_back("10");
    args.push_back(tmpTarFilePath);

    execCommand(program, args, true);

    QFileInfo fi(tmpTarFilePath);
    if(fi.exists())
    {
        return QPixmap(tmpTarFilePath).scaled(imageSize,
                                              Qt::IgnoreAspectRatio, Qt::FastTransformation);
    }

    return QPixmap();
}

QPixmap STATIC_FUNCTIONS::getPixmapFromPDF(const string &path, QSize imageSize)
{
    return getPixmapFromPDF(QString::fromStdString(path), imageSize);
}

//--------------------------------------------------------------------------------------------------


QString STATIC_FUNCTIONS::getUniqueFilePath(const QString &absFilePath)
{
    QFileInfo fi(absFilePath);

    if( !fi.exists() )
        return absFilePath;

    QString basePath = PATH::getBasePath(absFilePath);
    QString fileName_withoutExt = PATH::getFileNameWithoutExtension(absFilePath);
    QString ext = fi.completeSuffix();

    // QFileInfo::completeSuffix gibt die suffix ohne den fuehrenden punkt an:
    if( !ext.isEmpty() )
        ext.prepend('.');

    int cntr = 2;

    QString path = "";

    do{
        QString path = QString("%1%2%3_%4%5").arg(basePath)
                                             .arg(QDir::separator())
                                             .arg(fileName_withoutExt)
                                             .arg(cntr++)
                                             .arg(ext);
        fi = QFileInfo(path);
    }while(fi.exists());

    return path;
}

QString STATIC_FUNCTIONS::getUniqueFolderPath(const QString &absFolderPath)
{
    QFileInfo fi(absFolderPath);

    if( !fi.exists() )
        return absFolderPath;

    QString basePath = PATH::getBasePath(absFolderPath);
    QString fileName_withoutExt = fi.fileName();

    int cntr = 2;

    QString path = "";

    do{
        QString path = QString("%1%2%3_%4").arg(basePath)
                                           .arg(QDir::separator())
                                           .arg(fileName_withoutExt)
                                           .arg(cntr++);
        fi = QFileInfo(path);
    }while(fi.exists());

    return path;
}

QString STATIC_FUNCTIONS::getUniqueFilePathInDir(const QString &dir,
                                                 const QString& fileExtension)
{
    return getUniqueFilePathInDir_hlpr(dir, "new_file", fileExtension);
}
QString STATIC_FUNCTIONS::getUniqueRandomFilePathInDir(const QString& dir,
                                     const QString& fileExtension)
{
    qDebug() << genRandomFileBaseName();
    return getUniqueFilePathInDir_hlpr(dir, genRandomFileBaseName(), fileExtension);
}

QString STATIC_FUNCTIONS::getUniqueFolderPathInDir(const QString &dir)
{
    return getUniqueFolderPathInDir_hlpr(dir, "new_folder");
}

QString STATIC_FUNCTIONS::getUniqueRandomFolderPathInDir(const QString &dir)
{
    return getUniqueFolderPathInDir_hlpr(dir, genRandomFileBaseName());
}


QString STATIC_FUNCTIONS::getUniqueFilePathInDir_hlpr(const QString &dir,
                                                      const QString& fileBaseName,
                                                      const QString& fileExtension)
{
    QString fileExtWithDot = fileExtension;

    if( !fileExtWithDot.startsWith(".") )
        fileExtWithDot = fileExtWithDot.prepend(".");

    QString firstShot = dir + QDir::separator() + fileBaseName + fileExtension;
    return getUniqueFilePath(firstShot);
}

QString STATIC_FUNCTIONS::getUniqueFolderPathInDir_hlpr(const QString &dir, const QString &dirName)
{
    QString firstShot = dir + QDir::separator() + dirName;
    return getUniqueFolderPath(firstShot);
}

QString STATIC_FUNCTIONS::genRandomFileBaseName()
{
    QString s;
    for(int i=0; i < 20; ++i)
    {
        int r = rand() % 26 + 65;
        s.append( static_cast<char>(r) );
    }
    return s;
}

QString STATIC_FUNCTIONS::genRandomNumberString(int cnt)
{
    QString s;
    for(int i=0; i < cnt; ++i)
    {
        int r = rand() % 10 + 48;
        s.append( static_cast<char>(r) );
    }
    return s;
}


//--------------------------------------------------------------------------------------------------


void STATIC_FUNCTIONS::openTerminal(QString sourceDir)
{
    OS local_os = LOCAL_OS();
    if(local_os == OS::LINUX)
    {
        Process* proc = new Process();
        proc->setWorkingDirectory(sourceDir);
        proc->start("gnome-terminal");
    }else if (local_os == OS::WINDOWS)
    {
        Process* proc = new Process();
        proc->setWorkingDirectory(sourceDir);
        proc->start("cmd.exe");
    }else
        qDebug() << "STATIC_FUNCTIONS::openTerminal -> OS not supported!";
}

//--------------------------------------------------------------------------------------------------


bool STATIC_FUNCTIONS::isSubDirectory(const string &potentialSubFile, const string &potentialParentFolder)
{
    return isSubDirectory(QFileInfo(QString::fromStdString(potentialSubFile)), QFileInfo(QString::fromStdString(potentialParentFolder)));
}

bool STATIC_FUNCTIONS::isSubDirectory(const QString& potentialSubFile, const QString& potentialParentFolder)
{
    return isSubDirectory(QFileInfo(potentialSubFile), QFileInfo(potentialParentFolder));
}
bool STATIC_FUNCTIONS::isSubDirectory(const QFileInfo& potentialSubFile, const QFileInfo& potentialParentFolder)
{
    // eine datei kann kein parent-folder sein von irgendwas:
    if(potentialParentFolder.isFile())
        return false;

    QDir parentFolder   = PATH::getDirFromPath(potentialParentFolder);
    QDir subFilesFolder = PATH::getDirFromPath(potentialSubFile);

    int parentFolderPathLength = parentFolder.absolutePath().length();

    do{
        if(subFilesFolder == parentFolder)
            return true;
    }while(subFilesFolder.cdUp() &&
           subFilesFolder.absolutePath().length() >= parentFolderPathLength);


    return false;
}

//--------------------------------------------------------------------------------------------------

#include "file_handling_src/fileinfobd.h"

void STATIC_FUNCTIONS::removeSubDirsIfParentDirIsInContainer(std::vector<FileInfoBD*>& dirs_to_validate)
{
    std::unordered_map<std::string, FileInfoBD*> path_to_dir;
    std::vector<std::string> paths;
    for(auto* dir: dirs_to_validate)
    {
        std::string absPath = dir->absPath();
        path_to_dir[absPath] = dir;
        paths.push_back(absPath);
    }

    bool invalidPathDetected = removeSubDirsIfParentDirIsInContainer(paths);

    if(invalidPathDetected)
    {
        dirs_to_validate.clear();
        for(const auto& path: paths)
        {
            dirs_to_validate.push_back( path_to_dir[path] );
        }
    }
}

bool STATIC_FUNCTIONS::removeSubDirsIfParentDirIsInContainer(std::vector<std::string>& paths_to_validate)
{
    bool invalidPathDetected = false;

    for(unsigned long i=0; i < paths_to_validate.size(); ++i)
    {
        for(unsigned long j=0; j < paths_to_validate.size(); ++j)
        {
            if(i != j)
            {
                bool isSubDir = STATIC_FUNCTIONS::isSubDirectory(paths_to_validate[i], paths_to_validate[j]);
                if(isSubDir)
                {
                    invalidPathDetected = true;

                    paths_to_validate.erase(paths_to_validate.begin() + static_cast<int>(i));
                    --i;
                    if(j > i)
                        --j;
                }else{
                    isSubDir = STATIC_FUNCTIONS::isSubDirectory(paths_to_validate[j], paths_to_validate[i]);
                    if(isSubDir)
                    {
                        invalidPathDetected = true;

                        paths_to_validate.erase(paths_to_validate.begin() + static_cast<int>(j));
                        --j;
                        if(i > j)
                        {
                            --i;
                        }
                    }
                }
            }
        }
    }

    return invalidPathDetected;
}

//----------------------------------------------------------------------------------

OS STATIC_FUNCTIONS::LOCAL_OS()
{
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

//----------------------------------------------------------------------------------

bool STATIC_FUNCTIONS::isSupportedImage(const QFileInfo &fi)
{
    QString lowerFileName = fi.fileName().toLower();

    QMimeType mime_type = MIME_DATA_BASE.mimeTypeForFile(fi);
    auto supported_image_mimes = QImageReader::supportedImageFormats();
    for(const auto& bs: supported_image_mimes)
    {
        QString ss(bs.toLower());
        if(lowerFileName.endsWith(ss)){
            return true;
        }
    }
    return false;
}

bool STATIC_FUNCTIONS::isSupportedImage(const string& path)
{
    return isSupportedImage(QFileInfo(QString::fromStdString(path)));
}

bool STATIC_FUNCTIONS::isSupportedImage(const QString& path)
{
    return isSupportedImage(QFileInfo(path));
}

//----------------------------------------------------------------------------------

bool STATIC_FUNCTIONS::isPDF(const string& path)
{
    return isPDF(QFileInfo(QString::fromStdString(path)));
}

bool STATIC_FUNCTIONS::isPDF(const QString& path)
{
    return isPDF(QFileInfo(path));
}

bool STATIC_FUNCTIONS::isPDF(const QFileInfo& fi)
{
    return fi.fileName().toLower().endsWith("pdf");
}

QString STATIC_FUNCTIONS::getResourceDir()
{
    QDir dir;
    return QDir(QString("%1%2%3").arg(dir.absolutePath())
                                 .arg(QDir::separator())
                                 .arg("resources")).absolutePath();
}

QString STATIC_FUNCTIONS::getTempResourcesDir()
{

    QDir dir;
    return QDir(QString("%1%2%3%4%5").arg(dir.absolutePath())
                                     .arg(QDir::separator())
                                     .arg("resources")
                                     .arg(QDir::separator())
                                     .arg("tmp")).absolutePath();
}


void STATIC_FUNCTIONS::setIconToWidget(QWidget *widget)
{
    QString windowIconPath = QString("%1%2%3")
            .arg("pics")
            .arg(QDir::separator())
            .arg("KubuIcon.png");
    widget->setWindowIcon(QIcon(windowIconPath));
    widget->setWindowTitle(QString("Kubu"));
}

void STATIC_FUNCTIONS::setStyleSheet(QWidget *widget)
{
    QFile styleFile(QString("styles%1%2").arg(QDir::separator()).arg("stylesheet.qss"));
    if(styleFile.open(QIODevice::ReadOnly))
    {
        QTextStream textStream(&styleFile);
        QString styleSheet = textStream.readAll();
        styleFile.close();
        widget->setStyleSheet(styleSheet);
    }
}

//-------------------------------------------------------------------------------

bool STATIC_FUNCTIONS::createNewFile(QString absTarFilePath)
{
    absTarFilePath = STATIC_FUNCTIONS::askUserForNoneExistingFilePath(absTarFilePath);
    QFileInfo fi(absTarFilePath);
    if(absTarFilePath.isEmpty() || fi.exists())
    {
        return false;
    }

    QFile file( absTarFilePath );
    bool successfullyCreated = file.open(QIODevice::WriteOnly);
    file.close();
    return successfullyCreated;
}
bool STATIC_FUNCTIONS::createNewFile(const string& absTarFilePath)
{
    return createNewFile(QString::fromStdString(absTarFilePath));
}
//------------------
bool STATIC_FUNCTIONS::createNewFolder(QString absTarDirPath)
{
    absTarDirPath = STATIC_FUNCTIONS::askUserForNoneExistingFilePath(absTarDirPath);

    if(absTarDirPath.isEmpty() || QFileInfo(absTarDirPath).exists())
        return false;

    return makeDir(absTarDirPath);
}
bool STATIC_FUNCTIONS::createNewFolder(const string &absTarDirPath)
{
    return createNewFolder(QString::fromStdString(absTarDirPath));
}
//------------------
bool STATIC_FUNCTIONS::makeDir(const QString& tarPath)
{
    return QDir().mkdir(tarPath);
}
bool STATIC_FUNCTIONS::makeDir(const string& tarPath)
{
    return makeDir(QString::fromStdString(tarPath));
}
//------------------
bool STATIC_FUNCTIONS::copyFile(const QString &absSourcePath, const QString &absTarPath)
{
    return QFile::copy(absSourcePath, absTarPath);
}
bool STATIC_FUNCTIONS::copyFile(const string &absSourcePath, const string &absTarPath)
{
    return copyFile(QString::fromStdString(absSourcePath), QString::fromStdString(absTarPath));
}

//-------------------------------------------------------------------------------

//QPixmap STATIC_FUNCTIONS::loadPreviewIcon(const string& file_path,
//                                          const QSize &size)
//{
//    return loadPreviewIcon(QString::fromStdString(file_path), size);
//}

//QPixmap STATIC_FUNCTIONS::loadPreviewIcon(const QString &file_path,
//                                          const QSize &size)
//{
//    if(SUPPORTED_IMAGE_FORMATS.find(QFileInfo(file_path).completeSuffix().toStdString()) == SUPPORTED_IMAGE_FORMATS.end())
//        return QPixmap();

//    QPixmap p(file_path);
//    if(p.isNull())
//    {
//        return p;
//    }
//    auto loadedSize = p.size().width() * p.size().height();
//    if(loadedSize == 0)
//    {
//        qDebug() << "0-size image: " << file_path;
//        return QPixmap();
//    }
//    return p.scaled(size,
//                    Qt::IgnoreAspectRatio, Qt::FastTransformation);
//}

std::unordered_set<string> STATIC_FUNCTIONS::evalSupportedImageFileTypes()
{
    std::unordered_set<std::string> supportedImgeFileTypes;
    for(auto imf: QImageReader::supportedImageFormats())
    {
        supportedImgeFileTypes.emplace(imf.toStdString());
    }
    return supportedImgeFileTypes;
}


