#ifndef TABCOORDINATORPANE_H
#define TABCOORDINATORPANE_H

#include <QWidget>
#include <QVector>
#include <QDir>
#include <QString>
#include <QDebug>

class TabCoordinatorPane : public QWidget
{
    Q_OBJECT
public:
    explicit TabCoordinatorPane(int focused_id,
                                QVector<QDir> labels,
                                QWidget *parent = nullptr);
signals:
    void tabClicked(int id);
    void tabCloseClicked(int id);
    void tabAddClicked();
public slots:
    void revalidate();
    void updateTabLabels();
    void activeTabIdChanged(int id);
    void setCentralWidget(QWidget* widget);
};

#endif // TABCOORDINATORPANE_H
