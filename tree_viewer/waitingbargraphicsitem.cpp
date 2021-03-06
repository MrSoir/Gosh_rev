#include "waitingbargraphicsitem.h"


//---------------------------------------------------------------
// WaitingBar:

WaitingBarGraphicsItem::WaitingBarGraphicsItem(const QSize &size, const QPoint &pos,
                                               int rectCount, int padding,
                                               QGraphicsItem* parent)
    : GraphicItemsBD::GraphicsItemBD(size, pos, parent),
      m_rectCount(rectCount),
      m_padding(padding),
      m_animationTimer(new QTimer()),
      m_isRunning(false),
      m_frameRate(20)
{
    QObject::connect(m_animationTimer, &QTimer::timeout, [=](){
        update();
    });
}

WaitingBarGraphicsItem::~WaitingBarGraphicsItem()
{
//    qDebug() << "in WaitingBarGraphicsItem.DEstructor";
    if(m_animationTimer->isActive())
        m_animationTimer->stop();
    delete m_animationTimer;
}

void WaitingBarGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    GraphicItemsBD::GraphicsItemBD::paint(painter, option, widget);

    QRectF rct = boundingRect();
    QRect i_rct = rct.toRect();

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setClipRect(rct);

    int w = i_rct.width();

    int edge = (w - 2*m_padding) / m_rectCount;
    int x = static_cast<int>(m_padding);

    m_rectCol.prepareForNextRound();

    QPen pen(QColor(0,0,0,255));
    pen.setWidthF(0.5);

    for(int i=1; i <= m_rectCount; ++i)
    {
        painter->setPen(pen);
        painter->setBrush( QBrush(m_rectCol.getCurrentColor()) );
        painter->drawRect(QRect(i_rct.left() + x, i_rct.top(), edge, edge));

        m_rectCol.nextCurrentColor();

        x += edge;
    }
    int remainingPart = std::max( i_rct.width() - m_padding - x, 0);
    if( remainingPart > 4)
    {
        painter->setPen(pen);
        painter->setBrush( QBrush(m_rectCol.getCurrentColor()) );
        painter->drawRect(QRect(i_rct.left() + x, i_rct.top(), remainingPart, edge));
    }

    m_rectCol.nextColor();

//    painter->end();
}

void WaitingBarGraphicsItem::startAnimation()
{
    if(!m_isRunning)
    {
        m_rectCol.reset();
        m_animationTimer->start(m_frameRate);
        m_isRunning = true;
    }
}

void WaitingBarGraphicsItem::stopAnimation()
{
    if(m_isRunning)
    {
        m_animationTimer->stop();
        m_isRunning = false;
    }
}

bool WaitingBarGraphicsItem::isRunning()
{
    return m_isRunning;
}
