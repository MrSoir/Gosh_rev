#include "graphicsview.h"


GraphicsView::GraphicsView(FileManagerInfo* fmi,
                           std::unordered_map<int_bd, FiBDViewer> entriesToRender,
                           int hBarValue,
                           int vBarValue,
                           int zoomFactor,
                           QWidget* parent)
    : QGraphicsView(parent),
      m_scene(new QGraphicsScene()),
      m_fontSize(zoomFactor),
      m_isLoading(false),
      m_loadingId(0),
      m_animationTimer(new QTimer(this)),
      m_fileMangrInfo(fmi),
      m_entriesToRender(entriesToRender)
{
    revalFileManagerMetaData();

    revalidateRowHeight();

    connect(m_animationTimer, &QTimer::timeout, [=](){
        int id = m_loadingId;
        m_loadingId = (id+1) % m_loadingLength;

        QString cursor_pixmap_path = QString("%1%2%3")
                .arg("pics")
                .arg(QDir::separator())
                .arg("MrSoirIcon_cursor.png");
        QPixmap cursor_pixmap = QPixmap(cursor_pixmap_path);

        QTransform trans;
        double id_rl = static_cast<double>(id);
        double pi = 3.14159265;
        double d_loadeingLength = static_cast<double>(m_loadingLength);
        double value = (id_rl / d_loadeingLength) * (2.0*pi);
        double scaleFactor = (sin(value) +1.0) * 0.3;

        trans.rotate(id * 4);
        trans.scale(1.0 + scaleFactor, 1.0 + scaleFactor);

        cursor_pixmap = cursor_pixmap.transformed(trans);

        double cursor_width = cursor_pixmap.size().width();
        double cursor_height = cursor_pixmap.size().height();

        setCursor(QCursor(cursor_pixmap,
                          static_cast<int>(-cursor_width*0.5),
                          static_cast<int>(-cursor_height*0.5)));

        rePaintCanvas();
    });

    // wichtig: damit die scene immer ganz oben beginnt!
    this->setAlignment(Qt::AlignTop);

//    this->setContentsMargins(0, 0, 0, 0);

    this->setScene(m_scene);

    this->verticalScrollBar();
    connect(this->verticalScrollBar(),   &QScrollBar::valueChanged,  this, &GraphicsView::vScrollValueChanged);
    connect(this->horizontalScrollBar(), &QScrollBar::valueChanged,  this, &GraphicsView::hScrollValueChanged);

    rePaintCanvas();

    setHBarValue(hBarValue);
    setVBarValue(vBarValue);
}

GraphicsView::~GraphicsView()
{
    qDebug() << "~GrphicsView";
    if(m_mouseP != nullptr)
        delete m_mouseP;

    m_scene->clear();

    if(m_animationTimer != nullptr &&
            m_animationTimer->isActive())
        m_animationTimer->stop();
    m_animationTimer->deleteLater();

    if(m_fileMangrInfo)
        delete m_fileMangrInfo;
    m_fileMangrInfo = nullptr;

    delete m_upperRect;
}

void GraphicsView::revalidate(){
    rePaintCanvas();
}

int_bd GraphicsView::getFirstToDispFi(){
    double yOffs = getViewportYOffset();
    double d_rowHeight =  static_cast<double>(m_rowHeight);
    double row_offset = yOffs / d_rowHeight;

    int_bd firstToDispFi = static_cast<int_bd>(row_offset) - m_filePuffer;
    if(firstToDispFi < 0)
        firstToDispFi = 0;

    return firstToDispFi;
}
int_bd GraphicsView::getLastToDispFi(){
    double vwPrtHght = getDisplayableHeight();
    double frstToDisp = static_cast<double>(getFirstToDispFi());
    double d_rowHeight = static_cast<double>(m_rowHeight);
    double d_filePuffer = static_cast<double>(m_filePuffer);

    double entries_InViewport = (vwPrtHght / d_rowHeight);

    int_bd lastToDispFi = static_cast<int_bd>(frstToDisp + entries_InViewport + d_filePuffer);
    if(lastToDispFi >= m_fileCount)
        lastToDispFi = m_fileCount-1;

    return lastToDispFi;
}
bool GraphicsView::viewPortOutOfDisplayedRange(){
    auto newFirstToDispFi = getFirstToDispFi();
    auto newLastToDispFi = getLastToDispFi();
    auto curFirst = m_firstDispFI;
    auto curLast =  m_lastDispFI;

    if(curFirst < 0)
        curFirst = 0;
    if(curLast >= m_fileCount)
        curLast  = m_fileCount-1;

//    qDebug() << "newFirstToDispFi: " << newFirstToDispFi
//             << "    m_firstDispFI: " << m_firstDispFI
//             << "   newLastToDispFi: " << newLastToDispFi
//             << "   curLast: " << curLast
//             << "   viewPortOutOfDisplayedRange: " <<
//                (newFirstToDispFi < curFirst ||
//                 newLastToDispFi  > curLast);
    return newFirstToDispFi < curFirst ||
            newLastToDispFi  > curLast;
}

qreal GraphicsView::getDisplayableHeight()
{
    return this->viewport()->height()-m_elapseBarHeight;
}

void GraphicsView::vScrollValueChanged(int newValue)
{
    bool revalidate = false;
    if( viewPortOutOfDisplayedRange() ){
        revalidate = true;
    }
    if(!revalidate && m_graphicsGroup){
        m_graphicsGroup->setY(newValue);
    }

    if(revalidate)
    {
        revalFirstAndLastDisplayedFI();
    }
}

void GraphicsView::hScrollValueChanged(int newValue)
{
    if(m_graphicsGroup){
        m_graphicsGroup->setX(newValue);
    }
}

void GraphicsView::startWaitingAnimation()
{
    this->closeAllSubMenus();

    m_isLoading = true;
    m_animationTimer->start(m_loadingPeriodMS);
}
void GraphicsView::killWaitingAnimation()
{
    if(m_animationTimer && m_animationTimer->isActive()){
        m_isLoading = false;
        this->setCursor(QCursor(Qt::ArrowCursor));
        m_animationTimer->stop();
        update();
    }
}

void GraphicsView::closeAllSubMenus(){
    m_paintMenuBar = false;
    m_paintContBar = false;

    closeSearchMenu();
}


void GraphicsView::EnterPressedBD(QKeyEvent* event)
{
    Q_UNUSED(event);

    if( inSearchMode() )
        emit nextSearchResult();
    else
        emit openSelectedContent();
}

