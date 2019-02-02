#include "windowcoordinatorpane.h"

#include "windowcoordinator.h"

WindowCoordinatorPane::WindowCoordinatorPane(WindowCoordinator* wc,
                                             QList<QList<int>> splitterRatios,
                                             int windowCount,
                                             Orientation::ORIENTATION orientation,
                                             QWidget *parent)
    : QWidget(parent),
      m_windowCoordinator(wc),
      m_splitterRatios(splitterRatios),
      m_orientation(orientation),
      m_windowCount(windowCount),
      m_removeDialogSize(QSize(0,0))
{
}

WindowCoordinatorPane::~WindowCoordinatorPane()
{
    clearSplitter();

    m_toolBar = nullptr;
    m_vBox = nullptr;

    deleteMainLayout();
}


QSplitter* createSplitter(Qt::Orientation orientation){
    QSplitter* splitter = new QSplitter();
    splitter->setFixedWidth(1);
    splitter->setContentsMargins(0,0,0,0);
    splitter->setOrientation(orientation);
    return splitter;
}

void WindowCoordinatorPane::clearSplitter()
{
    m_splitter.clear();
}

void WindowCoordinatorPane::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter* painter = new QPainter();
    painter->setBrush(QColor(255,255,255));
    painter->drawRect(QRect(0,0, this->width(), this->height()));
    QWidget::paintEvent(event);
}

void WindowCoordinatorPane::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
    if(event->key() == Qt::Key_F11)
    {
//        emit requestFullScreen();
    }
}

void WindowCoordinatorPane::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    int edge = static_cast<int>(std::min(this->width(), this->height()) * m_dialogSizeFactor);

    m_removeDialogSize = QSize(edge, edge);

    auto pos_x = static_cast<int>(static_cast<double>(this->width()  -m_removeDialogSize.width())  * 0.5);
    auto pos_y = static_cast<int>(static_cast<double>(this->height() -m_removeDialogSize.height()) * 0.5);
    QPoint newPos( pos_x, pos_y );

    emit setRemoveDialogSize(m_removeDialogSize);
    emit setRemoveDialogPosition(newPos);

    QWidget::resizeEvent(event);
}

QList<QList<int> > WindowCoordinatorPane::generateSplitterRatios()
{
    QList<QList<int>> splitterRatios;
    for(auto splitter: m_splitter)
    {
        if(splitter)
        {
            splitterRatios.append(splitter->sizes());
        }
    }
    return splitterRatios;
}


void WindowCoordinatorPane::splitterRatiosChanged()
{
    emit splitterRatiosChangedSGNL( generateSplitterRatios() );
}

void WindowCoordinatorPane::showRemoveWindowDialog()
{
    int edge = int(float(std::min(this->width(), this->height()) * m_dialogSizeFactor));
    m_removeDialogSize = QSize(edge, edge);
    auto pos_x = static_cast<int>(static_cast<double>(this->width()  -m_removeDialogSize.width())  * 0.5);
    auto pos_y = static_cast<int>(static_cast<double>(this->height() -m_removeDialogSize.height()) * 0.5);
    QPoint position( pos_x, pos_y);

    RemoveWindowDialog* removeDialog = new RemoveWindowDialog(m_windowCount, m_orientation,
                            m_removeDialogSize, position,
                            this);

    removeDialog->show();
    removeDialog->raise();
    removeDialog->activateWindow();

    connect(removeDialog, &RemoveWindowDialog::close, removeDialog, &RemoveWindowDialog::deleteLater);
    connect(removeDialog, &RemoveWindowDialog::removeWindow, this, &WindowCoordinatorPane::removeWindow);
    connect(this, &WindowCoordinatorPane::setRemoveDialogSize,     removeDialog, &RemoveWindowDialog::setSize);
    connect(this, &WindowCoordinatorPane::setRemoveDialogPosition, removeDialog, &RemoveWindowDialog::setPosition);
}

void WindowCoordinatorPane::showInfoDialog()
{
    InfoDialog* infoDialog = new InfoDialog(QSize(600,600));
    infoDialog->exec();
}

