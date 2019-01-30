#include "fileinfobd.h"


//----------------------------------------------------------------


FileInfoBD::FileInfoBD(const std::string& path,
                       FileInfoBD* parentFiBD,
                       bool showHiddenFiles,
                       QObject *parent)
    : QObject(parent),      

      m_isElapsed(false),
      m_alrLoaded(false),

      m_fileInfo(QFileInfo(QString(path.c_str()))),

      m_files(std::unordered_map<std::string, QFileInfo>()),
      m_sub_folds(std::unordered_set<FileInfoBD*>()),
      m_sub_fold_names(std::unordered_map<std::string, FileInfoBD*>()),
      m_hidden_files(std::unordered_set<std::string>()),
      m_hidden_folds(std::unordered_set<std::string>()),

      m_filesCount(0),
      m_subFoldsCount(0),
      m_contentCount(0),

      m_showHiddenFiles(showHiddenFiles),
      m_disableSignals(false),

      m_parent(parentFiBD),

      m_cancelled_elapsing(false),
      m_cancelled_collapsing(false),
      m_cancelled_hidingHiddenFiles(false),

      m_order(Order()),
      m_current_ordering(Order())
{
}
FileInfoBD::FileInfoBD(const QFileInfo& fileInfo,
                    FileInfoBD* parentFiBD,
                    bool showHiddenFiles,
                    QObject *parent)
    : QObject(parent),

      m_isElapsed(false),
      m_alrLoaded(false),

      m_fileInfo(fileInfo),

      m_files(std::unordered_map<std::string, QFileInfo>()),
      m_sub_folds(std::unordered_set<FileInfoBD*>()),
      m_sub_fold_names(std::unordered_map<std::string, FileInfoBD*>()),
      m_hidden_files(std::unordered_set<std::string>()),
      m_hidden_folds(std::unordered_set<std::string>()),

      m_filesCount(0),
      m_subFoldsCount(0),
      m_contentCount(0),

      m_showHiddenFiles(showHiddenFiles),
      m_disableSignals(false),

      m_parent(parentFiBD),

      m_cancelled_elapsing(false),
      m_cancelled_collapsing(false),
      m_cancelled_hidingHiddenFiles(false),

      m_order(Order()),
      m_current_ordering(Order())
{
}

FileInfoBD::FileInfoBD(const FileInfoBD &fi)
    : QObject(fi.parent()),

      m_isElapsed(fi.m_isElapsed),
      m_alrLoaded(fi.m_alrLoaded),

      m_fileInfo(fi.m_fileInfo),

      m_files(fi.m_files),
      m_sub_folds(fi.m_sub_folds),
      m_sub_fold_names(fi.m_sub_fold_names),
      m_hidden_files(fi.m_hidden_files),
      m_hidden_folds(fi.m_hidden_folds),

      m_filesCount(fi.m_filesCount),
      m_subFoldsCount(fi.m_subFoldsCount),
      m_contentCount(fi.m_contentCount),

      m_showHiddenFiles(fi.m_showHiddenFiles),
      m_disableSignals(fi.m_disableSignals),

      m_parent(fi.m_parent),

      m_cancelled_elapsing(false),
      m_cancelled_collapsing(false),
      m_cancelled_hidingHiddenFiles(false),

      m_order(fi.m_order),
      m_current_ordering(fi.m_current_ordering)
{
}

FileInfoBD::~FileInfoBD()
{
    clearContainers();

    // removeSubFolder wird nicht verwendet, denn der FileSystemViewer wird beim loeschen eines folders
    // nur eine aenderung im parent-folder abfanden, nicht im zu loeschenden FileInfoBD selbst.
    // daher eliminiert der parent-folder ganz automatisch den zu loeschenden sub-folder
//    if(m_parent)
//        m_parent->removeSubFolder(this);
    m_parent = nullptr;

    // subolder werden nicht mehr hier deleted, denn das wird bereits in der FileInfoBD::close() erledigt:
//    for(auto it = m_sub_folds.begin(); it != m_sub_folds.end(); ++it)
//    {
//        delete (*it);
//    }
}

//-----------------------------------------------------

bool FileInfoBD::isElapsed() const
{
    return m_isElapsed;
}
bool FileInfoBD::isLoaded() const
{
    return m_alrLoaded;
}

