#ifndef REMOVEWINDOWDIALOG_H
#define REMOVEWINDOWDIALOG_H

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QPaintEvent>
#include <QPainter>
#include <QDialog>
#include <QString>
#include <QFile>
#include <QDataStream>

#include <memory>
#include <functional>
#include <vector>
#include <sstream>

#include "orientation.h"
#include "staticfunctions.h"

class RemoveWindowDialog : public QDialog
{
public:
    RemoveWindowDialog(int anzRects,
               Orientation::ORIENTATION orientation,
               QSize size,
               QPoint position,
               QWidget* parent = nullptr);
    ~RemoveWindowDialog();
    QRectF boundingRect();

    void setSize(const QSize &size);
    void setPosition(const QPoint& position);
signals:
    void removeWindow(int);
protected:
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
private:
    void setRectsPosition(QRect* rect, const QPoint& pos);

    bool m_hover = false;

    bool m_horizontal;

    int m_anzRects = 2;

    bool isPressed = false;
    qint64 lastTmePrsd = Q_INT64_C(0);

    QVector<QRect*> rects;
    int mouseRectId = -1;
};

#endif // REMOVEWINDOWDIALOG_H