void WindowCoordinatorPane::showHelpDialog()
{
    HelpDialog* helpDialog = new HelpDialog(QSize(600,600));
    helpDialog->exec();
}


void addLayoutToSplitter(QSplitter* splitter, QLayout* widget){
    QWidget* wrpr = new QWidget();
    wrpr->setLayout(widget);
    splitter->addWidget(wrpr);
}
void addWidgetToSplitter(QSplitter* splitter, QWidget* widget){
    splitter->addWidget(widget);
//    QVBoxLayout* vBox = new QVBoxLayout();
//    vBox->addWidget(widget);
//    vBox->setContentsMargins(3,3,3,3);
//    addLayoutToSplitter(splitter, vBox);
}

void WindowCoordinatorPane::revalidateLayout()
{
    auto windowLayouts = m_windowCoordinator->createCentralWidgets();
    auto orientation   = m_windowCoordinator->getOrientation();

    m_windowCount = windowLayouts.size();
    m_orientation = orientation;

    resetMainLayout();

    setToolBar();

    clearSplitter();

    if(m_windowCount == 1){
        m_vBox->addWidget(windowLayouts[0]);
    }else if(m_windowCount == 2){
        QSplitter* splitter;
        if(orientation == Orientation::ORIENTATION::HORIZONTAL){
            splitter = createSplitter(Qt::Vertical);
        }else{
            splitter = createSplitter(Qt::Horizontal);

        }
        addWidgetToSplitter(splitter, windowLayouts[0]);
        addWidgetToSplitter(splitter, windowLayouts[1]);

        m_splitter.push_back(splitter);

//            connect(splitter, &QSplitter::splitterMoved, this, &WindowCoordinatorPane::saveSplitterSizes);

        m_vBox->addWidget(splitter);
    }else if(m_windowCount == 3){
        QSplitter* hSplitter = createSplitter(Qt::Vertical);
        QSplitter* vSplitter = createSplitter(Qt::Horizontal);
        if(orientation == Orientation::ORIENTATION::VERTICAL){
            addWidgetToSplitter(hSplitter, windowLayouts[1]);
            addWidgetToSplitter(hSplitter, windowLayouts[2]);

            addWidgetToSplitter(vSplitter, windowLayouts[0]);
            addWidgetToSplitter(vSplitter, hSplitter);
            m_vBox->addWidget(vSplitter);
        }else{
            addWidgetToSplitter(vSplitter, windowLayouts[1]);
            addWidgetToSplitter(vSplitter, windowLayouts[2]);

            addWidgetToSplitter(hSplitter, windowLayouts[0]);
            addWidgetToSplitter(hSplitter, vSplitter);
            m_vBox->addWidget(hSplitter);
        }

        m_splitter.push_back(hSplitter);
        m_splitter.push_back(vSplitter);

//            connect(hSplitter, &QSplitter::splitterMoved, this, &WindowCoordinatorPane::saveSplitterSizes);
//            connect(vSplitter, &QSplitter::splitterMoved, this, &WindowCoordinatorPane::saveSplitterSizes);
    }else if (m_windowCount == 4){
        QSplitter* vSplitter = createSplitter(Qt::Horizontal);
        QSplitter* hSplitter1 = createSplitter(Qt::Vertical);
        QSplitter* hSplitter2 = createSplitter(Qt::Vertical);

        addWidgetToSplitter(hSplitter1, windowLayouts[0]);
        addWidgetToSplitter(hSplitter1, windowLayouts[1]);

        addWidgetToSplitter(hSplitter2, windowLayouts[2]);
        addWidgetToSplitter(hSplitter2, windowLayouts[3]);

        addWidgetToSplitter(vSplitter, hSplitter1);
        addWidgetToSplitter(vSplitter, hSplitter2);

        m_splitter.push_back(vSplitter);
        m_splitter.push_back(hSplitter1);
        m_splitter.push_back(hSplitter2);

//            connect(vSplitter,  &QSplitter::splitterMoved, this, &WindowCoordinatorPane::saveSplitterSizes);
//            connect(hSplitter1, &QSplitter::splitterMoved, this, &WindowCoordinatorPane::saveSplitterSizes);
//            connect(hSplitter2, &QSplitter::splitterMoved, this, &WindowCoordinatorPane::saveSplitterSizes);

        m_vBox->addWidget(vSplitter);
    }


    if(m_splitterRatios.size() == static_cast<int>(m_splitter.size()))
    {
        for(unsigned long i=0; i < static_cast<unsigned long>(m_splitterRatios.size()) && i < m_splitter.size(); ++i)
        {
            if(m_splitter[i])
            {
                m_splitter[i]->setSizes(m_splitterRatios[static_cast<int>(i)]);
            }
        }
    }
    for(auto splitter: m_splitter)
    {
        connect(splitter, &QSplitter::splitterMoved, this, &WindowCoordinatorPane::splitterRatiosChanged);
    }
}

