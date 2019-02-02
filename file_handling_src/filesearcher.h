#ifndef FILESEARCHER_H
#define FILESEARCHER_H

#include <QObject>
#include <QtDebug>

#include <string>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <algorithm>    // std::sort

#include "stringops.h"

#include "searchable.h"

#define int_bd long long

class FileSearcher : public QObject
{
    Q_OBJECT
public:
    explicit FileSearcher(QObject *parent = nullptr);
    ~FileSearcher();

    bool isSearched(const std::string& path) const;
    bool isFocused(const std::string& path) const;

    void processSearchables(std::vector<Searchable*>& searchables);
signals:
    void searchResultsChanged();
public slots:
    void entriesChanged(std::unordered_map<std::string, std::string>* fileName_paths,
                        std::unordered_map<std::string, int_bd>* path_ord);
    void search(std::string key_word);
    void clearSearch();

    void setSearched(std::string key_word, std::vector<std::string> matched_paths);

    void enable();
    void disable();

    void focusNextMatch();
    void focusPreviousMatch();

    void close();
private:
    void findMatches();

    void resetCurMatchVars();

    std::string m_key_word; // string that is searched for
    std::unordered_set<std::string> m_matched_paths; // all paths, which's fileNames contain the m_key_word
    std::unordered_map<int_bd, std::string> m_ord_matchedPaths;

//    std::unordered_set<std::string> m_paths; // all paths
    std::unordered_map<std::string, std::string>* m_fileName_path; // map: fileName -> path
    std::unordered_map<std::string, int_bd>* m_path_ord;

    bool m_enabled;
    int_bd  m_focused_match_id;
    std::string m_focused_path;
    int_bd m_matchCount;
};

#endif // FILESEARCHER_H
