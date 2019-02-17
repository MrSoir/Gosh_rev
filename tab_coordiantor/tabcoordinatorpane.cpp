#include "tabcoordinatorpane.h"

#include "tabcoordinator.h"

TabCoordinatorPane::TabCoordinatorPane(TabCoordinator* tc,
                                       int focused_id,
                                       QVector<QDir> labels,
                                       QWidget *parent)
    : QWidget(parent),
      m_tabCoordinator(tc),
      m_mainWidget(nullptr),
      m_mainLayout(nullptr),
      m_tabLabels(labels),
      m_curActiveTabId(focused_id),
      m_tabPane(nullptr),
      m_tabPaneScrollOffsetX(0)
{
}

TabCoordinatorPane::~TabCoordinatorPane()
{
    deleteMainLayout();
}

void TabCoordinatorPane::revalidateWidgets()
{
    revalidate();
}

void TabCoordinatorPane::revalidate()
{
    revalidateMainWidget();
}


void TabCoordinatorPane::setCentralWidget()
{
    QWidget* mainWidget = m_tabCoordinator->createCentralWidget();
    if(mainWidget)
    {
        m_mainLayout->addWidget( mainWidget );
    }
}


//----------------------------------------------------------------


void TabCoordinatorPane::updateTabLabels(QVector<QDir> tabLabels)
{
    m_tabLabels = tabLabels;

    emit updateTabLabelsSGNL(tabLabels);
}

void TabCoordinatorPane::activeTabIdChanged(int id)
{
    if(id == m_curActiveTabId)
        return;

    m_curActiveTabId = id;

    revalidateMainWidget();

    emit activeTabIdChangedSGNL(id);
}
void TabCoordinatorPane::revalidateMainWidget()
{
    clearLayout();
    setTabBar();

    setCentralWidget();
}

void TabCoordinatorPane::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_F11){
        emit setFullScreen();
    }
}

void TabCoordinatorPane::resizeEvent(QResizeEvent *event)
{
    revalidate();

    QWidget::resizeEvent(event);
}

void TabCoordinatorPane::clearLayout()
{
    if(m_tabPane)
    {
        m_tabPaneScrollOffsetX = m_tabPane->getScrollOffsetX();
    }

    deleteMainLayout();

    m_mainLayout = new QVBoxLayout();
    this->setLayout(m_mainLayout);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
}

void TabCoordinatorPane::deleteMainLayout()
{
    disconnectTabBar();

    if(this->layout())
        StaticFunctions::clearLayout_and_DeleteContainingWidgets(this->layout(), true);

    auto* oldLayout = this->layout();
    if(oldLayout)
        delete oldLayout;
}


void TabCoordinatorPane::setTabBar()
{
    m_tabPane = new TabSelectorPane( m_curActiveTabId, m_tabLabels, m_tabPaneScrollOffsetX );

    connectTabBar();

    m_mainLayout->addWidget(m_tabPane);
}

void TabCoordinatorPane::connectTabBar()
{
    if(m_tabPane)
    {
        connect(m_tabPane, &TabSelectorPane::addClicked, this, &TabCoordinatorPane::tabAddClicked);
        connect(m_tabPane, &TabSelectorPane::tabClicked, this, &TabCoordinatorPane::tabClicked);
        connect(m_tabPane, &TabSelectorPane::tabCloseClicked, this, &TabCoordinatorPane::tabCloseClicked);

        connect(this, &TabCoordinatorPane::updateTabLabelsSGNL, m_tabPane, &TabSelectorPane::setTabs);
        connect(this, &TabCoordinatorPane::activeTabIdChangedSGNL, m_tabPane, &TabSelectorPane::setActiveTabId);
        connect(this, &TabCoordinatorPane::revalidateTabSelectorPane, m_tabPane, &TabSelectorPane::revalidate);
    }
}

void TabCoordinatorPane::disconnectTabBar()
{
    if(m_tabPane)
    {
        disconnect(m_tabPane, &TabSelectorPane::addClicked, this, &TabCoordinatorPane::tabAddClicked);
        disconnect(m_tabPane, &TabSelectorPane::tabClicked, this, &TabCoordinatorPane::tabClicked);
        disconnect(m_tabPane, &TabSelectorPane::tabCloseClicked, this, &TabCoordinatorPane::tabCloseClicked);

        disconnect(this, &TabCoordinatorPane::updateTabLabelsSGNL, m_tabPane, &TabSelectorPane::setTabs);
        disconnect(this, &TabCoordinatorPane::activeTabIdChangedSGNL, m_tabPane, &TabSelectorPane::setActiveTabId);
        disconnect(this, &TabCoordinatorPane::revalidateTabSelectorPane, m_tabPane, &TabSelectorPane::revalidate);
    }
}