unsigned long FileInfoBD::getFileCount() const
{
    return m_filesCount;
}
unsigned long FileInfoBD::getFolderCount() const
{
    return m_subFoldsCount;
}
unsigned long FileInfoBD::getContentCount() const
{
    return m_contentCount;
}

unsigned long FileInfoBD::getShownFileCount() const
{
    return m_filesCount - (!m_showHiddenFiles ? m_hidden_files.size() : 0);
}
unsigned long FileInfoBD::getShownFolderCount() const
{
    return m_subFoldsCount - (!m_showHiddenFiles ? m_hidden_folds.size() : 0);
}
unsigned long FileInfoBD::getShownContentCount() const
{
    return m_contentCount - (!m_showHiddenFiles ? m_hidden_files.size() + m_hidden_folds.size() : 0);
}

void FileInfoBD::disableSignals(bool disableSignals)
{
    m_disableSignals = disableSignals;
}

void FileInfoBD::close()
{
    m_contentCount  = 0;
    m_filesCount    = 0;
    m_subFoldsCount = 0;

    m_disableSignals = true;
    for(auto it = m_sub_folds.begin(); it != m_sub_folds.end(); )
    {
        (*it)->close();
        delete (*it);
        it = m_sub_folds.erase(it);
    }
    m_sub_folds.clear();
    m_files.clear();
    m_sub_fold_names.clear();
    m_hidden_files.clear();
    m_hidden_folds.clear();

    emit closingFinished();
}

std::string FileInfoBD::fileName() const
{
    return m_fileInfo.fileName().toStdString();
}
std::string FileInfoBD::absPath() const
{
    return m_fileInfo.absoluteFilePath().toStdString();
}

void FileInfoBD::print() const
{
    qDebug() << m_fileInfo.fileName();
    int i=0;
    for(auto it = m_files.begin(); it != m_files.end(); ++it, ++i)
    {
        qDebug() << "file[" << i << "]: " << QString::fromStdString(it->first);
    }
    for(auto it = m_sub_folds.begin(); it != m_sub_folds.end(); ++it)
    {
        (*it)->print();
    }
}

void FileInfoBD::iterate(std::function<void(FileInfoBD*,
                                            std::string,
                                            std::string,
                                            bool,
                                            bool,
                                            bool,
                                            FileInfoBD*,
                                            int depthId)> f,
                         int initDepthId)
{
    bool isCollapsed = m_parent ? m_parent->m_isElapsed : false;
    FileInfoBD* fncf = this; // fncf = firstNonCollapsedFold
    FileInfoBD* pit = this; // parent_iterator
    while(pit)
    {
        if(!pit || pit->m_isElapsed)
        {
            break;
        }
        fncf = pit;
        pit = pit->m_parent;

    }
    iterate_helper(f, isCollapsed, fncf, initDepthId);
}
void FileInfoBD::iterate_helper(std::function<void(FileInfoBD*,
                                                   std::string,
                                                   std::string,
                                                   bool,
                                                   bool,
                                                   bool,
                                                   FileInfoBD* firstNonCollapsedFold,
                                                   int depthId)> f,
                                bool isCollapsed,
                                FileInfoBD* firstNonCollapsedFold,
                                int depthId)
{
    FileInfoBD* fncf = m_isElapsed ? this : firstNonCollapsedFold;

    bool isHidden = m_fileInfo.isHidden() && !m_showHiddenFiles;

    f(this, absPath(), fileName(), true, isCollapsed, isHidden, fncf, depthId);

    for(auto it=m_sub_folds.begin(); it != m_sub_folds.end(); ++it)
    {
        (*it)->iterate_helper(f, isCollapsed || !m_isElapsed, fncf, depthId+1);
    }
    for(auto it: m_files)
    {
        std::string filePath = it.first;
        std::string fileName = it.second.fileName().toStdString();

        bool file_isHidden = ((m_hidden_files.find(filePath) != m_hidden_files.end()) && !m_showHiddenFiles);

        f(this, filePath, fileName, false, isCollapsed, file_isHidden, fncf, depthId+1);
    }
}

void FileInfoBD::iterateOverFolders(std::function<void (FileInfoBD *)> f)
{
    f(this);
    for(auto* subFolder: m_sub_folds)
        subFolder->iterateOverFolders(f);
}

