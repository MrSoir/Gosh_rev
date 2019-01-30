#include "graphicsfile.h"

GraphicsFile::GraphicsFile(const FiBDViewer& fiv,
                           const QSize& size,
                           const int rowId,
                           const int rowHeight,
                           const int colId,
                           const int colOffs,
                           std::shared_ptr<DynamicFunctionCaller<QString, std::function<bool()>>> caller,
                           std::shared_ptr<DynamicFunctionCaller<QString, std::function<bool(Order)>>>  sortCaller,
                           const int fontSize,
                           QGraphicsItem *parent)
    : QGraphicsItem(parent),
      m_fiv(fiv),
      m_size(size),
      m_pos(QPoint(0,0)),
      m_hover(false),
      m_rowId(rowId),
      m_rowHeight(rowHeight),
      m_colId(colId),
      m_colOffs(colOffs),

      m_elapseRect(nullptr),
      m_sortRects(QVector<QRectF>()),
      m_sortRectHoverId(-1),

      m_textColor(QColor(0,0,0)),
      m_detailsTextColor(m_textColor),
      m_drawAbsoluteFilePath(false),
      m_openOnDoubleClick(true),
      m_rightClickEnabled(false),
      m_backgroundColor(QColor(255,255,255)),

      m_fontSize(fontSize),

      m_dragEntered(false),

      m_caller(caller),
      m_sortCaller(sortCaller),
      isPressed(false),
      m_initMousePrsdPos(QPointF()),
      lastTmePrsd(Q_INT64_C(0)),

      m_isCurrentlyRepainting(std::atomic<int>())
{
    m_isCurrentlyRepainting.store(0);

    setAcceptDrops(true);
    setAcceptHoverEvents(true);
}

GraphicsFile::~GraphicsFile(){
//    qDebug() << "in GraphicsFile.DEstructor";
    if(m_elapseRect)
        delete m_elapseRect;
    m_elapseRect = nullptr;

    if(m_caller)
        m_caller.reset();
    if(m_sortCaller)
        m_sortCaller.reset();

    m_initDraggingFunc = nullptr;
    m_dropFunc = nullptr;
}

void GraphicsFile::setPosition(const QPoint& position){
    m_pos = QPoint(position);
//    prepareGeometryChange();
}
QRectF GraphicsFile::boundingRect() const {
    return QRectF(m_pos, m_size);
}

QString convertBytesToFormattedString(unsigned long long int bytes){
    qreal dispNumber;
    QString typeStr;
    if(bytes < 1000){
        typeStr = QString("bytes");
        dispNumber = bytes;
    }else if(bytes >= 1000 &&
             bytes <  1000000){
        typeStr = QString("kB");
        dispNumber = bytes / 1000.;
    }else if(bytes >= 1000000 &&
             bytes <  1000000000){
        typeStr = QString("MB");
        dispNumber = bytes / 1000000.;
    }else if(bytes >= 1000000000 &&
             bytes <  1000000000000){
        typeStr = QString("GB");
        dispNumber = bytes / 1000000000.;
    }else{ // terrabytes
        typeStr = QString("TB");
        dispNumber = bytes / 1000000000000.;
    }

    QString numbString = bytes < 1000 ?
                             QString::number( dispNumber, 'f',  0) :
                             QString::number( dispNumber, 'f',  1);

    return QString("%1 %2").arg(numbString).arg(typeStr);
}
void GraphicsFile::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(widget)
    Q_UNUSED(option)

    if(m_isCurrentlyRepainting.fetch_add(0) > 0)
    {
//        qDebug() << "GraphicsFile already painting";
        return;
    }

    painter->save();

    // gaaanz wichtig: erstmal painter an boundingRect clippen!!!:
    painter->setClipRect(boundingRect());

    painter->setPen(QPen(QColor(0,0,0),2, Qt::SolidLine));

