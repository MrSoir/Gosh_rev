#include "previewiconloaderworker.h"


PreviewIconLoaderWorker::PreviewIconLoaderWorker(const std::unordered_set<std::string> &paths,
                                     QSize size,
                                     QObject *parent)
    : QueueTask(0, parent),
      m_paths(paths),
      m_path_icons(std::unordered_map<std::string, QPixmap>()),
      m_size(size)
{
}

PreviewIconLoaderWorker::PreviewIconLoaderWorker(const std::vector<std::string> &paths,
                                     QSize size,
                                     QObject *parent)
    : QueueTask(0, parent),
      m_paths(std::unordered_set<std::string>(paths.begin(), paths.end())),
      m_path_icons(std::unordered_map<std::string, QPixmap>()),
      m_size(size)
{
}

PreviewIconLoaderWorker::~PreviewIconLoaderWorker()
{
    qDebug() << "~PreviewIconLoaderWorker";
    std::unordered_set<std::string>().swap(m_paths);
    std::unordered_map<std::string, QPixmap>().swap(m_path_icons);
}

bool PreviewIconLoaderWorker::executableInParallel() const
{
    return true;
}

void PreviewIconLoaderWorker::run()
{
    loadIcons();

    if(!m_cancelled)
    {
        emit sendPreviewIcons(generatePreviewIcons());
    }
    emit finished();
}

std::vector<PreviewIcon> PreviewIconLoaderWorker::generatePreviewIcons() const
{
    std::vector<PreviewIcon> pis;
    for(const auto& it: m_path_icons)
    {
        pis.push_back(PreviewIcon(it.first, it.second));
    }
    return pis;
}

void PreviewIconLoaderWorker::loadIcons()
{
    for(const auto& path: m_paths)
    {
        if(m_cancelled)
            return;

        QString q_path = QString::fromStdString(path);
        QFileInfo fi(q_path);
        if(fi.exists())
        {
            QPixmap pixmap = STATIC_FUNCTIONS::loadPreviewIcon(path, m_size);

            if(m_cancelled)
            {
                if( !pixmap.isNull() )
                {
                    pixmap.detach();
                }
                return;
            }

            if(!pixmap.isNull())
            {
                m_path_icons[path] = pixmap;
            }
        }
    }
}
