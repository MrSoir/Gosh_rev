#ifndef PROCESS_H
#define PROCESS_H

#include <QProcess>
#include <QDebug>
#include <QObject>

// Process selbst implementiert: da im Constructor der QProcess sein signal finished(...) selbst direkt mit deletelater verbindet! -> somit entsteht nie ein haengeder pointer!

class Process: public QProcess
{
public:
    explicit Process(QString program,
                     QStringList params);
    explicit Process();

    virtual ~Process() override;
signals:
public slots:
    void start_Delayed();
    void cancel();
private:
    void connectSignals();

    //-----------------------------

    QString m_program;
    QStringList m_params;

    bool m_cancelled = false;
};
#endif // PROCESS_H
