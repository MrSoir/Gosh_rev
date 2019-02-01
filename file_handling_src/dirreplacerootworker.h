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
                                  FileInfoBD* current_rootDir);
    virtual ~DirReplaceRootWorker() override;

    virtual bool blockOtherThreads() const override;
    virtual bool revalidateDirStructureAfterWorkerHasFinished() const override;
signals:
public slots:
    void run();
private:
    // falls path ein sub-folder vom dir ist, dann sucht findSubDirInDir diesen sub-folder rekursiv
    FileInfoBD* findSubDirInDir(FileInfoBD* dir, std::string path);

    std::string m_new_root_path;
    FileInfoBD* m_current_rootDir;
};

#endif // DIRREPLACEROOTWORKER_H
