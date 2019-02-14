#ifndef QUEUETASK_H
#define QUEUETASK_H


#include <QObject>
#include <QString>
#include <QThread>
#include <QDebug>

#include <queue>
#include <functional>

// GAAANZ WICHTIGER HINWEIS:
// FileQueue geht folgendermassen vor:
//      wenn es einen QueueTask cancelled, dann wird erwartet, dass der QueueTask
//      dennoch finished, d.h. das signal finished(QueueTask*) sendet.
//      -> nur eben etwas frueher, da durch das cancellen, die routine QueueTask::run()
//      frueher zum abschluss kommen sollte!
// das macht das ganze etwas einfacher zu programmieren. somit muss ausschliesslich
// das signal QueueTask::finished(QueueTask*) => FileQueue::taskFinished(QueueTask*) abgefangen werden und nicht auch noch
// das signal QueueTask::cancelled(QueueTask*) => FileQueue::taskCancelled(QueueTask*)
// semantisch bleibt es aber natuerlich fragwuerdig, nach einem canceln noch zu finishen...

class QueueTask : public QObject
{
    Q_OBJECT
public:
    explicit QueueTask(int jobCounter = 0,
                       QObject* parent = nullptr);
    explicit QueueTask(const QueueTask& task);

    QueueTask& operator=(const QueueTask& qt);

    virtual ~QueueTask() override;

    virtual bool executableInParallel() const; // FileQueue muss QueueTask nicht auf Queue-Stack legen sondern kann ihn direkt starten, selbst wenn ein anderer Task laeuft. Beispiel: kopieren von dateien kann parallel ablaufen, da die aufgabe unabhaengig vom FileExplorer ist. Hingegen muss FileInfoBD.elapseAll() zwingend auf den Queue gepackt werden.

    bool cancelled() const;

    // default constructor & copy constructor & destructor sind gefordert, wenn QueueTask zwecks signal-slot angemeldet/registriert werden muss:
protected:
    int m_jobCounter; // protected, damit sub-classes direkten zugriff drauf haben

signals:
    void finished();
    void cancelSGNL();
public slots:
    virtual void run() = 0;
    virtual void pause();
    virtual void continue_();
    virtual void cancel();

    virtual void onFinished();

    virtual void createThread(); // Worker soll sich selbst in den QThread legen und diesen startetn

    virtual void execute(); // execute ruft im Normalfall direkt createThread auf. Damit legt sich der Worker selbst in einen selbst erstellten Thread und der worker legt los. Der Caller muss nur QueueTask::execute() aufrufen, und fettig -> ABER: z.B. ProgressDialogWorler -> diese launchen infolge fon ProgressDialogWorker::execute den ProgressDialog, der Dialog wird dann spaeter createThread aufrufen um den Worker loslegen zu lassen
private:
    virtual void connectSignals();
    virtual void disconnectSignals();

protected:
    virtual void connectThreadSignals(QThread* thread);

    bool m_cancelled;
    QThread* m_thread;
};


#endif // QUEUETASK_H