void GraphicsView::keyPressEvent(QKeyEvent *event)
{

    bool contrlPrsd = StaticFunctions::controlPressed() || event->key() == Qt::Key_Control;
    bool shiftPrsd = StaticFunctions::shiftPressed() || event->key() == Qt::Key_Shift;
//    bool altPrsd = StaticFunctions::altPressed() || event->key() == Qt::Key_Alt || event->key() == Qt::Key_AltGr; // wird bisher nicht gebraucht

//    qDebug() << "in keyPressed: controlPressed: " << contrlPrsd << "  shiftPressed: " << shiftPrsd;

    if(event->key() == Qt::Key_Escape){
        if(inSearchMode() || m_paintMenuBar || m_paintContBar){
            closeAllSubMenus();
            update();
        }else{
            emit clearSelection();
        }
    }else if (event->key() == Qt::Key_Enter ||
              event->key() == Qt::Key_Return){
        EnterPressedBD(event);
    }else if(event->key() == Qt::Key_Backspace){
        emit cdUp();
    }else if(event->key() == Qt::Key_Up){
        emit selectButtonUp(contrlPrsd, shiftPrsd);
    }else if(event->key() == Qt::Key_Down){
        emit selectButtonDown(contrlPrsd, shiftPrsd);
    }else if(event->key() == Qt::Key_Control){

    }else if(event->key() == Qt::Key_Shift){

    }else if(event->key() == Qt::Key_Plus){
        if(contrlPrsd)
            zoomIn();
    }else if(event->key() == Qt::Key_Minus){
        if(contrlPrsd)
            zoomOut();
    }else if(event->key() == Qt::Key_F4){
        if(contrlPrsd){
            executeFileAction(FILE_ACTION::Action::CLOSE_TAB);
        }
    }else if(event->key() == Qt::Key_F5){

    }else if(event->key() == Qt::Key_Alt ||
             event->key() == Qt::Key_AltGr){

    }else if (event->key() == Qt::Key_Left)
    {
        executeFileAction(FILE_ACTION::Action::COLLAPSE);
    }else if (event->key() == Qt::Key_Right)
    {
        executeFileAction(FILE_ACTION::Action::ELAPSE);
    }else if(event->key() == Qt::Key_A){
        if(StaticFunctions::controlPressed()){
            emit selectEntireContent();
        }
    }else if(event->key() == Qt::Key_C){
        if(contrlPrsd){
            emit copySelectedContent();
        }
    }else if(event->key() == Qt::Key_F){
        if(contrlPrsd){
            this->launchSearchMode();
        }
    }else if(event->key() == Qt::Key_M){
        if(contrlPrsd){
            this->createNewFile();
        }
    }else if(event->key() == Qt::Key_N){
        if(contrlPrsd){
            createNewFolder();
        }
    }else if(event->key() == Qt::Key_R){
        if(contrlPrsd){
            emit renameSelectedContent();
        }
    }else if(event->key() == Qt::Key_T){
        if(contrlPrsd){
            openTerminal();
        }
    }else if(event->key() == Qt::Key_V){
        if(contrlPrsd){
            emit pasteFromClipboard();
        }
    }else if(event->key() == Qt::Key_X){
        if(contrlPrsd){
            emit cutSelectedContent();
        }
    }else if(event->key() == Qt::Key_W){
        if(contrlPrsd){
            executeFileAction(FILE_ACTION::Action::TAB);
        }
    }else if(event->key() == Qt::Key_H){
        if(contrlPrsd){
            showHiddenFiles();
        }
    }else if(event->key() == Qt::Key_Delete){
        emit deleteSelectedContent();
    }

    if( !contrlPrsd && !shiftPrsd )
    {
        QString txt = event->text();
        bool startsWithBackslash = txt.startsWith("\\");
        if( !startsWithBackslash && !txt.isEmpty() )
            emit keyPressed(txt.toStdString());
    }

    this->rePaintCanvas();
}

void GraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
//    if(event->key() == Qt::Key_Control){
//        m_ctrl_prsd = false;
//    }else if(event->key() == Qt::Key_Shift){
//        m_shft_prsd = false;
//    }else if(event->key() == Qt::Key_Alt ||
//             event->key() == Qt::Key_AltGr){
//        m_alt_prsd = false;
//    }
    this->rePaintCanvas();
}

void GraphicsView::resizeEvent(QResizeEvent *event)
{
    rePaintCanvas();
    return QGraphicsView::resizeEvent(event);
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    return QGraphicsView::mousePressEvent(event);
}
void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if(m_mouseP)
        delete m_mouseP;

    m_mouseP = new QPoint(event->pos());

    int upperRectBound = 70;

    QPoint scrAdjMouP = QPoint(m_mouseP->x() + this->horizontalScrollBar()->value(),
                               m_mouseP->y() + this->verticalScrollBar()->value());

    if(!m_paintMenuBar && !inSearchMode() && m_upperRect &&
            m_upperRect->contains(scrAdjMouP)){
        m_paintUpperRect = false;
        m_paintMenuBar = true;
        rePaintCanvas();
    }else if(!m_paintMenuBar){
        if (m_paintUpperRect && m_mouseP->y() > upperRectBound){
            m_paintUpperRect = false;
            rePaintCanvas();
        }else if (!m_paintUpperRect && m_mouseP->y() <= upperRectBound){
            m_paintUpperRect = true;
            rePaintCanvas();
        }
    }
    return QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    executeFileAction(FILE_ACTION::Action::REQUEST_FOCUS); // <- keyboard-focus: damit sofort, wenn man mit der maus ueber das fenster geht, das fenster den fokus hat
}

void GraphicsView::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
}

void GraphicsView::setFileManager_MetaData(FileManagerInfo* fmi)
{
    if(m_fileMangrInfo)
        delete m_fileMangrInfo;

    m_fileMangrInfo = fmi;
    revalFileManagerMetaData();

//    qDebug() << "m_firstDispFI: " << m_firstDispFI;
//    qDebug() << "m_lastDispFI: " << m_lastDispFI;
}
void GraphicsView::revalFileManagerMetaData()
{
    m_fileCount = m_fileMangrInfo->displayedFileCount();
    revalFirstAndLastDisplayedFI();
}


void GraphicsView::updateGraphicsView()
{
    this->viewport()->update();
}

int GraphicsView::getVScrollBarValue()
{
    return this->verticalScrollBar()->value();
}

int GraphicsView::getHScrollBarValue()
{
    return this->horizontalScrollBar()->value();
}

void GraphicsView::receiveFileViewers(std::unordered_map<int_bd, FiBDViewer> new_files)
{
//    qDebug() << "GraphicsView::receiveFileViewers -  new_files:" << new_files.size();
    m_entriesToRender = new_files;
    revalidate();
}

void GraphicsView::receiveFileManagerMetaData(FileManagerInfo* fmi)
{
//    qDebug() << "GraphicsView::receiveFileManagerMetaData";

    setFileManager_MetaData(fmi);

    revalidate();
}

void GraphicsView::receiveFileManagerData(std::unordered_map<long long, FiBDViewer> new_files,
                                          FileManagerInfo *fmi)
{
//    qDebug() << "GraphicsView::receiveFileManagerData";

    m_entriesToRender = new_files;
    setFileManager_MetaData(fmi);
    revalidate();
}

void GraphicsView::requestFocus()
{
    this->setFocus();
}

