#include "elapsemenubd.h"

ElapseMenuBD::ElapseMenuBD(int buttonCount,
             int columnWidth,
             const QSize& size,
             const QPoint& pos,
             QGraphicsItem* parent)
    : GraphicItemsBD::GraphicsItemBD(size, pos, parent),
      m_colWidth(columnWidth),
      m_buttonCount(buttonCount)
{}

ElapseMenuBD::~ElapseMenuBD(){
    m_caller.reset();
}
void ElapseMenuBD::setCaller(std::shared_ptr<DynamicFunctionCaller<QString, std::function<bool(int)>>> caller){
    this->m_caller = caller;
}

void ElapseMenuBD::setBackroundColor(QColor grad1, QColor grad2)
{
    m_grad1 = grad1;
    m_grad2 = grad2;
    update();
}

void ElapseMenuBD::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(widget)
    Q_UNUSED(option)

    // gaaanz wichtig: erstmal painter an boundingRect clippen!!!:
    QRectF rct = boundingRect();

    painter->setRenderHint(QPainter::Antialiasing, true);

    painter->setClipRect(rct);

    StaticFunctions::paintRect(painter, rct, m_grad1, m_grad2);

    if(revalidate){
        int xOffs = m_pos.x();
        int yOffs = m_pos.y();
        int buttonHeight = m_size.height();
        int buttonWidth = m_colWidth;
        for(int i=0; i < m_buttonCount; i++){
            QRectF rect(xOffs, yOffs, buttonWidth, buttonHeight);
            m_buttons.append(rect);
            xOffs += buttonWidth;
        }
    }
    QColor basicColor1 = QColor(255, 255, 255, 255),
           basicColor2 = QColor(200, 0, 0, 255),
           hoverColor1 = QColor(255, 0, 0, 255),
           hoverColor2 = QColor(150, 0, 0, 255),
           elapsedColor1 = QColor(255, 255, 255, 255),
           elapsedColor2 = QColor(0, 150, 0, 255),
           hoverElapsedColor1 = QColor(255, 255, 255, 255),
           hoverElapsedColor2 = QColor(0, 100, 0, 255);
    QColor col1, col2;
    int fontSize, fontWeight;
    for(int i=0; i < m_buttons.size(); i++){
        if(m_mouInButtonId == i){
            if(m_caller->containsFunction(QString("elapsed")) &&
                                 m_caller->getFunction(QString("elapsed"))(i)) {
                col1 = hoverElapsedColor1;
                col2 = hoverElapsedColor2;
            }else{
                col1 = hoverColor1;
                col2 = hoverColor2;
            }
            fontSize = 9;
            fontWeight = QFont::Bold;
        }else{
            if(m_caller->containsFunction(QString("elapsed")) &&
                 m_caller->getFunction(QString("elapsed"))(i)) {
                col1 = elapsedColor1;
                col2 = elapsedColor2;
            }else{
                col1 = basicColor1;
                col2 = basicColor2;
            }
            fontSize = 9;
            fontWeight = QFont::Normal;
        }
        StaticFunctions::paintTextRect(painter,
                                       QString("%1").arg(i+1),
                                       m_buttons[i], col1, col2,
                                       QColor(255,255,255,255),
                                       StaticFunctions::getGoshFont(fontSize, fontWeight));
    }

    revalidate = false;

//        painter->end();
}

void ElapseMenuBD::mousePressEvent(QGraphicsSceneMouseEvent *event){
    QPointF mouP = event->pos();
    for(int i=0; i < m_buttons.size(); i++){
        if(m_buttons[i].contains(mouP) && m_caller->containsFunction(QString("call"))){
            m_caller->getFunction(QString("call"))(i);
        }
    }
    qint64 curTime = QDateTime::currentMSecsSinceEpoch();
    if(curTime - lastTmePrsd < 300){

    }
    lastTmePrsd = curTime;
    update();
}
void ElapseMenuBD::hoverMoveEvent(QGraphicsSceneHoverEvent * event){
    QPointF mouP = event->pos();
    int hoverId = -1;
    for(int i=0; i < m_buttons.size(); i++){
        if(m_buttons[i].contains(mouP)){
            hoverId = i;
            break;
        }
    }
    if(hoverId != m_mouInButtonId){
        m_mouInButtonId = hoverId;
        update();
    }
//        return QGraphicsItem::hoverEnterEvent(event);
}
void ElapseMenuBD::hoverLeaveEvent(QGraphicsSceneHoverEvent* event){
    m_mouInButtonId = -1;
    update();
}
