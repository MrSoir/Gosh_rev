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

#include "filesearcher.h"
#include "fileselector.h"
#include "filequeue.h"
#include "filewatcher.h"
#include "queuetasks.h"
#include "filedata.h"
#include "openfiles.h"
#include"dirmanager.h"
#include "dirmanagerinfo.h"
#include "directorystack.h"

#include "tree_viewer/graphicsview.h"
#include "tree_viewer/directoryselectionpane.h"

#include "file_manipulation/copyfiles.h"
#include "file_manipulation/zipfiles.h"
#include "file_manipulation/renamefile.h"
#include "file_manipulation/createfolder.h"
#include "file_manipulation/createfile.h"

#include "widgetcreator.h"

#define INIT_ZOOM_FACTOR 9

#define int_bd long long

class FileManager : public QObject,
                    public WidgetCreator
{
    Q_OBJECT
public:
    explicit FileManager(std::string root_path,
                         bool showHiddenFiles = false);
    virtual ~FileManager() override;

    void test();

    QLayout* genPane();

    virtual QWidget* createWidget() override;

    // for FileManagerInfo:
    QString curRootPath() const;
    QString curSearchResult() const;
    int maxDepth() const;
    int_bd selectionCount() const;
    int_bd displayedFileCount() const;
    int_bd indexCurSearchResult() const;
    int_bd searchIndex() const;
    int_bd searchResultsCount() const;
    bool includeHiddenFiles() const;
    bool inSearchMode() const;
    bool foldersSelected() const;
    bool filesSelected() const;
    bool selectionContainsZipFiles() const;
    std::vector<bool> depthIdElapsed() const;

signals:
    void addPathToFileWatcher(std::string path);

    // teilt dem Viewer mit, in die warteschleife zu verfallen bzw. wieder auszubrechen:
    void setViewerIntoWaitingLoop();
    void exitViewerWaitingLoop();



    // signals for FileQueue:
    void cancelFileQueueTask();
    void pauseFileQueueTask();
    void continueFileQueueTask();

    void addQueueTask(QueueTask* task);

    void openFoldersInTab_SGNL(std::vector<std::string> folders);



    // signals for GraphicsView:
    void closeCurrentTab(); // GraphicsView::requestCloseCurrentTab() (signal -> signal!)

    void focusGraphicsView(int_bd entry_id); // -> GraphicsView::focusId(int_bd)

    void startWaitingLoop(); // -> GraphicsView::startWaitingAnimation()
    void exitWaitingLoop();  // -> GraphicsView::killWaitingAnimation()

    void requestFocusSGNL(QDir); // -> GrahpicsView::requestFocus()

    void revalidateViewer_Entries(std::unordered_map<int_bd, FiBDViewer> entries); // -> GrahpicsView::receiveFileViewers
    void revalidateViewer_MetaData( FileManagerInfo* fmi ); // GraphicsView::receiveFileManagerMetaData
    void revalidateViewer_EntireData(std::unordered_map<int_bd, FiBDViewer> entries, FileManagerInfo* fmi);


    // signals for DirManager:
    void rootDirChanged(std::string);

    void deepSearch_dm(std::string keyword,
                       bool includeHiddenFiles);

    void elapse_dm(std::string path);
    void elapse_dm(std::vector<std::string> paths);
    void elapseRec_dm(std::string path);
    void elapseRec_dm(std::vector<std::string> paths);

    void collapse_dm(std::string path);
    void collapse_dm(std::vector<std::string> paths);
    void collapseRec_dm(std::string path);
    void collapseRec_dm(std::vector<std::string> paths);

    void deleteDir_dm(std::string dir);
    void deleteDirs_dm(std::vector<std::string> dirs);

    void cdUP_dm();

    void includeHiddenFiles_SGNL();
    void excludeHiddenFiles_SGNL();

    void sortDir_dm(Order order, std::string path);
    void sortDirs_dm(Order order, std::vector<std::string> paths);

    void sortAllDirs_dm(Order order);

public slots:
    void setRoot(const std::string& rootPath);
    void close();

    void selectionChanged();
    void searchResultsChanged();

    void elapseFolder(std::string path);
    void elapseFolder_rec(std::string path);
    void collapseFolder(std::string path);
    void collapseFolder_rec(std::string path);


    // DirectorySelectionPane -> FileManager:
    void setLastPathToRoot();

    // GrahpicsView -> FileManager:
    void setRoot_QDir(QDir dir); // setRoot(QDir) <- muss mit SIGNAL(...) - SLOT(...) connected werden!!!

    void saveGraphicsViewVBarValue(int value);
    void saveGraphicsViewHBarValue(int value);

    void setZoomFactor(int zoomFactor);

    void elapseAllFoldersOfDepthId(int id); // elapseAllFoldersOfDepthId
    void elapseSelectedFoldersRecursively(); // elapseSelectedFoldersRecursively
    void elapseSelectedFolders(); // elapseSelectedFolders
    void collapseSelectedFoldersRecursively(); // collapseSelectedFoldersRecursively
    void collapseSelectedFolders(); // collapseSelectedFolders
    void elapseOrCollapseFolderDependingOnCurrentState(QString path); // elapseOrCollapseFolderDependingOnCurrentState(QString)

    void setIncludeHiddenFiles(bool inclHdnFls); // showHiddenFilesSGNL

    void openSelectedFoldersInTab(); // requestOpenFoldersInTab

    void sortDir(QString dir, Order order); // sortDir
    void sortDirs(std::vector<QString> dirs, Order order);
    void sortAllFolders(Order order); // sortAllFolders

    void copySelectedFilePathToClipboard(); // copySelectedFilePathToClipboard
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

    void requestFocus(); // -> requestFocusSGNL()

    void initDragging(QString draggingSource); // initDragging(QString)

    void keyPressed(std::string s); // keyPressed(std::string)

    void setSelectionToRoot(); // setSelectionToRoot
    void setParentToRoot(); // cdUp

    void openTerminal(); // -> openTerminalSGNL()

    void zoomFactorChanged(int newZoomFactor); // zoomFactorChanged

    void requestFileViewerRevalidation(int_bd firstEntryToDispl, int_bd lastEntryToDisp); // requestFileViewerRevalidation



    // searching:   (depSearch wird hier direkt vom fileManager gehandelt, alles andere uebernimmt direkt der FileSearcher)
    void deepSearch(QString key_word);



    // DirManager -> FileManager:
    void dirChanged_dm(DirManagerInfo* changedDir);
    void treeChanged_dm(DirManagerInfo* entireTree);

    void deepSearchFinished_dm(std::vector<std::string> matchingPaths, std::string keyword);

    void focusPath(std::string absPath);

private:
    void setRoot_hlpr(std::string rootPath, bool addToDirStack = true);

    void connectSignals();
    void disconnectSiganls();

    void connectViewer(GraphicsView* viewer);
    void disconnectViewer(GraphicsView* viewer);
    void connectDirectorySelectionPane(DirectorySelectionPane* pane);


    void revalidateViewer_entries_hlpr();
    void revalidateViewer_metaData_hlpr();
    void revalidateViewer_EntireData_hlpr();
    std::unordered_map<int_bd, FiBDViewer> generateViewerData();

    void clearEntryContainers();

    void copyCutToClipboard_hlpr(bool deleteSourceAfterCopying);

    std::pair<std::vector<std::string>, bool> extractPathsFromPathsString(QString pathsStr);
    std::pair<std::vector<std::string>, bool> extractPathsFromUrls(const QList<QUrl>& urls);
    void copyFiles(std::vector<std::string> pathsToCopy, std::string targetPath, bool cut);

    std::string getBaseDirOfSelection() const;

    std::chrono::milliseconds getCurrentTime() const;

    void replaceTree(DirManagerInfo* tree);
    void revalidateTree();
    void revalidateTree_hlpr(DirManagerInfo* entry,
                          DirManagerInfo* firstNonCollapsedFold,
                          bool isCollapsed,
                          int_bd* cntr,
                          int_bd* cntr_clpsd,
                          int depthId,
                          int* maxDepthId);

    void printEntries();

//    --------------------------------

    std::string m_root_path;

    DirManager* m_dir_manager;
    DirManagerInfo* m_tree;

    std::unordered_map<std::string, DirManagerInfo*> m_folders; // path to FileInfoBD
    std::unordered_map<std::string, DirManagerInfo*> m_folders_colpsd;

    std::unordered_map<std::string, std::string> m_entry_to_firstElapsedFolder; // FileSearcher: hidden folders: path -> path of first non-hidden/elapsed folder
    std::unordered_set<std::string> m_paths;
    std::unordered_set<std::string> m_paths_colpsd;
    std::unordered_set<std::string> m_folder_paths;
    std::unordered_set<std::string> m_folder_paths_colpsd;

    std::unordered_map<std::string, int_bd> m_entries_order; // FileSearcher: path to order/id in list
    std::unordered_map<int_bd, std::string> m_order_entries; // FileSelector: order/id in list to path
    std::unordered_map<std::string, int_bd> m_entries_order_colpsd; // FileSearcher: path to order/id in list
    std::unordered_map<int_bd, std::string> m_order_entries_colpsd; // FileSelector: order/id in list to path

    std::unordered_map<std::string, int> m_path_depthId;
    std::unordered_map<std::string, int> m_path_depthId_colpsd;
    std::unordered_map<int, std::unordered_set<std::string>> m_depth_folders_colpsd; // FileManager: depthID of folder -> folder (wird fuer collapseFoldersOfDepthId benoetigt)

    std::unordered_map<std::string, std::string> m_path_fileNames; // FileSearcher: path to fileName
    std::unordered_map<std::string, std::string> m_path_fileNames_colpsd; // FileSearcher: path to fileName

    FileQueue* m_tasks_queue;

    std::vector<bool> m_depthId_elapsed;

    int_bd m_frstDispFile;
    int_bd m_lastDispFile;

    bool m_showHiddenFiles;

    std::chrono::milliseconds m_lastKeyPressed;
    std::string m_keysPressed;

    int m_zoomFactor;

    int m_graphicsViewVBarValueBackup;
    int m_graphicsViewHBarValueBackup;

    DirectoryStack* m_dirStack;

    FileSearcher* m_searcher;
    FileSelector* m_selector;
};

#endif // FILEMANAGER_H
