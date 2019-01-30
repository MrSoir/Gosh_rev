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
    Q_UNUSED(widget)
    Q_UNUSED(option)

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
            float fctr = 0.8;
            float wndwWdth = (float)rct.width()*fctr;
            float xStart = rct.x() + (rct.width()-wndwWdth)*0.5;
            float yStart = rct.y() + (rct.height()-wndwWdth)*0.5;
            QRect* wndwRct = new QRect(xStart,yStart, wndwWdth,wndwWdth);

            rects[0] = wndwRct;
        }
    }else if(m_anzRects == 2){
        float fctr = 0.9;
        float rctWidth = ((float)rct.width()) *0.4;
        float offs = ((float)rct.width())*(1.0-fctr)*0.5;

        if(horizontal){
            painter->drawLine(rct.left()+offs,
                              rct.center().y(),
                              rct.right()-offs,
                              rct.center().y());
            if(!rects[0]){
                QSize size(rctWidth, rctWidth);
                QPoint p1, p2;
                p1 = QPoint(rct.center().x()-rctWidth*0.5,
                          rct.top()+offs);
                p2 = QPoint(rct.center().x()-rctWidth*0.5,
                          rct.bottom()-offs-rctWidth);
                rects[0] = new QRect(p1, size);
                rects[1] = new QRect(p2, size);
            }
        }else{
            painter->drawLine(rct.center().x(),
                              rct.top()+offs,
                              rct.center().x(),
                              rct.bottom()-offs);

            if(!rects[0]){
                QSize size(rctWidth, rctWidth);
                QPoint p1, p2;
                p1 = QPoint(rct.left()+offs,
                          rct.center().y()-rctWidth*0.5);
                p2 = QPoint(rct.right()-offs-rctWidth,
                          rct.center().y()-rctWidth*0.5);
                rects[0] = new QRect(p1, size);
                rects[1] = new QRect(p2, size);
            }
        }
    }else if(m_anzRects == 3){
        float fctr = 0.9;
        float rctWidth = ((float)rct.width()) *0.4;
        float offs = ((float)rct.width())*(1.0-fctr)*0.5;
        if(horizontal){
            painter->drawLine(rct.left()+offs,
                              rct.center().y(),
                              rct.right()-offs,
                              rct.center().y());
            painter->drawLine(rct.center().x(),
                              rct.center().y(),
                              rct.center().x(),
                              rct.bottom()-offs);

            if(!rects[0]){
                QSize size(rctWidth, rctWidth);
                QPoint p1, p2, p3;
                p1 = QPoint(rct.center().x()-rctWidth*0.5,
                          rct.top()+offs);
                p2 = QPoint(rct.left()+offs,
                          rct.bottom()-offs-rctWidth);
                p3 = QPoint(rct.right()-offs-rctWidth,
                          rct.bottom()-offs-rctWidth);

                rects[0] = new QRect(p1, size);
                rects[1] = new QRect(p2, size);
                rects[2] = new QRect(p3, size);
            }
        }else{
            painter->drawLine(rct.center().x(),
                              rct.top()+offs,
                              rct.center().x(),
                              rct.bottom()-offs);
            painter->drawLine(rct.center().x(),
                              rct.center().y(),
                              rct.right()-offs,
                              rct.center().y());
            if(!rects[0]){
                QSize size(rctWidth, rctWidth);
                QPoint p1, p2, p3;
                p1 = QPoint(rct.left()+offs,
                          rct.center().y()-rctWidth*0.5);
                p2 = QPoint(rct.right()-offs-rctWidth,
                          rct.top()+offs);
                p3 = QPoint(rct.right()-offs-rctWidth,
                          rct.bottom()-offs-rctWidth);

                rects[0] = new QRect(p1, size);
                rects[1] = new QRect(p2, size);
                rects[2] = new QRect(p3, size);
            }
        }
    }else{ // == 4
        float fctr = 0.9;
        float rctWidth = ((float)rct.width()) *0.4;
        float offs = ((float)rct.width())*(1.0-fctr)*0.5;

        painter->drawLine(rct.left()+offs,
                          rct.center().y(),
                          rct.right()-offs,
                          rct.center().y());
        painter->drawLine(rct.center().x(),
                          rct.top()+offs,
                          rct.center().x(),
                          rct.bottom()-offs);
        if(!rects[0]){
                QSize size(rctWidth, rctWidth);
                QPoint p1, p2, p3, p4;
                p1 = QPoint(rct.left()+offs,
                          rct.top()+offs);
                p2 = QPoint(rct.right()-offs-rctWidth,
                          rct.top()+offs);
                p3 = QPoint(rct.left()+offs,
                          rct.bottom()-offs-rctWidth);
                p4 = QPoint(rct.right()-offs-rctWidth,
                          rct.bottom()-offs-rctWidth);
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
                && rects[i]->contains(QPoint(mouP.x(), mouP.y()))){
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
        if(rects[i] && rects[i]->contains(QPoint(mouP.x(), mouP.y()))){
            mouseRectId = i;
            break;
        }
    }
    update();
    return QGraphicsItem::hoverEnterEvent(event);
}
