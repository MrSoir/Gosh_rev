#include "tabcoordinator.h"

TabCoordinator::TabCoordinator(QObject *parent) : QObject(parent)
{
    addTab();
}

TabCoordinator::~TabCoordinator()
{
    m_currentlyDisplWindow = nullptr;
    m_curWindowId = -1;

    for(auto& window: m_windows)
        deleteWindow(window);
    m_windows.clear();

    deleteTCPane();
}

TabCoordinatorPane* TabCoordinator::genTabCoordinatorPane()
{
    QVector<QDir> labels = generateLabels();
    TabCoordinatorPane* tabCoordinatorPane = new TabCoordinatorPane(m_curWindowId, labels);

    connectTCPane();

    return tabCoordinatorPane;
}

void TabCoordinator::setFullScreen()
{
    m_isFullScreen = !m_isFullScreen;
    if(m_isFullScreen){

    }else{

    }
}

void TabCoordinator::addTab()
{
    addTabHelper();
}

void TabCoordinator::setTab(int id)
{
    if(m_windows.size() > id)
    {
        emit resetTabCoordPaneCentralWidget();

        if(m_curWindowId < m_windows.size())
            disconnectWindow(m_windows[m_curWindowId]);

        m_currentlyDisplWindow = m_windows[id];
        m_curWindowId = id;

        revalidateLabels();
        emit activeTabIdChanged(m_curWindowId);
    }
}

void TabCoordinator::removeTab(int id)
{
    // wenn nur ein tab existiert, darf dieser tab natuerlich nicht geloescht werden:
    if(m_windows.size() <= 1)
    {
        qDebug() << "TabCoordinator::removeTab: id: " << id << "    m_windows.size: " << m_windows.size();
        return;
    }

    if(id > -1 && id < m_windows.size())
    {
        deleteWindow(m_windows[id]);
        m_windows.removeAt(id);

        if(m_curWindowId >= id)
        {
            m_curWindowId = m_curWindowId-1 < 0 ? 0 : m_curWindowId-1;
            setTab(m_curWindowId);
        }
    }
}

void TabCoordinator::revalidateLabels()
{
    QVector<QDir> newLabels = generateLabels();
    emit labelsChanged( newLabels );
}

void TabCoordinator::receiveCentralWidgetForTCPane(QWidget* widget)
{
    emit sendCentralWidgetToTCPane(widget);
}

QVector<QDir> TabCoordinator::generateLabels()
{
    QVector<QDir> labels;
    for(int i=0; i < m_windows.size(); ++i)
    {
        if(m_windows[i])
        {
            labels.append( QDir(m_windows[i]->getCurrentFocusedDir()) );
        }
    }
    return labels;
}

void TabCoordinator::openFoldersInNewTab(QVector<QDir> foldersToOpen)
{
    addTabHelper(foldersToOpen);
}



void TabCoordinator::addTabHelper(QVector<QDir> initPaths)
{
    WindowCoordinator* window = new WindowCoordinator(initPaths);
    m_windows.push_back(window);
    connectWindow(window);

    setTab( m_windows.size()-1 );
}

void TabCoordinator::closeActiveTab()
{
    if(m_windows.size() > 1)
    {
        removeTab(m_curWindowId);
    }
}

void TabCoordinator::connectWindow(WindowCoordinator *window)
{
    connect(this, &TabCoordinator::resetTabCoordPaneCentralWidget, window, &WindowCoordinator::createPane);
    connect(window, &WindowCoordinator::paneCreated, this, &TabCoordinator::receiveCentralWidgetForTCPane);
    connect(window, &WindowCoordinator::openFoldersInNewTab, this, &TabCoordinator::openFoldersInNewTab);
    connect(window, &WindowCoordinator::labelChanged,        this, &TabCoordinator::revalidateLabels);
}

void TabCoordinator::disconnectWindow(WindowCoordinator *window)
{
    disconnect(this, &TabCoordinator::resetTabCoordPaneCentralWidget, window, &WindowCoordinator::createPane);
    disconnect(window, &WindowCoordinator::paneCreated, this, &TabCoordinator::receiveCentralWidgetForTCPane);
    disconnect(window, &WindowCoordinator::openFoldersInNewTab, this, &TabCoordinator::openFoldersInNewTab);
    disconnect(window, &WindowCoordinator::labelChanged,        this, &TabCoordinator::revalidateLabels);
}
void TabCoordinator::deleteWindow(WindowCoordinator *window)
{
    disconnectWindow(window);
    window->close();
    window->deleteLater();
}

void TabCoordinator::connectTCPane()
{
    connect(m_tcPane, &TabCoordinatorPane::tabClicked,    this, &TabCoordinator::setTab);
    connect(m_tcPane, &TabCoordinatorPane::tabCloseClicked, this, &TabCoordinator::removeTab);
    connect(m_tcPane, &TabCoordinatorPane::tabAddClicked,   this, &TabCoordinator::addTab);

    connect(this, &TabCoordinator::sendCentralWidgetToTCPane,    m_tcPane, &TabCoordinatorPane::setCentralWidget);
    connect(this, &TabCoordinator::revalidateTabCoordinatorPane, m_tcPane, &TabCoordinatorPane::revalidate);
    connect(this, &TabCoordinator::labelsChanged,                m_tcPane, &TabCoordinatorPane::updateTabLabels);
    connect(this, &TabCoordinator::activeTabIdChanged,           m_tcPane, &TabCoordinatorPane::activeTabIdChanged);
}
void TabCoordinator::disconnectTCPane()
{
    disconnect(m_tcPane, &TabCoordinatorPane::tabClicked,    this, &TabCoordinator::setTab);
    disconnect(m_tcPane, &TabCoordinatorPane::tabCloseClicked, this, &TabCoordinator::removeTab);
    disconnect(m_tcPane, &TabCoordinatorPane::tabAddClicked,   this, &TabCoordinator::addTab);

    disconnect(this, &TabCoordinator::sendCentralWidgetToTCPane,    m_tcPane, &TabCoordinatorPane::setCentralWidget);
    disconnect(this, &TabCoordinator::revalidateTabCoordinatorPane, m_tcPane, &TabCoordinatorPane::revalidate);
    disconnect(this, &TabCoordinator::labelsChanged,                m_tcPane, &TabCoordinatorPane::updateTabLabels);
    disconnect(this, &TabCoordinator::activeTabIdChanged,           m_tcPane, &TabCoordinatorPane::activeTabIdChanged);
}
void TabCoordinator::deleteTCPane()
{
    if(m_tcPane)
    {
        disconnectTCPane();
        m_tcPane->deleteLater();
        m_tcPane = nullptr;
    }
}
