#ifndef FILEMANAGERINFO_H
#define FILEMANAGERINFO_H

#include <QObject>
#include <QVector>
#include <QDebug>
#include <QString>

class FileManagerInfo: public QObject
{
    Q_OBJECT
public:
    FileManagerInfo();
    FileManagerInfo(const FileManagerInfo& fmi);

    QString curRootPath() const;
    QString curSearchResult() const;

    int maxDepth() const;
    int selectionCount() const;
    int displayedFileCount() const;
    int indexOfCurrentSearchResult() const;
    int searchIndex() const;
    int searchResultsCount() const;

    bool includeHiddenFiles() const;
    bool inSearchMode() const;
    bool singleFolderSelected() const;
    bool foldersSelected() const;
    bool filesSelected() const;
    bool selectionContainsZippedFile() const;
    bool depthIdElapsed(int id) const;
signals:
    void revalidateTreeViewer();
public slots:
    void curRootChanged(QString rootPath);
    void curSearchResultChanged(QString curSearRslt);

    void maxDepthChanged(int n);
    void selectionCountChanged(int n);
    void displayedFileCountChanged(int n);
    void indexOfCurrentSearchResultChanged(int n);
    void searchIndexChanged(int n);
    void searchResultsCountChanged(int n);

    void includeHiddenFilesChanged(bool b);
    void inSearchModeChanged(bool b);
    void singleFolderSelectedChanged(bool b);
    void foldersSelectedChanged(bool b);
    void filesSelectedChanged(bool b);
    void selectionContainsZippedFileChanged(bool b);

    void depthIdElapsedChanged(QVector<bool> v);
private:
    QString m_curRootPath;
    QString m_curSearchRslt;

    int m_maxDepth;
    int m_selectionCount;
    int m_displayedFileCount;
    int m_indxCurSearchRslt;
    int m_searchIndx;
    int m_searchRsltsCnt;

    bool m_includeHiddenFiles;
    bool m_inSearchMode;
    bool m_singleFolderSelected;
    bool m_foldersSelected;
    bool m_filesSelected;
    bool m_selctnCntnsZpdFle;

    QVector<bool> m_depthIdElapsed;
};

#endif // FILEMANAGERINFO_H
