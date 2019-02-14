#include "fibdviewer.h"

FiBDViewer::FiBDViewer()
    : QObject(),
      Searchable(),
      Selectable(),
      m_path(QString()),
      m_fileName(QString()),

      m_folder(false),
      m_searched(false),
      m_searchFocused(false),
      m_selected(false),
      m_isLoaded(false),
      m_isElapsed(false),
      m_isHidden(false),

      m_filesCount(0),
      m_subDirCount(0),

      m_depthId(0),

      m_order(Order()),

      m_fileSize(0),
      m_lastModified(QDateTime()),

      m_isCurrentlyRevalidating(false)
{
}

FiBDViewer::FiBDViewer(QString path,
                       QString fileName,
                       unsigned long long fileSize,
                       QDateTime lastModified,
                       bool isHidden,
                       int depthId)
    : QObject(),
      Searchable(),
      Selectable(),
      m_path(path),
      m_fileName(fileName),

      m_folder(false),
      m_searched(false),
      m_searchFocused(false),
      m_selected(false),
      m_isLoaded(false),
      m_isElapsed(false),
      m_isHidden(isHidden),

      m_filesCount(0),
      m_subDirCount(0),

      m_depthId(depthId),

      m_order(Order()),

      m_fileSize(fileSize),
      m_lastModified(lastModified),

      m_isCurrentlyRevalidating(false)
{
}

FiBDViewer::FiBDViewer(const QFileInfo &fi, int depthId)
    : QObject(),
      Searchable(),
      Selectable(),
      m_path(fi.absoluteFilePath()),
      m_fileName(fi.fileName()),

      m_folder(false),
      m_searched(false),
      m_searchFocused(false),
      m_selected(false),
      m_isLoaded(false),
      m_isElapsed(false),
      m_isHidden(fi.isHidden()),

      m_filesCount(0),
      m_subDirCount(0),

      m_depthId(depthId),

      m_order(Order()),

      m_fileSize( static_cast<unsigned long long>(fi.size()) ),
      m_lastModified(fi.lastModified()),

      m_isCurrentlyRevalidating(false)
{
}

FiBDViewer::FiBDViewer(const DirManagerInfo& dmi,
                       int depthId,
                       bool isCurrentlyRevalidating,
                       bool isSearched,
                       bool isSearchFocused,
                       bool isSelected)
    : QObject(),
      Searchable(),
      Selectable(),
      m_path(QString::fromStdString(dmi.absPath)),
      m_fileName(QString::fromStdString(dmi.fileName)),
      m_folder(true),
      m_searched(isSearched),
      m_searchFocused(isSearchFocused),
      m_selected(isSelected),
      m_isLoaded(dmi.isLoaded),
      m_isElapsed(dmi.isElapsed),
      m_isHidden(dmi.isHidden),
      m_filesCount(dmi.files_sorted.size()),
      m_subDirCount(dmi.subDirs_sorted.size()),
      m_depthId(depthId),
      m_order(dmi.order),
      m_fileSize(0),
      m_lastModified(dmi.lastModified),
      m_isCurrentlyRevalidating(isCurrentlyRevalidating)
{
}

FiBDViewer::FiBDViewer(DirManagerInfo* dmi,
                       int depthId,
                       bool isCurrentlyRevalidating,
                       bool isSearched,
                       bool isSearchFocused,
                       bool isSelected)
    : QObject(),
      Searchable(),
      Selectable(),
      m_path(QString::fromStdString(dmi->absPath)),
      m_fileName(QString::fromStdString(dmi->fileName)),
      m_folder(true),
      m_searched(isSearched),
      m_searchFocused(isSearchFocused),
      m_selected(isSelected),
      m_isLoaded(dmi->isLoaded),
      m_isElapsed(dmi->isElapsed),
      m_isHidden(dmi->isHidden),
      m_filesCount(dmi->files_sorted.size()),
      m_subDirCount(dmi->subDirs_sorted.size()),
      m_depthId(depthId),
      m_order(dmi->order),
      m_fileSize(0),
      m_lastModified(dmi->lastModified),
      m_isCurrentlyRevalidating(isCurrentlyRevalidating)
{
}

