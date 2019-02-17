#include "process.h"

Process::Process(QString program,
                 QStringList params)
    : QProcess(),
      m_program(program),
      m_params(params)
{
    connectSignals();
}

Process::Process()
    : QProcess()
{
    connectSignals();
}

Process::~Process()
{
    qDebug() << "~Process";
}

void Process::start_Delayed()
{
    this->start(m_program,m_params);
}

void Process::cancel()
{
    m_cancelled = true;
    this->close();
    this->deleteLater();
}

void Process::connectSignals()
{
    // dafuer sorgen, dass der process nach dem finished deleted wird:
    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &QProcess::deleteLater);
}
