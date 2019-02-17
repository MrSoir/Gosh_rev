#include "filemanagerinfo.h"

#include "file_handling_src/filemanager.h"

FileManagerInfo::FileManagerInfo()
    : QObject(nullptr),
      m_curRootPath(QString()),
      m_curSearchRslt(QString()),

      m_maxDepth(0),
      m_selectionCount(0),
      m_displayedFileCount(0),
      m_indxCurSearchRslt(0),
      m_searchIndx(0),
      m_searchRsltsCnt(0),

      m_includeHiddenFiles(false),
      m_inSearchMode(false),
      m_executingDeepSearch(false),
      m_foldersSelected(false),
      m_filesSelected(false),
      m_selctnCntnsZpdFle(false),

      m_depthIdElapsed(std::vector<bool>())
{
}

FileManagerInfo::FileManagerInfo(const FileManager& fmi)
    : QObject(nullptr),
      m_curRootPath(fmi.curRootPath()),
      m_curSearchRslt(fmi.curSearchResult()),

      m_maxDepth(fmi.maxDepth()),
      m_selectionCount(fmi.selectionCount()),
      m_displayedFileCount(fmi.displayedFileCount()),
      m_indxCurSearchRslt(fmi.indexCurSearchResult()),
      m_searchIndx(fmi.searchIndex()),
      m_searchRsltsCnt(fmi.searchResultsCount()),

      m_includeHiddenFiles(fmi.includeHiddenFiles()),
      m_inSearchMode(fmi.inSearchMode()),
      m_executingDeepSearch(fmi.executingDeepSearch()),
      m_foldersSelected(fmi.foldersSelected()),
      m_filesSelected(fmi.filesSelected()),
      m_selctnCntnsZpdFle(fmi.selectionContainsZipFiles()),

      m_depthIdElapsed(fmi.depthIdElapsed())
{
}

FileManagerInfo::FileManagerInfo(FileManager* fmi)
    : QObject(nullptr),
      m_curRootPath(fmi->curRootPath()),
      m_curSearchRslt(fmi->curSearchResult()),

      m_maxDepth(fmi->maxDepth()),
      m_selectionCount(fmi->selectionCount()),
      m_displayedFileCount(fmi->displayedFileCount()),
      m_indxCurSearchRslt(fmi->indexCurSearchResult()),
      m_searchIndx(fmi->searchIndex()),
      m_searchRsltsCnt(fmi->searchResultsCount()),

      m_includeHiddenFiles(fmi->includeHiddenFiles()),
      m_inSearchMode(fmi->inSearchMode()),
      m_executingDeepSearch(fmi->executingDeepSearch()),
      m_foldersSelected(fmi->foldersSelected()),
      m_filesSelected(fmi->filesSelected()),
      m_selctnCntnsZpdFle(fmi->selectionContainsZipFiles()),

      m_depthIdElapsed(fmi->depthIdElapsed())
{
}

FileManagerInfo::FileManagerInfo(const FileManagerInfo& fmi)
    : QObject (nullptr),
      m_curRootPath(fmi.m_curRootPath),
      m_curSearchRslt(fmi.m_curSearchRslt),

      m_maxDepth(fmi.m_maxDepth),
      m_selectionCount(fmi.m_selectionCount),
      m_displayedFileCount(fmi.m_displayedFileCount),
      m_indxCurSearchRslt(fmi.m_indxCurSearchRslt),
      m_searchIndx(fmi.m_searchIndx),
      m_searchRsltsCnt(fmi.m_searchRsltsCnt),

      m_includeHiddenFiles(fmi.m_includeHiddenFiles),
      m_inSearchMode(fmi.m_inSearchMode),
      m_executingDeepSearch(fmi.executingDeepSearch()),
      m_foldersSelected(fmi.m_foldersSelected),
      m_filesSelected(fmi.m_filesSelected),
      m_selctnCntnsZpdFle(fmi.m_selctnCntnsZpdFle),

      m_depthIdElapsed(fmi.m_depthIdElapsed)
{
}

