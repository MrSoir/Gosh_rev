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
#include <QStringList>

#include <memory>
#include <functional>
#include <vector>
#include <sstream>

#include "orientation.h"
#include "windowcoordinatorpane.h"
#include "file_handling_src/filemanager.h"

#include "widgetcreator.h"

class WindowCoordinator : public QObject,
                          public WidgetCreator
{
    Q_OBJECT
public:
    explicit WindowCoordinator(QVector<QDir> initPaths = QVector<QDir>(),
                               QObject *parent = nullptr);
    virtual ~WindowCoordinator() override;

    QDir getCurrentFocusedDir() const;

    virtual QWidget* createWidget() override;

    QVector<QWidget*> createCentralWidgets();
    Orientation::ORIENTATION getOrientation();

signals:
    void revalidateWCPane();
    void openFoldersInNewTab(QVector<QDir>);
    void labelChanged(QDir);

    void includeHiddenFiles_SGNL(bool includeHiddenFiles_SGNL);

//    void orientationChanged(Orientation::ORIENTATION);
public slots:
    void close();

    void removeWindow(int id = -1);
    void addWindow();

    void changeOrientation();

    void focusedWindowChanged(QDir dir);

    void setIncludeHiddenFiles(bool includeHiddenFiles_SGNL);
    void includeHiddenFiles();
    void excludeHiddenFiles();
private:
    void addWindowHelper(QDir initDir);

    void emitLabelChanged(QDir newLabel);

    int m_windowCounter;
    int m_maxWindows;

    void connectWCPane(WindowCoordinatorPane* pane);
    void disconnectWCPane(WindowCoordinatorPane* pane);

    void connectFileManager(FileManager* fm);
    void disconnectFileManager(FileManager* fm);

    QVector<FileManager*> m_windows;

    Orientation::ORIENTATION m_orientation;

    QStringList m_initPath = QStandardPaths::standardLocations(QStandardPaths::StandardLocation::DocumentsLocation);
    QDir m_curFocusedRootPath;

    QString m_starup_res_path;};

#endif // WINDOWCOORDINATOR_H