const std::vector<std::string> &FileInfoBD::getSortedFiles() const
{
    return m_showHiddenFiles ? m_sortedFilePaths_incl_hidden : m_sortedFilePaths_no_hidden;
}

const std::vector<std::string> &FileInfoBD::getSortedFolds() const
{
    return m_showHiddenFiles ? m_sortedFoldPaths_incl_hidden : m_sortedFoldPaths_no_hidden;
}

const std::vector<std::string> &FileInfoBD::getSortedEntries() const
{
    return m_showHiddenFiles ? m_sortedEntryPaths_incl_hidden : m_sortedEntryPaths_no_hidden;
}
//-----------------------------------------------------

// elapsing-functions:
void FileInfoBD::elapse()
{
    resetCancelFlags();

    elapse_hlpr();
    emit elapsingFinished();
}
void FileInfoBD::elapse_rec()
{
    resetCancelFlags();

    elapse_hlpr();

    for(auto it=m_sub_folds.begin(); it != m_sub_folds.end(); ++it)
    {
        if(m_cancelled_elapsing)
            break;

        (*it)->elapse_rec();
    }
    emit elapsingFinished();
}
void FileInfoBD::elapse_hlpr()
{
    m_isElapsed = true;
    doElapsing();
    checkIfSortingIsStillValid();
}

void FileInfoBD::cancelElapsing()
{
    m_cancelled_elapsing = true;
    for(auto it=m_sub_folds.begin(); it != m_sub_folds.end(); ++it)
    {
        (*it)->cancelElapsing();
    }
}

// collapsing-functions:
void FileInfoBD::collapse()
{
    resetCancelFlags();

    collapse_hlpr();
    emit collapsingFinished();
}
void FileInfoBD::collapse_rec()
{
    resetCancelFlags();

    collapse_hlpr();

    for(auto it=m_sub_folds.begin(); it != m_sub_folds.end(); ++it)
    {
        if(m_cancelled_collapsing)
            break;

        (*it)->collapse_rec();
    }
    emit collapsingFinished();
}
void FileInfoBD::collapse_hlpr()
{
    m_isElapsed = false;
    doElapsing();
}

void FileInfoBD::cancelCollapsing()
{
    m_cancelled_collapsing = true;
    for(auto it=m_sub_folds.begin(); it != m_sub_folds.end(); ++it)
    {
        (*it)->cancelCollapsing();
    }
}

// show-hide-hiddenFiles-functions:
void FileInfoBD::showHiddenFiles()
{
    resetCancelFlags();

    if( !m_showHiddenFiles )
    {
        showHiddenFiles_hlpr();
    }
    emit showingHiddenFilesFinished();
}
void FileInfoBD::hideHiddenFiles()
{
    resetCancelFlags();

    if(m_showHiddenFiles)
    {
        hideHiddenFiles_hlpr();
    }
    emit hidingHiddenFilesFinished();
}
void FileInfoBD::showHiddenFiles_rec()
{
    resetCancelFlags();

    showHiddenFiles_hlpr();

    for(auto it=m_sub_folds.begin(); it != m_sub_folds.end(); ++it)
    {
        if(m_cancelled_hidingHiddenFiles)
            break;

        (*it)->showHiddenFiles_rec();
    }
    emit showingHiddenFilesFinished();
}
void FileInfoBD::hideHiddenFiles_rec()
{
    resetCancelFlags();

    hideHiddenFiles_hlpr();
    for(auto it=m_sub_folds.begin(); it != m_sub_folds.end(); ++it)
    {
        if(m_cancelled_hidingHiddenFiles)
            break;

        (*it)->hideHiddenFiles_rec();
    }
    emit hidingHiddenFilesFinished();
}
void FileInfoBD::showHiddenFiles_hlpr()
{
    m_showHiddenFiles = true;
}
void FileInfoBD::hideHiddenFiles_hlpr()
{
    m_showHiddenFiles = false;
}

void FileInfoBD::cancelHidingHiddenFiles()
{
    m_cancelled_hidingHiddenFiles = true;
    for(auto it=m_sub_folds.begin(); it != m_sub_folds.end(); ++it)
    {
        (*it)->cancelHidingHiddenFiles();
    }
}

void FileInfoBD::revalidate()
{
    m_cancelled_revalidation = false;

    revalFolderContent();
    emit revalidationFinished();
}

