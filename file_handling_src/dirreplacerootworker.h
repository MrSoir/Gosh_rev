#ifndef DIRREPLACEROOTWORKER_H
#define DIRREPLACEROOTWORKER_H

#include <QObject>
#include <QDebug>
#include <QThread>

#include <vector>
#include <string>

#include "dirmanagerworker.h"
#include "fileinfobd.h"
#include "staticfunctions_files.h"

class DirReplaceRootWorker : public DirManagerWorker
{
    Q_OBJECT
public:
    explicit DirReplaceRootWorker(std::string root_path,
                                  FileInfoBD* current_rootDir,
                                  QThread* threadToMoveObjectsTo);
    explicit DirReplaceRootWorker(const DirReplaceRootWorker& w);
    explicit DirReplaceRootWorker();

    DirReplaceRootWorker& operator=(const DirReplaceRootWorker& w);

    virtual ~DirReplaceRootWorker() override;

    virtual bool blockOtherThreads() const override;
    virtual bool revalidateDirStructureAfterWorkerHasFinished() const override;
signals:
    void replaceRoot(FileInfoBD* newRoot, bool deleteOldRoot);
public slots:
    void run();

protected:
    virtual void workBeforeLaunchThread() override;
private:
    // falls path ein sub-folder vom dir ist, dann sucht findSubDirInDir diesen sub-folder rekursiv
    FileInfoBD* findSubDirInDir(FileInfoBD* dir, std::string path);
    FileInfoBD* findParentFromDir(FileInfoBD* dir, std::string path);

    std::string m_new_root_path;
    FileInfoBD* m_current_rootDir;

    QThread* m_threadToMoveObjectsTo;
};

#endif // DIRREPLACEROOTWORKER_H
