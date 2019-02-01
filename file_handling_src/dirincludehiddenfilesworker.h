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
                                         bool include);
    virtual ~DirIncludeHiddenFilesWorker() override;

    virtual bool blockOtherThreads() const override;
    virtual bool revalidateDirStructureAfterWorkerHasFinished() const override;
signals:
public slots:
    void run();
private:

    FileInfoBD* m_root_dir;
    bool m_include;
};

#endif // DIRINCLUDEHIDDENFILESWORKER_H
