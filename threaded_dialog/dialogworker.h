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
    explicit DialogWorker(const DialogWorker& dw);

    DialogWorker& operator=(const DialogWorker& dw);

    virtual ~DialogWorker() override;

    virtual bool executableInParallel() const override;
signals:
    void finished();
public slots:
//    virtual void cancel() override;

    virtual void launchDialog() = 0;
    virtual void execute() override;

    virtual void finish();
protected:

private:
};

#endif // DIALOGWORKER_H