void FileInfoBD::cancel_revalidation()
{
    m_cancelled_revalidation = true;
}


void FileInfoBD::sortFolder(Order order)
{
    resetCancelFlags();
    sortFolder_hlpr(order);
    emit sortingFinished();
}
void FileInfoBD::sortFolder_rec(Order order)
{
    resetCancelFlags();
    sortFolder_hlpr_rec(order);
    emit sortingFinished();
}
void FileInfoBD::cancelSorting()
{
    m_cancelled_sorting = true;
    for(auto it=m_sub_folds.begin(); it != m_sub_folds.end(); ++it)
    {
        (*it)->cancelSorting();
    }
}
void FileInfoBD::sortFolder_hlpr(Order order)
{
    m_order = order;
    if(m_cancelled_sorting)
        return;
    doSorting();
}
void FileInfoBD::sortFolder_hlpr_rec(Order order)
{
    sortFolder_hlpr(order);
    for(auto it=m_sub_folds.begin(); it != m_sub_folds.end() && !m_cancelled_sorting; ++it)
    {
        (*it)->sortFolder_hlpr_rec(order);
    }
}
void FileInfoBD::doSorting()
{
    m_sortedFilePaths_incl_hidden.clear();
    m_sortedFoldPaths_incl_hidden.clear();
    m_sortedEntryPaths_incl_hidden.clear();
    m_sortedFilePaths_no_hidden.clear();
    m_sortedFoldPaths_no_hidden.clear();
    m_sortedEntryPaths_no_hidden.clear();

    m_current_ordering = m_order;

//    1. sort folders:
    std::vector<FileInfoBD*> subFolds(m_sub_folds.begin(), m_sub_folds.end());

    if(m_order.ordered_by == ORDERED_BY::NAME)
    {
        auto sortFunc = genSortingFunction<FileInfoBD*, std::string>([](FileInfoBD* fi){return fi->fileName();});
        std::sort(subFolds.begin(), subFolds.end(), sortFunc);
    }else if(m_order.ordered_by == ORDERED_BY::SIZE)
    {
        auto sortFunc = genSortingFunction<FileInfoBD*, qint64>([](FileInfoBD* fi){return fi->m_fileInfo.size();});
        std::sort(subFolds.begin(), subFolds.end(), sortFunc);
    }else if(m_order.ordered_by == ORDERED_BY::TYPE)
    {
        auto sortFunc = genSortingFunction<FileInfoBD*, std::string>([](FileInfoBD* fi){return fi->m_fileInfo.completeSuffix().toStdString();});
        std::sort(subFolds.begin(), subFolds.end(), sortFunc);
    }else if(m_order.ordered_by == ORDERED_BY::MOD_DATE)
    {
        auto sortFunc = genSortingFunction<FileInfoBD*, qint64>([](FileInfoBD* fi){return fi->m_fileInfo.lastModified().toMSecsSinceEpoch();});
        std::sort(subFolds.begin(), subFolds.end(), sortFunc);
    }

    m_sortedFoldPaths_incl_hidden.reserve(subFolds.size());
    m_sortedFoldPaths_no_hidden.reserve(subFolds.size()); // nicht schaedlich, wenn mehr reserviert als spater tats. belegt wird -> std::vector::size() gibt dann lediglich die tats. anzahl elemente an, nicht die anzahl, die in std::vector::reserve gesetzt wurde!
    if(m_order.reversedOrdered)
    {
        for(std::vector<FileInfoBD*>::reverse_iterator it = subFolds.rbegin(); it != subFolds.rend(); ++it)
        {
            std::string path = (*it)->absPath();
            m_sortedFoldPaths_incl_hidden.push_back(path);
            if(m_hidden_folds.find(path) == m_hidden_folds.end())
                m_sortedFoldPaths_no_hidden.push_back(path);
        }
    }else{
        for(auto* fold: subFolds){
            std::string path = fold->absPath();
            m_sortedFoldPaths_incl_hidden.push_back(path);
            if(m_hidden_folds.find(path) == m_hidden_folds.end())
                m_sortedFoldPaths_no_hidden.push_back(path);
        }
    }

//---------------------------------------------------------

//    2. sort files:
    std::vector<const QFileInfo*> files(m_files.size());
    // nur die tatsaechlich nicht "hidden" files fuers sorting beruecksichtigen:
    for(const auto& it: m_files)
    {
        if(m_showHiddenFiles || (m_hidden_files.find(it.first) == m_hidden_files.end()) )
            files.push_back(&(it.second));
    }

    if(m_order.ordered_by == ORDERED_BY::NAME)
    {
        auto sortFunc = genSortingFunction<const QFileInfo*, std::string>([](const QFileInfo* fi){return fi->fileName().toStdString();});
        std::sort(files.begin(), files.end(), sortFunc);
    }else if(m_order.ordered_by == ORDERED_BY::SIZE)
    {
        auto sortFunc = genSortingFunction<const QFileInfo*, qint64>([](const QFileInfo* fi){return fi->size();});
        std::sort(files.begin(), files.end(), sortFunc);
    }else if(m_order.ordered_by == ORDERED_BY::TYPE)
    {
        auto sortFunc = genSortingFunction<const QFileInfo*, std::string>([](const QFileInfo* fi){return fi->completeSuffix().toStdString();});
        std::sort(files.begin(), files.end(), sortFunc);
    }else if(m_order.ordered_by == ORDERED_BY::MOD_DATE)
    {
        auto sortFunc = genSortingFunction<const QFileInfo*, qint64>([](const QFileInfo* fi){return fi->lastModified().toMSecsSinceEpoch();});
        std::sort(files.begin(), files.end(), sortFunc);
    }

    m_sortedFilePaths_incl_hidden.reserve(files.size());
    m_sortedFilePaths_no_hidden.reserve(files.size());
    if(m_order.reversedOrdered)
    {
        for(std::vector<const QFileInfo*>::reverse_iterator it = files.rbegin(); it != files.rend(); ++it)
        {
            std::string path = (*it)->absoluteFilePath().toStdString();
            m_sortedFilePaths_incl_hidden.push_back(path);
            if(m_hidden_files.find(path) == m_hidden_files.end())
                m_sortedFilePaths_no_hidden.push_back(path);
        }
    }else{
        for(auto& file: files)
        {
            std::string path = file->absoluteFilePath().toStdString();
            m_sortedFilePaths_incl_hidden.push_back(path);
            if(m_hidden_files.find(path) == m_hidden_files.end())
                m_sortedFilePaths_no_hidden.push_back(path);
        }
    }
    m_sortedEntryPaths_incl_hidden.reserve(m_sortedFilePaths_incl_hidden.size() + m_sortedFoldPaths_incl_hidden.size());
    for(auto& path: m_sortedFilePaths_incl_hidden)
        m_sortedEntryPaths_incl_hidden.push_back(path);
    for(auto& path: m_sortedFoldPaths_incl_hidden)
        m_sortedEntryPaths_incl_hidden.push_back(path);

    m_sortedEntryPaths_no_hidden.reserve(m_sortedFilePaths_no_hidden.size() + m_sortedFoldPaths_no_hidden.size());
    for(auto& path: m_sortedFilePaths_no_hidden)
        m_sortedEntryPaths_no_hidden.push_back(path);
    for(auto& path: m_sortedFoldPaths_no_hidden)
        m_sortedEntryPaths_no_hidden.push_back(path);
}