//    if (option->state & QStyle::State_Selected){
//        painter->fillRect(boundingRect(), Qt::green);
//    }else
    QRectF br = boundingRect();

    painter->save();
    painter->setPen(QPen(QColor(0,0,0, 50), 0.5, Qt::SolidLine));
    bool m_selected = (m_caller && m_caller->containsFunction(QString("isSelected")))
                        ? m_caller->getFunction("isSelected")()
                        : false;
    bool isSearched = m_caller && m_caller->containsFunction(QString("isSearched")) &&
                      m_caller->getFunction("isSearched")();

    QColor backgroundColor = m_backgroundColor;//(255,255,255);

    if(m_selected && m_hover || m_dragEntered){
        backgroundColor = QColor(150,255,255, 255);
        painter->setBrush(backgroundColor);
        painter->drawRect(br);
    }else if (isSearched){
        backgroundColor = QColor(255,180,180,255);
        painter->setBrush(backgroundColor);
        painter->drawRect(br);
    }else if (m_selected){
        backgroundColor = QColor(0,255,180,255);
        painter->setBrush(backgroundColor);
        painter->drawRect(br);
    }else if (m_hover){
        backgroundColor = StaticFunctions::getGoshBlueColor();
        painter->setBrush(backgroundColor);
        painter->drawRect(br);
    }else if(m_rowId % 2){
        qreal fctr = 0.98;
        backgroundColor = QColor(backgroundColor.red()*fctr,
                                 backgroundColor.green()*fctr,
                                 backgroundColor.blue()*fctr,
                                 backgroundColor.alpha());
        painter->setPen(Qt::transparent);
        painter->setBrush(QBrush(backgroundColor));
        painter->drawRect(br);
    }
    painter->restore();

    int icon_offs = 3;
    int xOffs = m_pos.x() + m_colId * m_colOffs +icon_offs;

    bool isDir = m_caller && m_caller->containsFunction(QString("isDir")) &&
                 m_caller->getFunction(QString("isDir"))();
    bool isElapsed = m_caller && m_caller->containsFunction(QString("isElapsed")) &&
                 m_caller->getFunction(QString("isElapsed"))();
    bool containsFiles = m_caller && m_caller->containsFunction(QString("containsFiles")) &&
            m_caller->getFunction(QString("containsFiles"))();

    if(isDir){
        float ellHeight = (float)m_rowHeight*0.5;
        float yStart = ((float)m_rowHeight-ellHeight)*0.5;
        if(!m_elapseRect){
            m_elapseRect = new QRectF(xOffs, m_pos.y()+yStart, ellHeight,ellHeight);
        }
        painter->save();

        QColor dirRctCol;
        if(m_caller && m_caller->containsFunction(QString("isElapsed"))
                    && m_caller->containsFunction(QString("isLoaded"))
                    && m_caller->containsFunction(QString("isEmpty"))){
            auto isElapsed = m_caller->getFunction(QString("isElapsed"));
            auto isLoaded = m_caller->getFunction(QString("isLoaded"));
            auto isEmpty = m_caller->getFunction(QString("isEmpty"));
            if(!isLoaded()){
                dirRctCol = QColor(255,0,0);
                painter->setBrush(QBrush(dirRctCol));
            }else if(isEmpty()){
                dirRctCol = QColor(220,220,220);
                painter->setBrush(QBrush(dirRctCol));
                painter->setPen(QColor(200,200,200));
            }else if(isElapsed()){
                dirRctCol = QColor(0,255,0);
                painter->setBrush(QBrush(dirRctCol));
            }else{
                dirRctCol = QColor(255,0,255);
                painter->setBrush(QBrush(dirRctCol));
            }
        }
        int rectArc = qMin(m_elapseRect->width(), m_elapseRect->height());
        int borderWidth = 1;//m_fontSize < 10 ? 1 : 2;
        painter->setPen(QPen(QColor(0,0,0, 150), borderWidth));
        painter->drawRoundRect(*m_elapseRect, rectArc,rectArc);
        xOffs += ellHeight + icon_offs;
        painter->restore();
    }