void GraphicsView::focusId(int_bd id)
{
    revalidate(); // das revalidate macht eigetnlich das repaintAnyway komplett zunichte. aber:
    // wenn beim suchergebnis bisher nur 10 dateien angezeigt wurden -> vScrollBar also nicht existent
    // und nun 1000 files dargestellt werden und man versucht vScrollBar->setValue(1000) zu setzen,
    // juckt das den vScrollBar nicht und sein wert bleibt bei 0.

    double potSearchMenuHeight = (inSearchMode() ? static_cast<double>(m_searchMenuHeight) : 0.0);

    double yStart = this->verticalScrollBar()->value() + m_elapseBarHeight + potSearchMenuHeight;
    double paneHeight = this->viewport()->height() -m_elapseBarHeight - potSearchMenuHeight;

    double d_rowHeight = static_cast<double>(m_rowHeight);

    int startId = static_cast<int>(yStart / d_rowHeight);
    int endId = startId + static_cast<int>(paneHeight / d_rowHeight-1.0);
    if(endId < startId)
        endId = startId;

    bool repaintingIsNecessary = false;
    if(startId > id){
        double newVScrBrVal = id * m_rowHeight - m_elapseBarHeight - potSearchMenuHeight + m_rowHeight - 10;
        if(newVScrBrVal < 0)
            newVScrBrVal = 0;
        this->verticalScrollBar()->setValue( static_cast<int>(newVScrBrVal) );
        repaintingIsNecessary = true;
    }else if(endId < id){
        double newVScrBrVal = id * m_rowHeight - m_elapseBarHeight
                - (this->viewport()->height()-m_elapseBarHeight) + m_rowHeight*2;
        if(newVScrBrVal < 0)
            newVScrBrVal = 0;
        this->verticalScrollBar()->setValue( static_cast<int>(newVScrBrVal) );

        repaintingIsNecessary = true;
    }

    if(repaintingIsNecessary){
        revalFirstAndLastDisplayedFI(true);
    }
}

void GraphicsView::setHBarValue(int hBarValue)
{
    this->horizontalScrollBar()->setValue(hBarValue);
}

void GraphicsView::setVBarValue(int vBarValue)
{
    this->verticalScrollBar()->setValue(vBarValue);
}

void GraphicsView::paintTopRectangle(const QPointF& center, const QSize& size)
{
    auto width  = static_cast<double>(size.width());
    auto height = static_cast<double>(size.height());

    const QSizeF halfSize(width*0.5, height*0.5);

    QPointF p1(-halfSize.width(), -halfSize.height());
    QPointF p2(+halfSize.width(), -halfSize.height());
    QPointF p3(0, +halfSize.height());
    p1 += center;
    p2 += center;
    p3 += center;

    if(m_upperRect != nullptr){
        delete m_upperRect;
    }

    m_upperRect = new QPainterPath();
    m_upperRect->moveTo(p1);
    m_upperRect->lineTo(p2);
    m_upperRect->lineTo(p3);
    m_upperRect->closeSubpath();

    QGraphicsPathItem* painterPthItm = new QGraphicsPathItem();
    painterPthItm->setBrush(QBrush(StaticFunctions::getGoshBlueColor()));
    painterPthItm->setPen(QPen(QColor(0,0,0,100), 1, Qt::SolidLine));
    painterPthItm->setPath(*m_upperRect);

    m_graphicsGroup->addToGroup(painterPthItm);
}

void GraphicsView::addMenuBar(){
    m_menuBar = new MenuBar(this->viewport()->width(),
                            QPointF(this->viewport()->width()*0.5, m_elapseBarHeight));
    m_menuBar->setPosition(QPoint(this->horizontalScrollBar()->value(),
                                   this->verticalScrollBar()->value()));

    auto menu_caller = std::make_shared<DynamicFunctionCaller<QString,std::function<void()>>>();
    auto searchModeFunc     = [=](){ launchSearchMode(); };
    auto zoomInFunc         = [=](){ zoomIn(); };
    auto zoomOutFunc        = [=](){ zoomOut(); };
    auto sortFunc           = [=](){ sortAllFoldersDialog(); };
    auto newFolderFunc      = [=](){ createNewFolder(); };
    auto newFileFunc        = [=](){ createNewFile(); };
    auto terminalFunc       = [=](){ openTerminal(); };
    auto incognitoFunc      = [=](){ showHiddenFiles(); };
    auto closeMenuBarFunc   = [=](){ closeMenuBar(); };

    menu_caller->setFunction(QString("buttonFunction0"), searchModeFunc);
    menu_caller->setFunction(QString("buttonFunction1"), zoomInFunc);
    menu_caller->setFunction(QString("buttonFunction2"), zoomOutFunc);
    menu_caller->setFunction(QString("buttonFunction3"), sortFunc);
    menu_caller->setFunction(QString("buttonFunction4"), newFolderFunc);
    menu_caller->setFunction(QString("buttonFunction5"), newFileFunc);
    menu_caller->setFunction(QString("buttonFunction6"), terminalFunc);
    menu_caller->setFunction(QString("buttonFunction7"), incognitoFunc);
    menu_caller->setFunction(QString("buttonFunction8"), closeMenuBarFunc);

    QString incognitoImageFileName = m_fileMangrInfo->includeHiddenFiles() ? tr("incognito.png") : tr("incognito_NOT.png");

    auto painting_caller = std::make_shared<DynamicFunctionCaller<QString,std::function<void(QPainter*,QRectF)>>>();
    auto paintSearchModeFunc     = [=](QPainter* painter, QRectF rct){ StaticFunctions::paintLoupe(painter, rct, StaticFunctions::SHAPE::NONE); };
    auto paintZoomInFunc         = [=](QPainter* painter, QRectF rct){ StaticFunctions::paintLoupe(painter, rct, StaticFunctions::SHAPE::PLUS); };
    auto paintZoomOutFunc        = [=](QPainter* painter, QRectF rct){ StaticFunctions::paintLoupe(painter, rct, StaticFunctions::SHAPE::MINUS); };
    auto paintSortFunc           = [=](QPainter* painter, QRectF rct){
        QString cursor_pixmap_path = QString("%1%2%3").arg("pics").arg(QDir::separator()).arg("sort_icon.png");
        StaticFunctions::paintPixmapRect(painter, QPixmap(cursor_pixmap_path), rct ,Qt::transparent,Qt::transparent,Qt::transparent); };
    auto paintNewFoldFunc           = [=](QPainter* painter, QRectF rct){
        QString cursor_pixmap_path = QString("%1%2%3").arg("pics").arg(QDir::separator()).arg("empty_fold_icon.png");
        StaticFunctions::paintPixmapRect(painter, QPixmap(cursor_pixmap_path), rct ,Qt::transparent,Qt::transparent,Qt::transparent); };
    auto paintNewFileFunc           = [=](QPainter* painter, QRectF rct){
        QString cursor_pixmap_path = QString("%1%2%3").arg("pics").arg(QDir::separator()).arg("empty_file_icon.png");
        StaticFunctions::paintPixmapRect(painter, QPixmap(cursor_pixmap_path), rct ,Qt::transparent,Qt::transparent,Qt::transparent); };
    auto paintTerminalFunc           = [=](QPainter* painter, QRectF rct){
        QString cursor_pixmap_path = QString("%1%2%3").arg("pics").arg(QDir::separator()).arg("terminal.png");
        StaticFunctions::paintPixmapRect(painter, QPixmap(cursor_pixmap_path), rct ,Qt::transparent,Qt::transparent,Qt::transparent); };
    auto paintIncognitoFunc           = [=](QPainter* painter, QRectF rct){
        QString cursor_pixmap_path = QString("%1%2%3").arg("pics").arg(QDir::separator()).arg(incognitoImageFileName);
        StaticFunctions::paintPixmapRect(painter, QPixmap(cursor_pixmap_path), rct ,Qt::transparent,Qt::transparent,Qt::transparent); };
    auto paintCloseMenuBarFunc   = [=](QPainter* painter, QRectF rct){ StaticFunctions::paintCrossRect(painter, rct); };

    painting_caller->setFunction(QString("paintingFunction0"), paintSearchModeFunc);
    painting_caller->setFunction(QString("paintingFunction1"), paintZoomInFunc);
    painting_caller->setFunction(QString("paintingFunction2"), paintZoomOutFunc);
    painting_caller->setFunction(QString("paintingFunction3"), paintSortFunc);
    painting_caller->setFunction(QString("paintingFunction4"), paintNewFoldFunc);
    painting_caller->setFunction(QString("paintingFunction5"), paintNewFileFunc);
    painting_caller->setFunction(QString("paintingFunction6"), paintTerminalFunc);
    painting_caller->setFunction(QString("paintingFunction7"), paintIncognitoFunc);
    painting_caller->setFunction(QString("paintingFunction8"), paintCloseMenuBarFunc);

    m_menuBar->setCaller(menu_caller, painting_caller);

    m_graphicsGroup->addToGroup(m_menuBar);
}

