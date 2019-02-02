#ifndef FIBDVIEWER_H
#define FIBDVIEWER_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QFileInfo>

#include <string>

#include "searchable.h"
#include "selectable.h"
#include "orderby.h"

#include "dirmanager.h"

//using namespace ORDERED_BY;

class FiBDViewer : public Searchable,
                   public Selectable
{
public:
    explicit FiBDViewer();
    explicit FiBDViewer(QString path,
                        QString fileName,
                        unsigned long long fileSize,
                        QDateTime lastModified,
                        bool isHidden,
                        int depthId);
    explicit FiBDViewer(const QFileInfo& fi,
                        int depthId = 0);
    explicit FiBDViewer(const DirManagerInfo& dmi,
                        int depthId,
                        bool isCurrentlyRevalidating,
                        bool isSearched = false,
                        bool isSearchFocused = false,
                        bool isSelected = false);
    explicit FiBDViewer(DirManagerInfo* dmi,
                        int depthId,
                        bool isCurrentlyRevalidating,
                        bool isSearched = false,
                        bool isSearchFocused = false,
                        bool isSelected = false);


    explicit FiBDViewer(const FiBDViewer& fi);

    virtual ~FiBDViewer() override;

    FiBDViewer& operator=(const FiBDViewer& fi);
    FiBDViewer* operator=(FiBDViewer* fi);


    // getters:

    bool searched() const;
    bool searchFocused() const;
    bool selected() const;
    bool isFolder() const;
    bool isLoaded() const;
    bool isElapsed() const;
    bool isHidden() const;

    bool containsFiles() const;
    bool isEmpty() const;
    bool isReversedSortedBy(Order ord) const;
    bool isSortedBy(ORDERED_BY ordered_by) const;
    Order order() const;

    int depthId() const;

    virtual std::string path() const override;
    QString q_path() const;
    QString fileName() const;

    unsigned long long int fileSize() const;
    QDateTime lastModified() const;

    bool isCurrentlyRevalidating() const;


    // setters:

    void setSearched(bool searched) override;
    void setSearchFocused(bool focused) override;
    void setSelected(bool selected) override;

private:
    QString m_path;
    QString m_fileName;

    bool m_folder;
    bool m_searched;
    bool m_searchFocused;
    bool m_selected;
    bool m_isLoaded;
    bool m_isElapsed;
    bool m_isHidden;

    unsigned long m_filesCount;
    unsigned long m_subDirCount;

    int m_depthId;

    Order m_order;

    unsigned long long int m_fileSize;
    QDateTime m_lastModified;

    bool m_isCurrentlyRevalidating;
};



#endif // FIBDVIEWER_H
