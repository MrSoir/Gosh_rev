#ifndef DIRSORTWORKER_H
#define DIRSORTWORKER_H

#include <QObject>
#include <QDebug>
#include <QThread>

#include <vector>
#include <string>

#include "dirmanagerworker.h"
#include "fileinfobd.h"
#include "staticfunctions_files.h"
#include "orderby.h"

class DirSortWorker : public DirManagerWorker
{
    Q_OBJECT
public:
    explicit DirSortWorker(Order order,
                           FileInfoBD* fi,
                           bool sort_recursive,
                           QThread* threadToMoveObjectsTo);
    explicit DirSortWorker(Order order,
                           std::vector<FileInfoBD*> fis,
                           bool sort_recursive,
                           QThread* threadToMoveObjectsTo);

    virtual ~DirSortWorker() override;

    virtual bool blockOtherThreads() const override;
    virtual bool revalidateDirStructureAfterWorkerHasFinished() const override;
signals:
public slots:
    void run();
protected:
    virtual void workBeforeLaunchThread() override;
private:
    Order m_order;
    std::vector<FileInfoBD*> m_fis;
    bool m_sort_recursive;

    QThread* m_threadToMoveObjectsTo;
};

#endif // DIRSORTWORKER_H
