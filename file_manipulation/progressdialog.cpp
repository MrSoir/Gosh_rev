#include "progressdialog.h"

ProgressDialog::ProgressDialog(const QString &msg,
                               ProgressDialogWorker* worker,
                               QWidget *parent)
    : ThreadedDialog(worker,parent),
      m_msg(new QLabel()),
      m_progBar(new QProgressBar())
{
    m_progBar->setRange(0, 100);
    m_msg->setText(msg);
    setLayout();

    connectSignals();

    this->setMinimumWidth(400);
    this->show();

    emit createAndLaunchWorkerThread();
}

ProgressDialog::~ProgressDialog()
{
    qDebug() << "~ProgressDialog";
}

void ProgressDialog::updateMessage(const QString &msg)
{
    m_msg->setText(msg);
}

void ProgressDialog::updateProgress(double progrs_in_perctg)
{
    m_progBar->setValue( static_cast<int>(progrs_in_perctg * 100.0) );
}

void ProgressDialog::addProgressUpperBound(unsigned long long plus)
{
    m_progBar->setMaximum(m_progBar->maximum() + static_cast<int>(plus));
}

void ProgressDialog::connectSignals()
{
    ProgressDialogWorker* worker = static_cast<ProgressDialogWorker*>(m_worker);
    connect(worker, &ProgressDialogWorker::updateMessage,  this, &ProgressDialog::updateMessage,  Qt::QueuedConnection);
    connect(worker, &ProgressDialogWorker::updateProgress, this, &ProgressDialog::updateProgress, Qt::QueuedConnection);

    connect(worker, &ProgressDialogWorker::finished, this, &ProgressDialog::close,       Qt::QueuedConnection);
    connect(worker, &ProgressDialogWorker::finished, this, &ProgressDialog::deleteLater, Qt::QueuedConnection);
}

void ProgressDialog::setLayout()
{
//    m_msg->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_msg->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    QHBoxLayout* hBox = new QHBoxLayout();
    hBox->addWidget(m_msg);
    hBox->addWidget(m_progBar);

    QPushButton* cnclBtn = new QPushButton("cancel");
    connect(cnclBtn, &QPushButton::clicked, this, &ProgressDialog::doCancelling);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addLayout(hBox);
    mainLayout->addWidget(cnclBtn);

    QWidget::setLayout(mainLayout);
}
