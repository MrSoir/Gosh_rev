#ifndef ELAPSEMENUBD_H
#define ELAPSEMENUBD_H

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

class ElapseMenuBD : public GraphicItemsBD::GraphicsItemBD{
public:
    ElapseMenuBD(int buttonCount,
                 int columnWidth,
                 const QSize& size = QSize(0,0),
                 const QPoint& pos = QPoint(0,0),
                 QGraphicsItem* parent = nullptr);

    virtual ~ElapseMenuBD() override;
    void setCaller(std::shared_ptr<DynamicFunctionCaller<QString, std::function<bool(int)>>> caller);

    void setBackroundColor(QColor grad1, QColor grad2);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
private:
    QVector<QRectF> m_buttons;
    int m_mouInButtonId = -1;
    int m_colWidth;
    int m_buttonCount;

    QColor m_grad1 = QColor(0,0,0, 255);
    QColor m_grad2 = QColor(0,0,0, 255);

    std::shared_ptr<DynamicFunctionCaller<QString, std::function<bool(int)>>> m_caller = std::shared_ptr<DynamicFunctionCaller<QString, std::function<bool(int)>>>();

    qint64 lastTmePrsd = Q_INT64_C(0);
};

#endif // ELAPSEMENUBD_H
