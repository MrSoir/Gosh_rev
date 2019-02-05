#ifndef ELAPSEWORKER_H
#define ELAPSEWORKER_H

#include <QObject>
#include <QDebug>
#include <QThread>

#include <vector>
#include <string>

#include "dirmanagerworker.h"
#include "fileinfobd.h"
#include "staticfunctions_files.h"

class DirElapseWorker : public DirManagerWorker
{
    Q_OBJECT
public:
    explicit DirElapseWorker(std::vector<FileInfoBD*> dirsToElapse,
                             bool recursive,
                             bool collapse,
                             QThread* threadToMoveObjectsTo);
    explicit DirElapseWorker(FileInfoBD* dirToElapse,
                             bool recursive,
                             bool collapse,
                             QThread* threadToMoveObjectsTo);
    virtual ~DirElapseWorker() override;

    virtual bool blockOtherThreads() const override;
    virtual bool revalidateDirStructureAfterWorkerHasFinished() const override;

signals:
    void killHelpers();
public slots:
    void run();
    void workerFinished(FileInfoBD* dir);

protected:
    virtual void workBeforeLaunchThread() override;
private:
    void connectSignals();

    void collapseHelper(FileInfoBD* dir);

    void moveDirsToCallerThread();

    std::vector<FileInfoBD*> m_dirsToElapse;
    bool m_recursive;
    bool m_collapse;
    bool m_useSubThreads; // collapsing verwendet keine sub-threads, da hier nur die flags umgesetzt werden. beim elapsen hingegen sind ggfs. ordnerstrukturen neu zu laden, was zeitintensiv sein kann
    int m_runningThreads;

    QThread* m_threadToMoveObjectsTo;
};


//----------------------------------------------------------------------------------------


class DirElapseWorkerHelper: public QObject
{
    Q_OBJECT
public:
    explicit DirElapseWorkerHelper(FileInfoBD* dir,
                                   bool recursive,
                                   QThread* threadToMoveObjectsTo);
    virtual ~DirElapseWorkerHelper() override;

signals:
    void finished(FileInfoBD* fi);
public slots:
    void run();
    void cancel();
private:
    void elapseHelper(FileInfoBD* dir);

    FileInfoBD* m_dir;
    bool m_recursive;
    bool m_cancelled;

    QThread* m_threadToMoveObjectsTo;
};


#endif // ELAPSEWORKER_H
