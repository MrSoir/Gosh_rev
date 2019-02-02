#ifndef TABCOORDINATORPANE_H
#define TABCOORDINATORPANE_H

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QPaintEvent>
#include <QPainter>
#include <QDialog>
#include <QString>
#include <QVector>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QKeyEvent>
#include <QDebug>

#include <memory>
#include <functional>
#include <vector>
#include <sstream>
#include <string>

#include "tabselectorpane.h"
#include "staticfunctions.h"

class TabCoordinator;
class TabSelectorPane;

class TabCoordinatorPane : public QWidget
{
    Q_OBJECT
public:
    explicit TabCoordinatorPane(TabCoordinator* tc,
                                int focused_id,
                                QVector<QDir> labels,
                                QWidget *parent = nullptr);
    virtual ~TabCoordinatorPane() override;

signals:
    void tabClicked(int id);
    void tabCloseClicked(int id);
    void tabAddClicked();

    void setFullScreen();

    // TabCoordinatorPane -> TabSelectorPane:
    void updateTabLabelsSGNL(QVector<QDir> tabLabels);
    void activeTabIdChangedSGNL(int id);
    void revalidateTabSelectorPane();

public slots:
    void revalidate();
    void updateTabLabels(QVector<QDir> tabLabels);
    void activeTabIdChanged(int id);
protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setCentralWidget();

    void revalidateMainWidget();

    void clearLayout();
    void deleteMainLayout();

    void setTabBar();

//    --------------------------------------

    TabCoordinator* m_tabCoordinator;

    QWidget* m_mainWidget;

    QVBoxLayout* m_mainLayout;

    QVector<QDir> m_tabLabels;

    int m_curActiveTabId = 0;

    TabSelectorPane* m_tabPane;
    int m_tabPaneScrollOffsetX;
};

#endif // TABCOORDINATORPANE_H
