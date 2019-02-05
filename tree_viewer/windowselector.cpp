#include "windowselector.h"

WindowSelector::WindowSelector(const QSize& size,
               const QPoint& pos,
               QGraphicsItem *parent)
    : GraphicItemsBD::GraphicsItemBD(size, pos, parent)
{
    rects.append(nullptr);
    rects.append(nullptr);
    rects.append(nullptr);
    rects.append(nullptr);
}
WindowSelector::~WindowSelector(){
    foreach(QRect* rct, rects){
        delete rct;
    }
    rects.clear();
}


void WindowSelector::setCaller(std::function<void(int)> caller){
    this->caller = caller;
}

void WindowSelector::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    GraphicItemsBD::GraphicsItemBD::paint(painter, option, widget);

    // gaaanz wichtig: erstmal painter an boundingRect clippen!!!:
    QRectF rct = boundingRect();

    painter->setRenderHint(QPainter::Antialiasing, true);

    painter->setClipRect(rct);

    QLinearGradient gradient(rct.topLeft(), rct.bottomRight());
    gradient.setColorAt(0, QColor(255,255,255, 200));
    gradient.setColorAt(1, QColor(220,220,255, 200));
    painter->fillRect(rct, gradient);

    if(m_anzRects == 1){
        if(!rects[0]){
            qreal fctr = 0.8;
            qreal wndwWdth = rct.width()*fctr;
            qreal xStart = rct.x() + (rct.width()-wndwWdth)*0.5;
            qreal yStart = rct.y() + (rct.height()-wndwWdth)*0.5;
            QRect* wndwRct = new QRect(static_cast<int>(xStart),
                                       static_cast<int>(yStart),
                                       static_cast<int>(wndwWdth),
                                       static_cast<int>(wndwWdth));

            rects[0] = wndwRct;
        }
    }else if(m_anzRects == 2){
        qreal fctr = 0.9;
        qreal rctWidth = rct.width() *0.4;
        qreal offs = rct.width()*(1.0-fctr)*0.5;

        if(horizontal){
            painter->drawLine(static_cast<int>(rct.left()+offs),
                              static_cast<int>(rct.center().y()),
                              static_cast<int>(rct.right()-offs),
                              static_cast<int>(rct.center().y()));
            if(!rects[0]){
                QSize size(static_cast<int>(rctWidth), static_cast<int>(rctWidth));
                QPoint p1, p2;
                p1 = QPointF(rct.center().x()-rctWidth*0.5,
                          rct.top()+offs).toPoint();
                p2 = QPointF(rct.center().x()-rctWidth*0.5,
                          rct.bottom()-offs-rctWidth).toPoint();
                rects[0] = new QRect(p1, size);
                rects[1] = new QRect(p2, size);
            }
        }else{
            painter->drawLine(static_cast<int>(rct.center().x()),
                              static_cast<int>(rct.top()+offs),
                              static_cast<int>(rct.center().x()),
                              static_cast<int>(rct.bottom()-offs));

            if(!rects[0]){
                QSize size(static_cast<int>(rctWidth), static_cast<int>(rctWidth));
                QPoint p1, p2;
                p1 = QPointF(rct.left()+offs,
                          rct.center().y()-rctWidth*0.5).toPoint();
                p2 = QPointF(rct.right()-offs-rctWidth,
                          rct.center().y()-rctWidth*0.5).toPoint();
                rects[0] = new QRect(p1, size);
                rects[1] = new QRect(p2, size);
            }
        }
    }else if(m_anzRects == 3){
        qreal fctr = 0.9;
        qreal rctWidth = rct.width() *0.4;
        qreal offs = rct.width()*(1.0-fctr)*0.5;
        if(horizontal){
            painter->drawLine(static_cast<int>(rct.left()+offs),
                              static_cast<int>(rct.center().y()),
                              static_cast<int>(rct.right()-offs),
                              static_cast<int>(rct.center().y()));
            painter->drawLine(static_cast<int>(rct.center().x()),
                              static_cast<int>(rct.center().y()),
                              static_cast<int>(rct.center().x()),
                              static_cast<int>(rct.bottom()-offs));

            if(!rects[0]){
                QSize size(static_cast<int>(rctWidth), static_cast<int>(rctWidth));
                QPoint p1, p2, p3;
                p1 = QPointF(rct.center().x()-rctWidth*0.5,
                          rct.top()+offs).toPoint();
                p2 = QPointF(rct.left()+offs,
                          rct.bottom()-offs-rctWidth).toPoint();
                p3 = QPointF(rct.right()-offs-rctWidth,
                          rct.bottom()-offs-rctWidth).toPoint();

                rects[0] = new QRect(p1, size);
                rects[1] = new QRect(p2, size);
                rects[2] = new QRect(p3, size);
            }
        }else{
            painter->drawLine(static_cast<int>(rct.center().x()),
                              static_cast<int>(rct.top()+offs),
                              static_cast<int>(rct.center().x()),
                              static_cast<int>(rct.bottom()-offs));
            painter->drawLine(static_cast<int>(rct.center().x()),
                              static_cast<int>(rct.center().y()),
                              static_cast<int>(rct.right()-offs),
                              static_cast<int>(rct.center().y()));
            if(!rects[0]){
                QSize size(static_cast<int>(rctWidth), static_cast<int>(rctWidth));
                QPoint p1, p2, p3;
                p1 = QPointF(rct.left()+offs,
                          rct.center().y()-rctWidth*0.5).toPoint();
                p2 = QPointF(rct.right()-offs-rctWidth,
                          rct.top()+offs).toPoint();
                p3 = QPointF(rct.right()-offs-rctWidth,
                          rct.bottom()-offs-rctWidth).toPoint();

                rects[0] = new QRect(p1, size);
                rects[1] = new QRect(p2, size);
                rects[2] = new QRect(p3, size);
            }
        }
    }else{ // == 4
        qreal fctr = 0.9;
        qreal rctWidth = rct.width() *0.4;
        qreal offs = rct.width()*(1.0-fctr)*0.5;

        painter->drawLine(static_cast<int>(rct.left()+offs),
                          static_cast<int>(rct.center().y()),
                          static_cast<int>(rct.right()-offs),
                          static_cast<int>(rct.center().y()));
        painter->drawLine(static_cast<int>(rct.center().x()),
                          static_cast<int>(rct.top()+offs),
                          static_cast<int>(rct.center().x()),
                          static_cast<int>(rct.bottom()-offs));
        if(!rects[0]){
                QSize size(static_cast<int>(rctWidth), static_cast<int>(rctWidth));
                QPoint p1, p2, p3, p4;
                p1 = QPointF(rct.left()+offs,
                          rct.top()+offs).toPoint();
                p2 = QPointF(rct.right()-offs-rctWidth,
                          rct.top()+offs).toPoint();
                p3 = QPointF(rct.left()+offs,
                          rct.bottom()-offs-rctWidth).toPoint();
                p4 = QPointF(rct.right()-offs-rctWidth,
                          rct.bottom()-offs-rctWidth).toPoint();
            rects[0] = new QRect(p1, size);
            rects[1] = new QRect(p2, size);
            rects[2] = new QRect(p3, size);
            rects[3] = new QRect(p4, size);
        }
        painter->drawRect(*rects[0]);
        painter->drawRect(*rects[1]);
        painter->drawRect(*rects[2]);
        painter->drawRect(*rects[3]);
    }
    for(int i=0; i < m_anzRects; i++){
        StaticFunctions::paintRect(painter, *rects[i]);
    }
    if(mouseRectId > -1 && rects[mouseRectId]){
        StaticFunctions::paintCrossRect(painter, *rects[mouseRectId]);
    }

//        painter->end();
}

void WindowSelector::mousePressEvent(QGraphicsSceneMouseEvent *event){
    QPointF mouP = event->pos();
    for(int i=0; i < m_anzRects; i++){
        if(rects[i] && caller
                && rects[i]->contains(QPointF(mouP.x(), mouP.y()).toPoint())){
            caller(i);
        }
    }
    qint64 curTime = QDateTime::currentMSecsSinceEpoch();
    if(curTime - lastTmePrsd < 300){

    }
    lastTmePrsd = curTime;
    isPressed = true;
    update();
}
void WindowSelector::hoverMoveEvent(QGraphicsSceneHoverEvent * event){
    m_hover = true;
    QPointF mouP = event->pos();
    mouseRectId = -1;
    for(int i=0; i < m_anzRects; i++){
        if(rects[i] && rects[i]->contains(QPointF(mouP.x(), mouP.y()).toPoint())){
            mouseRectId = i;
            break;
        }
    }
    update();
    return QGraphicsItem::hoverEnterEvent(event);
}
