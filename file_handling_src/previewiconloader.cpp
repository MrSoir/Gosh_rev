#include "previewiconloader.h"

PreviewIconLoader::PreviewIconLoader(QSize size,
                                   QObject *parent)
    : QObject(parent),
      m_size(size)
{
}

PreviewIconLoader::~PreviewIconLoader()
{
    qDebug() << "~PreviewIconLoader";
    for(auto it = m_path_icon.begin(); it != m_path_icon.end(); ++it)
    {
        (*it).second.detach();
    }

    std::queue<std::string>().swap(m_paths_queue);
    std::unordered_map<std::string, QPixmap>().swap(m_path_icon);
}

std::unordered_map<string, QPixmap> PreviewIconLoader::getPreviewIconsMap(const std::unordered_set<string> &paths) const
{
    std::unordered_map<string, QPixmap> prevIcns;
    for(const auto& path: paths)
    {
        auto it = m_path_icon.find(path);
        if(it != m_path_icon.end())
        {
            prevIcns[path] = it->second;
        }
    }
    return prevIcns;
}

void PreviewIconLoader::loadIcon(const std::string &path)
{
    loadIcons_Hlpr({path});
}

void PreviewIconLoader::loadIcons(const std::vector<std::string>& paths)
{
    loadIcons_Hlpr(paths);
}

void PreviewIconLoader::loadIcons(const std::unordered_set<string> &paths)
{
    loadIcons_Hlpr(std::vector<std::string>(paths.begin(), paths.end()));
}

void PreviewIconLoader::receiveIcons(std::vector<PreviewIcon> previewIcons)
{
    if(m_closed)
    {
        for(auto& pi: previewIcons)
        {
            pi.icon.detach();
        }
        std::vector<PreviewIcon>().swap(previewIcons);
        return;
    }

    for(const auto& pi: previewIcons)
    {
        addIcon(pi);
    }
    emit newIconsLoaded();
}

void PreviewIconLoader::close()
{
    m_closed = true;
    emit cancelWorkers();
    this->deleteLater();
}

void PreviewIconLoader::loadIcons_Hlpr(const std::vector<std::string>& paths)
{
    auto icnsToLoad = std::min(m_path_icon.size() + paths.size(), ICON_LIMIT - m_path_icon.size());
    if(icnsToLoad > 0)
    {
        auto pathsToLoad = std::vector<std::string>(paths.begin(), paths.begin() + static_cast<int>(icnsToLoad));
        PreviewIconLoaderWorker* worker = new PreviewIconLoaderWorker(pathsToLoad, m_size);
        connect(worker, &PreviewIconLoaderWorker::sendPreviewIcons, this, &PreviewIconLoader::receiveIcons, Qt::QueuedConnection);
        connect(this, &PreviewIconLoader::cancelWorkers, worker, &PreviewIconLoaderWorker::cancel, Qt::DirectConnection);
        worker->execute();
    }
}

void PreviewIconLoader::addIcon(const PreviewIcon& pi)
{
    if(pi.icon.isNull())
        return;

//    bool iconAlreadyRegistered = m_path_icon.find(pi.path) == m_path_icon.end();
    m_path_icon[pi.path] = pi.icon;
    m_paths_queue.push(pi.path);
    while(m_paths_queue.size() > ICON_LIMIT)
    {
        m_paths_queue.pop();
    }
}
