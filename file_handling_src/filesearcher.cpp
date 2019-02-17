#include "filesearcher.h"

FileSearcher::FileSearcher(std::unordered_map<std::string, std::string>* path_fileName,
                           std::unordered_map<std::string, int_bd>* path_ord,
                           std::unordered_map<std::string, std::string>* entry_to_firstElapsedEntry,
                           QObject *parent)
    : QObject(parent),
      m_key_word(std::string()),
      m_matched_paths(std::unordered_set<std::string>()),
      m_searchIndex_matchedPaths(std::unordered_map<int_bd, std::string>()),

      m_path_fileName_disp(path_fileName),
      m_path_ord_disp(path_ord),
      m_entry_to_firstElapsedEntry(entry_to_firstElapsedEntry),

      m_inSearchMode(false),
      m_focused_match_id(0),
      m_focused_path(""),
      m_matchCount(0)
{
}

FileSearcher::~FileSearcher()
{
    m_path_fileName_disp = nullptr; // aufgabe von FileManager dieses zu deleten
    m_path_ord_disp = nullptr; // aufgabe von FileManager dieses zu deleten

    m_matched_paths.clear();
    m_searchIndex_matchedPaths.clear();
    m_key_word = "";
    m_focused_match_id = 0;
    m_focused_path = "";
    m_matchCount = 0;
}

bool FileSearcher::isSearched(const std::string& path) const
{
//    return m_matched_paths.find(path) != m_matched_paths.end();
    return m_matched_firstElapsedEntryPaths.find(path) != m_matched_firstElapsedEntryPaths.end();
}
bool FileSearcher::isFocused(const std::string &path) const
{
    if(m_focused_path.empty())
        return false;

    auto foucsed_firstElpsdEntry = getFirstElapseEntryPath(m_focused_path);
    return foucsed_firstElpsdEntry == path;
//    return m_focused_path != "" && path == m_focused_path;
}

void FileSearcher::processSearchables(std::vector<Searchable*>& searchables)
{
    for(auto it = searchables.begin(); it != searchables.end(); ++it)
    {
        (*it)->setSearched( isSearched((*it)->path()) );
        (*it)->setSearchFocused( isFocused((*it)->path()) );
    }
}

std::string FileSearcher::focusedSearchPath() const
{
    return m_focused_path;
}

int_bd FileSearcher::indexCurSearchResult() const
{
    if(m_path_ord_disp->find(m_focused_path) != m_path_ord_disp->end())
    {
        return (*m_path_ord_disp)[m_focused_path];
    }
    return -1;
}

int_bd FileSearcher::searchIndex() const
{
    return m_focused_match_id;
}

int_bd FileSearcher::searchResultsCount() const
{
    return m_matchCount;
}


//-------------------------------------------------------

void FileSearcher::entriesChanged()
{
    if(m_inSearchMode)
        findMatches();
}

void FileSearcher::entriesChanged(std::unordered_map<std::string, std::string>* path_fileName,
                                  std::unordered_map<std::string, int_bd>* path_ord,
                                  std::unordered_map<std::string, std::string>* entry_to_firstElapsedEntry)
{
    m_path_fileName_disp = path_fileName;
    m_path_ord_disp = path_ord;
    m_entry_to_firstElapsedEntry = entry_to_firstElapsedEntry;

    entriesChanged();
}

void FileSearcher::search_QString(QString key_word)
{
    search(key_word.toStdString());
}

void FileSearcher::search(std::string key_word)
{
    key_word = StringOps::toLower(key_word);

    if( (key_word == m_key_word) && m_inSearchMode )
    {
        // nichts machen
    }else{
        m_key_word = key_word;
        findMatches();
    }
}

void FileSearcher::setSearched(std::string key_word, std::vector<std::string> matched_paths)
{
    m_key_word = key_word;
    m_matched_paths = std::unordered_set<std::string>(matched_paths.begin(), matched_paths.end());

    revalidateFirstElapsedEntryPaths();

    m_inSearchMode = matched_paths.size() > 0;

    if(matched_paths.size() == 0)
    {
        showNoMatchesFoundDialog();
    }

    if(m_path_ord_disp)
    {
        std::vector<int_bd> orders;
        std::unordered_map<int_bd, std::string> ord_paths;
        for(const auto& path: matched_paths)
        {
            auto ord_it = m_path_ord_disp->find(path);
            if(ord_it != m_path_ord_disp->end())
            {
                auto ord = ord_it->second;
                orders.push_back(ord);
                ord_paths[ord] = path;
            }
        }

        std::sort(orders.begin(), orders.end());

        int_bd cntr = 0;
        for(const auto& ord: orders)
        {
            m_searchIndex_matchedPaths[++cntr] = ord_paths[ord];
        }

        resetCurMatchVars();

        emit searchResultsChanged();
    }else{
        qDebug() << "FileSearcher::setSearched -> m_path_ord == nullptr!!!";
    }
}

