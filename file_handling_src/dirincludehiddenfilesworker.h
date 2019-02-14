#ifndef DIRINCLUDEHIDDENFILESWORKER_H
#define DIRINCLUDEHIDDENFILESWORKER_H


#include <QObject>
#include <QDebug>
#include <QThread>

#include <vector>
#include <string>

#include "dirmanagerworker.h"
#include "fileinfobd.h"
#include "staticfunctions_files.h"

class DirIncludeHiddenFilesWorker : public DirManagerWorker
{
public:
    explicit DirIncludeHiddenFilesWorker(FileInfoBD* root_dir,
                                         bool include,
                                         QThread* threadToMoveObjectsTo);
    explicit DirIncludeHiddenFilesWorker();
    explicit DirIncludeHiddenFilesWorker(const DirIncludeHiddenFilesWorker& wrkr);

    DirIncludeHiddenFilesWorker& operator=(const DirIncludeHiddenFilesWorker& wrkr);

    virtual ~DirIncludeHiddenFilesWorker() override;

    virtual bool blockOtherThreads() const override;
    virtual bool revalidateDirStructureAfterWorkerHasFinished() const override;
signals:
public slots:
    void run();
protected:
    virtual void workBeforeLaunchThread() override;
private:

    FileInfoBD* m_root_dir;
    bool m_include;

    QThread* m_threadToMoveObjectsTo;
};

#endif // DIRINCLUDEHIDDENFILESWORKER_H