void FileInfoBD::checkIfSortingIsStillValid()
{
    if(m_current_ordering != m_order)
    {
        sortFolder(m_order);
    }
}
template<typename T, typename K>
std::function<bool(T,T)> FileInfoBD::genSortingFunction(std::function<K(T)> characteristicGetter) const
{
    return [=](T t1, T t2){return characteristicGetter(t1) < characteristicGetter(t2);};
}
//-----------------------------------------------------

void FileInfoBD::revalFolderContent()
{
    if(m_cancelled_revalidation)
        return;

    QDir dir(m_fileInfo.absoluteFilePath());

    clearContainers();

    QFileInfoList filesLst = ListFiles::getFilesInDirectory(dir, true);
    for(int i=0; i < filesLst.size(); ++i)
    {
        m_files.emplace(std::make_pair(filesLst[i].absoluteFilePath().toStdString(), filesLst[i]));
        if(filesLst[i].isHidden())
            m_hidden_files.emplace(filesLst[i].absoluteFilePath().toStdString());
    }

    QFileInfoList foldersLst = ListFiles::getFoldersInDirectory(dir, true);

    std::unordered_set<std::string> newFolders_paths;

    for(int i=0; i < foldersLst.size(); ++i)
    {
//        if(m_cancelled_revalidation)
//            return;
        newFolders_paths.emplace( foldersLst[i].absoluteFilePath().toStdString() );
        if(foldersLst[i].isHidden())
            m_hidden_folds.emplace(foldersLst[i].absoluteFilePath().toStdString());
    }

    for(auto it = m_sub_fold_names.begin(); it != m_sub_fold_names.end(); )
    {
//        if(m_cancelled_revalidation)
//            break;
        if( newFolders_paths.find( it->first ) == newFolders_paths.end() )
        {
            emit requestClosing(it->second);
//            it->second->close();
//            delete (it->second);
//            it = m_sub_fold_names.erase(it);
        }/*else{*/
            ++it;
//        }
    }
    for(auto it=newFolders_paths.begin(); it != newFolders_paths.end(); ++it)
    {
//        if(m_cancelled_revalidation)
//            return;
        if( m_sub_fold_names.find( *it ) == m_sub_fold_names.end() )
        {
            auto sub = new FileInfoBD(QString(it->c_str()), this);
            m_sub_fold_names.emplace(std::make_pair(*it, sub));
        }
    }

    for(auto it = m_sub_fold_names.begin(); it != m_sub_fold_names.end(); ++it)
    {
        m_sub_folds.insert(it->second);
        m_sub_fold_paths.emplace(it->second->absPath());
    }

    m_subFoldsCount = m_sub_fold_names.size();
    m_filesCount = m_sub_fold_names.size();
    m_contentCount = m_subFoldsCount + m_filesCount;

    doSorting();

    emit contentHasChanged(m_fileInfo.absoluteFilePath());
}

