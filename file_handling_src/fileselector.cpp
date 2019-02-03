#include "fileselector.h"

FileSelector::FileSelector(std::unordered_set<std::string>* paths,
                           std::unordered_map<long long, std::string>* ord_paths,
                           std::unordered_map<std::string, long long>* paths_ord,
                           std::unordered_map<std::string, std::string>* fileName_paths,
                           std::unordered_set<std::string>* folder_paths,
                           QObject *parent)
    : QObject(parent),

      m_selected_paths(std::unordered_set<std::string>()),

      m_paths(paths),
      m_ord_paths(ord_paths),
      m_paths_ord(paths_ord),
      m_fileName_paths(fileName_paths),
      m_folder_paths(folder_paths),

      m_focused_ord(-1),
      m_focusedPath(std::string("")),
      m_slct_key_word(std::string(""))
{
}

FileSelector::~FileSelector()
{
    m_selected_paths.clear();
    m_paths = nullptr;
    m_ord_paths = nullptr;
    m_paths_ord = nullptr;
    m_fileName_paths = nullptr;
    m_focused_ord = -1;
    m_focusedPath = "";
    m_slct_key_word = "";
}

bool FileSelector::isSelected(const std::string& path) const
{
    return m_selected_paths.find(path) != m_selected_paths.end();
}

void FileSelector::processSelection(std::vector<Selectable*>& selection)
{
    for(auto it = selection.begin(); it != selection.end(); ++it)
    {
        (*it)->setSelected( isSelected((*it)->path()) );
    }
}

const std::unordered_set<std::string>& FileSelector::getSelectedFolders() const
{
    return m_selected_folders_paths;
}

const std::unordered_set<std::string>& FileSelector::getSelectedEntries() const
{
    return m_selected_paths;
}

std::string FileSelector::getLastSelectedEntry() const
{
    return m_focusedPath;
//    if(m_slctd_id > -1 && m_ord_paths &&
//            m_ord_paths->find(m_slctd_id) != m_ord_paths->end())
//    {
//        return (*m_ord_paths)[m_slctd_id];
//    }
    //    return "";
}

int_bd FileSelector::selectionCount() const
{
    return static_cast<int_bd>(m_selected_paths.size());
}

bool FileSelector::filesSelected() const
{
    return (m_selected_folders_paths.size() < m_selected_paths.size());
}

bool FileSelector::foldersSelected() const
{
    return (m_selected_folders_paths.size() > 0);
}

//---------------------------------------------------------

void FileSelector::entriesChanged()
{
    auto oldMatchCount = m_selected_paths.size();

    // schauen, ob in unter den neuen entries der alte m_focusedPath enthalten ist
    if(m_paths_ord && !m_focusedPath.empty() )
    {
        auto it = (*m_paths_ord).find(m_focusedPath);
        if(it != m_paths_ord->end())
        {
            m_focused_ord = it->second;
        }else{
            clearSelectionVars();
        }
    }else{
        clearSelectionVars();
    }

    for(auto it=m_selected_paths.begin(); it != m_selected_paths.end(); )
    {
        std::string path = *it;

        if( m_paths->find(path) == m_paths->end() )
        {
            it = m_selected_paths.erase(it);

            auto folder_it = m_selected_folders_paths.find( path );
            if(folder_it != m_selected_folders_paths.end())
            {
                m_selected_folders_paths.erase(folder_it);
            }
        }else{
            ++it;
        }
    }

    if(m_selected_paths.size() != oldMatchCount)
    {
        emit selectionChanged();
    }
}

void FileSelector::entriesChanged(std::unordered_set<std::string>* paths,
                                  std::unordered_map<int_bd, std::string>* ord_paths,
                                  std::unordered_map<std::string, int_bd>* paths_ord,
                                  std::unordered_map<std::string, std::string>* fileName_paths,
                                  std::unordered_set<std::string>* folder_paths)
{
    m_paths = paths;
    m_ord_paths = ord_paths;
    m_paths_ord = paths_ord;
    m_fileName_paths = fileName_paths;
    m_folder_paths = folder_paths;

    entriesChanged();
}

void FileSelector::select_QString(QString path, bool cntrl_prsd, bool shift_prsd)
{
    select(path.toStdString(), cntrl_prsd, shift_prsd);
}

