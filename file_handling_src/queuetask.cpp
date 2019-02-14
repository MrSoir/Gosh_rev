#include "queuetask.h"


QueueTask::QueueTask(int jobCounter,
                     QObject *parent)
    : QObject(parent),
      m_jobCounter(jobCounter),
      m_cancelled(false),
      m_thread(new QThread())
{
    connectThreadSignals(m_thread); // connecThreadSignals wird zum Teil von sub-klassen ueberschrieben, da dort nicht sofort die run-function beim start des threads loslaufen soll sondern erst spaeter -> z.B. beim TextDialog soll die run-function erst dann loslaufen, wenn der user den text ausgewaehlt hat und ok klickt!

}

QueueTask::QueueTask(const QueueTask &task)
    : QObject(task.parent()),
      m_jobCounter(task.m_jobCounter),
      m_cancelled(task.m_cancelled),
      m_thread(task.m_thread)
{
}

QueueTask &QueueTask::operator=(const QueueTask &qt)
{
    this->setParent(qt.parent());
    this->m_jobCounter = qt.m_jobCounter;
    this->m_cancelled = qt.m_cancelled;
    this->m_thread = qt.m_thread;
    return *this;
}

QueueTask::~QueueTask()
{
//    if(m_thread)
//        m_thread->quit();
}

bool QueueTask::executableInParallel() const
{
    return false;
}

bool QueueTask::cancelled() const
{
    return m_cancelled;
}

void QueueTask::pause()
{
}

void QueueTask::continue_()
{
}

void QueueTask::cancel()
{
    m_cancelled = true;
    emit cancelSGNL();
}

void QueueTask::onFinished()
{
    if(--m_jobCounter <= 0)
    {
        disconnectSignals();
        emit finished();
    }
}

void QueueTask::createThread()
{
    this->moveToThread(m_thread);

//    connectThreadSignals(m_thread); // connecThreadSignals wird zum Teil von sub-klassen ueberschrieben, da dort nicht sofort die run-function beim start des threads loslaufen soll sondern erst spaeter -> z.B. beim TextDialog soll die run-function erst dann loslaufen, wenn der user den text ausgewaehlt hat und ok klickt!

    m_thread->start();
}

void QueueTask::execute()
{
    createThread();
}

void QueueTask::connectSignals()
{
}

void QueueTask::disconnectSignals()
{
}

void QueueTask::connectThreadSignals(QThread* thread)
{
    connect(thread, &QThread::started,  this, &QueueTask::run);
    connect(this, &QueueTask::finished, this, &QueueTask::deleteLater);
    connect(this, &QueueTask::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
}


