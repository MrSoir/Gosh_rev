#ifndef FILEMANAGERINFO_H
#define FILEMANAGERINFO_H

#include <QObject>
#include <QVector>
#include <QDebug>
#include <QString>

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <QPixmap>

#define int_bd long long

class FileManager;

class FileManagerInfo: public QObject
{
    Q_OBJECT
public:
    explicit FileManagerInfo();
    explicit FileManagerInfo(const FileManager& fmi);
    explicit FileManagerInfo(FileManager* fmi);

    explicit FileManagerInfo(const FileManagerInfo& fmi);

    FileManagerInfo& operator=(const FileManagerInfo& fmi);

    virtual ~FileManagerInfo() override;

    QString curRootPath() const;
    QString curFocusedSearchPath() const;

    int_bd maxDepth() const;
    int_bd selectionCount() const;
    int_bd displayedFileCount() const;
    int_bd indexOfCurrentSearchResult() const;
    int_bd searchIndex() const;
    int_bd searchResultsCount() const;

    bool includeHiddenFiles() const;
    bool inSearchMode() const;
    bool executingDeepSearch() const;
    bool singleFolderSelected() const;
    bool foldersSelected() const;
    bool filesSelected() const;
    bool selectionContainsZippedFile() const;
    bool depthIdElapsed(int id) const;

    QPixmap getPreviewIcon(const QString& path);
    QPixmap getPreviewIcon(const std::string& path);
    bool containsPreviewIcon(const QString& path);
    bool containsPreviewIcon(const std::string& path);

    const std::unordered_map<std::string, QPixmap>& getPreviewIcons() const;

signals:
    void revalidateTreeViewer();

private:
    QString m_curRootPath;
    QString m_curSearchRslt;

    int_bd m_maxDepth;
    int_bd m_selectionCount;
    int_bd m_displayedFileCount;
    int_bd m_indxCurSearchRslt;
    int_bd m_searchIndx;
    int_bd m_searchRsltsCnt;

    bool m_includeHiddenFiles;
    bool m_inSearchMode;
    bool m_executingDeepSearch;
    bool m_foldersSelected;
    bool m_filesSelected;
    bool m_selctnCntnsZpdFle;

    std::vector<bool> m_depthIdElapsed;

    std::unordered_map<std::string, QPixmap> m_previewIcons;
};

#endif // FILEMANAGERINFO_H