//    QIcon icon = StaticFunctions::getFileIcon(m_fileInfo);
    float fctr = 0.8;
    float icnRctHght = ((float)m_size.height())*fctr;
    QRect iconRct(xOffs+icon_offs, m_pos.y()+((float)m_size.height()-icnRctHght)*0.5, m_size.height()*fctr,m_size.height()*fctr);
    QPixmap iconPixmap = StaticFunctions::getFilePixmap(m_fiv.path(), iconRct.size());
    painter->drawPixmap(iconRct, iconPixmap);//icon.pixmap(iconRct.size()));

    QFont txtFont(StaticFunctions::getGoshFont(m_fontSize, QFont::Normal));
    QFontMetrics fm(txtFont);
    int txt_offs = iconRct.right() + 3;
    painter->setPen(m_textColor);
    painter->setFont(txtFont);
    QString fileNameToDraw = m_drawAbsoluteFilePath ? m_fiv.q_path() : m_fiv.fileName();
    int txtX = txt_offs;
    int txtY = br.center().y() - fm.height()*0.5;
    int txtWidth = fm.width(fileNameToDraw);
    int txtAscent = fm.ascent();
    int txtHeight = fm.height();

    int smallerFont = m_fontSize * 0.8;

    if(isDir){
        if(isElapsed && containsFiles)
        {
            txt_offs += fm.width(fileNameToDraw) + 40;

            QFont sortFont = StaticFunctions::getGoshFont(smallerFont, QFont::Normal);
            fm = QFontMetrics (sortFont);

            QString sortLabel("sort files by:");
            int curTxtWidth = fm.width(sortLabel);

            QRectF labelRect(txt_offs, br.center().y()-fm.height()*0.5,
                             curTxtWidth, fm.height());
            QColor lblCol = Qt::transparent;
    //        QColor lblTxtCol(0,0,0);

            StaticFunctions::paintTextRect(painter,
                                           sortLabel,
                                           labelRect,
                                           lblCol,lblCol,
                                           m_detailsTextColor, sortFont,
                                           Qt::transparent);

            txt_offs += curTxtWidth +10;

            int selColIntensity = 100;

            QColor plainCol1(255,255,255,255),
                   plainCol2(230,230,255,255),

//               sortedCol1(255,255,255,255),
//               sortedCol2(selColIntensity,255,selColIntensity,255),

//               reverseSortedCol1(255,255,255,255),
//               reverseSortedCol2(255,selColIntensity,selColIntensity,255),

               hoverCol1(255,255,255,255),
               hoverCol2(180,180,255,255);

//        QColor txtClr(0,0,0,255);
            for(int i=0; i < 4; i++){
                QString str;

                Order curOrder;
                if(i==0){
                    str = QString("Name:");
                    curOrder.ordered_by = ORDERED_BY::NAME;
                }else if (i==1){
                    str = QString("Size:");
                    curOrder.ordered_by = ORDERED_BY::SIZE;
                }else if (i==2){
                    str = QString("Type:");
                    curOrder.ordered_by = ORDERED_BY::TYPE;
                }else if (i==3){
                    str = QString("Date:");
                    curOrder.ordered_by = ORDERED_BY::MOD_DATE;
                }

                bool isSortedBy =  m_sortCaller && m_sortCaller->containsFunction(QString("isSortedBy")) &&
                        m_sortCaller->getFunction(QString("isSortedBy"))(curOrder);

                bool isReversedSorted = m_sortCaller && m_sortCaller->containsFunction(QString("isReversedSortedBy")) &&
                                        m_sortCaller->getFunction(QString("isReversedSortedBy"))(curOrder);

                int curTxtWidth = fm.width(str) + 4;

                QRectF sortRct(txt_offs, br.center().y()-fm.height()*0.5,
                                curTxtWidth, fm.height());

                while(m_sortRects.size() <= i){
                    m_sortRects.append(QRectF(0,0,0,0));
                }
                m_sortRects[i] = sortRct;

                QColor col1, col2;
                if(m_sortRectHoverId == i){
                    col1 = hoverCol1;
                    col2 = hoverCol2;
                }/*else if(isSortedBy){
                    col1 = sortedCol1;
                    col2 = sortedCol2;
                }else if (isReversedSorted){
                    col1 = reverseSortedCol1;
                    col2 = reverseSortedCol2;
                }*/else{
                    col1 = plainCol1;
                    col2 = plainCol2;
                }
                StaticFunctions::paintTextRect(painter,
                                               str,
                                               sortRct,
                                               col1,col2,
                                               m_detailsTextColor, sortFont,
                                               Qt::transparent);

                txt_offs += curTxtWidth;

                if(isSortedBy || isReversedSorted){
                    qreal triHght = m_sortRects[i].height() * 0.5;
                    QRectF sortRct(txt_offs +2, m_sortRects[i].center().y() -triHght*0.5,
                                   5, triHght);
                    paintRectangle(painter, sortRct, isReversedSorted);
                }
                txt_offs += 10;
            }
        }
    }else{
        txt_offs = br.right()-3;

        QFont sortFont = StaticFunctions::getGoshFont(smallerFont, QFont::Normal);
        fm = QFontMetrics (sortFont);

        QColor bckgrndCol = Qt::transparent;//(255,255,255,255);
        QColor txtClr = m_detailsTextColor;//(0,0,0,255);

        int minColWidth;

        for(int i=2; i >= 0; --i){
            QString str;
            if(i==2)
            {
                str = m_fiv.lastModified().toString(QString("dd.MM.yy | hh:mm:ss"));
                minColWidth = fm.width(str) + 10;
            }else if (i==1){
                str = StaticFunctions::getFileType(m_fiv.fileName());
                minColWidth = 60;
            }else{// if (i==0){
                str = convertBytesToFormattedString(m_fiv.fileSize());
                minColWidth = fm.width(str);
            }

            int curTxtWidth = minColWidth  + 4;
            txt_offs -= curTxtWidth+3;
            StaticFunctions::paintTextRect(painter,
                                           str,
                                           QRectF(txt_offs, br.center().y()-fm.height()*0.5,
                                                  curTxtWidth, fm.height()),
                                           bckgrndCol,bckgrndCol,
                                           txtClr,sortFont,
                                           Qt::transparent);
            txt_offs -= 3;
        }
    }

    painter->setBrush(backgroundColor);
    painter->setPen(Qt::transparent);
    painter->drawRect(txtX, txtY, txtWidth+4, txtHeight);
    painter->setPen(m_textColor);
    painter->setFont(txtFont);
    painter->drawText(QPoint(txtX, txtY+txtAscent), fileNameToDraw);

