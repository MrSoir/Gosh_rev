#ifndef DIALOGWORKER_H
#define DIALOGWORKER_H

#include <QObject>
#include <QThread>

#include"file_handling_src/queuetask.h"

class DialogWorker : public QueueTask // QueueTasek, damit mans auf den FileManager.Queue werfen kann
{
    Q_OBJECT
public:
    explicit DialogWorker(QObject *parent = nullptr);
    virtual ~DialogWorker() override;

    virtual bool executableInParallel() const override;
signals:
    void finished();
public slots:
    virtual void cancel() override;
    virtual void createThread() override = 0; // muss von den sub-classes reimplementiert werden!
protected:
    virtual void finish();

private:
};

#endif // DIALOGWORKER_H
