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
    explicit TextDialogWorker();
    explicit TextDialogWorker(const TextDialogWorker& tdw);

    TextDialogWorker& operator=(const TextDialogWorker& tdw);

    virtual ~TextDialogWorker() override;

public slots:
    virtual void launchDialog() override;
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