//    painter->end();

    painter->restore();

    m_isCurrentlyRepainting.store(0);
}

void GraphicsFile::paintRectangle(QPainter* painter, QRectF triangleRect, bool upside_down){
    QPainterPath path;
    if(upside_down){
        path.moveTo(triangleRect.bottomLeft());
        path.lineTo(triangleRect.bottomRight());
        path.lineTo(triangleRect.center().x(), triangleRect.top());
        path.closeSubpath();
    }else{
        path.moveTo(triangleRect.topLeft());
        path.lineTo(triangleRect.topRight());
        path.lineTo(triangleRect.center().x(), triangleRect.bottom());
        path.closeSubpath();
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setBrush(m_detailsTextColor);
    painter->setPen(Qt::transparent);
    painter->drawPath(path);
    painter->restore();
}

void GraphicsFile::setTextColor(QColor textColor)
{
    m_textColor = textColor;
}

void GraphicsFile::setDetailsTextColor(QColor detailsTextColor)
{
    m_detailsTextColor = detailsTextColor;
}

void GraphicsFile::drawAbsoluteFilePath(bool drawAbsoluteFilePath)
{
//    qDebug() << "painting absolute file path: drawAbsoluteFilePath: " << drawAbsoluteFilePath
//             << "   absolute: "  << fileInfo.absoluteFilePath()
//             << "   relative: " << fileInfo.fileName();
    m_drawAbsoluteFilePath = drawAbsoluteFilePath;
    repaintBD();
}
void GraphicsFile::openOnDoubleClick(bool openOnDoubleClick){
    m_openOnDoubleClick = openOnDoubleClick;
}

void GraphicsFile::rightClickEnabled(bool rightClickEnabled)
{
    m_rightClickEnabled = rightClickEnabled;
}

void GraphicsFile::setBackgroundColor(QColor background)
{
    m_backgroundColor = background;
}

void GraphicsFile::setInitDraggingFunction(std::function<void (QString)> func)
{
    m_initDraggingFunc = func;
}

void GraphicsFile::setDropFunction(std::function<void(QString)> func)
{
    m_dropFunc = func;
}

void GraphicsFile::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
//    QGraphicsItem::mousePressEvent(event);

    QPointF mouP = event->pos();
    m_initMousePrsdPos = mouP;

    isPressed = false;

    bool sortRectPrsd = false;
    for(int i=0; i < m_sortRects.size(); i++){
        if(m_sortRects[i].contains(mouP)){
            sortRectPrsd = true;
            if(m_sortCaller && m_sortCaller->containsFunction(QString("sortBy"))){
                Order ord;
                if(i==0){
                    ord.ordered_by = ORDERED_BY::NAME;
                }else if(i==1){
                    ord.ordered_by = ORDERED_BY::SIZE;
                }else if(i==2){
                    ord.ordered_by = ORDERED_BY::TYPE;
                }else if(i==3){
                    ord.ordered_by = ORDERED_BY::MOD_DATE;
                }

                if(m_fiv.order().ordered_by == ord.ordered_by &&
                        !m_fiv.order().reversedOrdered)
                        ord.reversedOrdered = true;

                m_sortCaller->getFunction(QString("sortBy"))(ord);
            }
            break;
        }
    }
    if( !sortRectPrsd ){
        isPressed = true;
    }
}

