#ifndef DIRDEEPSEARCHWORKER_H
#define DIRDEEPSEARCHWORKER_H

#include <QObject>
#include <QDebug>
#include <QThread>

#include <vector>
#include <string>

#include "dirmanagerworker.h"
#include "fileinfobd.h"
#include "staticfunctions_files.h"
#include "listfiles.h"

class DeepSearchResult{
public:
    std::string absPath;
    bool isDir;
    bool containingFiBDExists;
    FileInfoBD* closestParentDir;
};

//----------------------------------------------------------------------------------------

class DirDeepSearchWorker : public DirManagerWorker
{
    Q_OBJECT
public:
    explicit DirDeepSearchWorker(std::string keyword,
                                 FileInfoBD* root_dir,
                                 bool includeHiddenFiles,
                                 QThread* threadToMoveObjectsTo);
    explicit DirDeepSearchWorker(const DirDeepSearchWorker& ds);
    explicit DirDeepSearchWorker();

    DirDeepSearchWorker& operator=(const DirDeepSearchWorker& ds);

    virtual ~DirDeepSearchWorker() override;

    virtual bool blockOtherThreads() const override;
    virtual bool revalidateDirStructureAfterWorkerHasFinished() const override;

signals:
    void killHelpers();
    void deepSearchFinished(std::vector<std::string> matchingPaths, std::string keyword);
public slots:
    void run();
    void workerFinished(std::vector<DeepSearchResult> results,
                        FileInfoBD* dir);

protected:
    virtual void workBeforeLaunchThread() override;

private:
    void connectSignals();

    void search(FileInfoBD* dir);

    void elapseMatchingFolders();
    bool elapseMatchingFolder_hlpr(const std::string &match_absPath,
                                   const std::string &match_dir_path,
                                   bool isDir,
                                   FileInfoBD* closestParentDir);

    std::string m_keyword;
    FileInfoBD* m_root_dir;
    bool m_includeHiddenFiles;
    std::vector<DeepSearchResult> m_matches;
    int m_runningThreads;

    QThread* m_threadToMoveObjectsTo;
};

//----------------------------------------------------------------------------------------


class DirDeepSearchHelper: public QObject
{
    Q_OBJECT
public:
    explicit DirDeepSearchHelper(std::string keyword,
                                 FileInfoBD* dir,
                                 bool includeHiddenFiles,
                                 QThread* threadToMoveObjectsTo);
    virtual ~DirDeepSearchHelper() override;

signals:
    void finished(std::vector<DeepSearchResult>,
                  FileInfoBD* fi);
public slots:
    void run();
    void cancel();
private:
    void search(FileInfoBD* dir);
    void search(const QString& dir_path,
                FileInfoBD *closestParentDir);

    std::string m_keyword;
    FileInfoBD* m_dir;
    bool m_includeHiddenFiles;
    std::vector<DeepSearchResult> m_matches;
    bool m_cancelled;

    QThread* m_threadToMoveObjectsTo;
};

#endif // DIRDEEPSEARCHWORKER_H
