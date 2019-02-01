#ifndef DIRCD_UPWORKER_H
#define DIRCD_UPWORKER_H

#include <QObject>
#include <QDebug>
#include <QThread>

#include <vector>
#include <string>

#include "dirmanagerworker.h"
#include "fileinfobd.h"
#include "staticfunctions_files.h"

class DirCD_UpWorker : public DirManagerWorker
{
    Q_OBJECT
public:
    explicit DirCD_UpWorker(FileInfoBD* current_rootDir);
    virtual ~DirCD_UpWorker() override;

    virtual bool blockOtherThreads() const override;
    virtual bool revalidateDirStructureAfterWorkerHasFinished() const override;
signals:
public slots:
    void run();
private:
    FileInfoBD* m_current_rootDir;
};

#endif // DIRCD_UPWORKER_H
