#include "rectcolor.h"

RectColor::RectColor() :
    rectColor(minColVal, minColVal, maxColVal),
    curRctCol(minColVal, minColVal, maxColVal)
{}

void RectColor::reset()
{
    rectColor = QColor(minColVal, minColVal, maxColVal);
    id = 0;
    appreciating = true;

    prepareForNextRound();
}

void RectColor::prepareForNextRound()
{
    curRctCol = rectColor;
    curId = id;
    curAppreciating = appreciating;
}
void RectColor::nextCurrentColor()
{
    genNextColor(curRctCol, &curId, offs, minColVal, maxColVal, &curAppreciating);
}
void RectColor::nextColor()
{
    genNextColor(rectColor, &id   , offs, minColVal, maxColVal, &appreciating   );
}

void RectColor::genNextColor(QColor& col, int* id, int offs, int minColVal, int maxColVal, bool* appreciating)
{
    int val = 0;
    int val_prev = 0;

    switch(*id)
    {
    case 0:
        val = col.red();
        val_prev = col.blue();
        break;
    case 1:
        val = col.green();
        val_prev = col.red();
        break;
    case 2:
        val = col.blue();
        val_prev = col.green();
        break;
    }

    int oldId = *id;

    if( *appreciating )
    {
        val += offs;
        if(val > maxColVal)
        {
            val = maxColVal;
            *appreciating = false;
        }
    }else
    {
        val_prev -= offs;
        if(val_prev < minColVal)
        {
            val_prev = minColVal;
            *id = (*id +1) % 3;
            *appreciating = true;
        }
    }

    switch (oldId)
    {
    case 0:
        col.setRed( val );
        col.setBlue( val_prev );
        break;
    case 1:
        col.setGreen( val );
        col.setRed( val_prev );
        break;
    case 2:
        col.setBlue( val );
        col.setGreen( val_prev );
        break;
    }
}

const QColor RectColor::getCurrentColor()
{
    return curRctCol;
}