void GraphicsFile::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
//    QGraphicsItem::mouseDoubleClickEvent(event);

    QPointF mouP = event->pos();
    bool sortRectsClicked = false;
    for(int i=0; i < m_sortRects.size(); ++i){
        if(m_sortRects[i].contains(mouP)){
            sortRectsClicked = true;
            break;
        }
    }
    if ( !sortRectsClicked && !elapseRectPressed(mouP)
               && m_openOnDoubleClick){
        openContent();
    }
}

void GraphicsFile::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
//    QGraphicsItem::mouseReleaseEvent(event);

    QPointF mouP = event->pos();

    if(boundingRect().contains(mouP)){
        bool sortRectPrsd = false;
        for(int i=0; i < m_sortRects.size(); i++){
            if(m_sortRects[i].contains(mouP)){
                sortRectPrsd = true;
                break;
            }
        }
        if( !sortRectPrsd ){
            qint64 curTime = QDateTime::currentMSecsSinceEpoch();
            if(curTime - lastTmePrsd < 300){

            }
            lastTmePrsd = curTime;

            bool elpsRctPrsd = false;
            if(elapseRectPressed(mouP)){
                elpsRctPrsd = true;
                if(    m_caller->containsFunction(QString("isEmpty"))
                    && m_caller->containsFunction(QString("isLoaded"))
                    && m_caller->containsFunction(QString("elapse"))){
                    auto isEmpty = m_caller->getFunction(QString("isEmpty"));
                    auto isLoaded = m_caller->getFunction(QString("isLoaded"));
                    if( !(isLoaded() && isEmpty()) ){
                        auto doElapsing = m_caller->getFunction(QString("elapse"));
                        doElapsing();
                    }
                }
            }
            if(!elpsRctPrsd){
                if(m_caller && m_caller->containsFunction(QString("select"))){
                    auto select = m_caller->getFunction(QString("select"));
                    select();
                }
            }
        }
    }
    isPressed = false;
}

