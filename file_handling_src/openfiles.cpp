#include "openfiles.h"

OpenFiles::OpenFiles(std::vector<std::string> pathsToOpen,
                     bool openWith)
    : QueueTask(),
      m_paths(pathsToOpen),
      m_openWith(openWith)
{
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
