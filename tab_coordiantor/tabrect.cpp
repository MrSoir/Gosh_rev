#include "tabrect.h"

TabRect::TabRect(const QString &str,
                 const QFont &font,
                 const QSize &size,
                 const QPoint &pos,
                 const QColor &gradCol1,
                 const QColor &gradCol2,
                 const QColor &selectionCol1,
                 const QColor &selectionCol2,
                 const QColor &textCol1, const QColor &textCol2,
                 QGraphicsItem *parent)
    : GraphicsItemBD(size, pos, parent),
      m_gradCol1(gradCol1),
      m_gradCol2(gradCol2),
      m_selectCol1(selectionCol1),
      m_selectCol2(selectionCol2),

      m_str(str),
      m_textCol1(textCol1),
      m_textCol2(textCol2),
      m_font(font)
{
    m_closeRect = QRect(0,0,0,0);
    setAcceptHoverEvents(true);
}

TabRect::~TabRect()
{
    m_onTabClicked   = nullptr;
    m_onCloseClicked = nullptr;
}

void TabRect::setCallbackFunctions(std::function<void()> onTabClicked,
                                   std::function<void()> onCloseClicked)
{
    m_onTabClicked   = onTabClicked;
    m_onCloseClicked = onCloseClicked;
}

void TabRect::setFont(QFont font)
{
    m_font = font;
}

void TabRect::setText(QString str)
{
    m_str = str;
}

void TabRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    GraphicItemsBD::GraphicsItemBD::paint(painter, option, widget);

    painter->setFont(m_font);

    QRectF rct = boundingRect();
    painter->setClipRect(rct);

    painter->setRenderHint(QPainter::Antialiasing, true);

    QColor grad1, grad2;
    if(mouIn)
    {
        grad1 = m_selectCol1;
        grad2 = m_selectCol2;
    }else
    {
        grad1 = m_gradCol1;
        grad2 = m_gradCol2;
    }

    QColor borderCol(0,0,0, 255);
    int bordWidth = 1;
    QLinearGradient gradient = QLinearGradient(rct.topLeft(), rct.bottomRight());
    gradient.setColorAt(0, grad1);
    gradient.setColorAt(1, grad2);
    painter->setBrush(QBrush(gradient));
    painter->setPen(QPen(borderCol, bordWidth));

    QPainterPath path = getBoundingPath();
    painter->setClipPath(path);
    painter->drawPath(path);

    QRectF textRect = getTextBoundingRect();
    painter->setClipRect(textRect);

    painter->setBrush(QBrush(QColor(0,0,0, 255)));
    painter->setPen(QPen(mouIn ? m_textCol2 : m_textCol1));
    painter->drawText(textRect, m_str);

    painter->setClipRect(rct);
    int t_right = static_cast<int>(textRect.right());
    int r_top = static_cast<int>(rct.top());
    int r_height = static_cast<int>(rct.height());
    int r_centY = static_cast<int>(rct.center().y());

    m_closeRect = QRect(t_right,
                        r_top,
                        m_text_paddingXright,
                        r_height);

    QColor crossCol = mouInClos ? m_closeXCol_hover : m_textCol1;
    painter->setPen(QPen(crossCol));
    QPoint crossCenter( t_right + static_cast<int>(m_text_paddingXright*0.5),
                        r_centY );
    paintCross(crossCenter, int(m_text_paddingXright*0.8), int(rct.height()*0.5), painter);
}

void TabRect::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPoint mouP(static_cast<int>(event->pos().x()),
                static_cast<int>(event->pos().y()));

    if( inClose(mouP) )
    {
        if(m_onCloseClicked)
            m_onCloseClicked();
    }else
    {
        if(m_onTabClicked)
            m_onTabClicked();
    }

    revalMousePos(mouP);
    update();
}

void TabRect::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    revalMousePos(event->pos());
    update();
}

void TabRect::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    mouIn = false;
    mouInClos = false;
    update();
}

void TabRect::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    revalMousePos(event->pos());
    update();
}

void TabRect::paintCross(QPoint centerP, int width, int height, QPainter* painter)
{
    painter->save();

    if(mouInClos)
    {
        QPen pen(painter->pen());
        pen.setWidth(3);
        painter->setPen(pen);
    }
    QPoint p1(static_cast<int>(centerP.x() - width*0.5), static_cast<int>(centerP.y() - height*0.5));
    QPoint p2(static_cast<int>(centerP.x() + width*0.5), static_cast<int>(centerP.y() + height*0.5));
    painter->drawLine(p1,p2);

    p1 = QPoint(static_cast<int>(centerP.x() - width*0.5), static_cast<int>(centerP.y() + height*0.5));
    p2 = QPoint(static_cast<int>(centerP.x() + width*0.5), static_cast<int>(centerP.y() - height*0.5));
    painter->drawLine(p1,p2);

    painter->restore();
}

void TabRect::revalMousePos(QPointF mouP)
{
    mouInClos = inClose(mouP);
    mouIn = !mouInClos;
}

QPainterPath TabRect::getBoundingPath() const
{
    QRectF rect = boundingRect();
    QPainterPath path;

    path.moveTo(rect.left() +m_cornerWidth, rect.bottom());

    path.quadTo(rect.left(), rect.center().y(),
                rect.left() +m_cornerWidth, rect.top());

    path.lineTo(rect.right()-m_cornerWidth, rect.top());

    path.quadTo(rect.right(), rect.center().y(),
                rect.right()-m_cornerWidth, rect.bottom());

    path.lineTo(rect.left() +m_cornerWidth, rect.bottom());
    path.closeSubpath();

//    path.moveTo(rect.left(), rect.bottom());
//    path.quadTo(rect.left(), rect.top(),
//                rect.left()+m_cornerWidth, rect.top());
//    path.lineTo(rect.right()-m_cornerWidth, rect.top());
//    path.quadTo(rect.right(), rect.top(),
//                rect.right(), rect.bottom());
//    path.lineTo(rect.left(), rect.bottom());
//    path.closeSubpath();

//    path.moveTo(rect.left()+m_cornerWidth, rect.top());
//    path.lineTo(rect.left(), rect.bottom());
//    path.lineTo(rect.right(), rect.bottom());
//    path.lineTo(rect.right()-m_cornerWidth, rect.top());
//    path.closeSubpath();
    return path;
}

QRect TabRect::getTextBoundingRect() const
{
    QRectF rect = boundingRect();
    QRect i_rect = rect.toRect();
    QRect textRect(i_rect.left() +m_cornerWidth + m_text_paddingXleft,
                   i_rect.top() +m_text_paddingYtop,
                   i_rect.width() - 2*m_cornerWidth - (m_text_paddingXleft + m_text_paddingXright),
                   i_rect.height() - (m_text_paddingYtop + m_text_paddingYbottom));
    return textRect;
}

bool TabRect::inClose(QPoint mouP) const
{
    return m_closeRect.contains(mouP);
}
bool TabRect::inClose(QPointF mouPf) const
{
    QPoint mouP( int(mouPf.x()), int(mouPf.y()) );
    return m_closeRect.contains(mouP);
}
