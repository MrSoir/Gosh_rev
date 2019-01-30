#include "searchmenubd.h"

SearchMenuBD::SearchMenuBD(const QSize& size,
             const QPoint& pos,
             QGraphicsItem* parent)
    : GraphicItemsBD::GraphicsItemBD(size, pos, parent)
{}
SearchMenuBD::~SearchMenuBD(){
    m_caller.reset();
}
void SearchMenuBD::setCaller(std::shared_ptr<DynamicFunctionCaller<QString, std::function<QString()>>> caller){
    this->m_caller = caller;
}

void SearchMenuBD::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(widget)
    Q_UNUSED(option)

    // gaaanz wichtig: erstmal painter an boundingRect clippen!!!:
    QRectF rct = boundingRect();

    painter->setRenderHint(QPainter::Antialiasing, true);

    painter->setClipRect(rct);

    QColor gradCol1(255,255,255, 200);
    QColor gradCol2(255,255,255, 200);
    StaticFunctions::paintRect(painter, rct, gradCol1, gradCol2);

    if(revalidate){
        float prevBtnWidth = (float)qMin((float)(rct.width() * 0.2), (float)80.);
        float prevBtnHeight = rct.height() * 0.5;
        float offs = 20;
        float prevBtnX = rct.center().x() - prevBtnWidth - offs;
        float nextBtnX = rct.center().x() + offs;
        float upBtnY = 3 + rct.y();
        m_preBtn = QRectF(prevBtnX, upBtnY, prevBtnWidth, prevBtnHeight);
        m_nextBtn = QRectF(nextBtnX, upBtnY, prevBtnWidth, prevBtnHeight);

        float closeBtnWidth = (float)qMin((float)(rct.width() * 0.5), (float)40.);
        float closeBtnHeight = rct.height() * 0.4;
        offs = rct.width() * 0.05;
        float closeBtnX = rct.right()-closeBtnWidth-offs;
        float closeBtnY = rct.center().y()-(float)(closeBtnHeight*0.5);
        m_closeBtn = QRectF(closeBtnX,
                            closeBtnY,
                            closeBtnWidth,
                            closeBtnHeight);
    }
    QColor nextColor1 = QColor(255, 255, 255, 255),
           nextColor2 = QColor(255, 255, 255, 255),
           prevColor1 = QColor(255, 255, 255, 255),
           prevColor2 = QColor(255, 255, 255, 255),
           closColor1 = QColor(255, 255, 255, 255),
           closColor2 = QColor(255, 255, 255, 255);
    if(mouInNextBtn){
        nextColor1 = QColor(255, 255, 255, 255);
        nextColor2 = QColor(200, 200, 200, 255);
    }else if (mouInPrevBtn){
        prevColor1 = QColor(255, 255, 255, 255);
        prevColor2 = QColor(200, 200, 200, 255);
    }else if (mouInCloseBtn){
        closColor1 = QColor(255, 180, 180, 255);
        closColor2 = QColor(255, 0, 0, 255);
    }

    QString curSearchId = m_caller->getFunction("getCurrentSearchIndex")();
    QString searchCount = m_caller->getFunction("getSearchResultsCount")();
    QString indexStr = QString("%1/%2").arg(curSearchId).arg(searchCount);


    QFont font = StaticFunctions::getGoshFont(7, QFont::Normal);
    QFontMetrics fm(font);
    int indxWidth = fm.width(indexStr);
    int indxHeight = fm.height();
    int indxPadding = 2;

    int indxCentX = int((m_preBtn.center().x() + m_nextBtn.center().x()) * 0.5);
    int indxLeft = indxCentX - int(indxWidth*0.5);
    int indxY = m_preBtn.top() + indxPadding;

    QRect indxRct = QRect( indxLeft,
                           indxY,
                           indxWidth,
                           indxHeight );
    painter->setFont(font);
    painter->drawText(indxRct, indexStr);

    StaticFunctions::paintRect(painter, m_nextBtn,
              nextColor1,
              nextColor2);
    StaticFunctions::paintArrowDown(painter, m_nextBtn);
    StaticFunctions::paintRect(painter, m_preBtn,
              prevColor1,
              prevColor2);
    StaticFunctions::paintArrowUp(painter, m_preBtn);

    StaticFunctions::paintRect(painter, m_closeBtn,
              closColor1,
              closColor2);
    StaticFunctions::paintCrossRect(painter, m_closeBtn);

    if(m_caller){
        QString str = m_caller->getFunction("getSearchResult")();
        if(str.isEmpty()){
            str = QString("no match!");
        }
         str.prepend("--").append("--");

        QFont font = StaticFunctions::getGoshFont(12);
        painter->setFont(font);
        QFontMetrics fm(font);
        int pixelsWidth = fm.width(str);
        int pixelsHeight = fm.height();
        int offsTxt = 2;
        painter->setPen(QPen(QColor(0,0,0), 3, Qt::SolidLine));
        painter->drawText(QRect(rct.center().x()-pixelsWidth*0.5,
                                rct.bottom() - pixelsHeight -offsTxt,
                                pixelsWidth*2,
                                pixelsHeight),
                                str);
    }

    revalidate = false;

//        painter->end();
}

void SearchMenuBD::mousePressEvent(QGraphicsSceneMouseEvent *event){
    QPointF mouP = event->pos();
    if(       m_nextBtn.contains(mouP) && m_caller){
        m_caller->getFunction(QString("next"))();
    }else if (m_preBtn.contains(mouP) && m_caller){
        m_caller->getFunction(QString("previous"))();
    }else if (m_closeBtn.contains(mouP) && m_caller){
//            qDebug() << "SearchMenu close-Btn pressed!";
        m_caller->getFunction(QString("close"))();
    }
    qint64 curTime = QDateTime::currentMSecsSinceEpoch();
    if(curTime - lastTmePrsd < 300){

    }
    lastTmePrsd = curTime;
    update();
}
void SearchMenuBD::hoverMoveEvent(QGraphicsSceneHoverEvent * event){
    QPointF mouP = event->pos();
    if(       m_nextBtn.contains(mouP)){
        if(!mouInNextBtn){
            mouInNextBtn = true;
            update();
            return;
        }
    }else if (m_preBtn.contains(mouP)){
        if(!mouInPrevBtn){
            mouInPrevBtn = true;
            update();
            return;
        }
    }else if (m_closeBtn.contains(mouP)){
        if(!mouInCloseBtn){
            mouInCloseBtn = true;
            update();
            return;
        }
    }else if(mouInNextBtn || mouInPrevBtn || mouInCloseBtn){
        mouInNextBtn = false;
        mouInPrevBtn = false;
        mouInCloseBtn = false;
        update();
        return;
    }
//        return QGraphicsItem::hoverEnterEvent(event);
}
