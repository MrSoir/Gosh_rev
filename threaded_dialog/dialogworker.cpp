#include "dialogworker.h"

DialogWorker::DialogWorker(QObject *parent)
    : QueueTask(0, parent)
{
//    connect(this, &DialogWorker::finished, this, &DialogWorker::deleteLater);
}

DialogWorker::~DialogWorker()
{
}

bool DialogWorker::executableInParallel() const
{
    return true;
}

void DialogWorker::cancel()
{
    m_cancelled = true;
    this->deleteLater();
}

void DialogWorker::finish()
{
    emit finished();
    this->deleteLater();
}
