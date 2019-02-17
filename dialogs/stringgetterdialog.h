#ifndef STRINGGETTERDIALOG_H
#define STRINGGETTERDIALOG_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRubberBand>
#include <QDialog>

#include <functional>

#include "staticfunctions.h"

class StringGetterDialog : public QDialog
{
    Q_OBJECT
public:
    explicit StringGetterDialog(const QString& headline,
                                const QString& message,
                                const QString& invalidInput_warning_message,
                                const QString& initInput = QString(""),
                                std::function<bool(const QString&)> m_stringValidator = [](const QString& s){Q_UNUSED(s) return true;},
                                QWidget *parent = nullptr);

signals:
    void stringSelected(QString);
public slots:
    void stringSelected_SLOT();
    void cancelClicked();
protected:
    virtual void closeEvent(QCloseEvent* event) override;
private:
    void createMainLayout();

    void closeDialog();

    bool isValid(const QString& s);

    QString m_headline;
    QString m_message;
    QString m_initInput;
    QString m_invalidInput_warning_message;
    std::function<bool(const QString&)> m_stringValidator;

    QLineEdit* m_lineEdit = nullptr;
};

class StringGetterDialogCreator : public QObject
{
    Q_OBJECT
public:
    explicit StringGetterDialogCreator();
    virtual ~StringGetterDialogCreator() override;

    StringGetterDialog* createDialog(const QString& headline,
                                     const QString& message,
                                     const QString& invalidInput_warning_message,
                                     const QString& initInput = QString(""),
                                     std::function<bool(const QString&)> m_stringValidator = [](const QString& s){Q_UNUSED(s) return true;});
};

#endif // STRINGGETTERDIALOG_H
