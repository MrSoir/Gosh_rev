#include "removewindowdialog.h"

RemoveWindowDialog::RemoveWindowDialog(int anzRects,
           Orientation::ORIENTATION orientation,
           QSize size,
           QPoint position,
           QWidget *parent)
    : QDialog(parent),
      m_anzRects(anzRects)
{
    connect(this, &RemoveWindowDialog::close, this, &RemoveWindowDialog::deleteLater);

    setSize(size);
    setPosition(position);

    if(orientation == Orientation::ORIENTATION::HORIZONTAL){
        m_horizontal = true;
    }else{
        m_horizontal = false;
    }
    setAttribute(Qt::WA_TranslucentBackground);
    setModal(true);

    setPosition(position);

    for(int i=0; i < 4; ++i)
        rects.append(new QRect());

    setMouseTracking(true);

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);

    QPushButton* closeBtn = new QPushButton("cancel");
    QObject::connect(closeBtn, &QPushButton::clicked, [=](){
        this->close();
        this->deleteLater();
    });

    QVBoxLayout* vBox = new QVBoxLayout();
    vBox->setAlignment(Qt::AlignBottom);
    vBox->addWidget(closeBtn);
    vBox->setContentsMargins(0, 0, 0, 0);
    this->setLayout(vBox);
}

RemoveWindowDialog::~RemoveWindowDialog()
{
    for(int i=0; i < rects.size(); i++){
        delete rects[i];
    }
    rects.clear();
}

QRectF RemoveWindowDialog::boundingRect()
{
    return QRectF(0,0, width(), height());
}

void RemoveWindowDialog::setSize(const QSize &size)
{
    setMinimumSize(size);
    setMaximumSize(size);
}


void RemoveWindowDialog::setPosition(const QPoint &position)
{
    this->move(position.x(), position.y());
    repaint();
}

void RemoveWindowDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter* painter = new QPainter(this);

    // gaaanz wichtig: erstmal painter an boundingRect clippen!!!:
    QRectF rct = boundingRect();

    painter->setRenderHint(QPainter::Antialiasing, true);