void FileInfoBD::doElapsing()
{
    if(m_isElapsed && !m_alrLoaded){
        m_alrLoaded = true;

        revalFolderContent();
    }
}

//void FileInfoBD::removeFromParent()
//{
//    m_parent->removeFolder(m_fileInfo);
//}

//bool FileInfoBD::removeFolder(const QFileInfo& fi){
//    for(int i=0; i < m_sub_folds.size(); ++i)
//    {
//        if(m_sub_folds[i]->m_fileInfo == fi)
//        {
//            m_sub_folds[i].reset();
//            m_sub_folds.erase(m_sub_folds.begin() + i);
//            m_sub_fold_names.erase(m_sub_fold_names.begin() + i);
//            return true;
//        }
//    }
//    return false;
//}
//bool FileInfoBD::removeFolder(const QString& foldName){
//    for(int i=0; i < m_sub_folds.size(); ++i)
//    {
//        if(m_sub_folds[i]->m_fileInfo.fileName() == foldName)
//        {
//            m_sub_folds[i].reset();
//            m_sub_folds.erase(m_sub_folds.begin() + i);
//            m_sub_fold_names.erase(m_sub_fold_names.begin() + i);
//            return true;
//        }
//    }
//    return false;resetCancelFlags
//}

bool FileInfoBD::isEmpty() const
{
    return m_contentCount == 0;
}

void FileInfoBD::clearContainers()
{
    m_filesCount = 0;
    m_subFoldsCount = 0;
    m_contentCount = 0;

    m_files.clear();
    m_sub_folds.clear();
    m_sub_fold_paths.clear();
    m_sub_fold_names.clear();
    m_hidden_files.clear();
    m_hidden_folds.clear();
    m_sortedFilePaths_incl_hidden.clear();
    m_sortedFoldPaths_incl_hidden.clear();
    m_sortedEntryPaths_incl_hidden.clear();
    m_sortedFilePaths_no_hidden.clear();
    m_sortedFoldPaths_no_hidden.clear();
    m_sortedEntryPaths_no_hidden.clear();
}


void FileInfoBD::resetCancelFlags()
{
    m_cancelled_elapsing            = false;
    m_cancelled_collapsing          = false;
    m_cancelled_hidingHiddenFiles   = false;
    m_cancelled_revalidation        = false;
    m_cancelled_sorting             = false;

    for(auto it=m_sub_folds.begin(); it != m_sub_folds.end(); ++it)
    {
        (*it)->resetCancelFlags();
    }
}
