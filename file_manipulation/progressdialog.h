#ifndef FILEMANIPULATIONDIALOG_H
#define FILEMANIPULATIONDIALOG_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QThread>

#include "threaded_dialog/threadeddialog.h"
#include "progressdialogworker.h"

class ProgressDialog : public ThreadedDialog
{
    Q_OBJECT
public:
    explicit ProgressDialog(const QString& msg,
                            ProgressDialogWorker* worker,
                            QWidget *parent = nullptr);
    virtual ~ProgressDialog() override;

public slots:
    void updateMessage(const QString& msg);
    void updateProgress(double progrs_in_perctg);
private:
    void connectSignals() override;

    void setLayout();

    QLabel* m_msg;
    QProgressBar* m_progBar;
};

#endif // FILEMANIPULATIONDIALOG_H