void GraphicsView::closeMenuBar()
{
    m_paintMenuBar = false;
    QTimer::singleShot(10,[=](){
        rePaintCanvas();
    });
}

void GraphicsView::addElapseBar()
{
    ElapseMenuBD* elapseMenu = new ElapseMenuBD(static_cast<int>(m_fileMangrInfo->maxDepth()),
                                                m_colOffs*2,
                                                QSize(this->viewport()->width(),m_elapseBarHeight),
                                                QPoint(0,0)
                                             );
    elapseMenu->setBackroundColor(m_elapseCol1, m_elapseCol2);
    elapseMenu->setPosition(QPoint(this->horizontalScrollBar()->value(), this->verticalScrollBar()->value()));

    auto button_caller = std::make_shared<DynamicFunctionCaller<QString,std::function<bool(int)>>>();
    auto buttonClickFunc = [=](int i){
        emit elapseAllFoldersOfDepthId(i);
        return false;
    };
    auto elapsedFunc = [=](int depthId){
        return m_fileMangrInfo->depthIdElapsed(depthId);
    };

    button_caller->setFunction(QString("call"), buttonClickFunc);
    button_caller->setFunction(QString("elapsed"), elapsedFunc);

    elapseMenu->setCaller(button_caller);

    m_graphicsGroup->addToGroup(elapseMenu);
}

void GraphicsView::executeFileAction(FILE_ACTION::Action action)
{
    switch(action)
    {
        case FILE_ACTION::Action::COPY:
            emit copySelectedContent();
            break;
        case FILE_ACTION::Action::CUT:
            emit cutSelectedContent();
            break;
        case FILE_ACTION::Action::PASTE:
            emit pasteFromClipboard();
            break;
        case FILE_ACTION::Action::DUPLICATE:
            emit duplicateSelectedContent();
            break;
        case FILE_ACTION::Action::OPEN:
            emit openSelectedContent();
            break;
        case FILE_ACTION::Action::OPEN_WITH:
            emit openSelectedContentWith();
            break;
        case FILE_ACTION::Action::DELETE:
            emit deleteSelectedContent();
            break;
        case FILE_ACTION::Action::ZIP:
            emit zipSelectedContent();
            break;
        case FILE_ACTION::Action::UNZIP:
            emit unzipSelectedContent();
            break;
        case FILE_ACTION::Action::RENAME:
            emit renameSelectedContent();
            break;
        case FILE_ACTION::Action::TERMINAL:
            openTerminal();
            break;
        case FILE_ACTION::Action::DETAILS:
            emit showDetailsOfSelectedContent();
            break;
        case FILE_ACTION::Action::ELAPSE_REC:
            emit elapseSelectedFoldersRecursively();
            break;
        case FILE_ACTION::Action::ELAPSE:
            emit elapseSelectedFolders();
            break;
        case FILE_ACTION::Action::LOAD:
            emit setSelectionToRoot();
            break;
        case FILE_ACTION::Action::COLLAPSE_REC:
            emit collapseSelectedFoldersRecursively();
            break;
        case FILE_ACTION::Action::COLLAPSE:
            emit collapseSelectedFolders();
            break;
        case FILE_ACTION::Action::PATH:
            emit copySelectedFilePathToClipboard();
            break;
        case FILE_ACTION::Action::CANCEL_CURRENT_ACTION:
            emit killCurrentBlockingAction();
            break;
        case FILE_ACTION::Action::REQUEST_FOCUS:
            emit requestFocusSGNL();
            break;
        case FILE_ACTION::Action::TAB:
            emit requestOpenFoldersInTab();
            break;
        case FILE_ACTION::Action::CLOSE_TAB:
            emit requestCloseCurrentTab();
            break;
    }
}
//void GraphicsView::executeFileAction(std::function<void (std::shared_ptr<FilesCoordinator>)> fctn)
//{
//    if(m_isLoading)
//        return;

//    if(auto locked = lockFilesCoordinator())
//    {
//        fctn(locked);
//    }
//}

