#ifndef OPENWITHGRAPHICSVIEW_H
#define OPENWITHGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWidget>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QObject>
#include <QLabel>

#include <string>
#include <vector>

#include "customgraphicitems.h"

#include "file_handling_src/appinfo.h"

class OpenWithGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit OpenWithGraphicsView(std::string fileToOpenName,
                                  std::string fileToOpenPath,
                                  std::vector<AppInfo> appInfos,
                                  QWidget* parent = nullptr);
    explicit OpenWithGraphicsView(QFileInfo fi,
                                  std::vector<AppInfo> appInfos,
                                  QWidget* parent = nullptr);
    virtual ~OpenWithGraphicsView() override;

signals:
    void clicked(int id);
private:

    void repaint();

    QGraphicsRectItem* genSeparator(int yOffs);
    void paintHandler(std::size_t i, int yOffs);

    void deselectAll();

    QGraphicsScene m_scene;

    int m_selectedItem = 0;

    int m_eadingRowHeight = 60;
    int m_rowHeight = 40;

    std::vector<AppInfo> m_appInfos;

    QSize m_headinRectSize = QSize(60,60);
    QSize m_iconSize = QSize(m_rowHeight,m_rowHeight);

    std::string m_fileToOpenName;
    std::string m_fileToOpenPath;

    int m_separatorHeight = 3;

    QVector<GraphicItemsBD::IconAndLabelItem*> items;
};

#endif // OPENWITHGRAPHICSVIEW_H
