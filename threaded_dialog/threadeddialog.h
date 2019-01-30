#ifndef THREADEDDIALOG_H
#define THREADEDDIALOG_H

#include <QObject>
#include <QWidget>
#include <QCloseEvent>
#include <QProgressBar>
#include <QThread>

#include "dialogworker.h"

class ThreadedDialog : public QWidget
{
    Q_OBJECT
public:
    explicit ThreadedDialog(DialogWorker* worker,
                            QWidget *parent = nullptr);
    virtual ~ThreadedDialog() override;
signals:
    void startThread(QThread::Priority priority = QThread::Priority::NormalPriority);
    void cancel();
public slots:
    void workerFinished();
protected:
    void closeEvent(QCloseEvent* event) override;
protected:
    void createThread();

    virtual void connectSignals() = 0;

    DialogWorker* m_worker; // wird ueber signal-slot->deleteLater geloescht, muss nicht im destructor geloescht werden!!!

    bool m_finishedCorrectly;
};

#endif // THREADEDDIALOG_H
