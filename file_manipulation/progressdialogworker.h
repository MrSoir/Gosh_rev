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

    explicit ProgressDialogWorker();
    explicit ProgressDialogWorker(const ProgressDialogWorker& w);

    ProgressDialogWorker& operator=(const ProgressDialogWorker& w);

    virtual ~ProgressDialogWorker() override;

public slots:
    virtual void launchDialog() override;
signals:
    void updateProgress(double);
    void addProgressUpperBound_SGNL(unsigned long long);
    void updateMessage(const QString&);

public slots:
    void incrementProgress();
    void addProgressUpperBound(unsigned long long plus);

protected:
    unsigned long long m_elementsToProgress;
    unsigned long long m_elementsProgressed;
    double m_progress;
};



#endif // FILEWORKER_H
