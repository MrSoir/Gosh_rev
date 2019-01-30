#ifndef RECTCOLOR_H
#define RECTCOLOR_H


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

struct RectColor{
public:
    RectColor();

    void reset();

    void prepareForNextRound();
    void nextCurrentColor();
    void nextColor();

    void genNextColor(QColor& col, int* id, int offs, int minColVal, int maxColVal, bool* appreciating);

    const QColor getCurrentColor();
private:
    int minColVal = 0;
    int maxColVal = 255;
    int curId = 0;
    int id = 0;
    int offs = 5;
    bool curAppreciating = true;
    bool appreciating = true;

    QColor rectColor;
    QColor curRctCol;
};

#endif // RECTCOLOR_H