//    painter->setClipRect(rct);

    QLinearGradient gradient(rct.topLeft(), rct.bottomRight());
    gradient.setColorAt(0, QColor(255,255,255, 255));
    gradient.setColorAt(1, QColor(220,220,255, 255));
    painter->fillRect(rct, gradient);

    QVector<QPoint> pos;
    QSize rectSize;

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

        if(m_horizontal){
            painter->drawLine(static_cast<int>(rct.left()+offs),
                              static_cast<int>(rct.center().y()),
                              static_cast<int>(rct.right()-offs),
                              static_cast<int>(rct.center().y()));

            QSize size(static_cast<int>(rctWidth), static_cast<int>(rctWidth));
            QPoint p1, p2;
            p1 = QPointF(rct.center().x()-rctWidth*0.5,
                      rct.top()+offs).toPoint();
            p2 = QPointF(rct.center().x()-rctWidth*0.5,
                      rct.bottom()-offs-rctWidth).toPoint();

            pos.append(p1);
            pos.append(p2);
            rectSize = size;
        }else{
            painter->drawLine(static_cast<int>(rct.center().x()),
                              static_cast<int>(rct.top()+offs),
                              static_cast<int>(rct.center().x()),
                              static_cast<int>(rct.bottom()-offs));

            QSize size(static_cast<int>(rctWidth), static_cast<int>(rctWidth));
            QPoint p1, p2;
            p1 = QPointF(rct.left()+offs,
                      rct.center().y()-rctWidth*0.5).toPoint();
            p2 = QPointF(rct.right()-offs-rctWidth,
                      rct.center().y()-rctWidth*0.5).toPoint();

            pos.append(p1);
            pos.append(p2);
            rectSize = size;
        }
    }else if(m_anzRects == 3){
        qreal fctr = 0.9;
        qreal rctWidth = rct.width() *0.4;
        qreal offs = rct.width()*(1.0-fctr)*0.5;
        if(m_horizontal){
            painter->drawLine(static_cast<int>(rct.left()+offs),
                              static_cast<int>(rct.center().y()),
                              static_cast<int>(rct.right()-offs),
                              static_cast<int>(rct.center().y()));
            painter->drawLine(static_cast<int>(rct.center().x()),
                              static_cast<int>(rct.center().y()),
                              static_cast<int>(rct.center().x()),
                              static_cast<int>(rct.bottom()-offs));

            QSize size(static_cast<int>(rctWidth), static_cast<int>(rctWidth));
            QPoint p1, p2, p3;
            p1 = QPointF(rct.center().x()-rctWidth*0.5,
                      rct.top()+offs).toPoint();
            p2 = QPointF(rct.left()+offs,
                      rct.bottom()-offs-rctWidth).toPoint();
            p3 = QPointF(rct.right()-offs-rctWidth,
                      rct.bottom()-offs-rctWidth).toPoint();

            pos.append(p1);
            pos.append(p2);
            pos.append(p3);
            rectSize = size;
        }else{
            painter->drawLine(static_cast<int>(rct.center().x()),
                              static_cast<int>(rct.top()+offs),
                              static_cast<int>(rct.center().x()),
                              static_cast<int>(rct.bottom()-offs));
            painter->drawLine(static_cast<int>(rct.center().x()),
                              static_cast<int>(rct.center().y()),
                              static_cast<int>(rct.right()-offs),
                              static_cast<int>(rct.center().y()));

            QSize size(static_cast<int>(rctWidth), static_cast<int>(rctWidth));
            QPoint p1, p2, p3;
            p1 = QPointF(rct.left()+offs,
                      rct.center().y()-rctWidth*0.5).toPoint();
            p2 = QPointF(rct.right()-offs-rctWidth,
                      rct.top()+offs).toPoint();
            p3 = QPointF(rct.right()-offs-rctWidth,
                      rct.bottom()-offs-rctWidth).toPoint();

            pos.append(p1);
            pos.append(p2);
            pos.append(p3);
            rectSize = size;
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

        pos.append(p1);
        pos.append(p2);
        pos.append(p3);
        pos.append(p4);
        rectSize = size;
    }

    for(int i=0; i < pos.size(); ++i)
    {
        setRectsPosition(rects[i], pos[i]);
        rects[i]->setSize(rectSize);
    }

    for(int i=0; i < m_anzRects; i++){
        StaticFunctions::paintRect(painter, *rects[i]);
    }
    if(mouseRectId > -1 && rects[mouseRectId]){
        StaticFunctions::paintCrossRect(painter, *rects[mouseRectId]);
    }
//    QDialog::paintEvent(event);
}

void RemoveWindowDialog::mousePressEvent(QMouseEvent *event){
    QPointF mouP = event->pos();
    for(int i=0; i < m_anzRects; i++){
        if(rects[i]
                && rects[i]->contains(QPointF(mouP.x(), mouP.y()).toPoint())){
            emit removeWindow(i);
            this->close();
            break;
        }
    }
    qint64 curTime = QDateTime::currentMSecsSinceEpoch();
    if(curTime - lastTmePrsd < 300){

    }
    lastTmePrsd = curTime;
    isPressed = true;
    repaint();
    //        update();
}

void RemoveWindowDialog::mouseMoveEvent(QMouseEvent *event){
    m_hover = true;
    QPointF mouP = event->pos();
    mouseRectId = -1;
    for(int i=0; i < m_anzRects; i++){
        if(rects[i] && rects[i]->contains(QPointF(mouP.x(), mouP.y()).toPoint())){
            mouseRectId = i;
            break;
        }
    }
//        update();
    repaint();
    return QWidget::mouseMoveEvent(event);
}

void RemoveWindowDialog::setRectsPosition(QRect *rect, const QPoint &pos)
{
    rect->setX(pos.x());
    rect->setY(pos.y());
}
