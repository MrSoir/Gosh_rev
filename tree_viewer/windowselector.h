#ifndef WINDOWSELECTOR_H
#define WINDOWSELECTOR_H

#include <QObject>
#include <QGraphicsItem>
#include <QDebug>
#include <QString>
#include <QGraphicsSceneMouseEvent>
#include <QRectF>
#include <QColor>
#include <QPoint>
#include <QPointF>

#include <functional>
#include <memory>

#include "staticfunctions.h"
#include "customgraphicitems.h"
#include "dynamicfunctioncaller.h"

class WindowSelector : public GraphicItemsBD::GraphicsItemBD{
public:
    WindowSelector(const QSize& size,
                   const QPoint& pos,
                   QGraphicsItem *parent = nullptr);
    ~WindowSelector() override;


    void setCaller(std::function<void(int)> caller);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent * event) override;
private:
    bool m_hover = false;

    bool horizontal = true;

    int m_anzRects = 2;

    bool isPressed = false;
    qint64 lastTmePrsd = Q_INT64_C(0);

    QVector<QRect*> rects;
    int mouseRectId = -1;

    std::function<void(int)> caller;
};

#endif // WINDOWSELECTOR_H
