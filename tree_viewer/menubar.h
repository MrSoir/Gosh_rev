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


class MenuBar : public GraphicItemsBD::GraphicsItemBD
{
public:
    explicit MenuBar(qreal maxWidthOrHeight,
            QPointF centerPosition,
            GraphicItemsBD::ORIENTATION orientation = GraphicItemsBD::ORIENTATION::HORIZONTAL,
            bool m_centerFromEnd = false,
            QGraphicsItem* parent = nullptr);
    virtual ~MenuBar() override;

    void closeOnMouseExit(std::function<void()> closeCaller);

    virtual void setPosition(QPoint position) override;

    virtual QRectF boundingRect() const override;
    void setOrientation(GraphicItemsBD::ORIENTATION orientation);
    void revalidateSize();

    void setCaller(std::shared_ptr<DynamicFunctionCaller<QString, std::function<void()>>> btnFunctions,
                   std::shared_ptr<DynamicFunctionCaller<QString, std::function<void(QPainter*, QRectF)>>> buttonPaintingFunctions,
                   std::function<int(int)> groupingFunc = nullptr);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    GraphicItemsBD::ORIENTATION orientation();
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
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

    void resetMouseTrackerVectors();

//    ------------------------------------------------------

    int m_anzRows = 1;
    int m_anzColumns = 1;
    qreal m_btnEdge = 45.0;
    qreal m_padding = 3.0;
    qreal m_offsets = 2.0;
    qreal m_maxWidthOrHeight = 0.0;
    QPointF m_centerOrientation;
    bool m_centerFromEnd = false;
    int m_btnsCount = 0;
    std::vector<QRectF> m_buttons = std::vector<QRectF>();
    std::vector<bool> m_mouInBtns = std::vector<bool>();
    std::vector<bool> m_mouPrsdBtns = std::vector<bool>();

    QColor mouPrsdCol1 = QColor(200, 255, 200, 255);
    QColor mouPrsdCol2 = QColor(100, 180, 100, 255);
    QColor mouInCol1 = QColor(255, 255, 255, 255);
    QColor mouInCol2 = QColor(100, 255, 100, 255);
    QColor backgrCol1 = QColor(255, 255, 255, 255);
    QColor backgrCol2 = QColor(255, 255, 255, 255);

    std::shared_ptr<DynamicFunctionCaller<QString, std::function<void()>>> m_btnFuncs = std::shared_ptr<DynamicFunctionCaller<QString, std::function<void()>>>();
    std::shared_ptr<DynamicFunctionCaller<QString, std::function<void(QPainter*, QRectF)>>> m_paintingFuncs = std::shared_ptr<DynamicFunctionCaller<QString, std::function<void(QPainter*, QRectF)>>>();
    std::function<int(int)> m_groupingFunc = nullptr;

    std::vector<std::pair<QPointF,QPointF>> m_groupSeparators = std::vector<std::pair<QPointF,QPointF>>();

    QString vec_to_str(const std::vector<int>& vec);

    std::map<int, std::vector<int>> m_groupingMap;

    int m_groupingBorder = 2;
    int m_groupingPadding = 1;

    qint64 lastTmePrsd = Q_INT64_C(0);

    GraphicItemsBD::ORIENTATION m_orientation = GraphicItemsBD::ORIENTATION::HORIZONTAL;

    bool m_closeOnMouseExit = false;
    std::function<void()> m_closeCaller = nullptr;
};

#endif // MENUBAR_H