void FileSearcher::focusNextMatch()
{
    if(m_matchCount <= 0)
    {
        resetCurMatchVars();
        return;
    }

    int_bd oldId = m_focused_match_id;

    if(++m_focused_match_id >= m_matchCount)
        m_focused_match_id = 0;

     m_focused_path = m_searchIndex_matchedPaths.find(m_focused_match_id) != m_searchIndex_matchedPaths.end()
                        ? m_searchIndex_matchedPaths[m_focused_match_id]
                        : "";

    if(oldId != m_focused_match_id)
        emit searchResultsChanged();
}
void FileSearcher::focusPreviousMatch()
{
    if(m_matchCount <= 0)
    {
        resetCurMatchVars();
        return;
    }

    int_bd oldId = m_focused_match_id;

    if(--m_focused_match_id < 0)
    {
        m_focused_match_id = m_matchCount - 1;
    }

    m_focused_path = m_searchIndex_matchedPaths.find(m_focused_match_id) != m_searchIndex_matchedPaths.end()
                        ? m_searchIndex_matchedPaths[m_focused_match_id]
                        : "";

    if(oldId != m_focused_match_id)
        emit searchResultsChanged();
}

void FileSearcher::exitSearchMode()
{
    m_inSearchMode = false;
    clearSearchWithoutSignaling();
    emit searchResultsChanged();
}

void FileSearcher::clearSearch()
{
    clearSearchWithoutSignaling();

    emit searchResultsChanged();
}

void FileSearcher::clearSearchWithoutSignaling()
{
    m_inSearchMode = false;
    clearContainers();
}

bool FileSearcher::inSearchMode()
{
    return m_inSearchMode;
}

void FileSearcher::close()
{
    delete this;
}

//--------------------------------------------------------

void FileSearcher::findMatches()
{
    m_inSearchMode = true;

    m_matched_paths.clear();
    m_matched_firstElapsedEntryPaths.clear();
    m_searchIndex_matchedPaths.clear();
    m_focused_path = "";

    resetCurMatchVars();

    if(m_path_fileName_disp && m_path_ord_disp)
    {
        std::vector<std::string> sorted_matched_paths;
        for(auto it=m_path_fileName_disp->begin(); it != m_path_fileName_disp->end(); ++it)
        {
            const std::string& entry_name = it->second;
            const std::string& entry_path = it->first;
            if(StringOps::inStringIgnoreCase(entry_name, m_key_word))
            {
                sorted_matched_paths.push_back( entry_path );
            }
        }
        auto sortFunc = [=](const auto& p1, const auto& p2){
            int_bd i1 = m_path_ord_disp->find(p1) != m_path_ord_disp->end() ? (*m_path_ord_disp)[p1] : -1;
            int_bd i2 = m_path_ord_disp->find(p2) != m_path_ord_disp->end() ? (*m_path_ord_disp)[p2] : -1;
            return i1 < i2;
        };
        std::sort(sorted_matched_paths.begin(), sorted_matched_paths.end(), sortFunc);
        for(std::size_t i=0; i < sorted_matched_paths.size(); ++i)
        {
            auto pth = sorted_matched_paths[i];
            m_matched_paths.insert( pth );
            findAndInsertFirstElapsedEntryPath( pth );
            m_searchIndex_matchedPaths[static_cast<int_bd>(i)] = pth;
        }
        resetCurMatchVars();
    }

    revalidateFirstElapsedEntryPaths();

    if(m_matched_paths.size() == 0)
    {
        showNoMatchesFoundDialog();
    }

//    printMatches();

    emit searchResultsChanged();
}

void FileSearcher::resetCurMatchVars()
{
    m_matchCount = static_cast<int_bd>(m_matched_paths.size());
    m_focused_match_id =  0;
    m_focused_path     = m_matchCount <= 0 ? "" : m_searchIndex_matchedPaths[static_cast<int_bd>(m_focused_match_id)];
}

void FileSearcher::clearContainers()
{
    m_matched_paths.clear();
    m_matched_firstElapsedEntryPaths.clear();
    m_searchIndex_matchedPaths.clear();
    m_key_word = "";
    resetCurMatchVars();
}

void FileSearcher::printMatches() const
{
    qDebug() << "Search-Matches: - inSearchMode: " << m_inSearchMode;
    for(const auto& path: m_matched_paths)
        qDebug() << "   " << QString::fromStdString(path);
}

void FileSearcher::showNoMatchesFoundDialog()
{
    m_inSearchMode = false;

    QString msg = QString("No matches found for '%1'!").arg(QString::fromStdString(m_key_word));
    QMessageBox* msgBox = new QMessageBox();
//    connect(msgBox, &QMessageBox::finished, [](){qDebug() << "msgBox finished";});
    connect(msgBox, &QMessageBox::finished, msgBox, &QMessageBox::deleteLater);
    msgBox->setIcon(QMessageBox::Information);
    msgBox->setText(msg);
    msgBox->setModal(false);
    msgBox->show();
}

std::string FileSearcher::getFirstElapseEntryPath(const std::string path) const
{
    if(m_entry_to_firstElapsedEntry)
    {
        auto it = m_entry_to_firstElapsedEntry->find(path);
        if(it != m_entry_to_firstElapsedEntry->end())
        {
            return it->second;
        }
    }
    return "";
}

void FileSearcher::revalidateFirstElapsedEntryPaths()
{
    m_matched_firstElapsedEntryPaths.clear();
    if(m_entry_to_firstElapsedEntry)
    {
        for(const auto& match: m_matched_paths)
        {
            findAndInsertFirstElapsedEntryPath(match);
        }
    }
}

void FileSearcher::findAndInsertFirstElapsedEntryPath(const std::string &path)
{
    if(m_entry_to_firstElapsedEntry)
    {
        auto it = m_entry_to_firstElapsedEntry->find(path);
        if(it != m_entry_to_firstElapsedEntry->end())
        {
            m_matched_firstElapsedEntryPaths.insert(it->second);
        }
    }
}



