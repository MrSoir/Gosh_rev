#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QScrollBar>
#include <QSize>
#include <QRect>
#include <QPoint>
#include <QFileInfo>
#include <QDebug>
#include <QObject>
#include <functional>
#include <QTimer>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsItemGroup>
#include <QWidget>
#include <QDir>
#include <QPixmap>
#include <memory>
#include <thread>
#include <atomic>

#include "math.h"
#include "graphicsfile.h"
#include "limits.h"
#include "directoryselectordialog.h"
#include "staticfunctions.h"
#include "searchfiledialog.h"
#include "customgraphicitems.h"
#include "menubar.h"
#include "waitingbargraphicsitem.h"
#include "searchmenubd.h"
#include "elapsemenubd.h"
#include "rectcolor.h"
#include "windowselector.h"
#include "filemanagerinfo.h"
#include "file_handling_src/fibdviewer.h"
#include "orderby.h"

#define int_bd long long

//using namespace ORDERED_BY;

namespace FILE_ACTION {
    enum Action{
        COPY, CUT, PASTE, DUPLICATE,
        ZIP, UNZIP,
        RENAME,
        DELETE,
        OPEN, OPEN_WITH,
        TERMINAL,
        DETAILS,
        ELAPSE, ELAPSE_REC, // ELAPSE_REC == elapse RECursively
        TAB, CLOSE_TAB,
        LOAD,
        COLLAPSE, COLLAPSE_REC, // COLLAPSE_REC == collapse RECursively
        PATH,
        CANCEL_CURRENT_ACTION,
        REQUEST_FOCUS
    };
}


class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphicsView(FileManagerInfo* fmi,
                          std::unordered_map<int_bd, FiBDViewer> entriesToRender,
                          int hBarValue = 0,
                          int vBarValue = 0,
                          int zoomFactor = 9,
                          QWidget* parent = nullptr);
    ~GraphicsView();

//    void setRoot(std::weak_ptr<FileInfoBD> fi);

    void updateGraphicsView();

//    void setFilesCoordinator(std::weak_ptr<FilesCoordinator> filesCoor);

    int getVScrollBarValue();
    int getHScrollBarValue();

signals:
    void requestCloseCurrentTab();
    void requestOpenFoldersInTab();

    void nextSearchResultSGNL(); // -> nextSearchResult()
    void prevSearchResultSGNL(); // -> previousSearchResult()
    void closeSearchMenuSGNL();
    void searchForKeyWord(QString keyword);
    void deepSearch(QString keyword);

    void elapseAllFoldersOfDepthId(int id);

//    -------------------------

    void selectEntireContent();
    void clearSelection();
    void selectButtonUp(bool contrlPrsd, bool shiftPrsd);
    void selectButtonDown(bool contrlPrsd, bool shiftPrsd);
    void selectContent(QString entry, bool contrlPrsd, bool shiftPrsd);
//    void selectFromDisplayedContent(QFileInfo fi, bool contrlPrsd, bool shiftPrsd);
    void sortDir(QString fi, Order order);

    void copySelectedContent();
    void cutSelectedContent();
    void duplicateSelectedContent();
    void openSelectedContent();
    void openSelectedContentWith();

    void renameSelectedContent();
    void pasteFromClipboard();
    void paste(QString dropStr, QString targetPath);
    void deleteSelectedContent();

    void showDetailsOfSelectedContent();

    void zipSelectedContent();
    void unzipSelectedContent();

    void createNewFolderSGNL(); // -> createNewFolder()
    void createNewFileSGNL(); // -> createNewFile()

    void elapseSelectedFoldersRecursively();
    void elapseSelectedFolders();
    void collapseSelectedFoldersRecursively();
    void collapseSelectedFolders();
    void copySelectedFilePathToClipboard();

    void killCurrentBlockingAction(); // -> killCurrentBlockingActionSLT()

    void requestFocusSGNL(); // -> requestFocus()

    void initDragging(QString draggingSource);

    void keyPressed(std::string s);

    void setSelectionToRoot();
    void setRootFolder(QDir newRoot);


//    void sort(FileInfoBD fiBD, ORDER_BY order);
    void sortAllFolders(Order order);

    void openTerminalSGNL(); // -> openTerminal
    void cdUp();

    void zoomFactorChanged(int newZoomFactor);

    void showHiddenFilesSGNL(bool showHiddenFiles);

    void elapseOrCollapseFolderDependingOnCurrentState(QString path);

    void requestFileViewerRevalidation(int_bd firstDispFI, int_bd lastDispFI);

public slots:
    void receiveFileViewers(std::unordered_map<int_bd, FiBDViewer> new_files);
    void receiveFileManagerMetaData(FileManagerInfo* fmi);
    void receiveFileManagerData(std::unordered_map<int_bd, FiBDViewer> new_files, FileManagerInfo* fmi);

    void requestFocus();