void FileSelector::select(std::string path, bool cntrl_prsd, bool shift_prsd)
{
    if(shift_prsd)
    {
        if(m_paths_ord &&
                ((*m_paths_ord).find(path) != (*m_paths_ord).end()))
        {
            auto ord = (*m_paths_ord)[path];
            auto lastId = m_focused_ord == -1 ? ord : m_focused_ord;
            auto lower = ord < lastId ? ord : lastId;
            auto upper = ord > lastId ? ord : lastId;
            for(auto i = lower; i <= upper; ++i)
            {
                const auto& cur_path = (*m_ord_paths)[i];

                m_selected_paths.insert( cur_path );

                if(m_folder_paths->find(cur_path) != m_folder_paths->end())
                    m_selected_folders_paths.insert(cur_path);
            }

            m_focused_ord = ord;
            m_focusedPath = path;

        }else{
            clearSelectionVars();
        }
    }else{
        if( !cntrl_prsd )
        {
            clearContainers();
        }
        if((*m_paths_ord).find(path) != (*m_paths_ord).end())
        {
            m_selected_paths.insert(path);
            m_focused_ord = (*m_paths_ord)[path];
            m_focusedPath = path;

            if(m_folder_paths->find(path) != m_folder_paths->end())
                m_selected_folders_paths.emplace(path);
        }else{
            clearSelectionVars();
        }
    }

    emit selectionChanged();
}

void FileSelector::selectEntireContent()
{
    for(auto& pth: *m_paths)
        m_selected_paths.emplace(pth);

    for(auto& pth: *m_folder_paths)
        m_selected_folders_paths.emplace(pth);

    if(m_ord_paths->find(m_focused_ord) != m_ord_paths->end())
    {
        m_focusedPath = (*m_ord_paths)[m_focused_ord];
    }
    else if( (m_ord_paths->find(0) != m_ord_paths->end()) )
    {
        m_focused_ord = 0;
        m_focusedPath = (*m_ord_paths)[0];
    }
    else
    {
        clearSelectionVars();
    }

    emit selectionChanged();
}

void FileSelector::clearSelection()
{
    m_selected_paths.clear();
    m_selected_folders_paths.clear();
    clearSelectionVars();
    m_slct_key_word = "";

    emit selectionChanged();
}

void FileSelector::selectNext(bool cntrl_prsd, bool shift_prsd)
{
    if( !(cntrl_prsd || shift_prsd) )
    {
        clearContainers();
    }

    if( m_ord_paths )
    {
        if( ++m_focused_ord >= static_cast<int_bd>(m_paths->size()) )
        {
            m_focused_ord = 0;
        }
        auto it = m_ord_paths->find(m_focused_ord);
        if(it != m_ord_paths->end())
        {
            auto path = it->second;

            m_focused_ord = it->first;
            m_focusedPath = path;

            m_selected_paths.insert(path);

            if(m_folder_paths->find(path) != m_folder_paths->end())
                m_selected_folders_paths.emplace(path);

        }else{
            clearSelectionVars();
        }
    }

    emit selectionChanged();
    emit focusPath(m_focusedPath);
}
void FileSelector::selectPrevious(bool cntrl_prsd, bool shift_prsd)
{
    if( !(cntrl_prsd || shift_prsd) )
    {
        clearContainers();
    }

    if( m_ord_paths )
    {
        if( --m_focused_ord < 0 )
        {
            m_focused_ord = static_cast<int_bd>(m_paths->size()) - 1;
        }
        auto it = m_ord_paths->find(m_focused_ord);
        if(it != m_ord_paths->end())
        {
            auto path = it->second;

            m_focused_ord = it->first;
            m_focusedPath = path;

            m_selected_paths.insert(path);

            if(m_folder_paths->find(path) != m_folder_paths->end())
                m_selected_folders_paths.emplace(path);
        }else{
            clearSelectionVars();
        }
    }
    emit selectionChanged();
    emit focusPath(m_focusedPath);
}

void FileSelector::selectKeyWord(std::string key)
{
    if( m_slct_key_word != key )
    {
        for(auto it = m_fileName_paths->begin(); it != m_fileName_paths->end(); ++it )
        {
            if( StringOps::startsWithIgnoreCase(it->first, key))
            {
                m_selected_paths.clear();
                m_selected_paths.emplace(it->second);
                m_focusedPath = it->second;
                m_focused_ord = (*m_paths_ord)[it->second];

                emit selectionChanged();
                emit focusPath(m_focusedPath);

                return;
            }
        }
    }
}

void FileSelector::close()
{
    delete this;
}

void FileSelector::printSelection() const
{
    qDebug() << "selected paths:";
    for(const auto& path: m_selected_paths)
    {
        qDebug() << "   " << QString::fromStdString(path);
    }
}

void FileSelector::clearContainers()
{
    m_selected_paths.clear();
    m_selected_folders_paths.clear();
}

void FileSelector::clearSelectionVars()
{
    m_focused_ord = -1;
    m_focusedPath = "";
}

//------------------------------------------------------------_
