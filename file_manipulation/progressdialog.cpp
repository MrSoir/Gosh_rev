#include "progressdialog.h"

ProgressDialog::ProgressDialog(const QString &msg,
                                               ProgressDialogWorker* worker,
                                               QWidget *parent)
    : ThreadedDialog(worker,parent),
      m_msg(new QLabel(this)),
      m_progBar(new QProgressBar(this))
{
    m_progBar->setRange(0, 100);
    m_msg->setText(msg);
    setLayout();

    connectSignals();
    emit startThread();

    this->show();
}

ProgressDialog::~ProgressDialog()
{
}

void ProgressDialog::updateMessage(const QString &msg)
{
    m_msg->setText(msg);
}

void ProgressDialog::updateProgress(double progrs_in_perctg)
{
    m_progBar->setValue( static_cast<int>(progrs_in_perctg * 100.0) );
}

void ProgressDialog::connectSignals()
{
    ProgressDialogWorker* worker = static_cast<ProgressDialogWorker*>(m_worker);
    connect(worker, &ProgressDialogWorker::updateMessage,  this, &ProgressDialog::updateMessage,  Qt::QueuedConnection);
    connect(worker, &ProgressDialogWorker::updateProgress, this, &ProgressDialog::updateProgress, Qt::QueuedConnection);
}

void ProgressDialog::setLayout()
{
//    m_msg->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_msg->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    QHBoxLayout* hBox = new QHBoxLayout(this);
    hBox->addWidget(m_msg);
    hBox->addWidget(m_progBar);

    QPushButton* cnclBtn = new QPushButton("cancel");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(hBox);
    mainLayout->addWidget(cnclBtn);

    QWidget::setLayout(mainLayout);
}
