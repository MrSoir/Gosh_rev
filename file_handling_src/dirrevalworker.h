#ifndef DIRREVALWORKER_H
#define DIRREVALWORKER_H

#include <QObject>
#include <QDebug>
#include <QThread>

#include <vector>
#include <string>

#include "dirmanagerworker.h"
#include "fileinfobd.h"
#include "staticfunctions_files.h"

class DirRevalWorker : public DirManagerWorker
{
    Q_OBJECT
public:
    explicit DirRevalWorker(FileInfoBD* fi,
                            QThread* threadToMoveObjectsTo);
    explicit DirRevalWorker(std::vector<FileInfoBD*> fis,
                            QThread* threadToMoveObjectsTo);
    explicit DirRevalWorker();

    explicit DirRevalWorker(const DirRevalWorker& w);

    DirRevalWorker& operator=(const DirRevalWorker& w);

    virtual ~DirRevalWorker() override;

    virtual bool blockOtherThreads() const override;
    virtual bool revalidateDirStructureAfterWorkerHasFinished() const override;
signals:
public slots:
    void run();
protected:
    virtual void workBeforeLaunchThread() override;
private:
    std::vector<FileInfoBD*> m_fis;

    QThread* m_threadToMoveObjectsTo;
};

#endif // DIRREVALWORKER_H
