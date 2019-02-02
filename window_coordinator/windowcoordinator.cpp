#include "windowcoordinator.h"

WindowCoordinator::WindowCoordinator(QVector<QDir> initPaths,
                                     QObject *parent)
    : QObject(parent),
      WidgetCreator(),

      m_windowCounter(0),
      m_maxWindows(4),
      m_windows(QVector<FileManager*>()),
      m_orientation(Orientation::ORIENTATION::VERTICAL),
      m_initPath(QStandardPaths::standardLocations(QStandardPaths::StandardLocation::DocumentsLocation)),
      m_curFocusedRootPath(QString("")),
      m_starup_res_path(QString("%1%2%3").arg("resources").arg(QDir::separator()).arg("startup.res"))
{
    if(initPaths.size() > 0)
    {
        for(int i=0; i < initPaths.size() && i < m_maxWindows; ++i)
            addWindowHelper( initPaths[i] );
    }
    else
        addWindow();
}

void WindowCoordinator::close()
{
    for(auto* window: m_windows)
    {
        disconnectFileManager(window);
        window->close();
    }
    m_windows.clear();
}


WindowCoordinator::~WindowCoordinator()
{
    qDebug() << "~WindowCoordinator";
}

QDir WindowCoordinator::getCurrentFocusedDir() const
{
    return m_curFocusedRootPath;
}

QWidget* WindowCoordinator::createWidget()
{
    WindowCoordinatorPane* pane = new WindowCoordinatorPane(this);
    connectWCPane(pane);
    return pane;
}

QVector<QWidget*> WindowCoordinator::createCentralWidgets()
{
    QVector<QWidget*> centralWidgets;
    for(auto* window: m_windows)
    {
        centralWidgets.push_back(window->createWidget());
    }
    return centralWidgets;
}

Orientation::ORIENTATION WindowCoordinator::getOrientation()
{
    return m_orientation;
}

void WindowCoordinator::removeWindow(int id)
{
    if(id < m_windowCounter && id >= 0 && m_windows.size() > 1){

        if(m_windows[id]){
            disconnectFileManager(m_windows[id]);
            m_windows[id]->close();
        }

        m_windows.remove(id);
        --m_windowCounter;

        emit revalidateWCPane();
    }
}

void WindowCoordinator::addWindow()
{
    QString initDirPath = m_curFocusedRootPath;

    if(    (initDirPath.isEmpty() || !QFileInfo(initDirPath).exists())
         && m_initPath.size() > 0 && QFileInfo(m_initPath[0]).exists() )
    {
        initDirPath = m_initPath[0];
    }

    addWindowHelper( QDir(initDirPath) );
}
void WindowCoordinator::addWindowHelper(QDir initPath)
{
    if(m_windowCounter++ < m_maxWindows){

        while(m_windows.size() < m_windowCounter){
            FileManager* fm = new FileManager(initPath.absolutePath().toStdString());

            m_curFocusedRootPath = initPath.absolutePath();

            connectFileManager(fm);

            m_windows.append(fm);
        }

        revalidateWCPane();
    }
}

void WindowCoordinator::connectWCPane(WindowCoordinatorPane* pane)
{
    if(pane)
    {
        connect(this, &WindowCoordinator::revalidateWCPane,      pane, &WindowCoordinatorPane::revalidateLayout);

        connect(pane, &WindowCoordinatorPane::addWindow,         this, &WindowCoordinator::addWindow);
        connect(pane, &WindowCoordinatorPane::removeWindow,      this, &WindowCoordinator::removeWindow);
        connect(pane, &WindowCoordinatorPane::changeOrientation, this, &WindowCoordinator::changeOrientation);
    }
}
void WindowCoordinator::disconnectWCPane(WindowCoordinatorPane* pane)
{
    if(pane)
    {
        disconnect(this, &WindowCoordinator::revalidateWCPane,      pane, &WindowCoordinatorPane::revalidateLayout);

        disconnect(pane, &WindowCoordinatorPane::addWindow,         this, &WindowCoordinator::addWindow);
        disconnect(pane, &WindowCoordinatorPane::removeWindow,      this, &WindowCoordinator::removeWindow);
        disconnect(pane, &WindowCoordinatorPane::changeOrientation, this, &WindowCoordinator::changeOrientation);
    }
}
void WindowCoordinator::connectFileManager(FileManager* fm)
{
    connect(fm, &FileManager::includeHiddenFiles_SGNL,   this, &WindowCoordinator::includeHiddenFiles);
    connect(fm, &FileManager::excludeHiddenFiles_SGNL,   this, &WindowCoordinator::excludeHiddenFiles);

    connect(fm, &FileManager::requestFocusSGNL,         this, &WindowCoordinator::emitLabelChanged);

//    connect(this, &WindowCoordinator::includeHiddenFiles_SGNL,   fm, &FileManager::setIncludeHiddenFiles);
}
void WindowCoordinator::disconnectFileManager(FileManager* fm)
{
    disconnect(fm, &FileManager::includeHiddenFiles_SGNL,   this, &WindowCoordinator::includeHiddenFiles);
    disconnect(fm, &FileManager::excludeHiddenFiles_SGNL,   this, &WindowCoordinator::excludeHiddenFiles);

    disconnect(fm, &FileManager::requestFocusSGNL,         this, &WindowCoordinator::emitLabelChanged);

//    disconnect(this, &WindowCoordinator::includeHiddenFiles_SGNL,   fm, &FileManager::setIncludeHiddenFiles);
}

void WindowCoordinator::changeOrientation()
{
    m_orientation = (m_orientation == Orientation::ORIENTATION::VERTICAL) ?
                    Orientation::ORIENTATION::HORIZONTAL :
                    Orientation::ORIENTATION::VERTICAL;
    revalidateWCPane();
}



void WindowCoordinator::setIncludeHiddenFiles(bool inclHdnFls)
{
    emit includeHiddenFiles_SGNL(inclHdnFls);
}

void WindowCoordinator::includeHiddenFiles()
{
    emit includeHiddenFiles_SGNL(true);
}

void WindowCoordinator::excludeHiddenFiles()
{
    emit includeHiddenFiles_SGNL(false);
}

void WindowCoordinator::emitLabelChanged(QDir newLabel)
{
    m_curFocusedRootPath = newLabel.absolutePath();
    emit labelChanged(newLabel);
}
void WindowCoordinator::focusedWindowChanged(QDir dir)
{
    emitLabelChanged(dir);
}
