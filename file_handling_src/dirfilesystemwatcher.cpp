#include "dirfilesystemwatcher.h"

DirFileSystemWatcher::DirFileSystemWatcher(QObject* parent)
    : QObject(parent),
      m_watcher(new QFileSystemWatcher(this)),
      m_dirs_watched(std::vector<std::string>())
{
    connectSignals();
}

DirFileSystemWatcher::DirFileSystemWatcher(const DirFileSystemWatcher &dfs)
    : QObject(dfs.parent()),
      m_watcher(dfs.m_watcher),
      m_dirs_watched(dfs.m_dirs_watched)
{
    connectSignals();
}

DirFileSystemWatcher &DirFileSystemWatcher::operator=(const DirFileSystemWatcher &dfsw)
{
    this->setParent(dfsw.parent());
    m_watcher = dfsw.m_watcher;
    m_dirs_watched = dfsw.m_dirs_watched;

    connectSignals();

    return *this;
}


DirFileSystemWatcher::~DirFileSystemWatcher()
{
}

void DirFileSystemWatcher::addDir(const std::string& dir_path)
{
    m_watcher->addPath(QString::fromStdString(dir_path));
    m_dirs_watched.push_back(dir_path);
}

void DirFileSystemWatcher::addDirs(const std::vector<std::string>& dir_paths)
{
    for(const auto& dir_path: dir_paths)
    {
        m_watcher->addPath(QString::fromStdString(dir_path));
        m_dirs_watched.push_back(dir_path);
    }
}

void DirFileSystemWatcher::clearDirs()
{
    m_watcher->removePaths(m_watcher->directories());
    m_dirs_watched.clear();
}

void DirFileSystemWatcher::connectSignals()
{
    connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, &DirFileSystemWatcher::dirChanged_hlpr);
}

void DirFileSystemWatcher::dirChanged_hlpr(const QString& q_dir)
{
    emit dirChanged(q_dir.toStdString());
}