//    void folderChanged(std::weak_ptr<const FileInfoBD> f = std::weak_ptr<FileInfoBD>());
    void revalidate();

    void focusId(int_bd id);

    void vScrollValueChanged(int newValue);
    void hScrollValueChanged(int newValue);
    void setHBarValue(int hBarValue);
    void setVBarValue(int vBarValue);

//    void setWaitingAnimation();
    void startWaitingAnimation();
    void killWaitingAnimation();

    void handleSearchKeyword(QString keyword, bool deepSearch);
protected:
     void EnterPressedBD(QKeyEvent* event);
     void keyPressEvent(QKeyEvent *event);
     void keyReleaseEvent(QKeyEvent *event);

     void resizeEvent(QResizeEvent *event);

     void mousePressEvent(QMouseEvent *event);
//     void mouseReleaseEvent(QMouseEvent *event);
     void mouseMoveEvent(QMouseEvent *event);

     void enterEvent(QEvent *event);
     void leaveEvent(QEvent* event);
private:
     void setFileManager_MetaData(FileManagerInfo* fmi);
     void revalFileManagerMetaData();

    void paintTopRectangle(const QPointF& center,
                           const QSize& size);

    int_bd getFirstToDispFi();
    int_bd getLastToDispFi();
    bool viewPortOutOfDisplayedRange();

    qreal getDisplayableHeight();
    void rePaintCanvas();

    void setWaitingBarSizeAndPos();

    void paintFileInfo(const FiBDViewer& fi,
                       int_bd rowId=0, int_bd colId=0,
                       std::shared_ptr<DynamicFunctionCaller<QString, std::function<bool()>>> caller = std::shared_ptr<DynamicFunctionCaller<QString, std::function<bool()>>>(),
                       std::shared_ptr<DynamicFunctionCaller<QString, std::function<bool(Order)>>> sortCaller = std::shared_ptr<DynamicFunctionCaller<QString,std::function<bool(Order)>>>());

    qreal getRelativeHorBarValue();
    qreal getRelativeVerBarValue();
    int getAbsoluteHorBarValue();
    int getAbsoluteVerticalBarValue();
    int getViewportYOffset();

    void addSearchMenu();
    void launchSearchMode();
    void nextSearchResult();
    void prevSearchResult();
    void closeSearchMenu();

    QString getFocusedSearchPath();
    long getSearchIndex();
    long getSearchResultsCount();

    void addMenuBar();
    void closeMenuBar();

    void executeFileAction(FILE_ACTION::Action action);
//    void executeFileAction(std::function<void (std::shared_ptr<FilesCoordinator>)> fctn);

    void addContentBar();
    void closeContentBar();
    void addElapseBar();

    void zoomOut();
    void zoomIn();

    void sortAllFoldersDialog();

    void createNewFolder();
    void createNewFile();

    void openTerminal();

    void showHiddenFiles();

    void showRootSelector();

    void revalidateRowHeight();

    bool inSearchMode();

    void revalFirstAndLastDisplayedFI(bool revalIfStillInBounds=false);

    QGraphicsScene* m_scene;

    int m_fontSize;

    int m_rowHeight = 20;
    int m_colOffs = 15;

    int_bd m_firstDispFI = 0;
    int_bd m_lastDispFI = 0;
    int_bd m_curDispFI = 0;
    int_bd m_fileCount = 0;
    int_bd m_minDisplayedEntries = 30;
    int_bd m_filePuffer = 10;

    QGraphicsItemGroup* m_graphicsGroup = new QGraphicsItemGroup();

    SearchMenuBD* m_searchMenu = nullptr;
    MenuBar* m_menuBar = nullptr;
    MenuBar* m_contBar = nullptr;

    QPainterPath* m_upperRect = nullptr;
    int m_upperRectWidth = 40;
    QPoint* m_mouseP = nullptr;

    void closeAllSubMenus();

    bool m_paintUpperRect = false;
//    bool m_paintSearchMenu = false;
    bool m_paintMenuBar = false;
    bool m_paintContBar = false;
    int m_searchMenuHeight = 50;
    int m_menuBarHeight = 40;
    int m_contBarWidth = 45;

    bool m_shwRtSel = true;
    int m_rootSelWidth = 80;
    int m_rootSelHeight = 20;
    int m_elapseBarHeight = 15;
    QColor m_elapseCol1 = QColor(255,255,255, 255),
           m_elapseCol2 = QColor(200,255,200, 255);

    std::atomic<bool> m_isLoading;
    std::atomic<int> m_loadingId;
    int m_loadingLength = 90;
    int m_loadingPeriodMS = 20;
    QTimer* m_animationTimer;
    WaitingBarGraphicsItem* m_waitingBar;

    QSize m_cancelBtnSize;
    GraphicItemsBD::PixmapRect* m_cancelBtn = nullptr;
    bool m_waitingBarIsAddedToScene = false;
    int m_waitingBarHeight = 40;

    RectColor m_rectColor;

    FileManagerInfo* m_fileMangrInfo;

//    std::vector<FiBDViewer> m_entriesToRender;
    std::unordered_map<int_bd, FiBDViewer> m_entriesToRender;
};

#endif // GRAPHICSVIEW_H