void GraphicsView::addContentBar()
{
    qreal fctr = 0.75;
    m_contBar = new MenuBar(getDisplayableHeight()*fctr,
                            QPointF(this->viewport()->width(),
                                    m_elapseBarHeight+getDisplayableHeight()*0.5),
                            GraphicItemsBD::ORIENTATION::VERTICAL,
                            true);
    m_contBar->setPosition(QPoint(this->horizontalScrollBar()->value(), this->verticalScrollBar()->value()));

    auto menu_caller = std::make_shared<DynamicFunctionCaller<QString,std::function<void()>>>();
    auto painting_caller = std::make_shared<DynamicFunctionCaller<QString,std::function<void(QPainter*,QRectF)>>>();
    std::map<int,int> groupingMap;

    int fontWeight = QFont::Bold;
    int fontSize = 6;

    int funcId = 0;

    bool rename = false;
    bool path = false;
    bool terminal = false;
    bool openWith = false;

    if(m_fileMangrInfo->foldersSelected()){
        if(m_fileMangrInfo->singleFolderSelected())
        {
        }

        // elapse folders:

        auto setElapseFunc = [=](){
            executeFileAction(FILE_ACTION::Action::ELAPSE_REC);
        };
        menu_caller->setFunction(QString("buttonFunction%1").arg(funcId), setElapseFunc);
        auto paintElapseFunc = [=](QPainter* painter, QRectF rct){
            StaticFunctions::paintTextRect(painter, QString("elapse"), rct, Qt::transparent,Qt::transparent,
                                           QColor(0,0,0),StaticFunctions::getGoshFont(fontSize, fontWeight));
        };
        painting_caller->setFunction(QString("paintingFunction%1").arg(funcId), paintElapseFunc);
        groupingMap[funcId] = 0;
        ++funcId;

        // collapse folder - hier wird reursiv collapsed
        // -> d.h. der ausgewaehlte ordner + alle darin eventuell bereits "elapseten" sub-folders:

        auto setCollapseFunc = [=](){
            executeFileAction(FILE_ACTION::Action::COLLAPSE_REC);
        };
        menu_caller->setFunction(QString("buttonFunction%1").arg(funcId), setCollapseFunc);
        auto paintCollapseFunc = [=](QPainter* painter, QRectF rct){
            StaticFunctions::paintTextRect(painter, QString("collapse"), rct, Qt::transparent,Qt::transparent,
                                           QColor(0,0,0),StaticFunctions::getGoshFont(fontSize, fontWeight));
        };
        painting_caller->setFunction(QString("paintingFunction%1").arg(funcId), paintCollapseFunc);
        groupingMap[funcId] = 0;
        ++funcId;

        // tab: open selected folders in new tab:

        auto setTabFunc = [=](){
            executeFileAction(FILE_ACTION::Action::TAB);
        };
        menu_caller->setFunction(QString("buttonFunction%1").arg(funcId), setTabFunc);
        auto paintTabFunc = [=](QPainter* painter, QRectF rct){
            StaticFunctions::paintTextRect(painter, QString("tab"), rct, Qt::transparent,Qt::transparent,
                                           QColor(0,0,0),StaticFunctions::getGoshFont(fontSize, fontWeight));
        };
        painting_caller->setFunction(QString("paintingFunction%1").arg(funcId), paintTabFunc);
        groupingMap[funcId] = 0;
        ++funcId;
    }
    if(m_fileMangrInfo->singleFolderSelected())
    {

        rename = true;

        terminal = true;

        path = true;

        if(m_fileMangrInfo->filesSelected())
            openWith = true;
    }
    if(m_fileMangrInfo->selectionCount() > 0)
    {
        // open files/folders:

        auto openFunc = [=](){
            executeFileAction(FILE_ACTION::Action::OPEN);
        };
        menu_caller->setFunction(QString("buttonFunction%1").arg(funcId), openFunc);
        std::function<void(QPainter*,QRectF)> paintElapseFunc = [=](QPainter* painter, QRectF rct){
            StaticFunctions::paintTextRect(painter, QString("open"), rct, Qt::transparent,Qt::transparent,
                                           QColor(0,0,0),StaticFunctions::getGoshFont(fontSize, fontWeight));
        };
        painting_caller->setFunction(QString("paintingFunction%1").arg(funcId), paintElapseFunc);
        groupingMap[funcId] = 1;
        ++funcId;

        if(openWith)
        {
            auto openWithFunc = [=](){
                executeFileAction(FILE_ACTION::Action::OPEN_WITH);
            };
            menu_caller->setFunction(QString("buttonFunction%1").arg(funcId), openWithFunc);
            paintElapseFunc = [=](QPainter* painter, QRectF rct){
                StaticFunctions::paintTextRect(painter, QString("open with"), rct, Qt::transparent,Qt::transparent,
                                               QColor(0,0,0),StaticFunctions::getGoshFont(fontSize, fontWeight));
            };
            painting_caller->setFunction(QString("paintingFunction%1").arg(funcId), paintElapseFunc);
            groupingMap[funcId] = 1;
            ++funcId;
        }


        if(rename)
        {
            // rename file/folder:

            auto renameFunc = [=](){
                executeFileAction(FILE_ACTION::Action::RENAME);
            };
            menu_caller->setFunction(QString("buttonFunction%1").arg(funcId), renameFunc);
            auto paintRenameFunc = [=](QPainter* painter, QRectF rct){
                StaticFunctions::paintTextRect(painter, QString("rename"), rct, Qt::transparent,Qt::transparent,
                                               QColor(0,0,0),StaticFunctions::getGoshFont(fontSize, fontWeight));
            };
            painting_caller->setFunction(QString("paintingFunction%1").arg(funcId), paintRenameFunc);
            groupingMap[funcId] = 2;
            ++funcId;
        }

        // copy files/folders:

        std::function<void()> setFunc = [=](){
            executeFileAction(FILE_ACTION::Action::COPY);
        };
        menu_caller->setFunction(QString("buttonFunction%1").arg(funcId), setFunc);
        paintElapseFunc = [=](QPainter* painter, QRectF rct){
            StaticFunctions::paintTextRect(painter, QString("copy"), rct, Qt::transparent,Qt::transparent,
                                           QColor(0,0,0),StaticFunctions::getGoshFont(fontSize, fontWeight));
        };
        painting_caller->setFunction(QString("paintingFunction%1").arg(funcId), paintElapseFunc);
        groupingMap[funcId] = 2;
        ++funcId;

        // duplicate files/folders:

        setFunc = [=](){
            executeFileAction(FILE_ACTION::Action::DUPLICATE);
        };
        menu_caller->setFunction(QString("buttonFunction%1").arg(funcId), setFunc);
        paintElapseFunc = [=](QPainter* painter, QRectF rct){
            StaticFunctions::paintTextRect(painter, QString("duplicate"), rct, Qt::transparent,Qt::transparent,
                                           QColor(0,0,0),StaticFunctions::getGoshFont(fontSize, fontWeight));
        };
        painting_caller->setFunction(QString("paintingFunction%1").arg(funcId), paintElapseFunc);
        groupingMap[funcId] = 2;
        ++funcId;

        // cut files/folders:
        setFunc = [=](){
            executeFileAction(FILE_ACTION::Action::CUT);
        };
        menu_caller->setFunction(QString("buttonFunction%1").arg(funcId), setFunc);
        paintElapseFunc = [=](QPainter* painter, QRectF rct){
            StaticFunctions::paintTextRect(painter, QString("cut"), rct, Qt::transparent,Qt::transparent,
                                           QColor(0,0,0),StaticFunctions::getGoshFont(fontSize, fontWeight));
        };
        painting_caller->setFunction(QString("paintingFunction%1").arg(funcId), paintElapseFunc);
        groupingMap[funcId] = 2;
        ++funcId;

        // paste files/folders:

        setFunc = [=](){
            executeFileAction(FILE_ACTION::Action::PASTE);
        };
        menu_caller->setFunction(QString("buttonFunction%1").arg(funcId), setFunc);
        paintElapseFunc = [=](QPainter* painter, QRectF rct){
            StaticFunctions::paintTextRect(painter, QString("paste"), rct, Qt::transparent,Qt::transparent,
                                           QColor(0,0,0),StaticFunctions::getGoshFont(fontSize, fontWeight));
        };
        painting_caller->setFunction(QString("paintingFunction%1").arg(funcId), paintElapseFunc);
        groupingMap[funcId] = 2;
        ++funcId;

        // zip files:

        auto zipFunc = [=](){
            executeFileAction(FILE_ACTION::Action::ZIP);
        };
        menu_caller->setFunction(QString("buttonFunction%1").arg(funcId), zipFunc);
        auto paintZipFunc = [=](QPainter* painter, QRectF rct){
            StaticFunctions::paintTextRect(painter, QString("zip"), rct, Qt::transparent,Qt::transparent,
                                           QColor(0,0,0),StaticFunctions::getGoshFont(fontSize, fontWeight));
        };
        painting_caller->setFunction(QString("paintingFunction%1").arg(funcId), paintZipFunc);
        groupingMap[funcId] = 4;
        ++funcId;


        // unzip files:

        if(m_fileMangrInfo->selectionContainsZippedFile())
        {
            auto unzipFunc = [=](){
                executeFileAction(FILE_ACTION::Action::UNZIP);
            };
            menu_caller->setFunction(QString("buttonFunction%1").arg(funcId), unzipFunc);
            auto paintUnZipFunc = [=](QPainter* painter, QRectF rct){
                StaticFunctions::paintTextRect(painter, QString("unzip"), rct, Qt::transparent,Qt::transparent,
                                               QColor(0,0,0),StaticFunctions::getGoshFont(fontSize, fontWeight));
            };
            painting_caller->setFunction(QString("paintingFunction%1").arg(funcId), paintUnZipFunc);
            groupingMap[funcId] = 4;
            ++funcId;
        }

        if(path)
        {
            // path:

            auto copyPathToClipboardFunc = [=](){
                executeFileAction(FILE_ACTION::Action::PATH);
            };
            menu_caller->setFunction(QString("buttonFunction%1").arg(funcId), copyPathToClipboardFunc);
            auto paintPathToClipboardFunc = [=](QPainter* painter, QRectF rct){
                StaticFunctions::paintTextRect(painter, QString("path"), rct, Qt::transparent,Qt::transparent,
                                               QColor(0,0,0),StaticFunctions::getGoshFont(fontSize, fontWeight));
            };
            painting_caller->setFunction(QString("paintingFunction%1").arg(funcId), paintPathToClipboardFunc);
            groupingMap[funcId] = 3;
            ++funcId;
        }

        if(terminal)
        {
            // terminal:

            auto terminalFunc = [=](){
                executeFileAction(FILE_ACTION::Action::TERMINAL);
            };
            menu_caller->setFunction(QString("buttonFunction%1").arg(funcId), terminalFunc);
            auto paintTerminalFunc = [=](QPainter* painter, QRectF rct){
                StaticFunctions::paintTextRect(painter, QString("terminal"), rct, Qt::transparent,Qt::transparent,
                                               QColor(0,0,0),StaticFunctions::getGoshFont(fontSize, fontWeight));
            };
            painting_caller->setFunction(QString("paintingFunction%1").arg(funcId), paintTerminalFunc);
            groupingMap[funcId] = 3;
            ++funcId;
        }

        // delete files/folders:

        setFunc = [=](){
            executeFileAction(FILE_ACTION::Action::DELETE);
        };
        menu_caller->setFunction(QString("buttonFunction%1").arg(funcId), setFunc);
        paintElapseFunc = [=](QPainter* painter, QRectF rct){
            StaticFunctions::paintTextRect(painter, QString("delete"), rct, Qt::transparent,Qt::transparent,
                                           QColor(0,0,0),StaticFunctions::getGoshFont(fontSize, fontWeight));
        };
        painting_caller->setFunction(QString("paintingFunction%1").arg(funcId), paintElapseFunc);
        groupingMap[funcId] = 5;
        ++funcId;
    }

//    std::function<int(int)> groupingFunc = [&](int id){return groupingMap[id];};
//    m_contBar->setCaller(menu_caller, painting_caller, groupingFunc);
    m_contBar->setCaller(menu_caller, painting_caller);

    m_graphicsGroup->addToGroup(m_contBar);
}

