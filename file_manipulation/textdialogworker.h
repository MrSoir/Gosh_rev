#ifndef TEXTDIALOGWORKER_H
#define TEXTDIALOGWORKER_H

#include <QObject>
#include <functional>
#include "threaded_dialog/dialogworker.h"

class TextDialogWorker: public DialogWorker
{
    Q_OBJECT
public:
    explicit TextDialogWorker(const QString &message,
                              const QString &initText,
                              QObject* parent = nullptr);
    virtual ~TextDialogWorker() override;

public slots:
    virtual void createThread() override;
    virtual void textSelected(QString text);
protected:
    virtual void connectThreadSignals(QThread* thread) override;
private:
    QString m_message;
    QString m_initText;

protected:
    virtual std::function<bool(const QString& txt)> getValidator();
    QString m_receivedTextFromDialog;
};

#endif // TEXTDIALOGWORKER_H