void GraphicsFile::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
//    QGraphicsItem::mouseMoveEvent(event);

    if (event->button() != Qt::RightButton){
        if( m_initDraggingFunc &&
                (m_initMousePrsdPos - event->pos()).manhattanLength() > 20){
            m_initDraggingFunc(m_fiv.q_path());
        }
    }
}
void GraphicsFile::hoverEnterEvent(QGraphicsSceneHoverEvent * event){
//    QGraphicsItem::hoverEnterEvent(event);

    m_hover = true;
    m_sortRectHoverId = -1;
    repaintBD();
}
void GraphicsFile::hoverLeaveEvent(QGraphicsSceneHoverEvent * event){
//    QGraphicsItem::hoverLeaveEvent(event);

    m_hover = false;
    m_sortRectHoverId = -1;
    repaintBD();
}
void GraphicsFile::hoverMoveEvent(QGraphicsSceneHoverEvent * event){
//    QGraphicsItem::hoverMoveEvent(event);

    int sortHoverId = -1;

    QPointF mouP = event->pos();

    for(int i=0; i < m_sortRects.size(); i++){
        if(m_sortRects[i].contains(mouP)){
            sortHoverId = i;
            break;
        }
    }
    if(sortHoverId != m_sortRectHoverId){
        m_sortRectHoverId = sortHoverId;
        repaintBD();
    }
}

QString getUniqueFileName(QFileInfo fileInfo){
    QFileInfo newInfo;
    QString fileType = StaticFunctions::getFileType(fileInfo.fileName());
    if(!fileType.isEmpty()){
        fileType.prepend('.');
    }
    QString blankFileName = StaticFunctions::getFileNameWithoutFileType(fileInfo.fileName());
    int counter = 1;
    do{
        newInfo = QFileInfo(fileInfo.absolutePath()
                            .append(QDir::separator())
                            .append(blankFileName)
                            .append(QString("_%1%2").arg(counter++).arg(fileType)));
    }while(newInfo.exists());
    return newInfo.absoluteFilePath();
}

void GraphicsFile::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QGraphicsItem::contextMenuEvent(event);

//    if(m_rightClickEnabled){
//        qDebug() << "contextMenuEvent";
//    //    QActionGroup* actionGroup = new QActionGroup(&menu);
//    //    actionGroup->addAction(QString("copy"));

//        QMenu menu;
//        int maxFileNamelngth = 20;
//        QString fileName = m_fileInfo.fileName();
//        fileName = QString("open '%1'").arg( fileName.length() > maxFileNamelngth ?
//                                                       fileName.left(maxFileNamelngth).append("..."):
//                                                       fileName);

//        menu.addAction(new QAction(fileName));
//        menu.addSeparator();
//        menu.addAction(new QAction(QString("cut")));
//        menu.addAction(new QAction(QString("copy")));
//        menu.addAction(new QAction(QString("duplicate")));
//        menu.addSeparator();
//        menu.addAction(new QAction(QString("rename")));
//        menu.addSeparator();
//        menu.addAction(new QAction(QString("delete")));
////        menu.addSeparator();
////        menu.addAction(new QAction(QString("compress")));
////        menu.addSeparator();
////        menu.addAction(new QAction(QString("properties")));

