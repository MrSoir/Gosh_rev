#include "filemimetypehandler.h"

//bool operator==(const AppInfo& lhs, const AppInfo& rhs) {
//    if(lhs.mime_types.size() != rhs.mime_types.size())
//        return false;

//    for(const auto& ai: lhs.mime_types)
//    {
//        if(rhs.mime_types.find(ai) != rhs.mime_types.end())
//            return false;
//    }
//    for(const auto& ai: rhs.mime_types)
//    {
//        if(lhs.mime_types.find(ai) != lhs.mime_types.end())
//            return false;
//    }
//    return lhs.name == rhs.name
//        && lhs.exec_cmnd == rhs.exec_cmnd
//        && lhs.icon_file_name == rhs.icon_file_name;
//}

FileMimeTypeHandler::FileMimeTypeHandler(std::string path,
                                         int min_icon_size,
                                         QObject *parent)
    : QObject(parent),
      m_matching_apps(std::unordered_set<AppInfo>()),
      m_mime_type(Exec::getMimeType_Linux(path)),
      m_path(path),
      m_min_icon_size(min_icon_size)
{
    evalSupportedImageFileTypes();
    evalMatchingApps();
}

FileMimeTypeHandler::FileMimeTypeHandler()
    : QObject(nullptr),
      m_matching_apps(std::unordered_set<AppInfo>()),
      m_mime_type(""),
      m_path(""),
      m_min_icon_size(0)
{
}

FileMimeTypeHandler::FileMimeTypeHandler(const FileMimeTypeHandler &h)
    : QObject(h.parent()),
      m_matching_apps(h.m_matching_apps),
      m_supportedImgeFileTypes(h.m_supportedImgeFileTypes),
      m_mime_type(h.m_mime_type),m_path(h.m_path),
      m_min_icon_size(h.m_min_icon_size)
{
    evalSupportedImageFileTypes();
    evalMatchingApps();
}

FileMimeTypeHandler &FileMimeTypeHandler::operator=(const FileMimeTypeHandler &fmth)
{
    this->setParent(fmth.parent());
    this->m_matching_apps = fmth.m_matching_apps;
    this->m_mime_type = fmth.m_mime_type;
    this->m_path = fmth.m_path;
    this->m_min_icon_size = fmth.m_min_icon_size;

    evalSupportedImageFileTypes();
    evalMatchingApps();

    return *this;
}


FileMimeTypeHandler::~FileMimeTypeHandler()
{
}


void FileMimeTypeHandler::evalMatchingApps()
{
    std::vector<AppInfo> apps = getAppInfosOfInstalledApps();
    std::vector<AppInfo> matching_apps;
    for(auto& app: apps)
    {
        if(app.mime_types.size() != 0 && app.mime_types.find(m_mime_type) != app.mime_types.end())
        {
            matching_apps.push_back(app);
        }
    }

//    qDebug() << "matching_apps: " << matching_apps.size() << "  mime_type: " << QString::fromStdString(m_mime_type);

    for(auto& app: matching_apps)
    {
        app.icon_file_path = getIconPath(app.icon_file_name);
        m_matching_apps.insert(app);
//        qDebug() << "   " << QString::fromStdString(app.name) << "  |   " << QString::fromStdString(app.genExecCmnd())
//                 << "   |   " << QString::fromStdString(app.icon_file_path);
    }
}

std::string FileMimeTypeHandler::getExePath() const
{
  char result[ PATH_MAX ];
  ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
  return std::string( result, (count > 0) ? static_cast<uint>(count) : 0 );
}

std::string FileMimeTypeHandler::getResourcesPath() const
{
    std::string sep = StringOps::path_separator();
    std::string exec_path = getExePath();
    return exec_path + sep + "resources";
}

std::string FileMimeTypeHandler::getMimesInfoDirPath() const
{
    std::string sep = StringOps::path_separator();
    return getResourcesPath() + sep + "mimes";
}

std::string FileMimeTypeHandler::getMimesInfoFilePath() const
{
    std::string sep = StringOps::path_separator();
    return getMimesInfoDirPath() + sep + "mime_info.txt";
}

std::vector<std::string> FileMimeTypeHandler::getDesktopFilesPaths() const
{
    std::vector<std::string> pths;
    pths.push_back("~/.local/share/applications");
    pths.push_back("/usr/local/share/applications");
    pths.push_back("/usr/share/applications");
    return pths;
}

std::vector<std::string> FileMimeTypeHandler::getIconFilePaths() const
{
    std::vector<std::string> pths;

    pths.push_back("~/.icons");

    std::vector<std::string> xdg_pths = Exec::getXDG_DATA_DIR();
    for(const auto& pth: xdg_pths)
        pths.push_back(pth);

    pths.push_back("/usr/share/pixmaps");

    return pths;
}

