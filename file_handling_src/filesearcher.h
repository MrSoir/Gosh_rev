#ifndef FILESEARCHER_H
#define FILESEARCHER_H

#include <QObject>
#include <QtDebug>
#include <QMessageBox>

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
    explicit FileSearcher(std::unordered_map<std::string, std::string>* path_fileName,
                          std::unordered_map<std::string, int_bd>* path_ord,
                          QObject *parent = nullptr);
    virtual ~FileSearcher() override;

    bool isSearched(const std::string& path) const;
    bool isFocused(const std::string& path) const;

    void processSearchables(std::vector<Searchable*>& searchables);

//    ---------------------------------------------------

    std::string focusedSearchPath() const;
    int_bd indexCurSearchResult() const;
    int_bd searchIndex() const;
    int_bd searchResultsCount() const;

    bool inSearchMode();

signals:
    void searchResultsChanged();
public slots:
    void entriesChanged();
    void entriesChanged(std::unordered_map<std::string, std::string>* path_fileName,
                        std::unordered_map<std::string, int_bd>* path_ord);

    void search_QString(QString key_word);
    void search(std::string key_word);

    void setSearched(std::string key_word, std::vector<std::string> matched_paths);

    void focusNextMatch();
    void focusPreviousMatch();

    void exitSearchMode();
    void clearSearch();
    void clearSearchWithoutSignaling();

    void close();
private:
    void findMatches();

    void resetCurMatchVars();

    void clearContainers();

    void printMatches() const;

    void showNoMatchesFoundDialog();

    //----------------------------------------

    std::string m_key_word; // string that is searched for
    std::unordered_set<std::string> m_matched_paths; // all paths, which's fileNames contain the m_key_word
    std::unordered_map<int_bd, std::string> m_ord_matchedPaths;

//    std::unordered_set<std::string> m_paths; // all paths
    std::unordered_map<std::string, std::string>* m_path_fileName; // map: fileName -> path
    std::unordered_map<std::string, int_bd>* m_path_ord;

    bool m_inSearchMode;
    int_bd  m_focused_match_id;
    std::string m_focused_path;
    int_bd m_matchCount;


};

#endif // FILESEARCHER_H
