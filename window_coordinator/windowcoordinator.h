#ifndef WINDOWCOORDINATOR_H
#define WINDOWCOORDINATOR_H

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QDialog>
#include <QString>
#include <QFile>
#include <QDataStream>
#include <QDir>
#include <QVector>
#include <QStandardPaths>

#include <memory>
#include <functional>
#include <vector>
#include <sstream>

#include "orientation.h"
#include "windowcoordinatorpane.h"
#include "file_handling_src/filemanager.h"

using namespace Orientation;

class WindowCoordinator : public QObject
{
    Q_OBJECT
public:
    explicit WindowCoordinator(QVector<QDir> initPaths = QVector<QDir>(),
                               QObject *parent = nullptr);
    ~WindowCoordinator();

    QDir getCurrentFocusedDir() const;
signals:
    void paneCreated(QWidget* pane);
    void openFoldersInNewTab(QVector<QDir>);
    void labelChanged(QDir);

    void revalidateWCPaneSGNL(QVector<QLayout*> windowsLayouts, ORIENTATION orientation);

    void includeHiddenFiles(bool includeHiddenFiles);

    void resetWCPaneFramesSGNL(QVector<QWidget*>);

    void orientationChanged(ORIENTATION);
public slots:
    void revalidateWCPane();
    void createPane();
    void close();

    void removeWindow(int id = -1);
    void addWindow();

    void changeOrientation();

    void genWindowCoordinatorPane(QWidget*);
    void focusedWindowChanged(QDir dir);

    void setIncludeHiddenFiles(bool includeHiddenFiles);

    void resetWCPaneFrames();
private:
    void addWindowHelper(QDir initDir);

    void emitLabelChanged(QDir newLabel);

    int m_windowCounter;
    int m_maxWindows;

    void connectWCPane();
    void disconnectWCPane();

    void connectFileManager(FileManager* fm);
    void disconnectFileManager(FileManager* fm);

    QVector<FileManager*> m_windows;

    Orientation::ORIENTATION m_orientation;

    QStringList m_initPath;
    QDir m_curFocusedRootPath;

    QString m_starup_res_path;

    WindowCoordinatorPane* m_windowCoordPane;
};

#endif // WINDOWCOORDINATOR_H
