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

TextDialogWorker::TextDialogWorker()
    : DialogWorker(nullptr),
      m_message(""),
      m_initText(""),
      m_receivedTextFromDialog(QString(""))
{
}

TextDialogWorker::TextDialogWorker(const TextDialogWorker &tdw)
    : DialogWorker(tdw),
      m_message(tdw.m_message),
      m_initText(tdw.m_initText),
      m_receivedTextFromDialog(tdw.m_receivedTextFromDialog)
{
}

TextDialogWorker &TextDialogWorker::operator=(const TextDialogWorker &tdw)
{
    DialogWorker::operator=(tdw);
    this->m_message = tdw.m_message;
    this->m_initText = tdw.m_initText;
    this->m_receivedTextFromDialog = tdw.m_receivedTextFromDialog;
    return *this;
}

TextDialogWorker::~TextDialogWorker()
{
}

void TextDialogWorker::launchDialog()
{
    new TextDialog(m_message,
                   m_initText,
                   this,
                   getValidator());
}

void TextDialogWorker::textSelected(QString text)
{
    m_receivedTextFromDialog = text;
    createThread();
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
