#include "textdialogworker.h"

#include "textdialog.h"

TextDialogWorker::TextDialogWorker(const QString& message,
                                   const QString& initText,
                                   QObject *parent)
    : DialogWorker(parent),
      m_message(message),
      m_initText(initText),
      m_receivedTextFromDialog(QString(""))
{
}

TextDialogWorker::~TextDialogWorker()
{
}

void TextDialogWorker::createThread()
{
    new TextDialog(m_message,
                   m_initText,
                   this,
                   getValidator());
}

void TextDialogWorker::textSelected(QString text)
{
    m_receivedTextFromDialog = text;
    run();
}

void TextDialogWorker::connectThreadSignals(QThread *thread)
{
    Q_UNUSED(thread)
    // nichts tun, einfach damit dafuer gesorgt wird, dass nicht bleich bei Therad::start -> Worker::run() passiert
}

std::function<bool (const QString& txt)> TextDialogWorker::getValidator()
{
    return [](const QString& txt){Q_UNUSED(txt) return true;};
}
