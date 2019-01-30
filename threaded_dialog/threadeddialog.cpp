#include "threadeddialog.h"

ThreadedDialog::ThreadedDialog(DialogWorker *worker,
                               QWidget *parent)
    : QWidget(parent),
      m_worker(worker)
{
    createThread();
}

ThreadedDialog::~ThreadedDialog()
{
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

void ThreadedDialog::createThread()
{
    QThread* thread = new QThread();
    m_worker->moveToThread(thread);

    connect(thread, &QThread::started,                      m_worker, &DialogWorker::run, Qt::QueuedConnection);

    connect(this, &ThreadedDialog::startThread,     thread, &QThread::start, Qt::QueuedConnection);
    connect(this, &ThreadedDialog::cancel,          m_worker, &DialogWorker::cancel, Qt::DirectConnection);

    connect(m_worker, &DialogWorker::finished,              this, &ThreadedDialog::workerFinished, Qt::QueuedConnection);

//    emit startThread(); // wird von sub-class gestartet!
}