void GraphicsView::closeContentBar()
{
    m_paintContBar = false;
    QTimer::singleShot(10,[=](){
        rePaintCanvas();
    });
}
void GraphicsView::addSearchMenu(){
    QSize menuSize(this->viewport()->width(), m_searchMenuHeight);

//    if(m_searchMenu == nullptr)
//        m_searchMenu = new SearchMenuBD(menuSize);
//    else
//        m_searchMenu->setSize(menuSize);

    m_searchMenu = new SearchMenuBD(menuSize);

    m_searchMenu->setPosition(QPoint(this->horizontalScrollBar()->value(),
                                     this->verticalScrollBar()->value() + m_elapseBarHeight));


    auto menu_caller = std::make_shared<DynamicFunctionCaller<QString,std::function<QString()>>>();
    auto nextPrevCaller = [=](){nextSearchResult();return QString("");};
    auto prevPrevCaller = [=](){prevSearchResult();return QString("");};
    auto closePrevCaller = [=](){closeSearchMenu();return QString("");};
    auto getNextSearchText = [=](){ return QFileInfo(getFocusedSearchPath()).fileName(); };
    auto getCurrentSearchIndex = [=](){ return QString("%1").arg( getSearchIndex()+1 ); };
    auto getSearRsltsCount = [=](){ return QString("%1").arg( getSearchResultsCount() ); };
    menu_caller->setFunction(QString("next"), nextPrevCaller);
    menu_caller->setFunction(QString("previous"), prevPrevCaller);
    menu_caller->setFunction(QString("close"), closePrevCaller);
    menu_caller->setFunction(QString("getSearchResult"), getNextSearchText);
    menu_caller->setFunction(QString("getCurrentSearchIndex"), getCurrentSearchIndex);
     menu_caller->setFunction(QString("getSearchResultsCount"), getSearRsltsCount);
    m_searchMenu->setCaller(menu_caller);

    m_graphicsGroup->addToGroup(m_searchMenu);
}


