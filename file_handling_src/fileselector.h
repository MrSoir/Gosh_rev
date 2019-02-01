#ifndef FILESELECTOR_H
#define FILESELECTOR_H

#include <QObject>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include "stringops.h"
#include "selectable.h"


class FileSelector : public QObject
{
    Q_OBJECT
public:
    explicit FileSelector(QObject *parent = nullptr);
    ~FileSelector();

    bool isSelected(const std::string& path) const;
    void processSelection(std::vector<Selectable*>& selection);

    const std::unordered_set<std::string>& getSelectedFolders() const;
    const std::unordered_set<std::string>& getSelectedEntries() const;

    std::string getLastSelectedEntry() const;
signals:
    void selectionChanged();
public slots:
    void entriesChanged(std::unordered_set<std::string>* paths,
                        std::unordered_map<unsigned long long, std::string>* ord_paths,
                        std::unordered_map<std::string, unsigned long long>* paths_ord,
                        std::unordered_map<std::string, std::string>* fileName_paths,
                        std::unordered_set<std::string>* folder_paths);

    void select(std::string path, bool cntrl_prsd, bool shift_prsd);
    void selectEntireContent();
    void clearSelection();

    void selectNext(bool cntrl_prsd, bool shift_prsd);
    void selectPrevious(bool cntrl_prsd, bool shift_prsd);

    void selectKeyWord(std::string key);

    void close();
private:
    std::unordered_set<std::string> m_selected_paths;
    std::unordered_set<std::string> m_selected_folders_paths;

    std::unordered_set<std::string>* m_paths;
    std::unordered_map<unsigned long long, std::string>* m_ord_paths;
    std::unordered_map<std::string, unsigned long long>* m_paths_ord;
    std::unordered_map<std::string, std::string>* m_fileName_paths;
    std::unordered_set<std::string>* m_folder_paths;

    long long m_slctd_id;
    std::string m_latestSelctdPath;
    std::string m_slct_key_word;
};

#endif // FILESELECTOR_H
