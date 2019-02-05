#ifndef THREADTESTWORKER_H
#define THREADTESTWORKER_H

#include <QObject>
#include <QThread>
#include <QDebug>

#include <string>

class ThreadTestWorker : public QObject
{
public:
    ThreadTestWorker(const std::string& root_path,
                     QObject* parent = nullptr);
public slots:
    void run();
    void threadFinished();
private:
    void createThread();

    std::string m_str;
};


#endif // THREADTESTWORKER_H
