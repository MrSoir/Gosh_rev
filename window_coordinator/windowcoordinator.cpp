#include "windowcoordinator.h"

WindowCoordinator::WindowCoordinator(QVector<QDir> initPaths,
                                     QObject *parent)
    : QObject(parent),
      m_windowCounter(0),
      m_maxWindows(4),
      m_windows(QVector<FileManager*>()),
      m_orientation(ORIENTATION::VERTICAL),
      m_initPath(QStandardPaths::standardLocations(QStandardPaths::StandardLocation::DocumentsLocation)),
      m_curFocusedRootPath(QDir("")),
      m_starup_res_path(QString("%1%2%3").arg("resources").arg(QDir::separator()).arg("startup.res")),
      m_windowCoordPane(nullptr)
{
    if(initPaths.size() > 0)
    {
        for(int i=0; i < initPaths.size() && i < m_maxWindows; ++i)
            addWindowHelper( initPaths[i] );
    }
    else
        addWindow();
}


void WindowCoordinator::createPane()
{
    if(m_windowCoordPane)
        disconnectWCPane();

    m_windowCoordPane = new WindowCoordinatorPane();
    connectWCPane();

    emit paneCreated(m_windowCoordPane);
}


WindowCoordinator::~WindowCoordinator()
{
    for(int i=0; i < m_windows.size(); i++)
    {
        if(m_windows[i])
            m_windows[i]->close();
    }
    m_windows.clear();
}

void WindowCoordinator::revalidateWCPane()
{
    QVector<QLayout*> frames;
    for(int i=0; i < m_windows.size(); ++i)
    {
        frames.push_back(m_windows[i]->genPane());
    }
    emit revalidateWCPaneSGNL(frames, m_orientation);
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

        revalidateWCPane();
    }
}

void WindowCoordinator::addWindow()
{
    QDir initDir = m_curFocusedRootPath;

    if( initDir.isEmpty() && m_initPath.size() > 0)
    {
        initDir = QDir(m_initPath[0]);
    }

    addWindowHelper(initDir);
}
void WindowCoordinator::addWindowHelper(QDir initPath)
{
    if(m_windowCounter++ < m_maxWindows){

        while(m_windows.size() < m_windowCounter){
            FileManager* fm = new FileManager(initPath.absolutePath().toStdString());

            connectFileManager(fm);

            m_windows.append(fm);
        }

        resetWCPaneFrames();
    }
}

void WindowCoordinator::connectWCPane()
{
    if(m_windowCoordPane)
    {
        connect(this, &WindowCoordinator::revalidateWCPaneSGNL,      m_windowCoordPane, &WindowCoordinatorPane::revalidateLayout);

        connect(m_windowCoordPane, &WindowCoordinatorPane::addWindow,         this, &WindowCoordinator::addWindow);
        connect(m_windowCoordPane, &WindowCoordinatorPane::removeWindow,      this, &WindowCoordinator::removeWindow);
        connect(m_windowCoordPane, &WindowCoordinatorPane::changeOrientation, this, &WindowCoordinator::changeOrientation);
    }
}
void WindowCoordinator::disconnectWCPane()
{
    if(m_windowCoordPane)
    {
        disconnect(this, &WindowCoordinator::revalidateWCPaneSGNL,      m_windowCoordPane, &WindowCoordinatorPane::revalidateLayout);

        disconnect(m_windowCoordPane, &WindowCoordinatorPane::addWindow,         this, &WindowCoordinator::addWindow);
        disconnect(m_windowCoordPane, &WindowCoordinatorPane::removeWindow,      this, &WindowCoordinator::removeWindow);
        disconnect(m_windowCoordPane, &WindowCoordinatorPane::changeOrientation, this, &WindowCoordinator::changeOrientation);
    }
}
void WindowCoordinator::connectFileManager(FileManager* fm)
{
    connect(fm, &FileManager::includeHiddenFiles,   this, &WindowCoordinator::setIncludeHiddenFiles);
    connect(fm, &FileManager::requestFocus,         this, &WindowCoordinator::emitLabelChanged);

    connect(this, &WindowCoordinator::includeHiddenFiles,   fm, &FileManager::setIncludeHiddenFiles);
}
void WindowCoordinator::disconnectFileManager(FileManager* fm)
{
    disconnect(fm, &FileManager::includeHiddenFiles,   this, &WindowCoordinator::setIncludeHiddenFiles);
    disconnect(fm, &FileManager::requestFocus,         this, &WindowCoordinator::emitLabelChanged);

    disconnect(this, &WindowCoordinator::includeHiddenFiles,   fm, &FileManager::setIncludeHiddenFiles);
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
    emit includeHiddenFiles(inclHdnFls);
}

void WindowCoordinator::emitLabelChanged(QDir newLabel)
{
    emit labelChanged(newLabel);
}
void WindowCoordinator::focusedWindowChanged(QDir dir)
{
    emitLabelChanged(dir);
}