void GraphicsView::rePaintCanvas()
{
//    qDebug() << "repainting canvas";

    // darauf achten, dass, wenn inSearchMode()==true,
    // auch nach dem searchMode m_paintUpperRect und m_paintMenuBar auf false stehen
    // denke es ist am besten das hier in rePaintCanvas zu platzieren, da der filescoordinaotr nicht
    // mehr ein signal gibt, umd den searchMode zu starten:
    if(inSearchMode())
    {
        m_paintUpperRect = false;
        m_paintMenuBar = false;
    }

    if(m_isLoading)
    {
        if( !m_waitingBarIsAddedToScene )
        {
//            qDebug() << "   -> rePaintCanvas:isLoading";
            QGraphicsRectItem* blockingRect = new QGraphicsRectItem(QRectF(0,0, 1000000,1000000));
            blockingRect->setBrush(QBrush(QColor(0,255,0, 50)));
            m_graphicsGroup->addToGroup(blockingRect);

            m_waitingBar = new WaitingBarGraphicsItem(QSize(0,0), QPoint(0,0));
            m_waitingBar->startAnimation();

            m_graphicsGroup->addToGroup( m_waitingBar );
            m_waitingBarIsAddedToScene = true;

            QString cancelPixmapPath = StaticFunctions::getPictureResourceFromFileName("cancel.png");
            QPixmap cancelPixmap(cancelPixmapPath);
            cancelPixmap = cancelPixmap.scaled(QSize(300,300));

            auto view_pix_width = static_cast<double>(this->viewport()->width()-cancelPixmap.width());
            auto x_pos = view_pix_width * 0.5;

            m_cancelBtnSize = QSize(cancelPixmap.width(), cancelPixmap.height());
            m_cancelBtn = new GraphicItemsBD::PixmapRect(cancelPixmap,
                                                         m_cancelBtnSize,
                                                         QPoint(static_cast<int>(x_pos), 50),
                                                         QColor(255,255,255, 255), QColor(240,240,255, 255),
                                                         QColor(255,150,150, 255), QColor(255,0,0, 255));
            m_cancelBtn->setCallFunction([=](){
                executeFileAction(FILE_ACTION::Action::CANCEL_CURRENT_ACTION);
            });

            m_graphicsGroup->addToGroup(m_cancelBtn);

//            setWaitingBarSizeAndPos();
        }/*else{
            setWaitingBarSizeAndPos();
        }*/
    }else
    {
        if(m_waitingBarIsAddedToScene)
        {
            m_waitingBarIsAddedToScene = false;
            m_waitingBar->stopAnimation();
        }

        m_scene->clear();

        m_graphicsGroup = new QGraphicsItemGroup();
        m_graphicsGroup->setX(getAbsoluteHorBarValue());
        m_graphicsGroup->setY(getAbsoluteVerticalBarValue());
        m_graphicsGroup->setHandlesChildEvents(false);

        auto height = m_rowHeight*m_fileCount + m_elapseBarHeight;
        if(height < this->viewport()->height()){
            height = this->viewport()->height();
        }
        m_scene->setSceneRect(QRect(0,0,this->viewport()->width(), static_cast<int>(height)));

//        auto lckdFiCoord = lockFilesCoordinator();

//        if(m_fileCount < m_fileMaxCount){
//            m_firstDispFI = 0;
//            m_lastDispFI = m_fileCount-1;
//            m_curDispFI = m_fileCount-1;
//        }else{
//            m_firstDispFI = getFirstToDispFi();
//            m_lastDispFI = getLastToDispFi();
//        }

        std::function<void(const FiBDViewer&,int_bd,int_bd)> dirFunc = [=](const FiBDViewer& fiv, int_bd row, int_bd col)
        {
            auto elapseFunc = [=](){
                emit elapseOrCollapseFolderDependingOnCurrentState(fiv.q_path());
                return false;
            };
            auto isElapsedFunc = [=](){return fiv.isElapsed();};
            auto isLoadedFunc = [=](){return fiv.isLoaded();};
            auto isDirFunc = [=](){ return true;};
            auto isEmptyFunc = [=](){return fiv.isEmpty();};
            auto containsFilesFunc = [=](){return fiv.containsFiles();};
            auto selectFunc = [=](){
                m_paintContBar = true;
                emit selectContent(fiv.q_path(), StaticFunctions::controlPressed(), StaticFunctions::shiftPressed());
                return false;
            };
            auto isSelectedFunc = [=](){
                return fiv.selected();
            };
            auto setSelectedFoldToRootFunc = [=](){
                emit setSelectionToRoot();
                return false;
            };
            auto isSearchedFunc = [=](){
                return fiv.searched();
            };

            auto caller = std::make_shared<DynamicFunctionCaller<QString, std::function<bool()>>>();
            caller->setFunction(QString("elapse"), elapseFunc);
            caller->setFunction(QString("isElapsed"), isElapsedFunc);
            caller->setFunction(QString("isLoaded"), isLoadedFunc);
            caller->setFunction(QString("isDir"), isDirFunc);
            caller->setFunction(QString("isEmpty"), isEmptyFunc);
            caller->setFunction(QString("select"), selectFunc);
            caller->setFunction(QString("isSelected"), isSelectedFunc);
            caller->setFunction(QString("setAsRoot"), setSelectedFoldToRootFunc);
            caller->setFunction(QString("isSearched"), isSearchedFunc);
            caller->setFunction(QString("containsFiles"), containsFilesFunc);
//            caller->setFunction(QString("executeAction"), [=](FILE_ACTION action){this->executeFileAction(action);});

            auto sortFunc = [=](Order order){
                emit sortDir(fiv.q_path(), order);
                return false;
            };
            auto isRevSortFunc = [=](Order order){
                return fiv.isReversedSortedBy(order);
            };
            auto isSortedByFunc = [=](Order order){
                return fiv.isSortedBy(order.ordered_by);
            };

            auto sortCaller = std::make_shared<DynamicFunctionCaller<QString, std::function<bool(Order)>>>();
            sortCaller->setFunction(QString("sortBy"), sortFunc);
            sortCaller->setFunction(QString("isReversedSortedBy"), isRevSortFunc);
            sortCaller->setFunction(QString("isSortedBy"), isSortedByFunc);


            paintFileInfo(fiv, row, col, caller, sortCaller);
        };

        std::function<void(const FiBDViewer&,int_bd,int_bd)> fileFunc = [=](const FiBDViewer& fiv, int_bd row, int_bd col)
        {
            auto isDirFunc = [=](){ return false; };
            auto selectFunc = [=](){
                m_paintContBar = true;
                emit selectContent(fiv.q_path(), StaticFunctions::controlPressed(), StaticFunctions::shiftPressed());
                return false;
            };
            auto isSelectedFunc = [=](){
                return fiv.selected();
            };
            auto isSearchedFunc = [=](){
                return fiv.searched();
            };

            auto caller = std::make_shared<DynamicFunctionCaller<QString, std::function<bool()>>>();
            caller->setFunction(QString("isDir"), isDirFunc);
            caller->setFunction(QString("select"), selectFunc);
            caller->setFunction(QString("isSelected"), isSelectedFunc);
            caller->setFunction(QString("isSearched"), isSearchedFunc);

            paintFileInfo(fiv, row, col, caller);
        };

        for(auto i=m_firstDispFI; (i <= m_lastDispFI) && (m_entriesToRender.find(i) != m_entriesToRender.end()); ++i){

            const FiBDViewer& fiv = m_entriesToRender[i];

            if(fiv.isFolder())
            {
                dirFunc(fiv, i, fiv.depthId());
            }else{
                fileFunc(fiv, i, fiv.depthId());
            }
        }

        m_scene->addItem(m_graphicsGroup);
        m_graphicsGroup->setY(this->verticalScrollBar()->value());

        if(!inSearchMode() && m_paintUpperRect){
            double trnglWidth = m_upperRectWidth;//qMin(trnglWidth, (qreal)20.);
            double trnglHeight = trnglWidth*0.5;
            auto viewport_width = static_cast<double>(this->viewport()->width());
            QPointF center( viewport_width*0.5, trnglHeight*0.5+5. + getViewportYOffset()+m_elapseBarHeight);
            paintTopRectangle(center, QSize(static_cast<int>(trnglWidth),
                                            static_cast<int>(trnglHeight)));
        }
        if(inSearchMode()){
            addSearchMenu();
        }else
        {
            m_searchMenu = nullptr;
        }

        if(m_paintMenuBar){
            addMenuBar();
        }
        if(m_paintContBar){
            addContentBar();
        }
        addElapseBar();

        if(m_shwRtSel){
            QString directory_pixmap_path = QString("%1%2%3")
                    .arg("pics")
                    .arg(QDir::separator())
                    .arg("root_scld.png");
            QPixmap directory_pixmap = QPixmap(directory_pixmap_path);
            GraphicItemsBD::PixmapRect* rotSlctr = new GraphicItemsBD::PixmapRect(directory_pixmap, QSize(50, 50),
                       QPoint(0,0),
                       QColor(255,255,255, 0),QColor(255,255,255, 0),
                       QColor(255,0,200, 255), QColor(255,200,200, 255));
            rotSlctr->setCallFunction([=](){showRootSelector();});

            auto p_x = static_cast<int>(this->viewport()->width() - (rotSlctr->boundingRect().width() + 4));
            rotSlctr->setPosition(QPoint(p_x, getViewportYOffset()));
            m_graphicsGroup->addToGroup(rotSlctr);
        }
    }
}

void GraphicsView::setWaitingBarSizeAndPos()
{
    if( !m_isLoading )
        return;

    m_waitingBar->setSize( QSize(this->viewport()->width(), m_waitingBarHeight) );
    m_waitingBar->setPosition( QPoint(0, 5) );//(this->viewport()->height()-m_waitingBarHeight) * 0.5f) );

//    m_cancelBtn->setSize(m_cancelBtnSize);
    auto view_cncl_width = static_cast<double>(this->viewport()->width() - m_cancelBtnSize.width());
    auto view_cncl_width_half = view_cncl_width * 0.5;
    m_cancelBtn->setPosition(QPoint( static_cast<int>(view_cncl_width_half), 50));
}

