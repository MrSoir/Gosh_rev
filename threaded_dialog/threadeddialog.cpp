#include "threadeddialog.h"

ThreadedDialog::ThreadedDialog(DialogWorker* worker,
                               QWidget *parent)
    : QWidget(parent),
      m_worker(worker)
{
    connectWorkerSignals();

    STATIC_FUNCTIONS::setIconToWidget(this);
    STATIC_FUNCTIONS::setStyleSheet(this);
}

ThreadedDialog::~ThreadedDialog()
{
    qDebug() << "~ThreadedDialog";
}


void ThreadedDialog::workerFinished()
{
    m_finishedCorrectly = true;

    this->close();
    this->deleteLater();
}


void ThreadedDialog::closeEvent(QCloseEvent* event)
{
    if( !m_finishedCorrectly )
        emit cancel();

    this->deleteLater();
    event->accept();
}

void ThreadedDialog::doCancelling()
{
    qDebug() << "doCancelling...";
    emit cancel();
}

void ThreadedDialog::connectWorkerSignals()
{
    if( !m_worker )
    {
        qDebug() << "ThreadedDialog::createThread - m_worker == nullptr -> Default-Constructor!!!";
        return;
    }

    connect(this, &ThreadedDialog::createAndLaunchWorkerThread,  m_worker, &DialogWorker::createThread, Qt::QueuedConnection);

    connect(this, &ThreadedDialog::cancel,        m_worker, &DialogWorker::cancel, Qt::DirectConnection);

    connect(m_worker, &DialogWorker::finished, this, &ThreadedDialog::workerFinished, Qt::QueuedConnection);

//    emit startThread(); // wird von sub-class gestartet, da sonst der thread bereits laeuft und vielleicht schon fertig ist, bevor die sub-class selbst sich connecten konnte -> sub-class.deleteLater <-> worker-finished laeuft nicht...
}
