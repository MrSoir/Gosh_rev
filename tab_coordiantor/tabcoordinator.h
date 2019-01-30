#ifndef TABCOORDINATOR_H
#define TABCOORDINATOR_H

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QPaintEvent>
#include <QPainter>
#include <QDialog>
#include <QFile>
#include <QDataStream>
#include <QLayoutItem>
#include <QLayout>
#include <QDebug>

#include <QDir>
#include <QString>
#include <QVector>

#include <vector>
#include <string>

#include "tabcoordinatorpane.h"
#include "window_coordinator/windowcoordinator.h"

class TabCoordinator : public QObject
{
    Q_OBJECT
public:
    explicit TabCoordinator(QObject *parent = nullptr);
    ~TabCoordinator();

    TabCoordinatorPane* genTabCoordinatorPane();
signals:
    void revalidateTabCoordinatorPane();
    void labelsChanged(QVector<QDir> labels);
    void activeTabIdChanged(int id);
    void resetTabCoordPaneCentralWidget();
    void sendCentralWidgetToTCPane(QWidget* wiget);
public slots:
    void setFullScreen();

    void addTab();
    void setTab(int id);
    void removeTab(int id);

    void closeActiveTab();
    void openFoldersInNewTab(QVector<QDir> foldersToOpen);

    void revalidateLabels();

    void receiveCentralWidgetForTCPane(QWidget* widget);
private:
    QVector<QDir> generateLabels();

    void addTabHelper(QVector<QDir> initPaths = QVector<QDir>());

    void deleteWindow(WindowCoordinator* window);
    void connectWindow(WindowCoordinator* window);
    void disconnectWindow(WindowCoordinator* window);

    void connectTCPane();
    void disconnectTCPane();
    void deleteTCPane();

    bool m_isFullScreen = false;

    QVector<WindowCoordinator*> m_windows;

    WindowCoordinator* m_currentlyDisplWindow;
    int m_curWindowId = -1;

    TabCoordinatorPane* m_tcPane;
};

#endif // TABCOORDINATOR_H
