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

//      m_previewIcons(QMap<QString, QImage>())
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

//      m_previewIcons(fmi.getPreviewIcons())
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

//      m_previewIcons(fmi->getPreviewIcons())
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

//      m_previewIcons(fmi.m_previewIcons)
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

//    m_previewIcons = fmi.m_previewIcons;

    return *this;
}

FileManagerInfo::~FileManagerInfo()
{
    std::vector<bool>().swap(m_depthIdElapsed);

//    for(auto it = m_previewIcons.begin(); it != m_previewIcons.end(); ++it)
//    {
//        if(it.isNull())
//        {
//            it.detach();
//        }
//    }
//    QMap<QString, QImage>().swap(m_previewIcons);
}

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
    return m_foldersSelected && m_selectionCount == 1;
}
bool FileManagerInfo::singleFileSelected() const
{
    return !m_foldersSelected && m_selectionCount == 1;
}
bool FileManagerInfo::singleEntrySelected() const
{
    return ( singleFolderSelected() && !singleFileSelected()) ||
           (!singleFolderSelected() &&  singleFileSelected());
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

//QImage FileManagerInfo::getPreviewIcon(const QString &path)
//{
//    return getPreviewIcon(path.toStdString());
//}

//QImage FileManagerInfo::getPreviewIcon(const string &path)
//{
//    if(containsPreviewIcon(path))
//    {
//       return  m_previewIcons[path];
//    }
//    return QImage();
//}

//bool FileManagerInfo::containsPreviewIcon(const QString &path)
//{
////    return containsPreviewIcon(path.toStdString());
//    return m_previewIcons.find(path) != m_previewIcons.end();
//}

//bool FileManagerInfo::containsPreviewIcon(const string &path)
//{
////    return m_previewIcons.find(path) != m_previewIcons.end();
//    return containsPreviewIcon(QString::fromStdString(path));
//}

//const QMap<QString, QImage> &FileManagerInfo::getPreviewIcons() const
//{
//    return m_previewIcons;
//}

