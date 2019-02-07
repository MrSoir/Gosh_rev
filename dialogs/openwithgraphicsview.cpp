#include "openwithgraphicsview.h"


OpenWithGraphicsView::OpenWithGraphicsView(std::string fileToOpenName,
                                           std::string fileToOpenPath,
                                           std::vector<AppInfo> appInfos,
                                           QWidget *parent)
    : QGraphicsView(parent),
      m_appInfos(appInfos),
      m_fileToOpenName(fileToOpenName),
      m_fileToOpenPath(fileToOpenPath),
      items(QVector<GraphicItemsBD::IconAndLabelItem*>())
{
    this->setScene(&m_scene);

    this->setAlignment(Qt::AlignTop);

    this->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );

    repaint();
}

OpenWithGraphicsView::OpenWithGraphicsView(QFileInfo fi,
                                           std::vector<AppInfo> appInfos,
                                           QWidget *parent)
    : QGraphicsView(parent),
      m_appInfos(appInfos),
      m_fileToOpenName(fi.fileName().toStdString()),
      m_fileToOpenPath(fi.absoluteFilePath().toStdString()),
      items(QVector<GraphicItemsBD::IconAndLabelItem*>())
{
    this->setScene(&m_scene);

    this->setAlignment(Qt::AlignTop);

    this->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );

    repaint();
}

//----------------------------

OpenWithGraphicsView::~OpenWithGraphicsView()
{
    m_scene.clear();
    items.clear();
}

//----------------------------

void OpenWithGraphicsView::repaint()
{
    m_scene.clear();

    int paddingX = 10;
    int paddingY = 5;

    int yOffs = 10;

    GraphicItemsBD::IconAndLabelItem* itm =
            new GraphicItemsBD::IconAndLabelItem(QString::fromStdString(m_fileToOpenPath),
                                                 true,
                                                 QString::fromStdString(m_fileToOpenName),
                                                 QSize(this->viewport()->width(),m_headinRectSize.height()),
                                                 QPoint(paddingX, yOffs));
//    itm->setTextColor(QColor(255,255,255, 255));
    itm->setSelectable(false);

    m_scene.addItem(itm);
    yOffs += itm->boundingRect().height();

    auto* infoItm = new QGraphicsTextItem("choose an app from the following list:");
    infoItm->setFont(StaticFunctions::getGoshFont());
    auto infoTxtXOffs = static_cast<int>( (this->width() - infoItm->boundingRect().width()) * 0.5 );
    infoItm->setPos(infoTxtXOffs, yOffs);
    m_scene.addItem(infoItm);
    yOffs += infoItm->boundingRect().height() + paddingY;

    QGraphicsRectItem* separator = genSeparator(yOffs);
    m_scene.addItem(separator);

    yOffs += m_separatorHeight*2;

    for(std::size_t i=0; i < m_appInfos.size(); ++i)
    {
        paintHandler(i, yOffs);
        yOffs += m_rowHeight + paddingY;
    }
}

QGraphicsRectItem* OpenWithGraphicsView::genSeparator(int yOffs)
{
    QRectF rct(0,yOffs, this->viewport()->width(), m_separatorHeight);
    QGraphicsRectItem* separator = new QGraphicsRectItem(rct);
    separator->setBrush(QBrush(QColor(0,0,0, 255)));
    return separator;
}

void OpenWithGraphicsView::paintHandler(std::size_t i, int yOffs)
{
    if(i >= m_appInfos.size())
        return;

    QString iconPath = QString::fromStdString(m_appInfos[i].icon_file_path);
    QString appName  = QString::fromStdString(m_appInfos[i].name);

    int int_i = static_cast<int>(i);

    GraphicItemsBD::IconAndLabelItem* itm =
            new GraphicItemsBD::IconAndLabelItem(iconPath,
                                                 false,
                                                 appName,
                                                 QSize(this->viewport()->width(),m_rowHeight),
                                                 QPoint(0, yOffs));

    m_scene.addItem(itm);
    items.push_back( itm );

    itm->setOnClicked([=](){
        deselectAll();
        itm->setSelected(true);
        m_selectedItem = int_i;
        emit clicked(int_i);
    });
}

void OpenWithGraphicsView::deselectAll()
{
    for(int i=0; i < items.size(); ++i)
    {
        items[i]->setSelected(false);
    }
}