void WindowCoordinatorPane::resetMainLayout()
{
    deleteMainLayout();

    m_vBox = new QVBoxLayout();
    this->setLayout(m_vBox);
    m_vBox->setContentsMargins(2, 2, 2, 2);
    m_vBox->setSpacing(0);
}

void WindowCoordinatorPane::deleteMainLayout()
{
    if(this->layout())
        StaticFunctions::clearLayout_and_DeleteContainingWidgets(this->layout(), true);

    auto* oldLayout = this->layout();
    if(oldLayout)
        delete oldLayout;
}

void WindowCoordinatorPane::setToolBar()
{
    m_toolBar = new QHBoxLayout();
    m_toolBar->setAlignment(Qt::AlignLeft);
    m_toolBar->setSpacing(10);

    int windowCount = m_windowCount;

    if(windowCount < 4){
        QPushButton* addBtn = StaticFunctions::PixmapButtonFromPicsResources("frame_add_scld.png", &TOOLBAR_ICON_SIZE);
        if(addBtn){
            connect(addBtn, &QPushButton::clicked, this, &WindowCoordinatorPane::addWindow);
            m_toolBar->addWidget(addBtn);
        }
    }

    if(windowCount > 1){
        QPushButton* closeBtn = StaticFunctions::PixmapButtonFromPicsResources("frame_remove_scld.png", &TOOLBAR_ICON_SIZE);
        if( closeBtn){
            QObject::connect(closeBtn, &QPushButton::clicked, this, &WindowCoordinatorPane::showRemoveWindowDialog);
        }

        if(windowCount < 4){
            QPushButton* orientationBtn = StaticFunctions::PixmapButtonFromPicsResources("compass_BD_scld.png", &TOOLBAR_ICON_SIZE);
            if(orientationBtn)
            {
                connect(orientationBtn, &QPushButton::clicked, this, &WindowCoordinatorPane::changeOrientation);
                m_toolBar->addWidget(orientationBtn);
            }
        }
    }

    QPushButton* fullscreenBtn = StaticFunctions::PixmapButtonFromPicsResources("fullscreen_scld.png", &TOOLBAR_ICON_SIZE);
    if(fullscreenBtn){
        connect(fullscreenBtn, &QPushButton::clicked, this, &WindowCoordinatorPane::requestFullScreen);
        m_toolBar->addWidget(fullscreenBtn);
    }

    QSize infoSize(20,20);
    QPushButton* helpBtn = StaticFunctions::PixmapButtonFromPicsResources("help.png", &infoSize);
    if( helpBtn){
        QObject::connect(helpBtn, &QPushButton::clicked, this, &WindowCoordinatorPane::showHelpDialog);
        m_toolBar->addWidget(helpBtn);
    }
    QPushButton* infoBtn = StaticFunctions::PixmapButtonFromPicsResources("info_scld.png", &infoSize);
    if( infoBtn){
        QObject::connect(infoBtn, &QPushButton::clicked, this, &WindowCoordinatorPane::showInfoDialog);
        m_toolBar->addWidget(infoBtn);
    }

    m_vBox->addLayout(m_toolBar);
}
