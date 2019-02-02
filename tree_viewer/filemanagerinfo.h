#ifndef FILEMANAGERINFO_H
#define FILEMANAGERINFO_H

#include <QObject>
#include <QVector>
#include <QDebug>
#include <QString>

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

    FileManagerInfo* operator=(FileManagerInfo* fmi);
    FileManagerInfo& operator=(const FileManagerInfo& fmi);

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
    bool singleFolderSelected() const;
    bool foldersSelected() const;
    bool filesSelected() const;
    bool selectionContainsZippedFile() const;
    bool depthIdElapsed(int id) const;
signals:
    void revalidateTreeViewer();
//public slots:
//    void curRootChanged(QString rootPath);
//    void curSearchResultChanged(QString curSearRslt);

//    void maxDepthChanged(int n);
//    void selectionCountChanged(int n);
//    void displayedFileCountChanged(int n);
//    void indexOfCurrentSearchResultChanged(int n);
//    void searchIndexChanged(int n);
//    void searchResultsCountChanged(int n);

//    void includeHiddenFilesChanged(bool b);
//    void inSearchModeChanged(bool b);
//    void singleFolderSelectedChanged(bool b);
//    void foldersSelectedChanged(bool b);
//    void filesSelectedChanged(bool b);
//    void selectionContainsZippedFileChanged(bool b);

//    void depthIdElapsedChanged(QVector<bool> v);
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
    bool m_foldersSelected;
    bool m_filesSelected;
    bool m_selctnCntnsZpdFle;

    std::vector<bool> m_depthIdElapsed;
};

#endif // FILEMANAGERINFO_H
