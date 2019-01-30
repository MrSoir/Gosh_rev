#include "progressdialogworker.h"

#include "progressdialog.h"

ProgressDialogWorker::ProgressDialogWorker(unsigned long long elementsToProgress,
                                           QObject *parent)
    : DialogWorker(parent),
      m_elementsToProgress(elementsToProgress),
      m_elementsProgressed(0),
      m_progress(0.0)
{
}

ProgressDialogWorker::~ProgressDialogWorker()
{
}

void ProgressDialogWorker::createThread()
{
    new ProgressDialog(QString("preparing..."),
                               this);
}

void ProgressDialogWorker::incrementProgress()
{
    double elmntsPrgrsd = static_cast<double>(++m_elementsProgressed);
    double elmntsToPrgrs  = static_cast<double>(m_elementsToProgress);
    m_progress = elmntsPrgrsd / elmntsToPrgrs;

    QString msg =  QString("copying file %1 of %2").arg(m_elementsProgressed).arg(m_elementsToProgress);
    emit updateMessage(msg);

    emit updateProgress(m_progress);
}
