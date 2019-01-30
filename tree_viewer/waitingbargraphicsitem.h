#ifndef WAITINGBARGRAPHICSITEM_H
#define WAITINGBARGRAPHICSITEM_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QGraphicsItem>
#include <QRectF>
#include <QPainter>
#include <QTimer>

#include <functional>
#include <memory>
#include <math.h>

#include "customgraphicitems.h"
#include "rectcolor.h"


class WaitingBarGraphicsItem : public GraphicItemsBD::GraphicsItemBD
{
public:
    WaitingBarGraphicsItem(const QSize &size, const QPoint &pos,
                           int rectCount = 20,//m_rectCount(20),
                           int padding = 5,//m_padding(5),
                           QGraphicsItem* parent = nullptr);

    ~WaitingBarGraphicsItem();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void startAnimation();
    void stopAnimation();
    bool isRunning();

private:
    RectColor m_rectCol;
    int m_rectCount;
    int m_padding;

    QTimer* m_animationTimer;
    bool m_isRunning;
    int m_frameRate = 20;
};

#endif // WAITINGBARGRAPHICSITEM_H