FileManagerInfo& FileManagerInfo::operator=(const FileManagerInfo& fmi)
{
    m_curRootPath = fmi.m_curRootPath;
    m_curSearchRslt = fmi.m_curSearchRslt;

    m_maxDepth = fmi.m_maxDepth;
    m_selectionCount = fmi.m_selectionCount;
    m_displayedFileCount = fmi.m_displayedFileCount;
    m_indxCurSearchRslt = fmi.m_indxCurSearchRslt;
    m_searchIndx = fmi.m_searchIndx;
    m_searchRsltsCnt = fmi.m_searchRsltsCnt;

    m_includeHiddenFiles = fmi.m_includeHiddenFiles;
    m_inSearchMode = fmi.m_inSearchMode;
    m_executingDeepSearch = fmi.m_executingDeepSearch;
    m_foldersSelected = fmi.m_foldersSelected;
    m_filesSelected = fmi.m_filesSelected;
    m_selctnCntnsZpdFle = fmi.m_selctnCntnsZpdFle;

    m_depthIdElapsed = fmi.m_depthIdElapsed;

    return *this;
}

//FileManagerInfo::~FileManagerInfo()
//{

//}

//------------------------------------------------------------------

QString FileManagerInfo::curRootPath() const
{
    return m_curRootPath;
}
QString FileManagerInfo::curFocusedSearchPath() const
{
    return m_curSearchRslt;
}


int_bd FileManagerInfo::maxDepth() const
{
    return m_maxDepth;
}
int_bd FileManagerInfo::selectionCount() const
{
    return m_selectionCount;
}
int_bd FileManagerInfo::displayedFileCount() const
{
    return m_displayedFileCount;
}
int_bd FileManagerInfo::indexOfCurrentSearchResult() const
{
    return m_indxCurSearchRslt;
}
int_bd FileManagerInfo::searchIndex() const
{
    return m_searchIndx;
}
int_bd FileManagerInfo::searchResultsCount() const
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

bool FileManagerInfo::executingDeepSearch() const
{
    return m_executingDeepSearch;
}

bool FileManagerInfo::singleFolderSelected() const
{
    return m_foldersSelected;
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
    if(id < 0)
        return false;

    auto id_st = static_cast<std::size_t>(id);
    return id_st < m_depthIdElapsed.size() ? m_depthIdElapsed[id_st] : false;
}

//------------------------------------------------------------------

//void FileManagerInfo::curRootChanged(QString rootPath)
//{
//    m_curRootPath = rootPath;
//    emit revalidateTreeViewer();
//}

//void FileManagerInfo::curSearchResultChanged(QString curSearRslt)
//{
//    m_curSearchRslt = curSearRslt;
//    emit revalidateTreeViewer();
//}

//void FileManagerInfo::maxDepthChanged(int n)
//{
//    m_maxDepth = n;
//    emit revalidateTreeViewer();
//}

//void FileManagerInfo::selectionCountChanged(int n)
//{
//    m_selectionCount = n;
//    emit revalidateTreeViewer();
//}

//void FileManagerInfo::displayedFileCountChanged(int n)
//{
//    m_displayedFileCount = n;
//    emit revalidateTreeViewer();
//}

//void FileManagerInfo::indexOfCurrentSearchResultChanged(int n)
//{
//    m_indxCurSearchRslt = n;
//    emit revalidateTreeViewer();
//}

//void FileManagerInfo::searchIndexChanged(int n)
//{
//    m_searchIndx = n;
//    emit revalidateTreeViewer();
//}

//void FileManagerInfo::searchResultsCountChanged(int n)
//{
//    m_searchRsltsCnt = n;
//    emit revalidateTreeViewer();
//}

//void FileManagerInfo::includeHiddenFilesChanged(bool b)
//{
//    m_includeHiddenFiles = b;
//    emit revalidateTreeViewer();
//}

//void FileManagerInfo::inSearchModeChanged(bool b)
//{
//    m_inSearchMode = b;
//    emit revalidateTreeViewer();
//}


//void FileManagerInfo::foldersSelectedChanged(bool b)
//{
//    m_foldersSelected = b;
//}

//void FileManagerInfo::filesSelectedChanged(bool b)
//{
//    m_filesSelected = b;
//    emit revalidateTreeViewer();
//}

//void FileManagerInfo::selectionContainsZippedFileChanged(bool b)
//{
//    m_selctnCntnsZpdFle = b;
//    emit revalidateTreeViewer();
//}

//void FileManagerInfo::depthIdElapsedChanged(QVector<bool> v)
//{
//    m_depthIdElapsed = v;
//}


