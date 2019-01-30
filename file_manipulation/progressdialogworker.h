#ifndef FILEWORKER_H
#define FILEWORKER_H

#include <QObject>
#include "threaded_dialog/dialogworker.h"

class ProgressDialogWorker: public DialogWorker
{
    Q_OBJECT
public:
    explicit ProgressDialogWorker(unsigned long long elementsToProgress,
                                  QObject* parent = nullptr);
    virtual ~ProgressDialogWorker() override;

public slots:
    virtual void createThread() override;
signals:
    void updateProgress(double);
    void updateMessage(const QString&);

protected:
    void incrementProgress();

    unsigned long long m_elementsToProgress;
    unsigned long long m_elementsProgressed;
    double m_progress;
};



#endif // FILEWORKER_H
