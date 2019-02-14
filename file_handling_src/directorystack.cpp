#include "directorystack.h"

DirectoryStack::DirectoryStack()
    : QObject(nullptr),
      m_dirs(std::vector<std::string>())
{
}

DirectoryStack::DirectoryStack(const DirectoryStack &ds)
    : QObject (ds.parent()),
      m_dirs(ds.m_dirs)
{
}

DirectoryStack &DirectoryStack::operator=(const DirectoryStack &ds)
{
    this->setParent(ds.parent());
    this->m_dirs = ds.m_dirs;

    return *this;
}

DirectoryStack::~DirectoryStack()
{
}

void DirectoryStack::addDir(QDir dir)
{
    addPath_hlpr(dir.absolutePath().toStdString());
}

void DirectoryStack::addPath(std::string path)
{
    addPath_hlpr(path);
}

std::string DirectoryStack::pop_back()
{
    std::string path;

    if(m_dirs.size() > 0)
    {
        path = m_dirs.front();
        m_dirs.pop_back();
    }

    if(m_dirs.size() <= 0)
    {
        emit stackIsEmpty();
    }

    return path;
}

std::size_t DirectoryStack::size() const
{
    return m_dirs.size();
}

void DirectoryStack::addPath_hlpr(std::string s)
{
    if(m_dirs.size() > 0 && m_dirs.back() == s) // es soll nicht derselbe path mehrfach hinzugefuegt werden!!!
    {
        return;
    }

    m_dirs.push_back(s);
    emit stackContainsPaths();
}
