#include "openfiles.h"

OpenFiles::OpenFiles(std::vector<std::string> pathsToOpen,
                     bool openWith)
    : QueueTask(),
      m_paths(pathsToOpen),
      m_openWith(openWith)
{
}

OpenFiles::OpenFiles()
    : QueueTask(),
      m_paths(std::vector<std::string>()),
      m_openWith(false)
{
}

OpenFiles::OpenFiles(const OpenFiles &of)
    : QueueTask(),
      m_paths(of.m_paths),
      m_openWith(of.m_openWith)
{
}

OpenFiles &OpenFiles::operator=(const OpenFiles &of)
{
    QueueTask::operator=(of);
    this->m_paths = of.m_paths;
    this->m_openWith = of.m_openWith;
    return *this;
}


OpenFiles::~OpenFiles()
{
    qDebug() << "~OpenFiles";
}

bool OpenFiles::executableInParallel() const
{
    return true;
}

void OpenFiles::run()
{
    open();
    emit finished();
}

void OpenFiles::open()
{
    for(const auto& path: m_paths)
    {
        if(cancelled())
            return;

        if(m_openWith)
        {
            new OpenWithDialog(path);
        }else{
            QFileInfo fi(QString::fromStdString(path));
            if(fi.isExecutable())
            {
                Process* p = new Process();
                p->start(fi.absoluteFilePath());
            }else{
                QDesktopServices::openUrl(QUrl(QString("file:%1").arg(fi.absoluteFilePath())));
            }
        }
    }
}
