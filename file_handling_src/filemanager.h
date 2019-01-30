#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QLayout>
#include <QClipboard>
#include <QList>
#include <QUrl>
#include <QMimeData>
#include <QTextStream>
#include <QVector>
#include <QClipboard>
#include <QMimeData>
#include <QDrag>
#include <QDropEvent>

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <functional>
#include <utility>
#include <memory>
#include <chrono>

#include "fileinfobd.h"
#include "filesearcher.h"
#include "fileselector.h"
#include "fibdviewer.h"
#include "filequeue.h"
#include "filewatcher.h"
#include "queuetasks.h"

#include "tree_viewer/graphicsview.h"

#include "file_manipulation/copyfiles.h"
#include "file_manipulation/zipfiles.h"
#include "file_manipulation/renamefile.h"
#include "file_manipulation/createfolder.h"
#include "file_manipulation/createfile.h"
#include "openfiles.h"

//class FileViewer : public QObject
//{
//    Q_OBJECT
//signals:
//    void cancel();
//    void pause();
//public slots:
//    virtual void repaint(std::vector<FiBDViewer> fileToRepaint, int firstIdToRender = -1, int lastIdToRender = -1) = 0;
//    virtual void startWaitingLoop() = 0;
//    virtual void exitWaitingLoop() = 0;
//};

class FiBD {
public:
    explicit FiBD(FileInfoBD* fiBD, bool isFolder)
        : fiBD(fiBD),
          isFolder(isFolder)
    {}

    FileInfoBD* fiBD;
    bool isFolder;
};

class FileManager : public QObject
{
    Q_OBJECT
public:
    explicit FileManager(std::string root_path,
                         bool showHiddenFiles = false);
    ~FileManager();

    void test();

    QLayout* genPane();
signals:
    void addPathToFileWatcher(std::string path);

    void revalidateViewer(std::vector<FiBDViewer> entries);

    // searching:
    void search(std::string key_word);
    void clearSearch();
    void showSearch();
    void hideSearch(); // closeSearchMenuSGNL
    void nextSearchResult(); // nextSearchResultSGNL
    void prevSearchResult(); // prevSearchResultSGNL

    // teilt dem Viewer mit, in die warteschleife zu verfallen bzw. wieder auszubrechen:
    void setViewerIntoWaitingLoop();
    void exitViewerWaitingLoop();
    void cancelFileQueueTask();
    void pauseFileQueueTask();
    void continueFileQueueTask();
    void startWaitingLoop();
    void exitWaitingLoop();

    void addQueueTask(QueueTask* task);

    void includeHiddenFiles(bool);
    void requestFocusSGNL(QDir);

    void closeCurrentTab(); // -> GraphicsView::requestCloseCurrentTab
    void openFoldersInTab(std::vector<std::string> folders);

    void focusGraphicsView(int entry_id);
public slots:
    void setRoot(const std::string& rootPath);
    void close();

    void closeFileInfoBD(std::string path);

    void selectionChanged();
    void searchResultsChanged();

    void directoryChanged(std::string path);

    void revalidateEntries();

    void elapseFolder(std::string path);
    void elapseFolder_rec(std::string path);
    void collapseFolder(std::string path);
    void collapseFolder_rec(std::string path);
    void showHiddenFiles(std::string path);
    void showHiddenFiles_rec(std::string path);
    void hideHiddenFiles(std::string path);
    void hideHiddenFiles_rec(std::string path);

    void elapseAllFoldersOfDepthId(int id); // elapseAllFoldersOfDepthId
    void collapseAllFoldersOfDepthId(int id); // collapseAllFoldersOfDepthId
    void elapseSelectedFoldersRecursively();
    void elapseSelectedFolders();
    void collapseSelectedFoldersRecursively();
    void collapseSelectedFolders();
    void elapseOrCollapseFolderDependingOnCurrentState(QString path);

    void setIncludeHiddenFiles(bool inclHdnFls);
//---------------------------------------------------------------------
    void openSelectedFoldersInTab(); // requestOpenFoldersInTab

    void searchForKeyWord(QString keyword, bool deepsearch); // searchForKeyWord

    void selectEntireContent(); // selectEntireContent
    void clearSelection(); // clearSelection
    void selectButtonUp(bool cntrlPrsd, bool shiftPrsd); // selectButtonUp
    void selectButtonDown(bool cntrlPrsd, bool shiftPrsd); // selectButtonDown
    void selectEntry(QString entry, bool contrlPrsd, bool shiftPrsd); // selectContent