FiBDViewer::FiBDViewer(const FiBDViewer& fi)
    : QObject(fi.parent()),
      Searchable(),
      Selectable(),
      m_path(fi.m_path),
      m_fileName(fi.fileName()),

      m_folder(fi.m_folder),
      m_searched(fi.m_searched),
      m_searchFocused(fi.m_searchFocused),
      m_selected(fi.m_selected),
      m_isLoaded(fi.m_isLoaded),
      m_isElapsed(fi.m_isElapsed),
      m_isHidden(fi.isHidden()),

      m_filesCount(fi.m_filesCount),
      m_subDirCount(fi.m_subDirCount),
      m_depthId(fi.m_depthId),

      m_order(fi.m_order),

      m_fileSize(fi.m_fileSize),
      m_lastModified(fi.m_lastModified),

      m_isCurrentlyRevalidating(fi.m_isCurrentlyRevalidating)
{
}

FiBDViewer::~FiBDViewer()
{
}

FiBDViewer& FiBDViewer::operator=(const FiBDViewer &fi)
{
    this->setParent(fi.parent());
    this->m_path = fi.m_path;
    this->m_fileName = fi.m_fileName;

    this->m_folder = fi.m_folder;
    this->m_searched = fi.m_searched;
    this->m_searchFocused = fi.m_searchFocused;
    this->m_selected = fi.m_selected;
    this->m_isLoaded = fi.m_isLoaded;
    this->m_isElapsed = fi.m_isElapsed;
    this->m_isHidden = fi.m_isHidden;

    this->m_filesCount = fi.m_filesCount;
    this->m_subDirCount = fi.m_subDirCount;
    this->m_depthId = fi.m_depthId;

    this->m_order = fi.m_order;

    this->m_fileSize = fi.m_fileSize;
    this->m_lastModified = fi.m_lastModified;

    this->m_isCurrentlyRevalidating = fi.m_isCurrentlyRevalidating;

    return *this;
}


void FiBDViewer::setSearched(bool searched)
{
    m_searched = searched;
}
void FiBDViewer::setSearchFocused(bool focused)
{
    m_searchFocused = focused;
}
bool FiBDViewer::searched() const
{
    return m_searched;
}
bool FiBDViewer::searchFocused() const
{
    return m_searchFocused;
}

void FiBDViewer::setSelected(bool selected)
{
    m_selected = selected;
}

std::string FiBDViewer::path() const
{
    return m_path.toStdString();
}

QString FiBDViewer::q_path() const
{
    return m_path;
}

QString FiBDViewer::fileName() const
{
    return m_fileName;
}

unsigned long long FiBDViewer::fileSize() const
{
    return m_fileSize;
}

QDateTime FiBDViewer::lastModified() const
{
    return m_lastModified;
}

bool FiBDViewer::isCurrentlyRevalidating() const
{
    return m_isCurrentlyRevalidating;
}
bool FiBDViewer::selected() const
{
    return m_selected;
}


bool FiBDViewer::isFolder() const
{
    return m_folder;
}

bool FiBDViewer::isLoaded() const
{
    return m_isLoaded;
}

bool FiBDViewer::isElapsed() const
{
    return m_isElapsed;
}

bool FiBDViewer::isHidden() const
{
    return m_isHidden;
}

bool FiBDViewer::containsFiles() const
{
    return m_filesCount > 0;
}

bool FiBDViewer::isEmpty() const
{
    return !(m_filesCount > 0 || m_subDirCount > 0);
}

bool FiBDViewer::isReversedSortedBy(Order ord) const
{
    return ord.ordered_by == m_order.ordered_by && m_order.reversedOrdered;
}

bool FiBDViewer::isSortedBy(ORDERED_BY ordered_by) const
{
    return m_order.ordered_by == ordered_by;
}

Order FiBDViewer::order() const
{
    return m_order;
}

int FiBDViewer::depthId() const
{
    return m_depthId;
}

//std::string FiBDViewer::path()
//{
//    return m_path;
//}