std::vector<AppInfo> FileMimeTypeHandler::getAppInfosOfInstalledApps() const
{
    std::vector<std::string> desktop_pths = getDesktopFilesPaths();

    std::vector<AppInfo> appInfos;

    for(const auto& desktop_pth: desktop_pths)
    {
        QFileInfoList files = ListFiles::getFilesInDirectory(QDir(QString::fromStdString(desktop_pth)), true);
        for(const auto& f: files)
        {
            std::string f_pth = f.absoluteFilePath().toStdString();

            if(StringOps::endsWith(f_pth, ".desktop"))
            {
                std::ifstream dsk_fle(f_pth);
                std::string line;
                AppInfo app;

                while(std::getline(dsk_fle, line))
                {
                    if(app.name.empty() && StringOps::startsWith(line, "Name=")){
                        app.name = StringOps::right(line, line.length() - std::string("Name=").length());
                    }
                    else if (app.exec_cmnd.empty() && StringOps::startsWith(line, "Exec=")){
                        app.exec_cmnd = StringOps::right(line, line.size() - std::string("Exec=").size());
                    }
                    else if (app.icon_file_name.empty() && StringOps::startsWith(line, "Icon=")){
                        std::string icon_name = StringOps::right(line, line.size() - std::string("Icon=").size());
                        app.icon_file_name = icon_name;
                    }
                    else if (StringOps::startsWith(line, "MimeType=")){
                        std::string mimes = StringOps::right(line, line.size() - std::string("MimeType=").size());
                        app.addMimeTypesFromVector( StringOps::split(mimes, ";") );
                    }
                }
                appInfos.push_back(app);
            }
        }
    }

//    qDebug() << "applicaionts detected: " << appInfos.size();
    return appInfos;
}

std::vector<AppInfo> FileMimeTypeHandler::matchingApps() const
{
    std::vector<AppInfo> apps(m_matching_apps.begin(), m_matching_apps.end());
    auto sortFunc = [](const AppInfo& ai1, const AppInfo& ai2){
        return ai1.name < ai2.name;
    };
    std::sort(apps.begin(), apps.end(), sortFunc);
    return apps;
}

std::unordered_set<std::string> FileMimeTypeHandler::findIconInPathsRec(const std::string &icon_name, const std::string &path) const
{
    std::unordered_set<std::string> icon_paths;
    const auto& files = ListFiles::getFilesInDirectory  (QDir(QString::fromStdString(path)), true);
    const auto& dirs  = ListFiles::getFoldersInDirectory(QDir(QString::fromStdString(path)), true);
    for(const auto& f: files)
    {
        std::string file_name_without_extension = f.baseName().toStdString();
        if(file_name_without_extension == icon_name)
        {
            std::string compl_suffix = QFileInfo(f.absoluteFilePath()).completeSuffix().toStdString();
            if(imageFileTypeIsSupported(compl_suffix))
            {
                QImage img(f.absoluteFilePath());
                if( !img.isNull() )
                {
                    icon_paths.insert(f.absoluteFilePath().toStdString());
                }
            }
        }
    }
    for(const auto& d: dirs)
    {
        const auto& ips = findIconInPathsRec(icon_name, d.absoluteFilePath().toStdString());
        for(auto& ip: ips)
        {
            icon_paths.insert(ip);
        }
    }
    return icon_paths;
}

std::string FileMimeTypeHandler::findSufficientlyBigIconInPathRec(const std::string &icon_name, const std::string &path) const
{
    const auto& files = ListFiles::getFilesInDirectory  (QDir(QString::fromStdString(path)), true);
    for(const auto& f: files)
    {
        std::string file_name_without_extension = f.baseName().toStdString();
        if(file_name_without_extension == icon_name)
        {
            std::string compl_suffix = QFileInfo(f.absoluteFilePath()).completeSuffix().toStdString();
            if(imageFileTypeIsSupported(compl_suffix))
            {
                QImage img(f.absoluteFilePath());
                if( !img.isNull() )
                {
                    auto img_sze = img.size();
                    auto sze = img_sze.width() * img_sze.height();
                    if(sze >= m_min_icon_size)
                        return f.absoluteFilePath().toStdString();
                }
            }
        }
    }
    const auto& dirs  = ListFiles::getFoldersInDirectory(QDir(QString::fromStdString(path)), true);
    for(const auto& d: dirs)
    {
        auto ip = findSufficientlyBigIconInPathRec(icon_name, d.absoluteFilePath().toStdString());
        if(!ip.empty())
            return ip;
    }
    return "";
}

std::string FileMimeTypeHandler::getIconPath(const std::string& icon_name) const
{
    std::vector<std::string> icon_file_pths = getIconFilePaths();
    for(const auto& pth: icon_file_pths)
    {
        auto matching_pth = findSufficientlyBigIconInPathRec(icon_name, pth);
        if( !matching_pth.empty() )
            return matching_pth;
    }
    return "";
}

void FileMimeTypeHandler::createMimeResourceDirIfNotExistent() const
{
    std::string sep = QString(QDir::separator()).toStdString();
    std::string exec_path = getExePath();
    QDir dir(QString::fromStdString(getResourcesPath()));
    if ( !dir.exists() )
    {
        bool succ = dir.mkdir(".");
        if (succ)
        {
            dir = QDir(QString::fromStdString(getMimesInfoDirPath()));
            if(!dir.exists())
            {
                dir.mkdir(".");
            }
        }
    }
}

void FileMimeTypeHandler::evalSupportedImageFileTypes()
{
    for(auto imf: QImageReader::supportedImageFormats())
    {
        m_supportedImgeFileTypes.emplace(imf.toStdString());
    }
}

bool FileMimeTypeHandler::imageFileTypeIsSupported(std::string &compl_suffix) const
{
    return m_supportedImgeFileTypes.find(compl_suffix) != m_supportedImgeFileTypes.end();
}
