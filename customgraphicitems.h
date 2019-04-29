#ifndef CUSTOMGRAPHICITEMS_H
#define CUSTOMGRAPHICITEMS_H

#include <QObject>
#include <QObject>
#include <QDebug>
#include <QString>
#include <QGraphicsItem>

#include <functional>
#include <memory>

#include "staticfunctions.h"

namespace GraphicItemsBD
{
    enum ORIENTATION{
        VERTICAL,
        HORIZONTAL
    };

    class GraphicsItemBD : public QGraphicsItem{
    public:
        explicit GraphicsItemBD(const QSize& size = QSize(0,0),
                       const QPoint& pos = QPoint(0,0),
                       QGraphicsItem* parent = nullptr);
        virtual ~GraphicsItemBD() override;

        virtual void setSize(QSize size);
        virtual void setPosition(QPoint position);
        virtual QRectF boundingRect() const override;

        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    protected:
        QSize m_size;
        QPoint m_pos;

        bool revalidate = true;
    };

    class ButtonBD : public GraphicsItemBD{
    public:
        explicit ButtonBD(const QSize& size,
                 const QPoint& pos,
                 const QColor& gradCol1,
                 const QColor& gradCol2,
                 const QColor& selectionCol1 = QColor(200,255,200),
                 const QColor& selectionCol2 = QColor(200,255,200),
                 QGraphicsItem* parent = nullptr);
        virtual ~ButtonBD() override;

        virtual void revalidateSize(float width, float height);

        void setCallFunction(std::function<void()> callFunc);

        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
        virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
        virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
        virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
        virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * event) override;

        void setClickColourEnabled(bool enabled);
//    signals:
//        void clicked();
    private:
        QColor m_gradCol1;
        QColor m_gradCol2;
        QColor m_selectCol1;
        QColor m_selectCol2;
//        QColor m_clickCol1 = QColor(200,255,200, 255);
//        QColor m_clickCol2 = QColor(100,255,100, 255);

        bool mouIn = false;
        bool mouClick = false;
        bool clickColourEnabled = true; // if false, there is no visual feedback when the user clicks the ButtonBD (sometimes needed to avoid a frozen clicked button...)
        std::function<void()> m_callFunc;

        qint64 lastTmePrsd = QDateTime::currentMSecsSinceEpoch();
    };

    class TextRect : public ButtonBD{
    public:
        explicit TextRect(QString str,
                         const QSize& size,
                         const QPoint& pos,
                         const QColor& gradCol1,
                         const QColor& gradCol2,
                         const QColor& selectionCol1 = QColor(200,255,200, 255),
                         const QColor& selectionCol2 = QColor(200,255,200, 255),
                         const QColor& textCol = QColor(0,0,0),
                         QGraphicsItem* parent = nullptr);

        virtual ~TextRect() override;

        void setFont(const QFont& font);
        QFont font() const;

        void setText(QString str);
        void setTextColor(QColor textCol);

        void setPadding(int left, int right, int top, int bottom);
        void setBorderColor(QColor col);

        void revalidateSize(float paddingX = 2, float paddingY = 2) override;

        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    private:
        QString m_str;
        QColor m_textCol;
        QFont m_font;
        QVector<int> m_padding;
        QColor m_boderCol = QColor(0,0,0, 100);
    };

    class PixmapRect : public ButtonBD{
    public:
        explicit PixmapRect(QPixmap pixmap,
                 const QSize& size,
                 const QPoint& pos,
                 const QColor& selGradient1 = QColor(255,255,255, 255),
                 const QColor& selGradient2 = QColor(255,255,255, 255),
                 const QColor& selectionCol1 = QColor(255,255,255),
                 const QColor& selectionCol2 = QColor(255,255,255),
                 QGraphicsItem* parent = nullptr);
        virtual ~PixmapRect() override;

        void setPixmap(QPixmap pixmap);

        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    private:
        QPixmap m_pixmap;
    };

    class CustomShapeRect : public ButtonBD{
    public:
        explicit CustomShapeRect(std::function<void(QPainter*, const QRectF&)> paintFunction,
                 const QSize& size,
                 const QPoint& pos,
                 const QColor& selGradient1 = QColor(255,255,255, 255),
                 const QColor& selGradient2 = QColor(255,255,255, 255),
                 const QColor& selectionCol1 = QColor(255,255,255),
                 const QColor& selectionCol2 = QColor(255,255,255),
                 QGraphicsItem* parent = nullptr);
        virtual ~CustomShapeRect() override;

        void setPaintFunction(std::function<void (QPainter*, const QRectF&)> paintFunction);

        virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    private:
        std::function<void(QPainter*, const QRectF&)> m_paintFunction;
    };

    class IconAndLabelItem : public GraphicsItemBD{
    public:
        explicit IconAndLabelItem(const QString& pixmapPath,
                                  bool loadAsFilIcon,
                                  const QString& labelTxt,
                                   const QSize& size = QSize(0,0),
                                   const QPoint& pos = QPoint(0,0),
                                   QGraphicsItem* parent = nullptr);
        virtual ~IconAndLabelItem() override;

        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
        void setSelected(bool selected);
        bool selected();
        void setOnClicked(std::function<void()> onClcked);
        void setSelectable(bool selectable);
        void setTextColor(QColor textCol);
    protected:
        virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
        virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
        virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
        virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * event) override;
    private:
        QString m_pixmapPath;
        bool m_loadAsFileIcon;
        QString m_labelTxt;

        int m_paddingX = 10;
        int m_paddingY = 0;

        bool m_selectable = true;

        bool m_selected = false;
        bool m_hover = false;

        QColor m_textCol = QColor(0,0,0, 255);
        QPixmap m_pixmap;

        std::function<void()> m_onClicked = nullptr;
    };
}

#endif // CUSTOMGRAPHICITEMS_H
