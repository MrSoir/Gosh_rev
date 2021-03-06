#ifndef FILESELECTOR_H
#define FILESELECTOR_H

#include <QObject>
#include <QDebug>

#include <string>
#include <unordered_set>
#include <unordered_map>

#include "stringops.h"
#include "selectable.h"

#define int_bd long long

class FileSelector : public QObject
{
    Q_OBJECT
public:
    explicit FileSelector(std::unordered_set<std::string>* paths,
                          std::unordered_map<int_bd, std::string>* ord_paths,
                          std::unordered_map<std::string, int_bd>* paths_ord,
                          std::unordered_map<std::string, std::string>* path_fileNames,
                          std::unordered_set<std::string>* folder_paths,
                          QObject* parent = nullptr);
    ~FileSelector();

    bool isSelected(const std::string& path) const;
    void processSelection(std::vector<Selectable*>& selection);

    const std::unordered_set<std::string>& getSelectedFolders() const;
    const std::unordered_set<std::string>& getSelectedEntries() const;

    std::string getLastSelectedEntry() const;

    int_bd selectionCount() const;
    bool filesSelected() const;
    bool foldersSelected() const;

signals:
    void selectionChanged();
    void focusPath(std::string path);

public slots:
    void entriesChanged();
    void entriesChanged(std::unordered_set<std::string>* paths,
                        std::unordered_map<int_bd, std::string>* ord_paths,
                        std::unordered_map<std::string, int_bd>* paths_ord,
                        std::unordered_map<std::string, std::string>* pathFileNames,
                        std::unordered_set<std::string>* folder_paths);

    void select_QString(QString path, bool cntrl_prsd, bool shift_prsd);
    void select(std::string path, bool cntrl_prsd, bool shift_prsd);
    void selectEntireContent();

    void clearSelection();
    void clearSelectionWithoutSignaling();

    void selectNext(bool cntrl_prsd, bool shift_prsd);
    void selectPrevious(bool cntrl_prsd, bool shift_prsd);

    void selectKeyWord(std::string key);

    void close();
private:
    void printSelection() const;

    void clearContainers();
    void clearSelectionVars();

    std::unordered_set<std::string> m_selected_paths;
    std::unordered_set<std::string> m_selected_folders_paths;

    std::unordered_set<std::string>* m_paths;
    std::unordered_map<int_bd, std::string>* m_ord_paths;
    std::unordered_map<std::string, int_bd>* m_paths_ord;
    std::unordered_map<std::string, std::string>* m_path_fileNames;
    std::unordered_set<std::string>* m_folder_paths;

    int_bd m_focused_ord;
    std::string m_focusedPath;

    std::string m_slct_key_word;
};

#endif // FILESELECTOR_H
