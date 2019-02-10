#include "openwithdialog.h"

OpenWithDialog::OpenWithDialog(std::string fileToOpenPath,
                               QSize size,
                               QPoint pos,
                               QWidget *parent)
    : QDialog(parent),
      m_fileToOpenPath(fileToOpenPath),
      m_handler(FileMimeTypeHandler(fileToOpenPath))
{
    setMouseTracking(true);


    STATIC_FUNCTIONS::setIconToWidget(this);

    this->setModal(true);
    this->setWindowFlags(Qt::FramelessWindowHint);

    this->setMaximumWidth (size.width());
    this->setMaximumHeight(size.height());
    this->setMinimumWidth (size.width());
    this->setMinimumHeight(size.height());

    this->move(pos.x(), pos.y());

    OpenWithGraphicsView* graphicsView = new OpenWithGraphicsView(QFileInfo(QString::fromStdString(fileToOpenPath)),
                                                                  m_handler.matchingApps());

    connect(graphicsView, &OpenWithGraphicsView::clicked, this, &OpenWithDialog::selected);
    QVBoxLayout* vBox = new QVBoxLayout();
    vBox->addWidget(graphicsView);

    QPushButton* cancelBtn = new QPushButton("cancel");
    connect(cancelBtn, &QPushButton::clicked, this, &OpenWithDialog::close);
    connect(cancelBtn, &QPushButton::clicked, this, &OpenWithDialog::deleteLater);

    QHBoxLayout* btnLyt = new QHBoxLayout();
    btnLyt->addWidget(cancelBtn);

    vBox->addLayout(btnLyt);

    this->setLayout(vBox);

    setMarginsOfLayout(btnLyt);
    setMarginsOfLayout(vBox);

    this->exec();
}

OpenWithDialog::~OpenWithDialog()
{
    qDebug() << "~OpenWithDialog";
}

void OpenWithDialog::onOkClicked()
{
    auto matchingApps = m_handler.matchingApps();
    if(static_cast<int>(matchingApps.size()) > m_selectedProgramId)
    {
        auto app_info = matchingApps[static_cast<size_t>(m_selectedProgramId)];
        auto cmnd_args = app_info.genExecCmndVec({m_fileToOpenPath});
        if(cmnd_args.size() > 0)
        {
            STATIC_FUNCTIONS::execCommand(cmnd_args[0], std::vector<std::string>(cmnd_args.begin()+1, cmnd_args.end()), false);
        }
    }
    this->close();
    this->deleteLater();
}

void OpenWithDialog::selected(int id)
{
    m_selectedProgramId = id;
    onOkClicked();
}

void OpenWithDialog::setMarginsOfLayout(QLayout* layout, int padding)
{
    layout->setSpacing(0);
    layout->setContentsMargins(padding, padding, padding, padding);
}
