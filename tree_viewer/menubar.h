#ifndef MENUBAR_H
#define MENUBAR_H

#include <QObject>
#include <QVector>
#include <QSize>
#include <QPoint>
#include <QSizeF>
#include <QPointF>
#include <QRect>
#include <QRectF>
#include <QDebug>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>

#include <functional>
#include <memory>
#include <utility>
#include <map>
#include <vector>
#include <string>
#include <sstream>

#include "dynamicfunctioncaller.h"
#include "customgraphicitems.h"

class MenuBar : public GraphicItemsBD::GraphicsItemBD{
    Q_OBJECT
public:
    MenuBar(qreal maxWidthOrHeight,
            QPointF centerPosition,
            GraphicItemsBD::ORIENTATION orientation = GraphicItemsBD::ORIENTATION::HORIZONTAL,
            bool m_centerFromEnd = false,
            QGraphicsItem* parent = nullptr);
    ~MenuBar() override;

    void setPosition(QPoint position);

    QRectF boundingRect() const override;
    void setOrientation(GraphicItemsBD::ORIENTATION orientation);
    void revalidateSize();

    void setCaller(std::shared_ptr<DynamicFunctionCaller<QString, std::function<void()>>> btnFunctions,
                   std::shared_ptr<DynamicFunctionCaller<QString, std::function<void(QPainter*, QRectF)>>> buttonPaintingFunctions,
                   std::function<int(int)> groupingFunc = nullptr);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    GraphicItemsBD::ORIENTATION orientation();
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
private:

    int groupCount();
    void revalGroupMap();

    int getMaxElementsPerRowOrColumn();
    qreal paintHeight();
    qreal paintWidth();
    qreal paneWidth();
    qreal paneHeight();
    qreal bordersToPaint();
    qreal buttonCount();
    int getBiggestGroupSize();
    qreal getButtonSpace(int btnCount);

    std::pair<int,int> evalColumnAndRowDimensions();

    int m_anzRows = 1;
    int m_anzColumns = 1;
    qreal m_btnEdge = 45.0;
    qreal m_padding = 3.0;
    qreal m_offsets = 2.0;
    qreal m_maxWidthOrHeight = 0.0;
    QPointF m_centerOrientation = QPointF(0,0);
    bool m_centerFromEnd = false;
    int m_btnsCount = 0;
    QVector<QRectF> m_buttons;
    QVector<bool> m_mouInBtns;

    std::shared_ptr<DynamicFunctionCaller<QString, std::function<void()>>> m_btnFuncs = std::shared_ptr<DynamicFunctionCaller<QString, std::function<void()>>>();
    std::shared_ptr<DynamicFunctionCaller<QString, std::function<void(QPainter*, QRectF)>>> m_paintingFuncs = std::shared_ptr<DynamicFunctionCaller<QString, std::function<void(QPainter*, QRectF)>>>();
    std::function<int(int)> m_groupingFunc = nullptr;

    std::vector<std::pair<QPointF,QPointF>> m_groupSeparators;

    QString vec_to_str(const std::vector<int> vec);

    std::map<int, std::vector<int>> m_groupingMap;

    int m_groupingBorder = 2;
    int m_groupingPadding = 1;

    qint64 lastTmePrsd = Q_INT64_C(0);

    GraphicItemsBD::ORIENTATION m_orientation = GraphicItemsBD::ORIENTATION::HORIZONTAL;
};

#endif // MENUBAR_H
