#include "dialogworker.h"

DialogWorker::DialogWorker(QObject *parent)
    : QueueTask(0, parent)
{
}

DialogWorker::DialogWorker(const DialogWorker &dw)
    : QueueTask(dw)
{
}

DialogWorker &DialogWorker::operator=(const DialogWorker &dw)
{
    QueueTask::operator=(dw);
    return *this;
}

DialogWorker::~DialogWorker()
{
}

bool DialogWorker::executableInParallel() const
{
    return true;
}

//void DialogWorker::cancel()
//{
//    qDebug() << "DialogWorker - cancelled!";
//    m_cancelled = true;
////    this->deleteLater();
//}

void DialogWorker::execute()
{
    launchDialog(); // normalerweise fur QueueTask::execute() direkt QueueTask::createThread auf. Dieser Worker soll aber den Dialog starten und dieser Dialog wird dann zu einem spaeteren Zeitpunkt den Worker loslegen lassen.
}

void DialogWorker::finish()
{
    emit finished();
    this->deleteLater();
}
