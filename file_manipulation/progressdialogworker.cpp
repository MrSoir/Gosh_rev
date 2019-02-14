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

ProgressDialogWorker::ProgressDialogWorker()
    : DialogWorker(),
      m_elementsToProgress(0),
      m_elementsProgressed(0),
      m_progress(0.0)
{
}

ProgressDialogWorker::ProgressDialogWorker(const ProgressDialogWorker &w)
    : DialogWorker(w),
      m_elementsToProgress(w.m_elementsToProgress),
      m_elementsProgressed(w.m_elementsProgressed),
      m_progress(w.m_progress)
{
}

ProgressDialogWorker &ProgressDialogWorker::operator=(const ProgressDialogWorker &w)
{
    DialogWorker::operator=(w);
    m_elementsToProgress = w.m_elementsToProgress;
    m_elementsProgressed = w.m_elementsProgressed;
    m_progress = w.m_progress;

    return *this;
}

ProgressDialogWorker::~ProgressDialogWorker()
{
}

void ProgressDialogWorker::launchDialog()
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

void ProgressDialogWorker::addProgressUpperBound(unsigned long long plus)
{
    m_elementsToProgress += static_cast<double>(plus);
    emit addProgressUpperBound_SGNL(plus);
    emit updateProgress(m_progress);
}
