#ifndef TEXTDIALOG_H
#define TEXTDIALOG_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QThread>
#include <QLineEdit>

#include <functional>

#include "threaded_dialog/threadeddialog.h"
#include "textdialogworker.h"

class TextDialog: public ThreadedDialog
{
    Q_OBJECT
public:
    explicit TextDialog(const QString& msg,
                        const QString& initLineEditMsg,
                        TextDialogWorker* worker,
                        std::function<bool(const QString& txt)> textValidator = [](const QString& s){Q_UNUSED(s) return true;},
                        QWidget* parent = nullptr);

    virtual ~TextDialog() override;

signals:
    void textSelected(QString txt);
public slots:
    void okClicked();
    void cancelClicked();

    void validateText();
private:
    void connectSignals() override;

    void setMainLayout();

    std::function<bool(const QString& txt)> m_textValidator;
    QLabel* m_msg;
    QLabel* m_validatorLbl;
    QLineEdit* m_lineEdit;

    QPushButton* okBtn;
    QPushButton* cnclBtn;

    bool m_textIsValid;
};

#endif // TEXTDIALOG_H
