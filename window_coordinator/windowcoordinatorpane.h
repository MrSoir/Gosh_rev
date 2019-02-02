#ifndef WINDOWCOORDINATORPANE_H
#define WINDOWCOORDINATORPANE_H

#include <QObject>
#include <QWidget>
#include <QList>
#include <QString>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QPaintEvent>
#include <QPainter>
#include <QDialog>
#include <QFile>
#include <QDataStream>
#include <QResizeEvent>

#include <memory>
#include <functional>
#include <vector>
#include <sstream>

#include "orientation.h"

#include "orientation.h"

#include "staticfunctions.h"
#include "removewindowdialog.h"
#include "helpdialog.h"
#include "infodialog.h"

class WindowCoordinator;

using namespace Orientation;

class WindowCoordinatorPane : public QWidget
{
    Q_OBJECT
public:
    explicit WindowCoordinatorPane(WindowCoordinator* wc,
                                   QList<QList<int>> splitterRatios = QList<QList<int>>(),
                                   int windowCount = 2,
                                   Orientation::ORIENTATION orientation = Orientation::ORIENTATION::HORIZONTAL,
                                   QWidget *parent = nullptr);

    virtual ~WindowCoordinatorPane() override;

signals:
    void addWindow();
    void removeWindow(int);
    void changeOrientation();
    void splitterRatiosChangedSGNL(QList<QList<int>>);

    void requestFullScreen();

    void setRemoveDialogPosition(QPoint);
    void setRemoveDialogSize(QSize);
public slots:
    void revalidateLayout();
//    void resetWindows(QVector<QWidget*> windows);
//    void orientationChanged(ORIENTATION o);
    void splitterRatiosChanged();

    void showRemoveWindowDialog();
    void showInfoDialog();
    void showHelpDialog();
protected:
    void paintEvent(QPaintEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;
private:
    void clearSplitter();

    QList<QList<int>> generateSplitterRatios();

    void resetMainLayout();
    void deleteMainLayout();

    void setToolBar();

//    -------------------------------------------------

    WindowCoordinator* m_windowCoordinator;

    QHBoxLayout* m_toolBar = nullptr;
    QVector<QWidget*> m_toolBarChilds;

    QSize TOOLBAR_ICON_SIZE = QSize(40,40);

    QVBoxLayout* m_vBox = nullptr;

    std::vector<QSplitter*> m_splitter;
    QList<QList<int>> m_splitterRatios;

    float m_dialogSizeFactor = 0.8f;

    Orientation::ORIENTATION m_orientation;
    int m_windowCount;

    QSize m_removeDialogSize;
};

#endif // WINDOWCOORDINATORPANE_H
