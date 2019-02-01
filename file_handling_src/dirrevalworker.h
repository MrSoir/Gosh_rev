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
    explicit DirRevalWorker(FileInfoBD* fi);
    explicit DirRevalWorker(std::vector<FileInfoBD*> fis);

    virtual ~DirRevalWorker() override;

    virtual bool blockOtherThreads() const override;
    virtual bool revalidateDirStructureAfterWorkerHasFinished() const override;
signals:
public slots:
    void run();
private:

    std::vector<FileInfoBD*> m_fis;
};

#endif // DIRREVALWORKER_H