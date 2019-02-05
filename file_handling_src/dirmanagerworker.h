#ifndef DIRMANAGERWORKER_H
#define DIRMANAGERWORKER_H

#include <QObject>
#include <QThread>
#include <QDebug>

#include "staticfunctions_files.h"

class DirManagerWorker : public QObject
{
    Q_OBJECT
public:
    explicit DirManagerWorker(QObject *parent = nullptr);
    virtual ~DirManagerWorker() override;

    virtual bool blockOtherThreads() const = 0;
    virtual bool revalidateDirStructureAfterWorkerHasFinished() const = 0;

    QThread* getThread();
signals:
    void finished(bool revalidateDirStructure);
    void runTask();
    void cancelled();
public slots:
    virtual void cancel();
    virtual void start();
protected:
    virtual void workBeforeLaunchThread();

    bool m_cancelled;
    QThread* m_thread;
};

#endif // DIRMANAGERWORKER_H