void GraphicsView::paintFileInfo(const FiBDViewer& fiv, int_bd rowId, int_bd colId,
                                 std::shared_ptr<DynamicFunctionCaller<QString, std::function<bool()>>> caller,
                                 std::shared_ptr<DynamicFunctionCaller<QString, std::function<bool(Order)>>> sortCaller){
    int colOffs = 30;
    GraphicsFile* gf = new GraphicsFile(fiv, QSize(this->viewport()->width(),m_rowHeight),
                                        rowId, m_rowHeight,
                                        colId, colOffs,
                                        caller,
                                        sortCaller,
                                        m_fontSize);
    gf->setDetailsTextColor(QColor(0,0,0, 150));
    gf->setDropFunction([=](QString dropStr){
        emit paste(dropStr, fiv.q_path());
    });
    gf->setInitDraggingFunction([=](QString draggingSource){
        emit initDragging(draggingSource);
    });
    int pos_y = static_cast<int>(rowId*m_rowHeight + m_elapseBarHeight);
    gf->setPosition(QPoint(0, pos_y));
    m_scene->addItem(gf);
}

qreal GraphicsView::getRelativeHorBarValue()
{
    qreal val = this->horizontalScrollBar()->value();
    qreal max = this->horizontalScrollBar()->maximum();
    qreal min = this->horizontalScrollBar()->minimum();
    qreal retVal = val / (max-min);
    return retVal;
}
qreal GraphicsView::getRelativeVerBarValue()
{

    qreal val = this->verticalScrollBar()->value();
    qreal max = this->verticalScrollBar()->maximum();
    qreal min = this->verticalScrollBar()->minimum();
    qreal retVal = val / (max-min);
    return retVal;
}

int GraphicsView::getAbsoluteHorBarValue()
{
    return this->horizontalScrollBar()->value();
}

int GraphicsView::getAbsoluteVerticalBarValue()
{
    return this->verticalScrollBar()->value();
}

int GraphicsView::getViewportYOffset()
{
    return getAbsoluteVerticalBarValue();
}

void GraphicsView::handleSearchKeyword(QString keyword, bool deep_search)
{
    if(!keyword.isEmpty()){
        if(deep_search)
            emit deepSearch(keyword);
        else
            emit searchForKeyWord(keyword);
    }
}
void GraphicsView::launchSearchMode()
{
    SearchFileDialog* sfd = new SearchFileDialog();
    sfd->setFixedWidth(350);
    sfd->show();

    connect( sfd, &SearchFileDialog::okClicked, this, [=](QString keyword, bool deepSearch){
            handleSearchKeyword(keyword, deepSearch);
        }, Qt::QueuedConnection);
}

void GraphicsView::nextSearchResult()
{
    emit nextSearchResultSGNL();
}

void GraphicsView::prevSearchResult()
{
    emit prevSearchResultSGNL();
}

void GraphicsView::closeSearchMenu()
{
    emit closeSearchMenuSGNL();
}

QString GraphicsView::getFocusedSearchPath()
{
    return m_fileMangrInfo->curFocusedSearchPath();
}
long GraphicsView::getSearchIndex()
{
    return static_cast<long>(m_fileMangrInfo->searchIndex());
}

long GraphicsView::getSearchResultsCount()
{
    return static_cast<long>(m_fileMangrInfo->searchResultsCount());
}

void GraphicsView::zoomOut()
{
    if(m_fontSize > 5){
        --m_fontSize;
        revalidateRowHeight();
        QTimer::singleShot(0, [=](){rePaintCanvas();});
        emit zoomFactorChanged(m_fontSize);
    }
}

void GraphicsView::zoomIn()
{
    if(m_fontSize < 20){
        ++m_fontSize;
        revalidateRowHeight();
        QTimer::singleShot(0, [=](){rePaintCanvas();});
        emit zoomFactorChanged(m_fontSize);
    }
}

void GraphicsView::sortAllFoldersDialog()
{
//    if(auto locked = lockFilesCoordinator()){
//        clearControlButtons();

        QComboBox* comboBox = new QComboBox();
        comboBox->addItem(QString("Name"));
        comboBox->addItem(QString("Type"));
        comboBox->addItem(QString("Modification Date"));
        comboBox->addItem(QString("Size"));

        QCheckBox* revChckBx = new QCheckBox(QString("Reverse ordering"));
        QPushButton* okBtn = new QPushButton(QString("ok"));
        QPushButton* cancelBtn = new QPushButton(QString("cancel"));

        QHBoxLayout* hBox = new QHBoxLayout();
        hBox->addWidget(okBtn);
        hBox->addWidget(cancelBtn);

        QVBoxLayout* lay = new QVBoxLayout();
        lay->addWidget(new QLabel(QString("Sort by:")));
        lay->addWidget(comboBox);
        lay->addWidget(revChckBx);
        lay->addLayout(hBox);

        QDialog* dialog = new QDialog();
        dialog->setModal(true);
        dialog->setLayout(lay);
        dialog->setFixedSize(QSize(200,110));

        connect(okBtn, &QPushButton::clicked,
                [=](){
            Order ord;
            ord.reversedOrdered = revChckBx->isChecked();
            if(comboBox->currentText() == QString("Name")){
                ord.ordered_by = ORDERED_BY::NAME;
            }else if(comboBox->currentText() == QString("Type")){
                ord.ordered_by = ORDERED_BY::TYPE;
            }else if(comboBox->currentText() == QString("Size")){
                ord.ordered_by = ORDERED_BY::SIZE;
            }else{// if(comboBox->currentText() == QString("Modification Date")){
                ord.ordered_by = ORDERED_BY::MOD_DATE;
            }
            dialog->close();

            emit sortAllFolders(ord);
        });
        connect(cancelBtn, &QPushButton::clicked, dialog, &QDialog::close);

        dialog->exec();
}

void GraphicsView::createNewFolder()
{
    emit createNewFolderSGNL();
}

void GraphicsView::createNewFile()
{
    emit createNewFileSGNL();
}

void GraphicsView::openTerminal()
{
    emit openTerminalSGNL();
}

void GraphicsView::showHiddenFiles()
{
    bool inclHdnFlse = m_fileMangrInfo->includeHiddenFiles();
    emit showHiddenFilesSGNL( !inclHdnFlse );
}

void GraphicsView::showRootSelector()
{
    QString curRootDir = m_fileMangrInfo->curRootPath();
    if( !curRootDir.isEmpty() ){
        DirectorySelectorDialog* dialog = new DirectorySelectorDialog(curRootDir);
        connect(dialog, &DirectorySelectorDialog::directorySelected, this, &GraphicsView::setRootFolder);
        dialog->show();
    }
}

void GraphicsView::revalidateRowHeight()
{
    QFont _font(StaticFunctions::getGoshFont(m_fontSize));
    QFontMetrics fm(_font);
    m_rowHeight = fm.height() +6;
}

bool GraphicsView::inSearchMode()
{
    return m_fileMangrInfo->inSearchMode();
}

void GraphicsView::revalFirstAndLastDisplayedFI(bool revalIfStillInBounds)
{
    auto old_firstDispFI = m_firstDispFI;
    auto old_lastDispFI = m_lastDispFI;

    if(m_fileCount < m_minDisplayedEntries){
        m_firstDispFI = 0;
        m_lastDispFI = m_fileCount-1;
        m_curDispFI = m_fileCount-1;
    }else{
        m_firstDispFI = getFirstToDispFi();
        m_lastDispFI = getLastToDispFi();
    }

    if( (old_firstDispFI != m_firstDispFI) ||
        (old_lastDispFI  != m_lastDispFI) )
    {
        emit requestFileViewerRevalidation(m_firstDispFI, m_lastDispFI);
    }else if(revalIfStillInBounds)
    {
        revalidate();
    }
}
