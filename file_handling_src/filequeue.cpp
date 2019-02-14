#include "filequeue.h"

FileQueue::FileQueue()
    : QObject(nullptr),
      m_tasks(std::queue<QueueTask*>()),
      m_taskIsRunning(false)
{
}

FileQueue::FileQueue(const FileQueue &fq)
    : QObject(fq.parent()),
      m_tasks(fq.m_tasks),
      m_taskIsRunning(fq.m_taskIsRunning)
{
}

FileQueue &FileQueue::operator=(const FileQueue &fq)
{
    this->setParent(fq.parent());
    this->m_tasks = fq.m_tasks;
    this->m_taskIsRunning = fq.m_taskIsRunning;
    return *this;
}


FileQueue::~FileQueue()
{
    emit cancelled();
    while(m_tasks.size() > 0)
    {
        auto task = m_tasks.front();
        m_tasks.pop();
        delete task;
    }
}

void FileQueue::addTask(QueueTask* task)
{
    m_tasks.push(task);
    if( task->executableInParallel() )
    {
        task->execute();
    }else if( !m_taskIsRunning )
    {
        processTasks();
    }
}


void FileQueue::pause()
{
    emit paused();
}
void FileQueue::continue_()
{
    emit continued();
}
void FileQueue::cancel()
{
    emit cancelled();
}


void FileQueue::taskFinished()
{
    if( m_tasks.size() > 0 )
    {
        processTasks();
    }else{
        m_taskIsRunning = false;
        emit blockingTaskHasFinished();
    }
}


void FileQueue::processTasks()
{
    auto task = m_tasks.front();
    m_tasks.pop();
    if(task)
    {
        m_taskIsRunning = true;
        emit blockUntilTaskHasFinish();

        connectTask(task);

        task->execute();
    }
}

void FileQueue::connectTask(QueueTask *task)
{
    connect(this, &FileQueue::cancelled,   task, &QueueTask::cancel, Qt::DirectConnection);
    connect(this, &FileQueue::paused,      task, &QueueTask::pause, Qt::DirectConnection);
    connect(this, &FileQueue::continued,   task, &QueueTask::continue_, Qt::DirectConnection);

    connect(task, &QueueTask::finished, this, &FileQueue::taskFinished, Qt::QueuedConnection);
}
void FileQueue::disconnectTask(QueueTask *task)
{
    disconnect(this, &FileQueue::cancelled,   task, &QueueTask::cancel);
    disconnect(this, &FileQueue::paused,      task, &QueueTask::pause);
    disconnect(this, &FileQueue::continued,   task, &QueueTask::continue_);

    disconnect(task, &QueueTask::finished, this, &FileQueue::taskFinished);
}