    void sortDisplFolder(QString dir, Order order); // sortFromDisplayedContent
    void sortAllFolders(Order order);

    void copySelectedFilePathToClipboard();
    void copySelectedContent(); // copySelectedContent
    void cutSelectedContent(); // cutSelectedContent
    void duplicateSelectedContent(); // duplicateSelectedContent
    void openSelectedContent(); // openSelectedContent
    void openSelectedContentWith(); // openSelectedContentWith

    void renameSelectedContent(); // renameSelectedContent

    void pasteFromClipboard(); // pasteFromClipboard
    void paste(QString dropStr, QString targetPath); // paste
    void deleteSelectedContent(); // deleteSelectedContent

    void showDetailsOfSelectedContent(); // showDetailsOfSelectedContent
    void zipSelectedContent(); // zipSelectedContent
    void unzipSelectedContent(); // unzipSelectedContent

    void createNewFolder(); // createNewFolderSGNL
    void createNewFile(); // createNewFileSGNL

    void killCurrentBlockingAction(); // -> killCurrentBlockingAction()

    void requestFocus(); // -> requestFocus()

    void initDragging(QString draggingSource);

    void keyPressed(char c);

    void setSelectionToRoot();
    void setParentToRoot();

    void openTerminal(); // -> openTerminal

    void zoomFactorChanged(int newZoomFactor);

    void showHiddenFiles(bool showHiddenFiles);
private:
    void deleteRoot();
    void deleteFileWatcher();

    void connectSignals();
    void disconnectSiganls();

    void revalidateViewer_helper();

    void clearEntryContainers();

    void removeFiBDFromContainers(std::string path);

    void evalFirstExistingRootFolder();

    void copyCutToClipboard_hlpr(bool deleteSourceAfterCopying);

    std::pair<std::vector<std::string>, bool> extractPathsFromPathsString(QString pathsStr);
    std::pair<std::vector<std::string>, bool> extractPathsFromUrls(const QList<QUrl>& urls);
    void copyFiles(std::vector<std::string> pathsToCopy, std::string targetPath, bool cut);

    std::string getBaseDirOfSelection() const;

    std::chrono::milliseconds getCurrentTime() const;

    void focusPath(const std::string& absPath);

//    --------------------------------

    FileInfoBD* m_root_folder;
    std::string m_root_path;

    std::unordered_map<std::string, FileInfoBD*> m_folders; // path to FileInfoBD
    std::unordered_map<std::string, FileInfoBD*> m_folders_colpsd;

    std::unordered_map<std::string, std::string> m_entry_to_firstElapsedFolder; // FileSearcher: hidden folders: path -> path of first non-hidden/elapsed folder
    std::unordered_set<std::string> m_paths;
    std::unordered_set<std::string> m_paths_colpsd;
    std::unordered_set<std::string> m_folder_paths;
    std::unordered_set<std::string> m_folder_paths_colpsd;

    std::unordered_map<std::string, int> m_entries_order; // FileSearcher: path to order/id in list
    std::unordered_map<int, std::string> m_order_entries; // FileSelector: order/id in list to path
    std::unordered_map<std::string, int> m_entries_order_colpsd; // FileSearcher: path to order/id in list
    std::unordered_map<int, std::string> m_order_entries_colpsd; // FileSelector: order/id in list to path

    std::unordered_map<std::string, FiBD> m_entries; // FileNamanger: path to FiBD struct {FileInfoBD | isFolder}
    std::unordered_map<std::string, FiBD> m_entries_colpsd; // FileNamanger: path to FiBD struct {FileInfoBD | isFolder}

    std::unordered_map<std::string, std::string> m_fileNames; // FileSearcher: path to fileName
    std::unordered_map<std::string, std::string> m_fileNames_colpsd; // FileSearcher: path to fileName

    std::unordered_map<int, std::unordered_set<FileInfoBD*>> m_depth_folders_colpsd; // FileManager: depthID of folder -> folder (wird fuer collapseFoldersOfDepthId benoetigt)

    FileSearcher* m_searcher;
    FileSelector* m_selector;
    FileQueue* m_tasks_queue;

    FileWatcher* m_watcher;

    GraphicsView* m_viewer;

    std::vector<bool> m_depthId_elapsed;

    int m_frstDispFile;
    int m_lastDispFile;

    bool m_showHiddenFiles;
    bool m_inSearchMode;

    std::chrono::milliseconds m_lastKeyPressed;
    std::string m_keysPressed;

    int m_zoomFactor;
};

#endif // FILEMANAGER_H
