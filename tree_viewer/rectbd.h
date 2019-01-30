#ifndef RECTBD_H
#define RECTBD_H

#include <QGraphicsRectItem>
#include <QColor>
#include <QBrush>

class RectBD: public QGraphicsRectItem
{
public:
    RectBD(int x, int y, int width, int height);
};

#endif // RECTBD_H
