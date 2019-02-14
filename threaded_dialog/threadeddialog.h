#ifndef THREADEDDIALOG_H
#define THREADEDDIALOG_H

#include <QObject>
#include <QWidget>
#include <QCloseEvent>
#include <QProgressBar>
#include <QThread>
#include <QDebug>

#include "dialogworker.h"
#include "staticfunctions_files.h"

class ThreadedDialog : public QWidget
{
    Q_OBJECT
public:
    explicit ThreadedDialog(DialogWorker* worker,
                            QWidget *parent = nullptr);

    virtual ~ThreadedDialog() override;
signals:
    void createAndLaunchWorkerThread(QThread::Priority priority = QThread::Priority::NormalPriority);
    void cancel();
public slots:
    void workerFinished();
    void doCancelling();
protected:
    void closeEvent(QCloseEvent* event) override;
protected:
    virtual void connectSignals() = 0;

    DialogWorker* m_worker; // wird ueber signal-slot->deleteLater geloescht, muss nicht im destructor geloescht werden!!!

    bool m_finishedCorrectly;

private:

    void connectWorkerSignals();
};

#endif // THREADEDDIALOG_H
