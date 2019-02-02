#ifndef SEARCHMENUBD_H
#define SEARCHMENUBD_H

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

class SearchMenuBD : public GraphicItemsBD::GraphicsItemBD
{
public:
    SearchMenuBD(const QSize& size = QSize(0,0),
                 const QPoint& pos = QPoint(0,0),
                 QGraphicsItem* parent = nullptr);
    virtual ~SearchMenuBD() override;
    void setCaller(std::shared_ptr<DynamicFunctionCaller<QString, std::function<QString()>>> caller);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent * event) override;
private:
    QRectF m_nextBtn;
    QRectF m_preBtn;
    QRectF m_closeBtn;

    bool mouInNextBtn = false;
    bool mouInPrevBtn = false;
    bool mouInCloseBtn = false;

    std::shared_ptr<DynamicFunctionCaller<QString, std::function<QString()>>> m_caller = std::shared_ptr<DynamicFunctionCaller<QString, std::function<QString()>>>();

    qint64 lastTmePrsd = Q_INT64_C(0);
};

#endif // SEARCHMENUBD_H
