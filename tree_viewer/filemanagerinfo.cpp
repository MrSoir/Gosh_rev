#include "filemanagerinfo.h"

FileManagerInfo::FileManagerInfo()
    : QObject(nullptr),

      m_curRootPath(tr("")),
      m_curSearchRslt(tr("")),

      m_maxDepth(0),
      m_selectionCount(0),
      m_displayedFileCount(0),
      m_indxCurSearchRslt(0),
      m_searchIndx(0),
      m_searchRsltsCnt(0),

      m_includeHiddenFiles(false),
      m_inSearchMode(false),
      m_singleFolderSelected(false),
      m_filesSelected(false),
      m_selctnCntnsZpdFle(false),

      m_depthIdElapsed(QVector<bool>())
{
}

FileManagerInfo::FileManagerInfo(const FileManagerInfo &fmi)
    : QObject (nullptr),
      m_curRootPath(fmi.m_curRootPath),
      m_curSearchRslt(fmi.m_indxCurSearchRslt),

      m_maxDepth(fmi.m_maxDepth),
      m_selectionCount(fmi.m_selectionCount),
      m_displayedFileCount(fmi.m_displayedFileCount),
      m_indxCurSearchRslt(fmi.m_indxCurSearchRslt),
      m_searchIndx(fmi.m_searchIndx),
      m_searchRsltsCnt(fmi.m_searchRsltsCnt),

      m_includeHiddenFiles(fmi.m_includeHiddenFiles),
      m_inSearchMode(fmi.m_inSearchMode),
      m_singleFolderSelected(fmi.m_singleFolderSelected),
      m_filesSelected(fmi.m_filesSelected),
      m_selctnCntnsZpdFle(fmi.m_selctnCntnsZpdFle),

      m_depthIdElapsed(fmi.m_depthIdElapsed)
{
}

//------------------------------------------------------------------

QString FileManagerInfo::curRootPath() const
{
    return m_curRootPath;
}
QString FileManagerInfo::curSearchResult() const
{
    return m_curSearchRslt;
}


int FileManagerInfo::maxDepth() const
{
    return m_maxDepth;
}
int FileManagerInfo::selectionCount() const
{
    return m_selectionCount;
}
int FileManagerInfo::displayedFileCount() const
{
    return m_displayedFileCount;
}
int FileManagerInfo::indexOfCurrentSearchResult() const
{
    return m_indxCurSearchRslt;
}
int FileManagerInfo::searchIndex() const
{
    return m_searchIndx;
}
int FileManagerInfo::searchResultsCount() const
{
    return m_searchRsltsCnt;
}


bool FileManagerInfo::includeHiddenFiles() const
{
    return m_includeHiddenFiles;
}
bool FileManagerInfo::inSearchMode() const
{
    return m_inSearchMode;
}
bool FileManagerInfo::singleFolderSelected() const
{
    return m_singleFolderSelected;
}

bool FileManagerInfo::foldersSelected() const
{
    return m_foldersSelected;
}
bool FileManagerInfo::filesSelected() const
{
    return m_filesSelected;
}
bool FileManagerInfo::selectionContainsZippedFile() const
{
    return m_selctnCntnsZpdFle;
}

bool FileManagerInfo::depthIdElapsed(int id) const
{
    return id < m_depthIdElapsed.size() ? m_depthIdElapsed[id] : false;
}

//------------------------------------------------------------------

void FileManagerInfo::curRootChanged(QString rootPath)
{
    m_curRootPath = rootPath;
    emit revalidateTreeViewer();
}

void FileManagerInfo::curSearchResultChanged(QString curSearRslt)
{
    m_curSearchRslt = curSearRslt;
    emit revalidateTreeViewer();
}

void FileManagerInfo::maxDepthChanged(int n)
{
    m_maxDepth = n;
    emit revalidateTreeViewer();
}

void FileManagerInfo::selectionCountChanged(int n)
{
    m_selectionCount = n;
    emit revalidateTreeViewer();
}

void FileManagerInfo::displayedFileCountChanged(int n)
{
    m_displayedFileCount = n;
    emit revalidateTreeViewer();
}

void FileManagerInfo::indexOfCurrentSearchResultChanged(int n)
{
    m_indxCurSearchRslt = n;
    emit revalidateTreeViewer();
}

void FileManagerInfo::searchIndexChanged(int n)
{
    m_searchIndx = n;
    emit revalidateTreeViewer();
}

void FileManagerInfo::searchResultsCountChanged(int n)
{
    m_searchRsltsCnt = n;
    emit revalidateTreeViewer();
}

void FileManagerInfo::includeHiddenFilesChanged(bool b)
{
    m_includeHiddenFiles = b;
    emit revalidateTreeViewer();
}

void FileManagerInfo::inSearchModeChanged(bool b)
{
    m_inSearchMode = b;
    emit revalidateTreeViewer();
}

void FileManagerInfo::singleFolderSelectedChanged(bool b)
{
    m_singleFolderSelected = b;
    emit revalidateTreeViewer();
}

void FileManagerInfo::foldersSelectedChanged(bool b)
{
    m_foldersSelected = b;
}

void FileManagerInfo::filesSelectedChanged(bool b)
{
    m_filesSelected = b;
    emit revalidateTreeViewer();
}

void FileManagerInfo::selectionContainsZippedFileChanged(bool b)
{
    m_selctnCntnsZpdFle = b;
    emit revalidateTreeViewer();
}

void FileManagerInfo::depthIdElapsedChanged(QVector<bool> v)
{
    m_depthIdElapsed = v;
}


