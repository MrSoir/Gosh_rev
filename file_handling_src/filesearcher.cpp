#include "filesearcher.h"

FileSearcher::FileSearcher(std::unordered_map<std::string, std::string>* fileName_paths,
                           std::unordered_map<std::string, long long>* path_ord,
                           QObject *parent)
    : QObject(parent),
      m_key_word(std::string()),
      m_matched_paths(std::unordered_set<std::string>()),
      m_ord_matchedPaths(std::unordered_map<int_bd, std::string>()),

      m_fileName_path(fileName_paths),
      m_path_ord(path_ord),

      m_inSearchMode(false),
      m_focused_match_id(-1),
      m_focused_path(""),
      m_matchCount(0)
{
}

FileSearcher::~FileSearcher()
{
    m_fileName_path = nullptr; // aufgabe von FileManager dieses zu deleten
    m_path_ord = nullptr; // aufgabe von FileManager dieses zu deleten

    m_matched_paths.clear();
    m_ord_matchedPaths.clear();
    m_key_word = "";
    m_focused_match_id = -1;
    m_focused_path = "";
    m_matchCount = 0;
}

bool FileSearcher::isSearched(const std::string& path) const
{
    return m_matched_paths.find(path) != m_matched_paths.end();
}
bool FileSearcher::isFocused(const std::string &path) const
{
    return m_focused_path != "" && path == m_focused_path;
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
    if(m_path_ord->find(m_focused_path) != m_path_ord->end())
    {
        return (*m_path_ord)[m_focused_path];
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
        findMatches();
    }
}

void FileSearcher::setSearched(std::string key_word, std::vector<std::string> matched_paths)
{
    m_key_word = key_word;
    m_matched_paths = std::unordered_set<std::string>(matched_paths.begin(), matched_paths.end());

    m_inSearchMode = true;

    if(m_path_ord)
    {
        std::vector<int_bd> orders;
        std::unordered_map<int_bd, std::string> ord_paths;
        for(const auto& path: matched_paths)
        {
            if(m_path_ord->find(path) != m_path_ord->end())
            {
                auto ord = (*m_path_ord)[path];
                orders.push_back(ord);
                ord_paths[ord] = path;
            }
        }

        std::sort(orders.begin(), orders.end());

        int_bd cntr = 0;
        for(const auto& ord: orders)
        {
            m_ord_matchedPaths[++cntr] = ord_paths[ord];
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
        m_focused_path = "";
        return;
    }

    int_bd oldId = m_focused_match_id;

    if(static_cast<int_bd>(++m_focused_match_id) >= m_matchCount)
        m_focused_match_id = 0;

    m_focused_path = m_focused_match_id > -1 ? m_ord_matchedPaths[static_cast<int_bd>(m_focused_match_id)] : "";

    if(oldId != m_focused_match_id)
        emit searchResultsChanged();
}
void FileSearcher::focusPreviousMatch()
{
    if(m_matchCount <= 0)
    {
        m_focused_path = "";
        return;
    }

    int_bd oldId = m_focused_match_id;

    if(--m_focused_match_id < 0)
    {
        m_focused_match_id = static_cast<int_bd>(m_matchCount) - 1;
    }

    m_focused_path = m_ord_matchedPaths[static_cast<int_bd>(m_focused_match_id)];

    if(oldId != m_focused_match_id)
        emit searchResultsChanged();
}

void FileSearcher::exitSearchMode()
{
    m_inSearchMode = false;
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
    m_ord_matchedPaths.clear();

    m_focused_match_id = -1;
    m_focused_path = "";

    if(m_fileName_path)
    {
        int_bd cntr = 0;
        for(auto it=m_fileName_path->begin(); it != m_fileName_path->end(); ++it)
        {
            const std::string& entry_name = it->first;
            if(StringOps::inStringIgnoreCase(entry_name, m_key_word))
            {
                m_matched_paths.insert( it->second );
                m_ord_matchedPaths[cntr++] = it->second;
            }
        }
        resetCurMatchVars();
    }

    emit searchResultsChanged();
}

void FileSearcher::resetCurMatchVars()
{
    m_matchCount = static_cast<int_bd>(m_matched_paths.size());
    m_focused_match_id = m_matchCount <= 0 ? -1 : 0;
    m_focused_path     = m_matchCount <= 0 ? "" : m_ord_matchedPaths[static_cast<int_bd>(m_focused_match_id)];
}