//        QAction* action = menu.exec(event->screenPos());
//        if(action){
////            if(action->text() == QString("CUT")){
////                if( m_caller
////                        && m_caller->containsFunction("executeAction") )
////                {
////                    m_caller->getFunction("executeAction")(FILE_ACTION::CUT);
////                }
////            }else if(action->text() == QString("copy")){
////                if( m_caller
////                        && m_caller->containsFunction("executeAction") )
////                {
////                    m_caller->getFunction("executeAction")(FILE_ACTION::COPY);
////                }
////            }else if(action->text() == QString("duplicate")){
////                if( m_caller
////                        && m_caller->containsFunction("executeAction") )
////                {
////                    m_caller->getFunction("executeAction")(FILE_ACTION::DUPLICATE);
////                }
////            }else if(action->text() == QString("rename")){
////                if( m_caller
////                        && m_caller->containsFunction("executeAction") )
////                {
////                    m_caller->getFunction("executeAction")(FILE_ACTION::RENAME);
////                }
////            }else if(action->text() == QString("delete")){
////                if( m_caller
////                        && m_caller->containsFunction("executeAction") )
////                {
////                    m_caller->getFunction("executeAction")(FILE_ACTION::DELETE);
////                }
////            }else if(action->text() == QString("zip")){
////                if( m_caller
////                        && m_caller->containsFunction("executeAction") )
////                {
////                    m_caller->getFunction("executeAction")(FILE_ACTION::ZIP);
////                }
////            }else if(action->text() == QString("unzip")){
////                if( m_caller
////                        && m_caller->containsFunction("executeAction") )
////                {
////                    m_caller->getFunction("executeAction")(FILE_ACTION::UNZIP);
////                }
////            }
//        }
//    }
}

void GraphicsFile::keyPressEvent(QKeyEvent *event)
{
//    Q_UNUSED(event);
    QGraphicsItem::keyPressEvent(event);
}

void GraphicsFile::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
//    QGraphicsItem::dragEnterEvent(event);

    bool dragAccepted = event->mimeData()->hasUrls() ||
                       event->mimeData()->hasText();
    event->setAccepted(dragAccepted);
    if(dragAccepted != m_dragEntered){
        m_dragEntered = dragAccepted;
        repaintBD();
    }
}
void GraphicsFile::dragLeaveEvent(QGraphicsSceneDragDropEvent *event){
//    QGraphicsItem::dragLeaveEvent(event);

    m_dragEntered = false;
    repaintBD();
}

void GraphicsFile::dropEvent(QGraphicsSceneDragDropEvent *event)
{
//    QGraphicsItem::dropEvent(event);

    if(m_dropFunc){
        if(event->mimeData()->hasUrls()){
            QString str;
            foreach (const auto& url, event->mimeData()->urls()) {
                if(url.toLocalFile().isEmpty()){
                    str.append(url.toString().append("\n"));
                }else{
                    QFileInfo fi(url.toLocalFile());
                    if(fi.exists()){
                        str.append(fi.absoluteFilePath()).append("\n");
                    }
                }
            }
            m_dropFunc(str.trimmed());
        }else if (event->mimeData()->hasText()){
            m_dropFunc(event->mimeData()->text());
        }
    }
}

void GraphicsFile::repaintBD()
{
    if(m_isCurrentlyRepainting.load() <= 0)
    {
        update();
    }/*else
    {
        qDebug() << "obacht -> graphicsfile is already updating: " << m_fileInfo.fileName();
    }*/
}

void GraphicsFile::openContent()
{
    if(m_caller
            && m_caller->containsFunction("isDir")
            && m_caller->getFunction("isDir")()
            && m_caller->containsFunction("setAsRoot")){
        m_caller->getFunction("setAsRoot")();
    }else{
        QDesktopServices::openUrl(QUrl(QString("file:%1").arg(m_fiv.q_path())));
    }
}

bool GraphicsFile::elapseRectPressed(QPointF mouP)
{
    return m_elapseRect && m_elapseRect->contains(mouP);
}
